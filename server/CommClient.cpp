// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "CommClient.h"
#include "CommServer.h"
#include "ServerRouting.h"

#include "common/log.h"
#include "common/debug.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Codec.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

static const bool debug_flag = false;

CommClient::CommClient(CommServer & svr, int fd, BaseEntity & c) :
            CommSocket(svr), Idle(svr),
            m_clientIos(fd),
            m_codec(NULL), m_encoder(NULL),
            m_connection(c), m_connectTime(svr.time())
{
    m_clientIos.setTimeout(0,1000);

    m_negotiate = new Atlas::Net::StreamAccept("cyphesis " + m_commServer.m_server.getName(), m_clientIos);
}

CommClient::CommClient(CommServer & svr, BaseEntity & c) :
            CommSocket(svr), Idle(svr),
            m_codec(NULL), m_encoder(NULL),
            m_connection(c), m_connectTime(svr.time())
{
    m_clientIos.setTimeout(0,1000);

    m_negotiate = new Atlas::Net::StreamConnect("cyphesis " + m_commServer.m_server.getName(), m_clientIos);
}

CommClient::~CommClient()
{
    delete &m_connection;
    if (m_negotiate != NULL) {
        delete m_negotiate;
    }
    if (m_encoder != NULL) {
        delete m_encoder;
    }
    if (m_codec != NULL) {
        delete m_codec;
    }
    m_clientIos.close();
}

void CommClient::setup()
{
    debug( std::cout << "Negotiating started" << std::endl << std::flush; );
    // Create the server side negotiator

    m_negotiate->poll(false);

    m_clientIos << std::flush;
}

int CommClient::negotiate()
{
    debug(std::cout << "Negotiating... " << std::flush;);
    // poll and check if negotiation is complete
    m_negotiate->poll();

    if (m_negotiate->getState() == Atlas::Negotiate::IN_PROGRESS) {
        return 0;
    }
    debug(std::cout << "done" << std::endl;);

    // Check if negotiation failed
    if (m_negotiate->getState() == Atlas::Negotiate::FAILED) {
        log(NOTICE, "Failed to negotiate");
        return -1;
    }
    // Negotiation was successful

    // Get the codec that negotiation established
    m_codec = m_negotiate->getCodec(*this);

    // Create a new encoder to send high level objects to the codec
    m_encoder = new Atlas::Objects::ObjectsEncoder(*m_codec);

    // This should always be sent at the beginning of a session
    m_codec->streamBegin();

    // Acceptor is now finished with
    delete m_negotiate;
    m_negotiate = NULL;

    return 0;
}

int CommClient::operation(const Atlas::Objects::Operation::RootOperation & op)
{
    OpVector reply;
    m_connection.operation(op, reply);
    OpVector::const_iterator Iend = reply.end();
    for(OpVector::const_iterator I = reply.begin(); I != Iend; ++I) {
        debug(std::cout << "sending reply" << std::endl << std::flush;);
        (*I)->setRefno(op->getSerialno());
        if (send(*I) != 0) {
            return -1;
        }
    }
    return 0;
}

void CommClient::dispatch()
{
    DispatchQueue::const_iterator Iend = m_opQueue.end();
    for(DispatchQueue::const_iterator I = m_opQueue.begin(); I != Iend; ++I) {
        debug(std::cout << "dispatching op" << std::endl << std::flush;);
        if (operation(*I) != 0) {
            return;
        }
    }
    m_opQueue.clear();
}

void CommClient::objectArrived(const Atlas::Objects::Root & obj)
{
    Atlas::Objects::Operation::RootOperation op = Atlas::Objects::smart_dynamic_cast<Atlas::Objects::Operation::RootOperation>(obj);
    if (!op.isValid()) {
        // FIXME report the parents and objtype
        log(ERROR, "Non op object received from client");
        return;
    }
    debug(std::cout << "A " << op->getParents().front() << " op from client!" << std::endl << std::flush;);
    m_opQueue.push_back(op);
}

void CommClient::idle(time_t t)
{
    if (m_negotiate != 0) {
        if ((t - m_connectTime) > 10) {
            log(NOTICE, "Client disconnected because of negotiation timeout.");
            m_clientIos.shutdown();
        }
    }
}

int CommClient::read()
{
    if (m_codec != NULL) {
        m_codec->poll();
        return 0;
    } else {
        return negotiate();
    }
}

int CommClient::getFd() const
{
    return m_clientIos.getSocket();
}

bool CommClient::isOpen() const
{
    return m_clientIos.is_open();
}

bool CommClient::eof()
{
    return (m_clientIos.fail() || m_clientIos.peek() == EOF);
}

int CommClient::send(const Atlas::Objects::Operation::RootOperation & op)
{
    if (!isOpen()) {
        log(ERROR, "Writing to closed client");
        return -1;
    }
    if (m_clientIos.fail()) {
        return -1;
    }
    m_encoder->streamObjectsMessage(op);
    struct timeval tv = {0, 0};
    fd_set sfds;
    int cfd = m_clientIos.getSocket();
    FD_ZERO(&sfds);
    FD_SET(cfd, &sfds);
    if (select(++cfd, NULL, &sfds, NULL, &tv) > 0) {
        // We only flush to the client if the client is ready
        m_clientIos << std::flush;
    } else {
        debug(std::cout << "Client not ready" << std::endl << std::flush;);
    }
    // This timeout should only occur if the client was really not
    // ready
    if (m_clientIos.timeout()) {
        log(NOTICE, "Client disconnected because of write timeout.");
        m_clientIos.shutdown();
        m_clientIos.setstate(std::iostream::failbit);
        return -1;
    }
    return 0;
}

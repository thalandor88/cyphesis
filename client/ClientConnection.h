// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id$

#ifndef CLIENT_CLIENT_CONNECTION_H
#define CLIENT_CLIENT_CONNECTION_H

#include <common/AtlasStreamClient.h>

#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Root.h>

#include <skstream/skstream.h>

#include <deque>

/// \brief Class to handle socket connection to a cyphesis server from an
/// an admin client
class ClientConnection : public AtlasStreamClient {
  private:
    /// \brief Flag to indicate that a reply has been received from the server
    bool reply_flag;
    /// \brief Flag to indicate that an error has been received from the server
    bool error_flag;
    /// \brief Store for reply data from the server
    Atlas::Objects::Root reply;
    /// \brief Counter used to track serial numbers sent to the server
    int serialNo;

    /// \brief Store for operations arrived from the server
    std::deque<Atlas::Objects::Operation::RootOperation> operationQueue;

    void operation(const Atlas::Objects::Operation::RootOperation&);

    virtual void objectArrived(const Atlas::Objects::Root &);

    void errorArrived(const Atlas::Objects::Operation::RootOperation &);
    void infoArrived(const Atlas::Objects::Operation::RootOperation &);

  public:
    ClientConnection();
    ~ClientConnection();

    int read();
    void login(const std::string &, const std::string &);
    void create(const std::string &, const std::string &);
    int wait();
    void send(const Atlas::Objects::Operation::RootOperation & op);

    /// \brief Check the iostream for waiting data
    int peek() {
        return m_ios->peek();
    }

    /// \brief Check the iostream to see if it is disconnected
    int eof() {
        return m_ios->eof();
    }

    /// \brief Accessor to the server file descriptor
    int get_fd() {
        return m_fd;
    }

    /// \brief Read only accessor for Info reply data from the server
    const Atlas::Objects::Root & getReply() {
        return reply;
    }

    void poll(int timeOut = 0);
    Atlas::Objects::Operation::RootOperation pop();
    bool pending();
};

#endif // CLIENT_CLIENT_CONNECTION_H

// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2013 Alistair Riddoch
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

#include "rulesets/BiomassProperty.h"

#include "rulesets/Entity.h"

#include "common/debug.h"

#include "common/Nourish.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Nourish;
using Atlas::Objects::Operation::Set;

static const bool debug_flag = false;

HandlerResult BiomassProperty::eat_handler(Entity * e,
                                           const Operation & op,
                                           OpVector & res)
{
    const Property<double> * pb = e->getPropertyType<double>("biomass");
    if (pb == NULL) {
        debug(std::cout << "Eat HANDLER no biomass" << std::endl 
                        << std::flush;);
        return OPERATION_IGNORED;
    }
    
    const double & biomass = pb->data();

    Anonymous self;
    self->setId(e->getId());
    self->setAttr("status", -1);

    Set s;
    s->setTo(e->getId());
    s->setArgs1(self);

    const std::string & to = op->getFrom();
    Anonymous nour_arg;
    nour_arg->setId(to);
    nour_arg->setAttr("mass", biomass);

    Nourish n;
    n->setTo(to);
    n->setArgs1(nour_arg);

    res.push_back(s);
    res.push_back(n);

    return OPERATION_IGNORED;
}

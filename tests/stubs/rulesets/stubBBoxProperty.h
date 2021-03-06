/*
 Copyright (C) 2014 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef STUBBBOXPROPERTY_H_
#define STUBBBOXPROPERTY_H_


BBoxProperty::BBoxProperty()
{
}

void BBoxProperty::apply(LocatedEntity * ent)
{
}

int BBoxProperty::get(Atlas::Message::Element & val) const
{
    return -1;
}

void BBoxProperty::set(const Atlas::Message::Element & val)
{
}

void BBoxProperty::add(const std::string & key,
        Atlas::Message::MapType & map) const
{
}

void BBoxProperty::add(const std::string & key,
                       const Atlas::Objects::Entity::RootEntity & ent) const
{
}

BBoxProperty * BBoxProperty::copy() const
{
    return 0;
}


#endif /* STUBBBOXPROPERTY_H_ */

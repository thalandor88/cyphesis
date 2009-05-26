// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

#include <Python.h>

#include "rulesets/Python_API.h"

#include <cassert>

int main()
{
    init_python_api();

    assert(PyRun_SimpleString("print 'hello'") == 0);
    assert(PyRun_SimpleString("import sys") == 0);
    assert(PyRun_SimpleString("sys.stdout.write('hello')") == 0);
    assert(PyRun_SimpleString("sys.stdout.write(1)") == -1);
    assert(PyRun_SimpleString("sys.stderr.write('hello')") == 0);
    assert(PyRun_SimpleString("sys.stderr.write(1)") == -1);

    assert(PyRun_SimpleString("from common import log") == 0);
    assert(PyRun_SimpleString("log.debug('foo')") == 0);
    assert(PyRun_SimpleString("log.thinking('foo')") == 0);

    assert(PyRun_SimpleString("import atlas") == 0);

    assert(PyRun_SimpleString("l=atlas.Location()") == 0);
    assert(PyRun_SimpleString("atlas.isLocation(l)") == 0);
    assert(PyRun_SimpleString("atlas.isLocation(1)") == 0);
    assert(PyRun_SimpleString("l1=atlas.Location()") == 0);
    assert(PyRun_SimpleString("l2=atlas.Location()") == 0);

    assert(PyRun_SimpleString("import physics") == 0);
    assert(PyRun_SimpleString("physics.distance_to()") == -1);
    assert(PyRun_SimpleString("physics.square_distance()") == -1);
    assert(PyRun_SimpleString("physics.square_horizontal_distance()") == -1);
    assert(PyRun_SimpleString("physics.distance_to(l1, l2)") == 0);
    assert(PyRun_SimpleString("physics.square_distance(l1, l2)") == 0);
    assert(PyRun_SimpleString("physics.square_horizontal_distance(l1, l2)") == 0);
    assert(PyRun_SimpleString("physics.distance_to('1', l2)") == -1);
    assert(PyRun_SimpleString("physics.square_distance('1', l2)") == -1);
    assert(PyRun_SimpleString("physics.square_horizontal_distance('1', l2)") == -1);

    shutdown_python_api();
    return 0;
}
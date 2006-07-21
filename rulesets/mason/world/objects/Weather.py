#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Al Riddoch (See the file COPYING for details).

from atlas import *
from world.objects.Thing import Thing
from common import log,const
try:
  from random import *
except ImportError:
  from whrandom import *

class Weather(Thing):
    def setup_operation(self, op):
        return Operation("tick",to=self)
    def tick_operation(self, op):
        res = Message()
        optick = Operation("tick", to=self)
        res = res + optick
        if self.rain<0.5:
            optick.setFutureSeconds(randint(120,300))
            self.rain=1.0
        else:
            optick.setFutureSeconds(randint(600,1200))
            self.rain=0.0
        res = res+Operation("set", Entity(self.id,rain=self.rain), to=self)
        return res

#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2005-2006 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *
from Quaternion import Quaternion
from Vector3D import Vector3D

try:
  from random import *
except ImportError:
  from whrandom import *

from world.objects.Thing import Thing

class Combat(Thing):
    """A very simple combat system example."""
    def attack_operation(self, op):
        """ The attack op is FROM the the character that initiated combat which
            we term the attacker, TO the character that is attacker which we
            term the defender. We store the IDs of both. """
        # Check if the attacked characters stamina is too low for combat
        if self.character.stamina < 0.1:
            print "Aborting defender stamina low"
            self.irrelevant()
            return
        defender = op.to
        if defender != self.character.id:
            print "Attack operation is wierd"
        self.defender=defender
        attacker = op.from_
        self.attacker=attacker
        # Attach this task to the attacker. Its already implicitly attached
        # to the defender who owns this task.
        a=self.character.world.get_object(self.attacker)
        # Check if the attacking characters stamina is too low for combat
        if not a or a.stamina < 0.1:
            self.irrelevant()
            return
        a.set_task(self.cppthing)
        # Send ourselves a tick immediatly to start things going.
        res=Message()

        # Consequence of this is currently that the defender gets to
        # hit first. Might be a good idea to try and avoid this.
        tick=Operation("tick", to=self.attacker)
        tick.setFutureSeconds(1.75)
        tick.sub_to="task"
        res.append(tick)

        return res
    def tick_operation(self, op):
        """ This method is called repeatedly, each time a combat turn occurs.
            In this example the interval is fixed, but it can be varied.
            self.attacker is the ID of the character that initiated the combat
            self.defender is the ID of the character that was initially
            attacked The self.attack flag is used to alternate the attack from
            one combatant to the other. """
        if self.count() < 2:
            print "Someone has dropped out"
            self.irrelevant()
            return

        if self.attacker == op.to:
            print "Attacker attacking"
            a=self.attacker
            d=self.defender
        else:
            print "Defender attacking"
            d=self.attacker
            a=self.defender

        attacker = self.character.world.get_object(a)
        if not attacker:
            self.irrelevant()
            return
        if attacker.stamina <= 0:
            self.irrelevant()
            return

        defender = self.character.world.get_object(d)
        if not defender:
            self.irrelevant()
            return

        # A very simple formula is used to determine the damage done
        damage = (attacker.statistics.attack / defender.statistics.defence) / uniform(2,10)
        # Damage is counted against stamina, to ensure combat is non lethal,
        # and make recovery easier.
        stamina=defender.stamina-damage
        if stamina<0: stamina=0
        set_arg=Entity(d, stamina=stamina)

        # We send 3 operations to indicate what is going on. The imginary ops
        # provide emotes for the actions. The sight(attack) operation
        # indicates that a singleshot animation of attacking should be
        # triggered on the attacker.
        attacker.send_world(Operation("imaginary", Entity(description="hits for massive damage."), to=attacker))
        attacker.send_world(Operation("sight", Operation("attack", to=d, from_=a)))
        defender.send_world(Operation("imaginary", Entity(description="defends skillfully."), to=defender))

        # If the defenders stamina has reached zero, combat is over, and emotes
        # are sent to indicate this.
        if stamina <= 0:
            set_arg.status = defender.status - 0.1
            defender.send_world(Operation("imaginary", Entity(description="has been defeated"), to=defender))
            defender.send_world(Operation("sight", Operation("collapse", from_=d)))
            attacker.send_world(Operation("imaginary", Entity(description="is victorious"), to=attacker))
            self.irrelevant()

        res=Message()
        # This set op makes the change to defenders stamina, and a small health
        # change if they have been defeated
        res.append(Operation("set", set_arg, to=defender))

        # Turn the attacker to face the defender. This has to go through
        # the mind2body interface, so it does not interrupt what the
        # the character is doing.
        faceop=self.face(attacker, defender)
        if faceop:
            faceop=attacker.mind2body(faceop)
            if faceop:
                res.append(faceop)

        # Don't return the following tick op if this task is now complete
        if self.obsolete():
            return res

        # Schedule a new tick op
        tick=Operation("tick", to=op.to)
        tick.sub_to="task"
        tick.setFutureSeconds(1.75)
        res.append(tick)
        return res
    def face(self, character, other):
        """ Turn to face that another character, ensuring that
            we are facing the character we are hitting """
        vector = distance_to(character.location, other.location)
        vector.z = 0
        if vector.square_mag() < 0.1:
            return
        vector = vector.unit_vector()
        newloc = Location(character.location.parent)
        newloc.orientation = Quaternion(Vector3D(1,0,0), vector)
        return Operation("move", Entity(character.id, location=newloc))

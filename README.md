# What's that

Simple demo of terraforming the heightmap by group of Diggers, coordinated by Brigadier.
Original idea taken from game, the RTS Perimeter (2004).

Diggers order of action:
* grab some "soil" (height points) from planned above-zerolayer tiles up to capacity, if no such tiles - take soil from nearest unplanned tiles;
* put soil into planned below-zerolayer tiles, if no such - put into any nearest tiles, up to height limit.


Brigadier order of action:
* just follow user's guidance and give orders to siblings (Diggers) of where they can found desired tile to terraform.

# Controls

* WASD - move camera;
* QE - camera raise up/down;
* LMB - plan terraform;
* RMB - set target for Brigadier to move;
* +/- - increase/decrease planing tool radius;
* F1 - show/hide zerolayer highlighing;
* TAB - switch planing/texture mode;
* Space - pause/resume demo.

# Known issues

* Light shader - doesn't updates, when map mesh changes.
* Tile counting - inconsistency of to-terraform-count and actually planed tiles. Or inconsistency with already-terraformed tiles, dunno.
* Possible app crushed cause of digging where is no soil to dig, but thats should have been fixed.

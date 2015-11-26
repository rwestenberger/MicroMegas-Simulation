#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#import cog
from configparser import ConfigParser, ExtendedInterpolation
import os

conf = ConfigParser(interpolation=ExtendedInterpolation())
conf.read(os.path.join(os.path.dirname(os.path.abspath(__file__)), "simulation.conf"))

'''In the source file you now just need to do:
	/*[[[cog
	from MMconfig import *
	cog.outl("cout << " + config["drift"]["gap"] + " << endl;")
	]]]*/
	//[[[end]]]
'''
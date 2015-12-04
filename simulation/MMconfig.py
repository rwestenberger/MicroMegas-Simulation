#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import cog
from configparser import ConfigParser, ExtendedInterpolation
import os

conf = ConfigParser(interpolation=ExtendedInterpolation())
conf.read(os.path.join(os.path.dirname(os.path.abspath(__file__)), "simulation.conf"))

if not os.path.exists(conf["general"]["output_path"]):
	os.makedirs(conf["general"]["output_path"])
	print("Created output directory: {}".format(conf["general"]["output_path"]))

'''In the source file you now just need to do:
	/*[[[cog
	from MMconfig import *
	cog.outl("cout << " + config["drift"]["gap"] + " << endl;")
	]]]*/
	//[[[end]]]
'''
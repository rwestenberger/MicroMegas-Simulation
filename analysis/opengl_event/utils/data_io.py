# -*- coding: utf-8 -*-

import ROOT
from root_numpy import tree2array

def read_data(input_file_path, tree_name, event=1):
	input_file = ROOT.TFile(input_file_path, 'read')
	input_tree = input_file.Get(tree_name)
	data = tree2array(input_tree)
	input_file.Close()
	event_data = data[event-1] # choose event to display
	return event_data
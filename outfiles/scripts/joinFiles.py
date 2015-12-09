#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import argparse, sys, re, math, os

def get_tree_dict(input_files):
	tree_dict = {}
	for input_file_name, input_file in input_files.items():
		tree_dict[input_file_name] = [key.GetName() for key in input_file.GetListOfKeys() if key.GetClassName() == 'TTree']
	return tree_dict

def joinFiles(input_file_names, output_file_name):
	from ROOT import TFile, TTree # here, otherwise -h/--help will not work

	print('Joining files %s to %s.'%(', '.join(input_file_names), output_file_name))

	input_files = {input_file_name:TFile(input_file_name, 'read') for input_file_name in input_file_names}
	input_tree_names = get_tree_dict(input_files)
	input_trees = {}
	for input_file_name, tree_names in input_tree_names.items():
		input_trees[input_file_name] = [input_files[input_file_name].Get(tree_name) for tree_name in tree_names]

	output_file = TFile(output_file_name, 'recreate')
	for input_file_name, input_file in input_files.items():
		for input_tree in input_trees[input_file_name]:
			if input_tree.GetName() not in [key.GetName() for key in output_file.GetListOfKeys()]:
				print('Creating new tree: %s [%s]'%(input_tree.GetName(), input_file_name))
				output_tree = input_tree.CloneTree(0);
				output_file.Write()
			for i in range(input_tree.GetEntries()):
				input_tree.GetEntry(i)
				output_tree.Fill()
			output_tree.Write()
	output_file.Close()

	print('Wrote: %s'%(output_file_name))

def main():
	parser = argparse.ArgumentParser(description='Join multiple ROOT files to one.')
	parser.add_argument('-o', '--outputFileName', help='ROOT output files', required=True)
	parser.add_argument('inputFileNames', help='ROOT input files to join', nargs='+')

	args = parser.parse_args()
	joinFiles(args.inputFileNames, args.outputFileName)

if __name__ == '__main__':
	main()

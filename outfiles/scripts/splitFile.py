#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse, sys, re, math, os

def get_tree_list(input_file):
	tree_list = [key.GetName() for key in input_file.GetListOfKeys() if key.GetClassName() == 'TTree']
	return tree_list

def check_for_tree(input_file, input_tree_name):
	tree_list = get_tree_list(input_file)
	if len(tree_list) == 0:
		print('No tree in {} found!'.format(input_file_name))
		sys.exit(1)
	
	if len(tree_list) > 1 and not input_tree_name:
		print('Multiple trees found: {}. Provide -t/--tree!'.format(', '.join(tree_list)))
		sys.exit(1)

	if len(tree_list) == 1 and not input_tree_name:
		input_tree_name = tree_list[0]
		print('Choosing tree {} to split.'.format(input_tree_name))

	if input_tree_name not in tree_list:
		print('Could not find tree {} in {}'.format(input_file_name, input_file_name))
		sys.exit(1)

def splitFile(input_file_name, n_events_per_file, input_tree_name):
	from ROOT import TFile, TTree # here, otherwise -h/--help will not work

	print('Splitting tree {} of file {} into parts of {} events each.'.format(input_tree_name, input_file_name, n_events_per_file))

	input_file = TFile(input_file_name, 'read')
	check_for_tree(input_file, input_tree_name)
	input_tree = input_file.Get(input_tree_name)

	output_file_names = []

	nevents = input_tree.GetEntries()

	for file_num, event_nums in enumerate([range(nevents)[i:i+n_events_per_file] for i in range(0, nevents, n_events_per_file)]):
		input_file_base, extension = os.path.splitext(input_file_name)
		output_file_name = '{}_{}{}'.format(input_file_base, file_num, extension)
		output_file_names.append(output_file_name)
		output_file = TFile(output_file_name, 'recreate')
		output_tree = input_tree.CloneTree(0);

		for event_num in event_nums:
			input_tree.GetEntry(event_num)
			output_tree.Fill()

		output_file.Write()
		output_file.Close()

	print('Wrote: {}'.format(', '.join(output_file_names)))

def main():
	parser = argparse.ArgumentParser(description='Split ROOT in multiple files.')
	parser.add_argument('inputFileName', help='ROOT input file to split')
	parser.add_argument('n', help='Number of events per file', type=int)
	parser.add_argument('-t', '--tree', help='TTree to split')

	args = parser.parse_args()
	splitFile(args.inputFileName, args.n, args.tree)

if __name__ == '__main__':
	main()
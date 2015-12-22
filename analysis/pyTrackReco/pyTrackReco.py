#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
import numpy as np

import matplotlib.pyplot as plt

from mpl_toolkits.mplot3d import Axes3D

from ROOT import TFile, TTree

from root_numpy import root2rec
from numpy.lib.recfunctions import append_fields

class Data:
	'''Namespace for the data.'''
	def __init__(self, drift_file_path, avalanche_file_path):
		self.drift = root2rec(drift_file_path, 'driftTree')
		self.avalanche = root2rec(avalanche_file_path, 'avalancheTree')

		# TODO: read in data, move primary electron data to self.primary

	def reconstructZvalues(self, drift_velocity=4.6698e-3):
		'''Reconstructs z values of given dataset with the given drift_velocity.'''
		# unit drift_velocity: cm/ns

		start_time = np.array([min(times) if len(times)>0 else 0. for times in self.avalanche.t1])
		z_reco = np.array([drift_velocity * (self.avalanche.t1[i] - start_time[i]) for i in range(len(self.avalanche.t1))], dtype=[('z_reco', float)])

		self.avalanche = append_fields(self.avalanche, 'z_reco', z_reco, usemask=False, asrecarray=True)

class Track:
	'''Represents a single primary electron track, described by the given x, y, z values of data.'''
	def __init__(self, drift_data, avalanche_data):
		self.drift_data = drift_data
		self.avalanche_data = avalanche_data

	def draw(self):
		fig = plt.figure()
		ax = fig.add_subplot(111, projection='3d')

		ax.scatter(self.avalanche_data.x1, self.avalanche_data.y1, self.avalanche_data.z_reco)
		print(self.avalanche_data.x1)
		ax.scatter(np.array([4,3,2,1,0]), np.array([4,3,2,1,0]), np.array([.2,.4,.6,.8,1]))
		ax.set_xlabel('x [cm]')
		ax.set_ylabel('y [cm]')
		ax.set_zlabel('z [cm]')

		plt.show()

def main():
	input_file_path = '/home/rwestenb/simulation/MicroMegas-Simulation/outfiles/results/theta0_200keV_100k'
	drift_file_path = os.path.join(input_file_path, 'drift.root')
	avalanche_file_path = os.path.join(input_file_path, 'avalanche.root')

	data = Data(drift_file_path, avalanche_file_path)
	data.reconstructZvalues()

	track = Track(data.drift[0], data.avalanche[0])

	#print(track.get_number_of_deflections())
	#track.interpolate()

	track.draw()


if __name__ == '__main__':
	main()

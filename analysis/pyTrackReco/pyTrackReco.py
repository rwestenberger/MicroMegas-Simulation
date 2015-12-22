#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
import numpy as np

import matplotlib.pyplot as plt
from matplotlib import cm

from mpl_toolkits.mplot3d import Axes3D

from root_numpy import root2rec
from numpy.lib.recfunctions import append_fields, merge_arrays

from scipy import interpolate
from scipy.spatial import distance

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
		z_reco = np.array([drift_velocity * (self.avalanche.t1[i] - start_time[i]) for i in range(len(self.avalanche.t1))])
		self.reco_data = z_reco

class Track:
	'''Represents a single primary electron track, described by the given x, y, z values of data.'''
	def __init__(self, drift_data, avalanche_data, z_reco_data):
		self.drift_data = drift_data
		self.avalanche_data = avalanche_data
		self.z_reco_data = z_reco_data

	def get_cleaned_data(self, cut_value=4.):
		'''Returns data without outliers, cut by the mean pairwise mahalanobis distances.'''
		data = np.array([self.avalanche_data.x1, self.avalanche_data.y1, self.z_reco_data])

		data_transp = data.transpose()
		dists = distance.cdist(data_transp, data_transp, 'mahalanobis', VI=None)
		mean_dists = np.mean(dists, axis=1)
		
		return data.transpose()[np.where(mean_dists <= cut_value)].transpose(), data.transpose()[np.where(mean_dists > cut_value)].transpose()

	def draw(self):
		fig = plt.figure()

		data = np.array([self.avalanche_data.x1, self.avalanche_data.y1, self.z_reco_data])
		data_clean, data_outliers = self.get_cleaned_data()

		# 3d interpolation
		ax = fig.add_subplot(121, projection='3d')
		ax.set_aspect('equal')
		tck, u = interpolate.splprep(data_clean, k=5, t=100.)
		interp = interpolate.splev(np.linspace(0,1,100), tck)

		ax.scatter(*data_clean, c='b', alpha=.2, lw = 0)
		ax.scatter(*data_outliers, c='y', alpha=1., lw = 0)
		ax.scatter(self.drift_data.x0, self.drift_data.y0, self.drift_data.z0, c='r', lw = 0)
		ax.plot(interp[0], interp[1], interp[2], c='r')
		ax.set_xlabel('x [cm]')
		ax.set_ylabel('y [cm]')
		ax.set_zlabel('z [cm]')
		ax.set_zlim([0, 1])
		#ax.set_xlim([-3,3]); ax.set_ylim([-3,3]); ax.set_zlim([-3,3])

		# hit contour
		ax.plot(data_clean[0], data_clean[1], '+k', c='b', alpha=.2, zdir='z', zs=0)

		# distances
		data_transp = data_clean.transpose()
		dists = distance.cdist(data_transp, data_transp, 'mahalanobis', VI=None)
		mean_dists = np.mean(dists, axis=1)

		hist2d_dists = fig.add_subplot(222)
		hist2d_dists.imshow(dists, interpolation="nearest")
		hist2d_dists.set_xlabel('Index')
		hist2d_dists.set_ylabel('Index')

		hist_dists = fig.add_subplot(224)
		hist_dists.hist(mean_dists, 100)
		hist_dists.set_xlabel('Mean mahalanobis distance')

		fig.tight_layout()
		plt.show()

def main():
	input_file_path = '/localscratch/simulation_files/MicroMegas-Simulation/outfiles/theta0_200keV_100k'
	drift_file_path = os.path.join(input_file_path, 'drift.root')
	avalanche_file_path = os.path.join(input_file_path, 'avalanche.root')

	data = Data(drift_file_path, avalanche_file_path)
	data.reconstructZvalues()

	event = 164
	track = Track(data.drift[event], data.avalanche[event], data.reco_data[event])

	#print(track.get_number_of_deflections())
	#track.interpolate()

	track.draw()


if __name__ == '__main__':
	main()

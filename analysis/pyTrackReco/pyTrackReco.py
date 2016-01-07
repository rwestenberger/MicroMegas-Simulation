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
	def __init__(self, drift_file_path, avalanche_file_path):
		self.drift = root2rec(drift_file_path, 'driftTree')
		self.avalanche = root2rec(avalanche_file_path, 'avalancheTree')

		# TODO: read in data, move primary electron data to self.primary

	def reconstructZvalues(self, drift_velocity=4.6698e-3):
		'''Reconstructs z values of given dataset with the given drift_velocity.'''
		# unit drift_velocity: cm/ns

		start_times = np.array([min(times) if len(times)>0 else 0. for times in self.avalanche.t1])
		z_reco = drift_velocity * (self.avalanche.t1 - start_times)
		self.reco = np.array([self.avalanche.x1, self.avalanche.y1, z_reco])
		# self.reco_data[0:x,1:y,2:z][event_num][electron_num]

	def calculate_clean_indizes(self, cut_value=4.):
		'''Cuts outliers by the mean pairwise mahalanobis distances.'''
		self.clean_indizes = [[]]*len(self.reco[0])
		self.outlier_indizes = [[]]*len(self.reco[0])

		for event in range(self.reco.shape[1]):
			data_transp = np.array(list(self.reco[:,event])).transpose()
			if len(data_transp) == 0: continue # no electrons in this event
			dists = distance.cdist(data_transp, data_transp, 'mahalanobis', VI=None)
			mean_dists = np.mean(dists, axis=1)
			
			self.clean_indizes[event] = np.where(mean_dists <= cut_value)[0]
			self.outlier_indizes[event] = np.where(mean_dists > cut_value)[0]

		self.clean_indizes = np.array(self.clean_indizes)
		self.outlier_indizes = np.array(self.outlier_indizes)

	def draw(self, event, fig, ax, plot_area):
		# sketch detector
		x, y = np.mgrid[-plot_area:plot_area+1:1, -plot_area:plot_area+1:1]
		ax.plot_wireframe(x, y, np.ones(len(x)), alpha=.3, color='black')
		ax.plot_wireframe(x, y, np.zeros(len(x)), alpha=.3, color='black')

		reco = np.array(list(self.reco[:,event]))
		ax.scatter(*reco[:,self.clean_indizes[event]], c='b', alpha=.2, lw=0, label='Reco data cleaned')
		ax.scatter(*reco[:,self.outlier_indizes[event]], c='y', alpha=1., lw=0, label='Reco data outliers')

		# sort real track, just connect closest points
		drift_data_transp = np.array([self.drift.x0[event], self.drift.y0[event], self.drift.z0[event]]).transpose()

		point_mask = np.ones(len(self.drift.x0[event]), np.bool) # used to not select the same points again
		point_indizes = np.arange(len(self.drift.x0[event]))

		point_index = np.argmax(self.drift.z0[event])
		point_mask[point_index] = False
		order = [point_index]
		for i in range(1,len(self.drift.z0[event])):
			dists = distance.cdist(np.array([drift_data_transp[point_index]]), drift_data_transp[point_mask])
			point_index = point_indizes[point_mask][np.argmin(dists[0])]
			order.append(point_index)
			point_mask[point_index] = False

		ax.plot(self.drift.x0[event][order], self.drift.y0[event][order], self.drift.z0[event][order], c='red', lw=2, alpha=1., label='Real track')
		#ax.scatter(self.drift.x0[event], self.drift.y0[event], self.drift.z0[event], c='red', lw=0, alpha=.5, label='Real track')

		# point order
		distances = fig.add_subplot(243)
		distances.plot(order)
		distances.grid()
		distances.set_title('Point order of real track')
		distances.set_xlabel('Old')
		distances.set_ylabel('New')

		# hit contour
		ax.plot(self.avalanche.x0[event], self.avalanche.y0[event], ',k', alpha=1., zdir='z', zs=0, label='Hit positions')

		# distances
		data_transp = reco.transpose()
		dists = distance.cdist(data_transp, data_transp, 'mahalanobis', VI=None)
		mean_dists = np.mean(dists, axis=1)

		hist2d_dists = fig.add_subplot(244)
		hist2d_dists.imshow(dists, interpolation="nearest")
		hist2d_dists.grid()
		hist2d_dists.set_title('Reco data outliers by mahalanobis distance')
		hist2d_dists.set_xlabel('Electron')
		hist2d_dists.set_ylabel('Electron')

		hist_dists = fig.add_subplot(224)
		hist_dists.hist(mean_dists, 100, log=True)
		hist_dists.grid(which='both')
		hist_dists.set_xlabel('Mean mahalanobis distance')

class Track:
	'''Represents a single primary electron track, abstract class.'''
	def __init__(self, data, event):
		# copy event specific data
		self.drift_data = data.drift[event] # drift_data.x0,x1,y0,y1,...
		self.avalanche_data = data.avalanche[event] # avalanche_data.x0,x1,y0,y1,...
		self.reco_data = np.array(list(data.reco[:,event])) # reco_data[0:x,1:y,2:z]

		self.clean_indizes = data.clean_indizes[event]
		self.outlier_indizes = data.outlier_indizes[event]

class TrackSpline(Track):
	'''Reconstructed by B-splines.'''
	def reconstruct_track(self):
		data_cleaned = self.reco_data[:,self.clean_indizes]

		#sort_indices = np.argsort(self.drift_data.z0, axis=0)[::-1]
		#tck, u = interpolate.splprep([self.drift_data.x0[sort_indices], self.drift_data.y0[sort_indices], self.drift_data.z0[sort_indices]], k=3, s=1e-3)

		sort_indices = np.argsort(self.avalanche_data.t1[self.clean_indizes], axis=0)[::-1]
		self.tck, u = interpolate.splprep(data_cleaned[:,sort_indices], k=3, s=10.)
		#self.tck, u = interpolate.splprep(data_cleaned, k=3, s=10.)
	
	def draw(self, fig, ax):	
		ax.plot(*interpolate.splev(np.linspace(0,1,1000), self.tck), color='orange', lw=2, label='TrackSpline')

class TrackMovingAverage(Track):
	'''Reconstructed as moving average.'''
	def reconstruct_track(self, n_average=40):
		data_cleaned = self.reco_data[:,self.clean_indizes] # get cleaned data
		data_cleaned = data_cleaned[:,np.argsort(data_cleaned[2])] # sort data by z coordinate

		n_electrons = data_cleaned.shape[1]
		self.track = []
		for i in range(0, n_electrons - n_average):
			positions = data_cleaned[:,i:i+n_average]
			center = np.mean(positions, axis=1)
			self.track.append(center)

	def draw(self, fig, ax):
		ax.plot(*np.array(self.track).transpose(), color='green', lw=2, label='TrackMovingAverage')

class TrackMovingAverageSpline(Track):
	'''Reconstructed by B-splines of moving average.'''
	def reconstruct_track(self, n_average=30):
		data_cleaned = self.reco_data[:,self.clean_indizes] # get cleaned data
		data_sorted = data_cleaned[:,np.argsort(data_cleaned[2])] # sort data by z coordinate

		n_electrons = data_sorted.shape[1]
		mvg_average = []
		for i in range(n_electrons - n_average):
			positions = data_sorted[:,i:i+n_average]
			center = np.mean(positions, axis=1)
			mvg_average.append(center)

		self.tck, u = interpolate.splprep(data_sorted, k=3, s=10.)

	def draw(self, fig, ax):
		ax.plot(*interpolate.splev(np.linspace(0,1,1000), self.tck), color='blue', lw=2, label='TrackMovingAverageSpline')

class TrackAdaptiveMovingAverage(Track):
	'''Reconstructed by an adaptive moving average.'''
	def _rotation_matrix(self, direction, axis=np.array([1,0,0])):
		'''Returns the rotation matrix needed to rotate direction onto given axis (default: x-axis).'''
		if np.allclose(direction,axis) or np.linalg.norm(direction) == 0.: return np.identity(3)
		direction /= np.linalg.norm(direction)

		rotation_axis = np.cross(axis, direction)
		rotation_axis /= np.linalg.norm(rotation_axis)
		angle = np.arccos(np.dot(axis, direction)) # already norm 1

		a = np.cos(angle/2.)
		b, c, d = -rotation_axis*np.sin(angle/2.)

		# Euler-Rodrigues formula
		aa, bb, cc, dd = a*a, b*b, c*c, d*d
		bc, ad, ac, ab, bd, cd = b*c, a*d, a*c, a*b, b*d, c*d
		return np.array([[aa+bb-cc-dd, 2*(bc+ad), 2*(bd-ac)], [2*(bc-ad), aa+cc-bb-dd, 2*(cd+ab)], [2*(bd+ac), 2*(cd-ab), aa+dd-bb-cc]])

	def _directional_argsort(self, data, offset, direction):
		M = self._rotation_matrix(direction)
		data_rot = np.array([np.dot(d-offset, M) for d in data]) # rotate on x-axis
		return list(np.argsort(data_rot[:,0])) # sort for x

	def _distance_argsort(self, data, start_point):
		distances = distance.cdist(np.array([start_point]), data)
		return list(np.argsort(distances[0]))

	def reconstruct_track(self, n_average=30, smoothness=100):
		'''Reconstructs track from z=min to z=max.'''
		data_cleaned = np.array(list(self.reco_data[:,self.clean_indizes])) # get cleaned data
		data_transp = list(data_cleaned.transpose()) # data_transp = [[x0, y0, z0], [x1, y1, z1], ...]

		current_start = data_transp[np.argmax(data_cleaned[2])]
		current_direction = current_start - data_transp[np.argmin(data_cleaned[2])] # approximate the first direction

		'''
		self.track = []
		directions = []
		while len(data_transp) >= n_average: # until all points are used
			directions.append(current_direction)
			#next_points = self._directional_argsort(data_transp, current_start, current_direction)[:n_average]
			next_points = self._distance_argsort(data_transp, current_start)[:n_average]
			print(next_points)

			# calculate weight for all points:
			# 1. calculate distances to last point of track
			# 2. directional sort
			# 3. weight directional sort with distances from 1.
			# 4. select best fitting points

			self.track.append(np.mean([data_transp[n] for n in next_points], axis=0))
			del(data_transp[next_points[0]])

			if len(self.track) >= 2:
				current_start = self.track[-1]
				new_direction = current_start - self.track[-2]
				current_direction = np.average(np.array([new_direction] + directions[-smoothness:]), weights=[.5**i for i in range(len(directions[-smoothness:])+1)], axis=0)
		'''

		self.track = []
		sorted_points = []
		while len(data_transp) >= n_average:
			next_point = self._distance_argsort(data_transp, current_start)[0]
			sorted_points.append(data_transp[next_point])
			del(data_transp[next_point])

		for i in range(len(sorted_points)-n_average):
			self.track.append(np.mean(sorted_points[i:i+n_average], axis=0))

		self.tck, u = interpolate.splprep(np.array(self.track).transpose(), k=5, s=.1)

	def draw(self, fig, ax):
		ax.plot(*np.array(self.track).transpose(), color='green', lw=1, label='TrackAdaptiveMovingAverage')
		spline = np.array(interpolate.splev(np.linspace(0.,1.,1000), self.tck)).transpose()
		ax.plot(*spline.transpose(), color='orange', lw=2, label='TrackDirect')

		tangents = np.array(interpolate.splev(np.linspace(0.,1.,1000), self.tck, der=1)).transpose()
		ax.plot(*np.array([spline[0], spline[0]+0.1*tangents[0]]).transpose())
		#ax.plot(*interpolate.splev(np.linspace(0,1,1000), self.tck_avg), color='green', lw=2, label='TrackAverage')

def main():
	input_file_path = '/localscratch/simulation_files/MicroMegas-Simulation/outfiles/theta0_200keV_100k'
	drift_file_path = os.path.join(input_file_path, 'drift.root')
	avalanche_file_path = os.path.join(input_file_path, 'avalanche.root')

	data = Data(drift_file_path, avalanche_file_path)
	data.reconstructZvalues()
	data.calculate_clean_indizes(cut_value=4.)

	event = 10 # 223 to test for scattering events
	track_spline = TrackSpline(data, event)
	track_spline.reconstruct_track()
	track_mvavrg = TrackMovingAverage(data, event)
	track_mvavrg.reconstruct_track()
	track_mvavrg_spline = TrackMovingAverageSpline(data, event)
	track_mvavrg_spline.reconstruct_track()
	track_amvavrg = TrackAdaptiveMovingAverage(data, event)
	track_amvavrg.reconstruct_track()

	# Drawing
	fig = plt.figure()
	ax = fig.add_subplot(121, projection='3d')
	ax.set_xlabel('x [cm]'); ax.set_ylabel('y [cm]'); ax.set_zlabel('z [cm]')
	plot_area = 1.
	ax.set_xlim([-plot_area,plot_area]); ax.set_ylim([-plot_area,plot_area]); ax.set_zlim([-plot_area+.5,plot_area+.5])
	data.draw(event, fig, ax, plot_area)

	#track_spline.draw(fig, ax)
	#track_mvavrg.draw(fig, ax)
	#track_mvavrg_spline.draw(fig, ax)
	track_amvavrg.draw(fig, ax)

	ax.legend()
	fig.subplots_adjust(left=.02, bottom=.1, right=.98, top=.97, wspace=.27, hspace=.23)
	plt.show()

if __name__ == '__main__':
	main()

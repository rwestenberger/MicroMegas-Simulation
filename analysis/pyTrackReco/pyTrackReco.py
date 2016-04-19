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
	def __init__(self, photoconversion_path, drift_file_path, avalanche_file_path):
		self.photoconversion = root2rec(photoconversion_path, 'coatingTree')
		self.drift = root2rec(drift_file_path, 'driftTree')
		self.avalanche = root2rec(avalanche_file_path, 'avalancheTree')

		# TODO: move primary electron data to self.primary

	def reconstructZvalues(self, drift_velocity=4.627e-3):
		'''Reconstructs z values of given dataset with the given drift_velocity.'''
		# unit drift_velocity: cm/ns, obtained from fit of (z1-z0)/(t1-t0)

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
	def _distance_argsort(self, data, start_point):
		distances = distance.cdist(np.array([start_point]), data)
		return list(np.argsort(distances[0]))

	def reconstruct_track(self, n_average=10, n_average_tangent=10):
		'''Reconstructs track from z=min to z=max.'''
		data_cleaned = np.array(list(self.reco_data[:,self.clean_indizes])) # get cleaned data
		data_transp = list(data_cleaned.transpose()) # data_transp = [[x0, y0, z0], [x1, y1, z1], ...]
		assert len(data_transp) > n_average

		current_start = data_transp[np.argmax(data_cleaned[2])]
		current_direction = current_start - data_transp[np.argmin(data_cleaned[2])] # approximate the first direction

		# sort points by pairwise distance
		sorted_points = []
		while len(data_transp) > 0:
			next_point = self._distance_argsort(data_transp, current_start)[0]
			sorted_points.append(data_transp[next_point])
			del(data_transp[next_point])

		# moving average
		self.track = []
		for i in range(len(sorted_points)-n_average):
			self.track.append(np.mean(sorted_points[i:i+n_average], axis=0))

		# spline
		assert len(self.track) > 5 # m > k for interpolation
		self.tck, u = interpolate.splprep(np.array(self.track).transpose(), k=5, s=.1)

		# calculate tangents
		self.tangent_firstlast = self.track[-1] - self.track[0]
		self.tangent_firstlast /= np.linalg.norm(self.tangent_firstlast)

		tr = np.array(self.track)
		diff = tr[1:] - tr[:-1] # calculate differences
		assert len(diff) >= n_average_tangent
		self.tangent_mvavrg = np.average(diff[:n_average_tangent], axis=0, weights=np.array([1./2**n for n in range(n_average_tangent)]))
		self.tangent_mvavrg /= np.linalg.norm(self.tangent_mvavrg)

		tangents = np.array(interpolate.splev(np.linspace(0.,1.,1000), self.tck, der=1)).transpose()
		self.tangent_spline = tangents[0]
		self.tangent_spline /= np.linalg.norm(self.tangent_spline)

	def draw(self, fig, ax):
		ax.plot(*np.array(self.track).transpose(), color='green', lw=1, label='Track Moving Average')

		spline = np.array(interpolate.splev(np.linspace(0.,1.,1000), self.tck)).transpose()
		ax.plot(*spline.transpose(), color='orange', lw=1, label='TrackSortedMovingAverageSpline')

		ax.plot(*np.array([self.track[0], self.track[0]+0.1*self.tangent_mvavrg]).transpose(), color='green', lw=3, label='Tangent Moving Average')
		ax.plot(*np.array([spline[0], spline[0]+0.1*self.tangent_spline]).transpose(), color='orange', lw=3, label='Tangent Spline')
		#ax.plot(*interpolate.splev(np.linspace(0,1,1000), self.tck_avg), color='green', lw=2, label='TrackAverage')

def angle(v1, v2):
	'''Returns the angle in radians between vectors v1 and v2'''
	cosang = np.dot(v1, v2)
	sinang = np.linalg.norm(np.cross(v1, v2))
	return np.arctan2(sinang, cosang)

def main():
	input_file_path = '/localscratch/simulation_files/MicroMegas-Simulation/outfiles/theta0_200keV_100k'
	photoconversion_file_path = os.path.join(input_file_path, 'photoconversion.root')
	drift_file_path = os.path.join(input_file_path, 'drift.root')
	avalanche_file_path = os.path.join(input_file_path, 'avalanche.root')

	data = Data(photoconversion_file_path, drift_file_path, avalanche_file_path)
	data.reconstructZvalues()
	data.calculate_clean_indizes(cut_value=4.)

	'''
	event = 43 # 223 to test for scattering events
	track = TrackSpline(data, event)
	track.reconstruct_track()

	# Drawing
	fig = plt.figure()
	ax = fig.add_subplot(121, projection='3d')
	ax.set_xlabel('x [cm]'); ax.set_ylabel('y [cm]'); ax.set_zlabel('z [cm]')
	plot_area = 1.
	ax.set_xlim([-plot_area,plot_area]); ax.set_ylim([-plot_area,plot_area]); ax.set_zlim([-plot_area+.5,plot_area+.5])
	data.draw(event, fig, ax, plot_area)

	track.draw(fig, ax)

	ax.legend()
	fig.subplots_adjust(left=.02, bottom=.1, right=.98, top=.97, wspace=.27, hspace=.23)
	plt.show()
	'''

	deviation_firstlast = []
	deviation_mvavrg = []
	deviation_spline = []

	# event loop
	for event in range(len(data.drift)):
		track = TrackSpline(data, event)
		try:
			track.reconstruct_track(n_average=30, n_average_tangent=6)
		except AssertionError:
			print('Skipping event, to little data...')
			continue

		real_direction = np.array([data.photoconversion[event].Px, data.photoconversion[event].Py, data.photoconversion[event].Pz])
		real_direction /= np.linalg.norm(real_direction)
		deviation_firstlast.append(np.degrees(angle(real_direction, track.tangent_firstlast)))
		deviation_mvavrg.append(np.degrees(angle(real_direction, track.tangent_mvavrg)))
		deviation_spline.append(np.degrees(angle(real_direction, track.tangent_spline)))
		print('Event {}: MvgAvrg: {}, Spline: {}'.format(event, deviation_mvavrg[-1], deviation_spline[-1]))

	# plotting
	fig = plt.figure()
	ax = fig.add_subplot()

	hist_firstlast = fig.add_subplot(131)
	hist_firstlast.hist(deviation_firstlast, bins=np.arange(0, 180, 2))
	hist_firstlast.axvline(x=np.mean(deviation_firstlast), ymin=0., ymax=1., linewidth=2, color='r')
	hist_firstlast.set_xlim([0.,180.])
	hist_firstlast.set_title(r'First-Last method: $\mu$={}$^\circ$'.format(round(np.mean(deviation_firstlast), 1)))
	hist_firstlast.grid()

	hist_mvavrg = fig.add_subplot(132)
	hist_mvavrg.hist(deviation_mvavrg, bins=np.arange(0, 180, 2))
	hist_mvavrg.axvline(x=np.mean(deviation_mvavrg), ymin=0., ymax=1., linewidth=2, color='r')
	hist_mvavrg.set_xlim([0.,180.])
	hist_mvavrg.set_title(r'Moving average method: $\mu$={}$^\circ$'.format(round(np.mean(deviation_mvavrg), 1)))
	hist_mvavrg.grid()

	hist_spline = fig.add_subplot(133)
	hist_spline.hist(deviation_spline, bins=np.arange(0, 180, 2))
	hist_spline.axvline(x=np.mean(deviation_spline), ymin=0., ymax=1., linewidth=2, color='r')
	hist_spline.set_xlim([0.,180.])
	hist_spline.set_title(r'Spline method: $\mu$={}$^\circ$'.format(round(np.mean(deviation_spline), 1)))
	hist_spline.grid()

	plt.show()

if __name__ == '__main__':
	main()

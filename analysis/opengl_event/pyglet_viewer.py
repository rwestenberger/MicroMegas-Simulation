#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from pyglet import clock, font, image, window
from pyglet.gl import *

import numpy as np
import math
import ROOT
from root_numpy import tree2array

from utils.trackball_camera import TrackballCamera, norm1

def read_data(input_file_path, event=1, tree_name='driftLineTree'):
	input_file = ROOT.TFile(input_file_path, 'read')
	input_tree = input_file.Get(tree_name)
	data = tree2array(input_tree)
	event_data = data[event-1] # choose event to display
	return event_data

class Hud():
	def __init__(self):
		self.font = font.load('Helvetica', 10)
		self.update_text('Text')
		self.fps = clock.ClockDisplay()

	def update_text(self, text):
		props = dict(x=1, y=1, halign=font.Text.RIGHT, valign=font.Text.BOTTOM, color=(1, 1, 1, 0.5))
		self.text = font.Text(self.font, text)

	def draw(self):
		glMatrixMode(GL_MODELVIEW)
		glPushMatrix()
		glLoadIdentity()
		#gluOrtho2D(-100, 100, -100, 100)
		self.text.draw()
		glScalef(.2,.2,.2)
		self.fps.draw()
		glPopMatrix()

class World():
	def __init__(self, event_data):
		self.bounding_box = [(-1,1), (-1,1), (0,3)]
		self.init_coordinate_system(1)
		self.init_vertex_lists(event_data)

	def init_vertex_lists(self, event_data):
		vertex_lists = []
		for drift_line in range(len(event_data['x_e'])):
			number_of_vertices = len(event_data['x_e'][drift_line])
			vertex_list = pyglet.graphics.vertex_list(number_of_vertices, 'v3f/static')
			vertex_list.vertices = np.hstack(np.array([event_data['x_e'][drift_line], event_data['y_e'][drift_line], event_data['z_e'][drift_line]]).T)
			vertex_lists.append(vertex_list)
		self.vertex_lists = vertex_lists

	def init_coordinate_system(self, num_list):
		marker_len = .02
		step_size = .1

		glNewList(num_list, GL_COMPILE)
		glPushMatrix()
		glBegin(GL_LINES)
		# coordinate system
		glColor4f(0.863,0.196,0.184,.5) #r
		glVertex3f(self.bounding_box[0][0],0,0); glVertex3f(self.bounding_box[0][1],0,0)
		for x in np.arange(self.bounding_box[0][0], self.bounding_box[0][1], step_size):
			glVertex3f(x, -marker_len, 0); glVertex3f(x, marker_len, 0)
			glVertex3f(x, 0, -marker_len); glVertex3f(x, 0, marker_len)

		glColor4f(0.522,0.6,0.,.5) #g
		glVertex3f(0,self.bounding_box[1][0],0); glVertex3f(0,self.bounding_box[1][1],0)
		for y in np.arange(self.bounding_box[1][0], self.bounding_box[1][1], step_size):
			glVertex3f(-marker_len, y, 0); glVertex3f(marker_len, y, 0)
			glVertex3f(0, y, -marker_len); glVertex3f(0, y, marker_len)

		glColor4f(0.149,0.545,0.824,.5) #b
		glVertex3f(0,0,self.bounding_box[2][0]); glVertex3f(0,0,self.bounding_box[2][1])
		for z in np.arange(self.bounding_box[2][0], self.bounding_box[2][1], step_size):
			glVertex3f(-marker_len, 0, z); glVertex3f(marker_len, 0, z)
			glVertex3f(0, -marker_len, z); glVertex3f(0, marker_len, z)

		# x-y grid
		glColor4f(0.576,0.631,0.631,.3)
		for x in np.arange(self.bounding_box[0][0], self.bounding_box[0][1]+step_size, step_size):
			glVertex3f(x, self.bounding_box[1][0], 0); glVertex3f(x, self.bounding_box[1][1], 0)
		for y in np.arange(self.bounding_box[1][0], self.bounding_box[1][1]+step_size, step_size):
			glVertex3f(self.bounding_box[0][0], y, 0); glVertex3f(self.bounding_box[0][1], y, 0)
		glEnd()
		glPopMatrix()
		glEndList()

	def draw(self):
		glMatrixMode(GL_MODELVIEW)
		glPushMatrix()
		glTranslatef(0,0,-1.5)

		glCallList(1) # draw coordinate system

		glColor3f(0,0,0) # draw drift lines
		for vertex_list in self.vertex_lists:
			vertex_list.draw(pyglet.gl.GL_LINE_STRIP)
		glPopMatrix()

class View():
	def __init__(self, world, hud):
		self.camera = TrackballCamera(3.0)
		self.world = world
		self.hud = hud

	def update(self, width, height):
		glViewport(0, 0, width, height)

		glMatrixMode(GL_PROJECTION)
		glLoadIdentity()
		aspect_ratio = width/height
		gluPerspective(40., aspect_ratio, 1, 1000)

		self.camera.update_modelview()

	def draw(self):
		self.world.draw()
		self.hud.draw()

class Window(pyglet.window.Window):
	def __init__(self, width, height, title=''):
		super(Window, self).__init__(width, height, title)
		glClearColor(0.992,0.965,0.890,1.)
		glEnable(GL_DEPTH_TEST)
		self.data = read_data('/localscratch/simulation_files/MicroMegas-Simulation/outfiles/drift_lines.root')
		self.world = World(self.data)
		self.hud = Hud()
		self.view = View(self.world, self.hud)
		#self.push_handlers(pyglet.window.event.WindowEventLogger()) # to show window events

	def on_draw(self):
		self.clear()
		self.view.draw()

	def on_resize(self, width, height):
		self.view.update(width, height)

	def on_mouse_press(self, x, y, button, modifiers):
		if button == window.mouse.MIDDLE and modifiers == window.key.MOD_SHIFT:
			self.view.camera.mouse_zoom(norm1(x, self.width), norm1(y, self.height), dragging=False)
		if button == window.mouse.MIDDLE and not modifiers:
			self.view.camera.mouse_roll(norm1(x, self.width), norm1(y, self.height), dragging=False)

	def on_mouse_drag(self, x, y, dx, dy, button, modifiers):
		if button == window.mouse.MIDDLE and modifiers == window.key.MOD_SHIFT:
			pass
		if button == window.mouse.MIDDLE and not modifiers:
			self.view.camera.mouse_roll(norm1(x, self.width), norm1(y, self.height))

	def on_mouse_scroll(self, x, y, dx, dy):
		self.view.camera.mouse_zoom(0., dy*0.1)

class App():
	def __init__(self):
		self.window = Window(800, 600, 'OpenGL Micromegas Event Viewer')
		clock.set_fps_limit(60)
		pyglet.app.run()

if __name__ == '__main__':
	App()
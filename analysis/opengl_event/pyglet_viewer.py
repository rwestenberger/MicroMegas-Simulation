#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from pyglet import clock, font, image, window
from pyglet.gl import *

import numpy as np

from progressbar import ProgressBar, SimpleProgress, Percentage, Bar

from utils.trackball_camera import TrackballCamera, norm1
from utils import data_io
from utils.solarized import colors

class Hud():
	def __init__(self, view):
		self.view = view
		self.font = font.load('Helvetica', 10)
		self.fps = clock.ClockDisplay(font=self.font, interval=0.2, color=(0,0,0, 1))
		self.update_text()

	def update_text(self, text='Test'):
		props = dict(x=self.view.width-10, y=10, halign=font.Text.RIGHT, valign=font.Text.BOTTOM, color=(0, 0, 0, 0.5))
		self.text = font.Text(self.font, text, **props)

	def draw(self):
		glMatrixMode(GL_MODELVIEW)
		glPushMatrix()
		glLoadIdentity()
		#self.text.draw()
		self.fps.draw()
		glPopMatrix()

class World():
	def __init__(self, input_file_path):
		self.bounding_box = [(-1,1), (-1,1), (0,3)]
		self.init_coordinate_system(1)
		self.init_vertex_lists(input_file_path)

	def init_vertex_lists(self, input_file_path):
		event_data = data_io.read_data(input_file_path, 'driftLineTree', event=1)

		self.vertex_lists = []
		self.start_points = []
		self.end_points = []
		num_drift_lines = len(event_data['x_e'])
		pbar = ProgressBar(widgets=[SimpleProgress(sep='/'), ' ', Percentage(), ' ', Bar(marker='█', left='|', right='|')], maxval=num_drift_lines).start()
		for drift_line in range(num_drift_lines):
			pbar.update(drift_line)
			number_of_vertices = len(event_data['x_e'][drift_line])
			vertex_list = pyglet.graphics.vertex_list(number_of_vertices, 'v3f/static')
			vertex_list.vertices = np.hstack(np.array([event_data['x_e'][drift_line], event_data['y_e'][drift_line], event_data['z_e'][drift_line]]).T)
			self.vertex_lists.append(vertex_list)
			self.start_points.append([event_data['x_e'][drift_line][0], event_data['y_e'][drift_line][0], event_data['z_e'][drift_line][0]])
			self.end_points.append([event_data['x_e'][drift_line][-1], event_data['y_e'][drift_line][-1], event_data['z_e'][drift_line][-1]])
		pbar.finish()

	def init_coordinate_system(self, num_list):
		marker_len = .02
		step_size = .1

		glNewList(num_list, GL_COMPILE)
		glPushMatrix()
		glBegin(GL_LINE_STRIP)
		# bounding box
		glColor3f(*colors['base1'])
		glVertex3f(self.bounding_box[0][0],self.bounding_box[1][0],self.bounding_box[2][1])
		glVertex3f(self.bounding_box[0][1],self.bounding_box[1][0],self.bounding_box[2][1])
		glVertex3f(self.bounding_box[0][1],self.bounding_box[1][1],self.bounding_box[2][1])
		glVertex3f(self.bounding_box[0][0],self.bounding_box[1][1],self.bounding_box[2][1])
		glVertex3f(self.bounding_box[0][0],self.bounding_box[1][0],self.bounding_box[2][1])
		glEnd()

		glBegin(GL_LINES)
		# coordinate system
		glColor3f(*colors['red'])
		glVertex3f(self.bounding_box[0][0],0,0); glVertex3f(self.bounding_box[0][1],0,0)
		for x in np.arange(self.bounding_box[0][0], self.bounding_box[0][1], step_size):
			glVertex3f(x, -marker_len, 0); glVertex3f(x, marker_len, 0)
			glVertex3f(x, 0, -marker_len); glVertex3f(x, 0, marker_len)

		glColor3f(*colors['green'])
		glVertex3f(0,self.bounding_box[1][0],0); glVertex3f(0,self.bounding_box[1][1],0)
		for y in np.arange(self.bounding_box[1][0], self.bounding_box[1][1], step_size):
			glVertex3f(-marker_len, y, 0); glVertex3f(marker_len, y, 0)
			glVertex3f(0, y, -marker_len); glVertex3f(0, y, marker_len)

		glColor3f(*colors['blue'])
		glVertex3f(0,0,self.bounding_box[2][0]); glVertex3f(0,0,self.bounding_box[2][1])
		for z in np.arange(self.bounding_box[2][0], self.bounding_box[2][1], step_size):
			glVertex3f(-marker_len, 0, z); glVertex3f(marker_len, 0, z)
			glVertex3f(0, -marker_len, z); glVertex3f(0, marker_len, z)

		# x-y grid
		glColor3f(*colors['base1'])
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
		
		glPointSize(5)
		glBegin(GL_POINTS)
		glColor3f(*colors['green']) # draw start points
		for start_point in self.start_points: glVertex3f(*start_point)
		glColor3f(*colors['red']) # draw end points
		for end_point in self.end_points: glVertex3f(*end_point)
		glEnd()

		glColor4f(0,0,0,.3) # draw drift lines
		for vertex_list in self.vertex_lists:
			vertex_list.draw(pyglet.gl.GL_LINE_STRIP)

		glPopMatrix()

class View():
	def __init__(self, width, height, world, hud):
		self.width, self.height = width, height
		self.world = world
		self.hud = hud
		self.camera = TrackballCamera(radius=4.)
		self.fov = 60.

	def update(self, width, height):
		self.width, self.height = width, height
		glViewport(0, 0, width, height)
		self.hud.update_text()
		self.camera.update_modelview()

	def world_projection(self):
		glMatrixMode(GL_PROJECTION)
		glLoadIdentity()
		aspect_ratio = self.width/self.height
		gluPerspective(self.fov, aspect_ratio, 0.01, 100)

	def hud_projection(self):
		glMatrixMode(GL_PROJECTION)
		glLoadIdentity()
		gluOrtho2D(0, self.width, 0, self.height)

	def draw(self):
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
		self.world_projection()
		self.world.draw()

		self.hud_projection()
		self.hud.draw()

class Window(pyglet.window.Window):
	def __init__(self, width, height, input_file_path, title=''):
		super(Window, self).__init__(width, height, title)
		self.init_opengl()

		self.world = World(input_file_path)
		self.hud = Hud(self)
		self.view = View(self.width, self.height, self.world, self.hud)
		#self.push_handlers(pyglet.window.event.WindowEventLogger()) # to show window events

	def init_opengl(self):
		#glClearColor(colors['base3'][0], colors['base3'][1], colors['base3'][2], 1.)
		glClearColor(1,1,1,1)
		glEnable(GL_DEPTH_TEST)
		glEnable(GL_BLEND)

	def on_draw(self):
		self.clear()
		self.view.draw()

	def on_resize(self, width, height):
		self.view.update(width, height)

	def on_mouse_press(self, x, y, button, modifiers):
		norm_x, norm_y = norm1(x, self.width), norm1(y, self.height)
		if button == window.mouse.MIDDLE and not modifiers:
			self.view.camera.mouse_roll(norm_x, norm_y, dragging=False)
		if button == window.mouse.MIDDLE and modifiers == window.key.MOD_SHIFT:
			self.view.camera.mouse_move(norm_x, norm_y, dragging=False)
		if button == window.mouse.MIDDLE and modifiers == window.key.MOD_CTRL:
			self.view.camera.mouse_zoom(norm_x, norm_y, dragging=False)

	def on_mouse_drag(self, x, y, dx, dy, button, modifiers):
		norm_x, norm_y = norm1(x, self.width), norm1(y, self.height)
		if button == window.mouse.MIDDLE and not modifiers:
			self.view.camera.mouse_roll(norm_x, norm_y)
		if button == window.mouse.MIDDLE and modifiers == window.key.MOD_SHIFT:
			self.view.camera.mouse_move(norm_x, norm_y)
		if button == window.mouse.MIDDLE and modifiers == window.key.MOD_CTRL:
			self.view.camera.mouse_zoom(norm_x, norm_y)

	def on_mouse_scroll(self, x, y, dx, dy):
		zoom_factor = 1.05
		self.view.fov *= zoom_factor if dy<0 else 1/zoom_factor

class EventViewer():
	def __init__(self, input_file_path):
		self.window = Window(1200, 800, input_file_path, title='OpenGL Micromegas Event Viewer')
		clock.set_fps_limit(60)
		pyglet.app.run()

if __name__ == '__main__':
	EventViewer('/localscratch/simulation_files/MicroMegas-Simulation/outfiles/drift_lines_90_10.root')
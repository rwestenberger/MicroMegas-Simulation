# -*- coding: utf-8 -*-
# Create a wire mesh

import FreeCAD
from PySide import QtGui,QtCore
import Part
import Draft
from math import *

class ParamCurv(QtGui.QWidget):
	def __init__(self):
		super(ParamCurv, self).__init__()
		self.initUI()
		
	def __del__(self):
		return
		
	def initUI(self):
		# Title
		self.title = QtGui.QLabel("<b>Mesh Creator<b>", self)
		self.title.show()
		# Parameters
		self.wire_count_label = QtGui.QLabel("    Wire count ",self)
		self.wire_count_edit = QtGui.QLineEdit(self)
		self.wire_count_edit.setText("4")
		self.mesh_lattice_const_label = QtGui.QLabel(u"    Mesh lattice constant (µm) ",self)
		self.mesh_lattice_const_edit = QtGui.QLineEdit(self)
		self.mesh_lattice_const_edit.setText("62.5")
		self.wire_diameter_label = QtGui.QLabel(u"    Wire diameter (µm) ",self)
		self.wire_diameter_edit = QtGui.QLineEdit(self)
		self.wire_diameter_edit.setText("20")
		# Ok and cancel buttons
		self.button_create = QtGui.QPushButton("Create Mesh",self)
		self.button_exit = QtGui.QPushButton("Close",self)
		# Layout
		layout = QtGui.QGridLayout()
		layout.addWidget(self.wire_count_label, 1, 0)
		layout.addWidget(self.wire_count_edit, 1, 1)
		layout.addWidget(self.mesh_lattice_const_label, 2, 0)
		layout.addWidget(self.mesh_lattice_const_edit, 2, 1)
		layout.addWidget(self.wire_diameter_label, 3, 0)
		layout.addWidget(self.wire_diameter_edit, 3, 1)
		layout.addWidget(self.button_create, 4, 0)
		layout.addWidget(self.button_exit, 4, 1)
		self.setLayout(layout)
		# Connectors
		QtCore.QObject.connect(self.button_create, QtCore.SIGNAL("pressed()"),self.draw_mesh)
		QtCore.QObject.connect(self.button_exit, QtCore.SIGNAL("pressed()"),self.Close)

	def draw_mesh(self):
		try:
			wire_count = int(self.wire_count_edit.text())
			mesh_lattice_const = float(self.mesh_lattice_const_edit.text())*1e-3
			wire_diameter = float(self.wire_diameter_edit.text())*1e-3
		except:
			FreeCAD.Console.PrintError("Error in evaluating the parameters")

		objects = {}
		for direction in ["x", "y"]:
			objects[direction] = []
			for wire_num in range(wire_count):
				FreeCAD.Console.PrintMessage("{}: {}\n".format(direction, wire_num))

				curve, circle, wire = self.draw_wire(wire_diameter, wire_count, mesh_lattice_const)
				objects[direction].append((curve, circle, wire))

				# position objects
				if direction == "x":
					for obj in objects[direction][wire_num]:
						obj.Placement.Position += FreeCAD.Vector(0, wire_num*mesh_lattice_const, 0)
				else:
					for obj in objects[direction][wire_num]:
						obj.Placement.Rotation += FreeCAD.Vector(90, 0, 0)
						obj.Placement.Position += FreeCAD.Vector(wire_num*mesh_lattice_const, 0, 0)

		
	def draw_wire(self, wire_diameter, wire_count, mesh_lattice_const):
		vectors = []
		num_steps = wire_count*10
		for i in range(int(num_steps)+1):
			t = float(i)/float(num_steps) * 2*pi * wire_count/2. + pi/2. # to start at max/min
			vector_x = wire_count * mesh_lattice_const * float(i)/float(num_steps)
			vector_z = sin(t)*wire_diameter/2. # TODO: safety distance
			vectors.append(FreeCAD.Vector(vector_x, 0, vector_z))

		curve = Part.makePolygon(vectors)
		wire_spline = Draft.makeBSpline(curve, closed=False, face=False)
		circle = Draft.makeCircle(radius=wire_diameter/2., face=True, support=None)
		circle.Placement = FreeCAD.Placement(FreeCAD.Vector(0,0,wire_diameter/2.), FreeCAD.Rotation(FreeCAD.Vector(0,1,0), 90))

		obj = FreeCAD.activeDocument().addObject('Part::Feature', 'wire')
		sweep = Part.Wire(wire_spline.Shape).makePipeShell([circle.Shape], True, False)
		obj.Shape = sweep

		FreeCAD.activeDocument().removeObject(wire_spline.Name)
		FreeCAD.activeDocument().removeObject(circle.Name)

		return obj

	def Close(self):
		self.close()
		d.close()

# Run ParamCurv
mw = FreeCADGui.getMainWindow()
d = QtGui.QDockWidget()
d.setWidget(ParamCurv())
d.toggleViewAction().setText("Mesh Creator")
d.setAttribute(QtCore.Qt.WA_DeleteOnClose)
mw.addDockWidget(QtCore. Qt.RightDockWidgetArea, d)

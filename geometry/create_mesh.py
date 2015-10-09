# -*- coding: utf-8 -*-
# Create a wire mesh

import FreeCAD, Part, Draft
from PySide import QtGui,QtCore
from math import *

class ParamCurv(QtGui.QWidget):
	def __init__(self):
		super(ParamCurv, self).__init__()
		self.initUI()
		
	def __del__(self):
		return
		
	def initUI(self):
		# Parameters
		self.label_wire_count = QtGui.QLabel("Wire count ",self)
		self.sb_wire_count = QtGui.QSpinBox(self)
		self.sb_wire_count.setValue(4)
		self.sb_wire_count.setMaximum(1000)
		self.sb_wire_count.setMinimum(1)

		self.cb_create_unit = QtGui.QCheckBox("Create Unit Cell", self)

		self.label_mesh_lattice_const = QtGui.QLabel(u"Mesh lattice constant (µm) ",self)
		self.sb_mesh_lattice_const = QtGui.QDoubleSpinBox(self)
		self.sb_mesh_lattice_const.setValue(62.5)
		self.sb_mesh_lattice_const.setMaximum(1000.)
		self.sb_mesh_lattice_const.setMinimum(.01)

		self.label_wire_diameter = QtGui.QLabel(u"Wire diameter (µm) ",self)
		self.sb_wire_diameter = QtGui.QDoubleSpinBox(self)
		self.sb_wire_diameter.setValue(20.)
		self.sb_wire_diameter.setMaximum(500.)
		self.sb_wire_diameter.setMinimum(.01)

		# Ok and cancel buttons
		self.button_create = QtGui.QPushButton("Create Mesh", self)
		self.button_exit = QtGui.QPushButton("Close", self)

		# Layout
		layout = QtGui.QGridLayout()
		layout.addWidget(self.label_wire_count, 1, 0)
		layout.addWidget(self.sb_wire_count, 1, 1)
		layout.addWidget(self.cb_create_unit, 2, 0)
		layout.addWidget(self.label_mesh_lattice_const, 3, 0)
		layout.addWidget(self.sb_mesh_lattice_const, 3, 1)
		layout.addWidget(self.label_wire_diameter, 4, 0)
		layout.addWidget(self.sb_wire_diameter, 4, 1)
		layout.addWidget(self.button_create, 5, 0)
		layout.addWidget(self.button_exit, 5, 1)
		self.setLayout(layout)
		# Connectors
		QtCore.QObject.connect(self.button_create, QtCore.SIGNAL("pressed()"),self.draw_mesh)
		QtCore.QObject.connect(self.button_exit, QtCore.SIGNAL("pressed()"),self.Close)
		self.cb_create_unit.stateChanged.connect(self.create_unit_state)

	def create_unit_state(self, state):
		if state == QtCore.Qt.Checked:
			self.sb_wire_count.setEnabled(False)
		else:
			self.sb_wire_count.setEnabled(True)

	def draw_mesh(self):
		try:
			wire_count = 4 if self.cb_create_unit.isChecked() else int(self.sb_wire_count.text())
			mesh_lattice_const = float(self.sb_mesh_lattice_const.text())*1e-3
			wire_diameter = float(self.sb_wire_diameter.text())*1e-3
			if wire_diameter > mesh_lattice_const/2.:
				FreeCAD.Console.PrintError("Wire radius must be smaller than mesh lattice constant!")
		except:
			FreeCAD.Console.PrintError("Error in evaluating the parameters")

		safety_distance = 1e-3

		wires = {}
		for direction in ["x", "y"]:
			wires[direction] = []
			for wire_num in range(wire_count+1):
				wire = self.draw_wire(wire_diameter, wire_count, mesh_lattice_const, safety_distance=safety_distance)

				# position wire
				if direction == "x":
					wire.Placement.Base += FreeCAD.Vector(0, wire_num*mesh_lattice_const, 0)
					if wire_num%2:
						wire.Placement.Rotation = FreeCAD.Rotation(FreeCAD.Vector(1,0,0), 180)
						wire.Placement.Base += FreeCAD.Vector(0, 0, safety_distance/2.)
				else:
					wire.Placement.Rotation = FreeCAD.Rotation(FreeCAD.Vector(0,0,1), 90)
					wire.Placement.Base += FreeCAD.Vector(wire_num*mesh_lattice_const, 0, -safety_distance/2.)
					if not wire_num%2:
						wire.Placement.Rotation = wire.Placement.Rotation.multiply(FreeCAD.Rotation(FreeCAD.Vector(1,0,0), 180))

				wires[direction].append(wire)

		mesh = FreeCAD.activeDocument().addObject("Part::MultiFuse","Mesh")
		mesh.Shapes = wires["x"] + wires["y"]
		FreeCAD.activeDocument().recompute()

		if self.cb_create_unit.isChecked():
			unit_cell_bounding_box = FreeCAD.activeDocument().addObject("Part::Box","Unit Cell Bounding Box")
			unit_cell_bounding_box.Length = mesh_lattice_const*2.
			unit_cell_bounding_box.Width = mesh_lattice_const*2.
			unit_cell_bounding_box.Height = mesh_lattice_const*2.
			unit_cell_bounding_box.Placement.Base = FreeCAD.Vector(mesh_lattice_const, mesh_lattice_const, -mesh_lattice_const)

			unit_cell = FreeCAD.activeDocument().addObject("Part::MultiCommon", "Unit Cell")
			unit_cell.Shapes = [unit_cell_bounding_box, mesh]
			mesh.Visibility = False

			FreeCAD.activeDocument().recompute()

		
	def draw_wire(self, wire_diameter, wire_count, mesh_lattice_const, safety_distance=0.1e-3):
		vectors = []
		num_steps = wire_count*10
		for i in range(int(num_steps)+1):
			t = float(i)/float(num_steps) * 2*pi * wire_count/2. + pi/2. # to start at max/min
			vector_x = wire_count * mesh_lattice_const * float(i)/float(num_steps)
			vector_z = sin(t)*(wire_diameter/2. + safety_distance)
			vectors.append(FreeCAD.Vector(vector_x, 0, vector_z))

		curve = Part.makePolygon(vectors)
		wire_spline = Draft.makeBSpline(curve, closed=False, face=False)
		circle = Draft.makeCircle(radius=wire_diameter/2., face=False, support=None)
		circle.Placement = FreeCAD.Placement(FreeCAD.Vector(0,0,wire_diameter/2.), FreeCAD.Rotation(FreeCAD.Vector(0,1,0), 90))

		wire = FreeCAD.activeDocument().addObject('Part::Feature', 'wire')
		sweep = Part.Wire(wire_spline.Shape).makePipeShell([circle.Shape], True, False)
		wire.Shape = sweep

		FreeCAD.activeDocument().removeObject(wire_spline.Name)
		FreeCAD.activeDocument().removeObject(circle.Name)

		return wire

	def Close(self):
		self.close()
		d.close()

# Run ParamCurv
mw = FreeCADGui.getMainWindow()
d = QtGui.QDockWidget()
d.setWidget(ParamCurv())
d.toggleViewAction().setText("Mesh Creator")
d.setAttribute(QtCore.Qt.WA_DeleteOnClose)
d.setWindowTitle("Mesh Creator")
mw.addDockWidget(QtCore. Qt.RightDockWidgetArea, d)

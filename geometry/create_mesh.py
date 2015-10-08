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
		FreeCAD.Console.PrintMessage("Hello World!")
		print("test")
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
		QtCore.QObject.connect(self.button_create, QtCore.SIGNAL("pressed()"),self.DrawCurve)
		QtCore.QObject.connect(self.button_exit, QtCore.SIGNAL("pressed()"),self.Close)

	def DrawCurve(self):
		try:
			wire_count = int(self.wire_count_edit.text())
			mesh_lattice_const = float(self.mesh_lattice_const_edit.text())
			wire_diameter = float(self.wire_diameter_edit.text())
		except:
			FreeCAD.Console.PrintError("Error in evaluating the parameters")

		try:
			for wire in range(wire_count):
				# draw wire
				vectors = []
				num_steps = wire_count*4
				sign = 1 if wire % 2 else -1 # to mirror every second wire

				for i in range(int(num_steps)+1):
					t = float(i)/float(num_steps) * wire_count * 2*pi + pi/2. # to start at max

					vector_x = wire_count * mesh_lattice_const * float(i)/float(num_steps)
					vector_y = wire*mesh_lattice_const
					vector_z = sign*sin(t)*wire_diameter/2. # TODO: safety distance

					vectors.append(FreeCAD.Vector(vector_x,vector_y,vector_z))

				curve = Part.makePolygon(vectors)
				Draft.makeBSpline(curve, closed=False, face=False)

				# draw circle
				pl = FreeCAD.Placement()
				pl.Rotation = FreeCAD.Rotation(FreeCAD.Vector(0,1,0), 90)
				pl.Base = FreeCAD.Vector(vectors[0])
				Draft.makeCircle(radius=wire_diameter/2., placement=pl, face=True, support=None)

				# sweep
				#App.getDocument('Unnamed').ActiveObject.Sections=[App.getDocument('Unnamed').Circle, ]
				#App.getDocument('Unnamed').ActiveObject.Spine=(App.ActiveDocument.BSpline,["Edge1"])
				#App.getDocument('Unnamed').ActiveObject.Solid=True
				#App.getDocument('Unnamed').ActiveObject.Frenet=False
		except:
			FreeCAD.Console.PrintError("Error while creating the mesh")
			
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



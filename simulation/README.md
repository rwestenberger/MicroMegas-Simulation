# Simulation
Dependencies: FreeCAD, Elmer, gmsh, Garfield++, Geant4, ROOT

## Photoconversion simulation procedure
Assuming you are in the photoconversion simulation directory:

1. Create build directory:

	`mkdir build && cd build`

2. Configure Geant4 build:

	`cmake -DCMAKE_INSTALL_PREFIX=.. ..`

3. Build Geant4 simulation:

	`make && make install`

4. Run simulation interactively or with macro file:

	`../bin/photoconversion [file.mac]`

5. If run interactively you might want to init visualisation:

	`/control/execute vis.mac`

## Drift simulation procedure
Assuming you are in the drift simulation directory:

1. Build Garfield++ executable:

	`make`
2. Run simulation:

	`./drift`

## Avalanche simulation procedure
Assuming you are in the avalanche simulation directory:

1. Export FreeCAD model (geometry/geometry.fcstd) to .step file.

2. Mesh .step file to .msh file with gmsh:

	`gmsh geometry.step -3 -order 2 -clmax 0.02`
3. Call ElmerGrid on the .msh file:

	`ElmerGrid 14 2 geometry.msh -autoclean`
4. Call ElmerSolver to generate electric field and weighting field:

	`ElmerSolver calculate_field.sif && ElmerSolver calculate_field_weight.sif`
5. Build Garfield++ executable: 

	`make`
6. Run simulation:

	`./avalanche`

## More information

* Avalanche step 2: clmax option can be adapted as needed. Note that to large clmax will create intersecting surfaces.

* Avalanche step 3: autoclean option is important to avoid possible segmentation faults later:

	```
	Program received signal SIGSEGV: Segmentation fault - invalid memory reference.

	Backtrace for this error:
	#0  0x7FF90DA2A407
	#1  0x7FF90DA2AA1E
	#2  0x7FF90BEBC17F
	#3  0x7FF90E378AB8
	#4  0x7FF90E38B7D8
	#5  0x7FF90E3A94B7
	#6  0x7FF90E226100
	#7  0x7FF90E49C414
	#8  0x400F7E in solver at Solver.F90:69
	Segmentation fault
	```

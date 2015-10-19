# MicroMegas-Simulation
Dependencies: FreeCAD, Elmer, Garfield++, ROOT

## Build procedure
Assuming you are in the project root directory:

1. Export FreeCAD model to .step file.

2. Mesh .step file to .msh file with gmsh:

	`gmsh geometry.step -3 -order 2 -clmax 0.02`
3. Call ElmerGrid on the .msh file:

	`ElmerGrid 14 2 geometry.msh -autoclean`
4. Call ElmerSolver to generate electric field and weighting field:

	`ElmerSolver calculate_field.sif && ElmerSolver calculate_field_weight.sif`
5. Build Garfield++ executable: 

	`make`
6. Run simulation:

	`./simulation`

## More information

2. clmax option can be adapted as needed. Note that to large clmax will create intersecting surfaces.

3. autoclean option is important to avoid possible segmentation faults later.
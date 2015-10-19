# MicroMegas-Simulation
Dependencies: FreeCAD, Elmer, Garfield++, ROOT

## Build procedure
Assuming you are in the project root directory:

1. Export FreeCAD model to .step file.

2. Import .step file to ElmerGUI with nglib.

3. Call ElmerSolver to generate electric field and weighting field:

	`ElmerSolver calculate_field.sif && ElmerSolver calculate_field_weight.sif`
4. Build Garfield++ executable: 

	`make`
5. Run simulation:

	`./simulation`

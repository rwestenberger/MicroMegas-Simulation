# MicroMegas-Simulation
Dependencies: ROOT, Garfield++, Elmer, gmsh

## Build procedure
Assuming you are in the project root directory:

1. Generate .mesh from .geo file:

	`gmsh geometry/elementaryCell.geo -3 -order 2`
2. Convert .mesh for Elmer:

	`ElmerGrid 14 2 geometry/elementaryCell.msh -autoclean`
3. Call ElmerSolver to generate electric field and weighting field:

	`ElmerSolver geometry/elementaryCell.sif && ElmerSolver geometry/elementaryCell_weight.sif`
4. Build Garfield++ executable: 

	`make`
5. Run simulation:

	`./avalanche`

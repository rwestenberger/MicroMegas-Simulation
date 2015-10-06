
// ---------------------------------------------------------------------
// Parameters  units: cm
// ---------------------------------------------------------------------
sW = 0.02;			// strip width
sD = 0.005;			// strip distance
hC = 0.5;			// heigth of cell	
hS = 0.0017;			// heigth of strip
hMesh = 0.0117;			// z-position of the mesh
hPCB = 0.01;			// heigth of PCB
d = sW+sD;			// depth of cell
lcW = 0.007;
lcM = 0.003;			// wide mesh element size 0.0075
lcF = 0.003;			// fine mesh element size
lcMesh =  0.0004;		// mesh elemt size at the mesh s 0.0001
rMesh = 0.001;			// radius of the wire of the mesh 
s = 0.001;			// distance between two layers of the mesh 
//lpmm = 400;			// lines of the mesh per mm
linesX = 4;			// number of lines in x-direction
linesY = 4;			// number of lines in y-direction


//check if number of lines is even (necessary for periodic boundary)
If((-1)^linesX<0)
	linesX++;
	Printf("Number of lines in x-direction set to even number %g", linesX);
EndIf

If((-1)^linesY<0)
	linesY++;
	Printf("Number of lines in y-direction set to even number %g", linesY);
EndIf

lX = (sW+sD)/linesX;		// line distance in x
lY = d/linesY;			// line distance in y


// +++++++++++++++++++++++++++++++
// create Mesh
// +++++++++++++++++++++++++++++++
index_X0_up = 1;
index_X0_down = 1;
index_XN_up = 1;
index_XN_down = 1;
index_Y0_up = 1;
index_Y0_down = 1;
index_YN_up = 1;
index_YN_down = 1;



//--------------------------------
// wires parallel to x
//--------------------------------

pm = -1;
i=0;
//create first half wire at y=0;
c1center = newp; Point(c1center) = {0, i*lY, hMesh+pm*(rMesh+s/2), lcMesh};
c1_2 = newp; Point(c1_2) = {0, i*lY, hMesh+rMesh+pm*(rMesh+s/2), lcMesh};
c1_3 = newp; Point(c1_3) = {0, i*lY+rMesh, hMesh+pm*(rMesh+s/2), lcMesh};
c1_4 = newp; Point(c1_4) = {0, i*lY, hMesh-rMesh+pm*(rMesh+s/2), lcMesh};
pm = pm*(-1);

circle1_2 = newl; Circle(circle1_2) = {c1_2,c1center,c1_3};
circle1_3 = newl; Circle(circle1_3) = {c1_3,c1center,c1_4};
border_line_1 = newl; Line(border_line_1) = {c1_4,c1_2};

spline_2_points[0] = c1_2;
spline_3_points[0] = c1_3;
spline_4_points[0] = c1_4;	
	
//create splines for the bend wires of the mesh 
For j In {1:linesX-1}
	point_s_2 = newp; Point(point_s_2) = {j*lX, i*lY, hMesh+rMesh+pm*(rMesh+s/2), lcMesh}; 	spline_2_points[j] = point_s_2;
	point_s_3 = newp; Point(point_s_3) = {j*lX, i*lY+rMesh, hMesh+pm*(rMesh+s/2), lcMesh}; 	spline_3_points[j] = point_s_3;
	point_s_4 = newp; Point(point_s_4) = {j*lX, i*lY, hMesh-rMesh+pm*(rMesh+s/2), lcMesh}; 	spline_4_points[j] = point_s_4;
	pm = pm*(-1);	
EndFor

c2center = newp; Point(c2center) = {sW+sD, i*lY, hMesh+pm*(rMesh+s/2), lcMesh};
c2_2 = newp; Point(c2_2) = {sW+sD, i*lY, hMesh+rMesh+pm*(rMesh+s/2), lcMesh};
c2_3 = newp; Point(c2_3) = {sW+sD, i*lY+rMesh, hMesh+pm*(rMesh+s/2), lcMesh};
c2_4 = newp; Point(c2_4) = {sW+sD, i*lY, hMesh-rMesh+pm*(rMesh+s/2), lcMesh};
pm = pm*(-1);
	
circle2_2 = newl; Circle(circle2_2) = {c2_2,c2center,c2_3};
circle2_3 = newl; Circle(circle2_3) = {c2_3,c2center,c2_4};
border_line_2 = newl; Line(border_line_2) = {c2_4,c2_2};
	
spline_2_points[linesX] = c2_2;
spline_3_points[linesX] = c2_3;
spline_4_points[linesX] = c2_4;

spline_2 = newl; Spline(spline_2) = {spline_2_points[]};
spline_3 = newl; Spline(spline_3) = {spline_3_points[]};
spline_4 = newl; Spline(spline_4) = {spline_4_points[]};

axial_surface_2_loop = newll; Line Loop(axial_surface_2_loop) = {spline_2,circle2_2,-spline_3,-circle1_2};
axial_surface_3_loop = newll; Line Loop(axial_surface_3_loop) = {spline_3,circle2_3,-spline_4,-circle1_3};
axial_surface_border_loop = newll; Line Loop(axial_surface_border_loop) = {spline_4,border_line_2,-spline_2,-border_line_1};
end_surface_0_loop = newll; Line Loop(end_surface_0_loop) = {circle1_2, circle1_3,border_line_1};
end_surface_N_loop = newll; Line Loop(end_surface_N_loop) = {circle2_2, circle2_3,border_line_2};
	
axial_surface_2 = news; Ruled Surface(axial_surface_2) = {axial_surface_2_loop};
axial_surface_3 = news; Ruled Surface(axial_surface_3) = {axial_surface_3_loop};
axial_surface_border = news; Ruled Surface(axial_surface_border) = {axial_surface_border_loop};
end_surface_0 = news; Ruled Surface(end_surface_0) = {end_surface_0_loop};
end_surface_N = news; Ruled Surface(end_surface_N) = {end_surface_N_loop};

surfaceLoop_i = newsl; Surface Loop(surfaceLoop_i) = {axial_surface_2,axial_surface_3,axial_surface_border,end_surface_0,end_surface_N};
volume_wire = newc; Volume(volume_wire) = {surfaceLoop_i};



//Fill lists for later use
border_mesh_Y0[linesX+1] = axial_surface_border;
border_mesh_X0[0] = end_surface_0;
border_mesh_XN[0] = end_surface_N;
Y0_axial_surface_1 = axial_surface_2;
Y0_axial_surface_2 = axial_surface_3;
Y0_spline_1 = spline_2;
Y0_spline_2 = spline_4;
X0_circle_Y0_1 = circle1_2;
X0_circle_Y0_2 = circle1_3;
XN_circle_Y0_1 = circle2_2;
XN_circle_Y0_2 = circle2_3;
X0Y0_3 = c1_2;
X0Y0_4 = c1_4;
XNY0_3 = c2_2;
XNY0_4 = c2_4;
wire_volumes[0] = volume_wire;
//wire_surface[0] = axial_surface_border;
//wire_surface[1] = end_surface_0;
//wire_surface[2] = end_surface_N;
wire_surface[0] = axial_surface_2;
wire_surface[1] = axial_surface_3;
//loop for full wires in the middle of the cell
For i In {1:linesY-1}

	//create circle on begin and end of wire (alway 4 parts)
	c1center = newp; Point(c1center) = {0, i*lY, hMesh+pm*(rMesh+s/2), lcMesh};
	c1_1 = newp; Point(c1_1) = {0, i*lY-rMesh, hMesh+pm*(rMesh+s/2), lcMesh};
	c1_2 = newp; Point(c1_2) = {0, i*lY, hMesh+rMesh+pm*(rMesh+s/2), lcMesh};
	c1_3 = newp; Point(c1_3) = {0, i*lY+rMesh, hMesh+pm*(rMesh+s/2), lcMesh};
	c1_4 = newp; Point(c1_4) = {0, i*lY, hMesh-rMesh+pm*(rMesh+s/2), lcMesh};
	pm = pm*(-1);
	pos_end = pm;
	
	circle1_1 = newl; Circle(circle1_1) = {c1_1,c1center,c1_2};
	circle1_2 = newl; Circle(circle1_2) = {c1_2,c1center,c1_3};
	circle1_3 = newl; Circle(circle1_3) = {c1_3,c1center,c1_4};
	circle1_4 = newl; Circle(circle1_4) = {c1_4,c1center,c1_1};
	
	spline_1_points[0] = c1_1;
	spline_2_points[0] = c1_2;
	spline_3_points[0] = c1_3;
	spline_4_points[0] = c1_4;	
	
	//create splines for the bend wires of the mesh 
	For j In {1:linesX-1}
		point_s_1 = newp; Point(point_s_1) = {j*lX, i*lY-rMesh, hMesh+pm*(rMesh+s/2), lcMesh}; 	spline_1_points[j] = point_s_1;
		point_s_2 = newp; Point(point_s_2) = {j*lX, i*lY, hMesh+rMesh+pm*(rMesh+s/2), lcMesh}; 	spline_2_points[j] = point_s_2;
		point_s_3 = newp; Point(point_s_3) = {j*lX, i*lY+rMesh, hMesh+pm*(rMesh+s/2), lcMesh}; 	spline_3_points[j] = point_s_3;
		point_s_4 = newp; Point(point_s_4) = {j*lX, i*lY, hMesh-rMesh+pm*(rMesh+s/2), lcMesh}; 	spline_4_points[j] = point_s_4;
		pm = pm*(-1);	
	EndFor

	c2center = newp; Point(c2center) = {sW+sD, i*lY, hMesh+pm*(rMesh+s/2), lcMesh};
	c2_1 = newp; Point(c2_1) = {sW+sD, i*lY-rMesh, hMesh+pm*(rMesh+s/2), lcMesh};
	c2_2 = newp; Point(c2_2) = {sW+sD, i*lY, hMesh+rMesh+pm*(rMesh+s/2), lcMesh};
	c2_3 = newp; Point(c2_3) = {sW+sD, i*lY+rMesh, hMesh+pm*(rMesh+s/2), lcMesh};
	c2_4 = newp; Point(c2_4) = {sW+sD, i*lY, hMesh-rMesh+pm*(rMesh+s/2), lcMesh};
	pm = pm*(-1);
	
	circle2_1 = newl; Circle(circle2_1) = {c2_1,c2center,c2_2};
	circle2_2 = newl; Circle(circle2_2) = {c2_2,c2center,c2_3};
	circle2_3 = newl; Circle(circle2_3) = {c2_3,c2center,c2_4};
	circle2_4 = newl; Circle(circle2_4) = {c2_4,c2center,c2_1};
	
	spline_1_points[linesX] = c2_1;
	spline_2_points[linesX] = c2_2;
	spline_3_points[linesX] = c2_3;
	spline_4_points[linesX] = c2_4;

	spline_1 = newl; Spline(spline_1) = {spline_1_points[]};
	spline_2 = newl; Spline(spline_2) = {spline_2_points[]};
	spline_3 = newl; Spline(spline_3) = {spline_3_points[]};
	spline_4 = newl; Spline(spline_4) = {spline_4_points[]};

	axial_surface_1_loop = newll; Line Loop(axial_surface_1_loop) = {spline_1,circle2_1,-spline_2,-circle1_1};
	axial_surface_2_loop = newll; Line Loop(axial_surface_2_loop) = {spline_2,circle2_2,-spline_3,-circle1_2};
	axial_surface_3_loop = newll; Line Loop(axial_surface_3_loop) = {spline_3,circle2_3,-spline_4,-circle1_3};
	axial_surface_4_loop = newll; Line Loop(axial_surface_4_loop) = {spline_4,circle2_4,-spline_1,-circle1_4};
	end_surface_0_loop = newll; Line Loop(end_surface_0_loop) = {circle1_1,circle1_2, circle1_3,circle1_4};
	end_surface_N_loop = newll; Line Loop(end_surface_N_loop) = {-circle2_1,-circle2_4, -circle2_3,-circle2_2};
	
	axial_surface_1 = news; Ruled Surface(axial_surface_1) = {axial_surface_1_loop};
	axial_surface_2 = news; Ruled Surface(axial_surface_2) = {axial_surface_2_loop};
	axial_surface_3 = news; Ruled Surface(axial_surface_3) = {axial_surface_3_loop};
	axial_surface_4 = news; Ruled Surface(axial_surface_4) = {axial_surface_4_loop};
	end_surface_0 = news; Ruled Surface(end_surface_0) = {end_surface_0_loop};
	end_surface_N = news; Ruled Surface(end_surface_N) = {end_surface_N_loop};
	
	surfaceLoop_i = newsl; Surface Loop(surfaceLoop_i) = {axial_surface_1, axial_surface_2, axial_surface_3, axial_surface_4, end_surface_0, end_surface_N};
	volume_wire = newc; Volume(volume_wire) = {surfaceLoop_i};

	//Fill lists for later use
	border_mesh_X0[i] = end_surface_0;
	border_mesh_XN[i] = end_surface_N;
	//surface_gas_X0[i] = end_surface_0_loop;
	//surface_gas_XN[i] = end_surface_N_loop;
	If(pos_end>0)
		surface_gas_X0_down[index_X0_down] = end_surface_0_loop;		//change i (now: empty indices)
		surface_gas_XN_down[index_XN_down] = end_surface_N_loop;
		index_X0_down ++;
		index_XN_down ++;
	EndIf
	If(pos_end<0)
		surface_gas_X0_up[index_X0_up] = end_surface_0_loop;
		surface_gas_XN_up[index_XN_up] = end_surface_N_loop;
		index_X0_up ++;
		index_XN_up ++;
	EndIf
	axialSurfaceLoop = newsl; Surface Loop(axialSurfaceLoop) = {axial_surface_1, axial_surface_2, axial_surface_3, axial_surface_4};
	wire_surface_loop[i] = axialSurfaceLoop;
	wire_volumes[i] = volume_wire;	

	wire_surface[4*i-2] = axial_surface_1;
	wire_surface[4*i-1] = axial_surface_2;
	wire_surface[4*i] = axial_surface_3;
	wire_surface[4*i+1] = axial_surface_4;
	//wire_surface[4*i+3] = end_surface_0;
	//wire_surface[4*i+4] = end_surface_N;
	
EndFor

//create first half wire at y=N;
i=linesY;	
c1center = newp; Point(c1center) = {0, i*lY, hMesh+pm*(rMesh+s/2), lcMesh};
c1_1 = newp; Point(c1_1) = {0, i*lY-rMesh, hMesh+pm*(rMesh+s/2), lcMesh};
c1_2 = newp; Point(c1_2) = {0, i*lY, hMesh+rMesh+pm*(rMesh+s/2), lcMesh};
c1_4 = newp; Point(c1_4) = {0, i*lY, hMesh-rMesh+pm*(rMesh+s/2), lcMesh};
pm = pm*(-1);
pos_end = pm;
	
circle1_1 = newl; Circle(circle1_1) = {c1_1,c1center,c1_2};
circle1_4 = newl; Circle(circle1_4) = {c1_4,c1center,c1_1};
border_line_1 = newl; Line(border_line_1) =  {c1_4,c1_2};
	
spline_1_points[0] = c1_1;
spline_2_points[0] = c1_2;
spline_4_points[0] = c1_4;	
	
	//create splines for the bend wires of the mesh 
For j In {1:linesX-1}
	point_s_1 = newp; Point(point_s_1) = {j*lX, i*lY-rMesh, hMesh+pm*(rMesh+s/2), lcMesh}; 	spline_1_points[j] = point_s_1;
	point_s_2 = newp; Point(point_s_2) = {j*lX, i*lY, hMesh+rMesh+pm*(rMesh+s/2), lcMesh}; 	spline_2_points[j] = point_s_2;
	point_s_4 = newp; Point(point_s_4) = {j*lX, i*lY, hMesh-rMesh+pm*(rMesh+s/2), lcMesh}; 	spline_4_points[j] = point_s_4;
	pm = pm*(-1);	
EndFor

c2center = newp; Point(c2center) = {sW+sD, i*lY, hMesh+pm*(rMesh+s/2), lcMesh};
c2_1 = newp; Point(c2_1) = {sW+sD, i*lY-rMesh, hMesh+pm*(rMesh+s/2), lcMesh};
c2_2 = newp; Point(c2_2) = {sW+sD, i*lY, hMesh+rMesh+pm*(rMesh+s/2), lcMesh};
c2_4 = newp; Point(c2_4) = {sW+sD, i*lY, hMesh-rMesh+pm*(rMesh+s/2), lcMesh};
pm = pm*(-1);
	
circle2_1 = newl; Circle(circle2_1) = {c2_1,c2center,c2_2};
circle2_4 = newl; Circle(circle2_4) = {c2_4,c2center,c2_1};
border_line_2 = newl; Line(border_line_2) =  {c2_4,c2_2};
	
spline_1_points[linesX] = c2_1;
spline_2_points[linesX] = c2_2;
spline_4_points[linesX] = c2_4;

spline_1 = newl; Spline(spline_1) = {spline_1_points[]};
spline_2 = newl; Spline(spline_2) = {spline_2_points[]};
spline_4 = newl; Spline(spline_4) = {spline_4_points[]};

axial_surface_1_loop = newll; Line Loop(axial_surface_1_loop) = {spline_1,circle2_1,-spline_2,-circle1_1};
axial_surface_4_loop = newll; Line Loop(axial_surface_4_loop) = {spline_4,circle2_4,-spline_1,-circle1_4};
axial_surface_border_loop =newll; Line Loop(axial_surface_border_loop) = {spline_4, border_line_2, -spline_2, -border_line_1};
end_surface_0_loop = newll; Line Loop(end_surface_0_loop) = {circle1_1,-border_line_1,circle1_4};
end_surface_N_loop = newll; Line Loop(end_surface_N_loop) = {-circle2_1,-circle2_4, border_line_2};
	
axial_surface_1 = news; Ruled Surface(axial_surface_1) = {axial_surface_1_loop};
axial_surface_4 = news; Ruled Surface(axial_surface_4) = {axial_surface_4_loop};
axial_surface_border = news; Ruled Surface(axial_surface_border) = {axial_surface_border_loop};
end_surface_0 = news; Ruled Surface(end_surface_0) = {end_surface_0_loop};
end_surface_N = news; Ruled Surface(end_surface_N) = {end_surface_N_loop};

surfaceLoop_i = newsl; Surface Loop(surfaceLoop_i) = {axial_surface_1,axial_surface_border,axial_surface_4,end_surface_0,end_surface_N};
volume_wire = newc; Volume(volume_wire) = {surfaceLoop_i};



//Fill lists for later use
border_mesh_YN[linesX+1] = axial_surface_border;
border_mesh_X0[linesY] = end_surface_0;
border_mesh_XN[linesY] = end_surface_N;
YN_axial_surface_1 = axial_surface_1;
YN_axial_surface_2 = axial_surface_4;
YN_spline_1 = spline_2;
YN_spline_2 = spline_4;
X0_circle_YN_1 = circle1_1;
X0_circle_YN_2 = circle1_4;
XN_circle_YN_1 = circle2_1;
XN_circle_YN_2 = circle2_4;
X0YN_3 = c1_2;
X0YN_4 = c1_4;
XNYN_3 = c2_2;
XNYN_4 = c2_4;
wire_volumes[linesY] = volume_wire;
//wire_surface[6*linesY-1] = axial_surface_border;
//wire_surface[6*linesY] = end_surface_0;
//wire_surface[6*linesY+1] = end_surface_N;
wire_surface[4*linesY-2] = axial_surface_1;
wire_surface[4*linesY-1] = axial_surface_4;

//--------------------------------
// wires parallel to y
//--------------------------------

pm = 1;
i=0;
//create first half wire at x=0;
c1center = newp; Point(c1center) = {i*lX, 0,hMesh+pm*(rMesh+s/2), lcMesh};
c1_2 = newp; Point(c1_2) = {i*lX, 0, hMesh+rMesh+pm*(rMesh+s/2), lcMesh};
c1_3 = newp; Point(c1_3) = {i*lX+rMesh, 0, hMesh+pm*(rMesh+s/2), lcMesh};
c1_4 = newp; Point(c1_4) = {i*lX, 0, hMesh-rMesh+pm*(rMesh+s/2), lcMesh};
pm = pm*(-1);

circle1_2 = newl; Circle(circle1_2) = {c1_2,c1center,c1_3};
circle1_3 = newl; Circle(circle1_3) = {c1_3,c1center,c1_4};
border_line_1 = newl; Line(border_line_1) = {c1_4,c1_2};

spline_2_points[0] = c1_2;
spline_3_points[0] = c1_3;
spline_4_points[0] = c1_4;	
	
//create splines for the bend wires of the mesh 
For j In {1:linesY-1}
	point_s_2 = newp; Point(point_s_2) = {i*lX, j*lY, hMesh+rMesh+pm*(rMesh+s/2), lcMesh}; 	spline_2_points[j] = point_s_2;
	point_s_3 = newp; Point(point_s_3) = {i*lX+rMesh, j*lY, hMesh+pm*(rMesh+s/2), lcMesh}; 	spline_3_points[j] = point_s_3;
	point_s_4 = newp; Point(point_s_4) = {i*lX, j*lY, hMesh-rMesh+pm*(rMesh+s/2), lcMesh}; 	spline_4_points[j] = point_s_4;
	pm = pm*(-1);	
EndFor

c2center = newp; Point(c2center) = {i*lX, d, hMesh+pm*(rMesh+s/2), lcMesh};
c2_2 = newp; Point(c2_2) = {i*lX, d, hMesh+rMesh+pm*(rMesh+s/2), lcMesh};
c2_3 = newp; Point(c2_3) = {i*lX+rMesh, d, hMesh+pm*(rMesh+s/2), lcMesh};
c2_4 = newp; Point(c2_4) = {i*lX, d, hMesh-rMesh+pm*(rMesh+s/2), lcMesh};
pm = pm*(-1);
	
circle2_2 = newl; Circle(circle2_2) = {c2_2,c2center,c2_3};
circle2_3 = newl; Circle(circle2_3) = {c2_3,c2center,c2_4};
border_line_2 = newl; Line(border_line_2) = {c2_4,c2_2};
	
spline_2_points[linesX] = c2_2;
spline_3_points[linesX] = c2_3;
spline_4_points[linesX] = c2_4;

spline_2 = newl; Spline(spline_2) = {spline_2_points[]};
spline_3 = newl; Spline(spline_3) = {spline_3_points[]};
spline_4 = newl; Spline(spline_4) = {spline_4_points[]};

axial_surface_2_loop = newll; Line Loop(axial_surface_2_loop) = {spline_2,circle2_2,-spline_3,-circle1_2};
axial_surface_3_loop = newll; Line Loop(axial_surface_3_loop) = {spline_3,circle2_3,-spline_4,-circle1_3};
axial_surface_border_loop = newll; Line Loop(axial_surface_border_loop) = {spline_4,border_line_2,-spline_2,-border_line_1};
end_surface_0_loop = newll; Line Loop(end_surface_0_loop) = {circle1_2, circle1_3,border_line_1};
end_surface_N_loop = newll; Line Loop(end_surface_N_loop) = {circle2_2, circle2_3,border_line_2};
	
axial_surface_2 = news; Ruled Surface(axial_surface_2) = {axial_surface_2_loop};
axial_surface_3 = news; Ruled Surface(axial_surface_3) = {axial_surface_3_loop};
axial_surface_border = news; Ruled Surface(axial_surface_border) = {axial_surface_border_loop};
end_surface_0 = news; Ruled Surface(end_surface_0) = {end_surface_0_loop};
end_surface_N = news; Ruled Surface(end_surface_N) = {end_surface_N_loop};

surfaceLoop_i = newsl; Surface Loop(surfaceLoop_i) = {axial_surface_2,axial_surface_3,axial_surface_border,end_surface_0,end_surface_N};
volume_wire = newc; Volume(volume_wire) = {surfaceLoop_i};



//Fill lists for later use
border_mesh_X0[linesY+1] = axial_surface_border;
border_mesh_Y0[0] = end_surface_0;
border_mesh_YN[0] = end_surface_N;
X0_axial_surface_1 = axial_surface_2;
X0_axial_surface_2 = axial_surface_3;
X0_spline_1 = spline_2;
X0_spline_2 = spline_4;
Y0_circle_X0_1 = circle1_2;
Y0_circle_X0_2 = circle1_3;
YN_circle_X0_1 = circle2_2;
YN_circle_X0_2 = circle2_3;
X0Y0_1 = c1_2;
X0Y0_2 = c1_4;
X0YN_1 = c2_2;
X0YN_2 = c2_4;
wire_volumes[linesY+1] = volume_wire;
//wire_surface[6*linesY+4] = axial_surface_border;
//wire_surface[6*linesY+5] = end_surface_0;
//wire_surface[6*linesY+6] = end_surface_N;
wire_surface[4*linesY] = axial_surface_2;
wire_surface[4*linesY+1] = axial_surface_3;
//loop for full wires in the middle of the cell
For i In {1:linesX-1}

	//create circle on begin and end of wire (alway 4 parts)
	c1center = newp; Point(c1center) = {i*lX, 0, hMesh+pm*(rMesh+s/2), lcMesh};
	c1_1 = newp; Point(c1_1) = {i*lX-rMesh, 0, hMesh+pm*(rMesh+s/2), lcMesh};
	c1_2 = newp; Point(c1_2) = {i*lX, 0, hMesh+rMesh+pm*(rMesh+s/2), lcMesh};
	c1_3 = newp; Point(c1_3) = {i*lX+rMesh, 0, hMesh+pm*(rMesh+s/2), lcMesh};
	c1_4 = newp; Point(c1_4) = {i*lX, 0, hMesh-rMesh+pm*(rMesh+s/2), lcMesh};
	pm = pm*(-1);
	pos_end = pm;
	
	circle1_1 = newl; Circle(circle1_1) = {c1_1,c1center,c1_2};
	circle1_2 = newl; Circle(circle1_2) = {c1_2,c1center,c1_3};
	circle1_3 = newl; Circle(circle1_3) = {c1_3,c1center,c1_4};
	circle1_4 = newl; Circle(circle1_4) = {c1_4,c1center,c1_1};
	
	spline_1_points[0] = c1_1;
	spline_2_points[0] = c1_2;
	spline_3_points[0] = c1_3;
	spline_4_points[0] = c1_4;	
	
	//create splines for the bend wires of the mesh 
	For j In {1:linesY-1}
		point_s_1 = newp; Point(point_s_1) = {i*lX-rMesh, j*lY, hMesh+pm*(rMesh+s/2), lcMesh}; 	spline_1_points[j] = point_s_1;
		point_s_2 = newp; Point(point_s_2) = {i*lX, j*lY, hMesh+rMesh+pm*(rMesh+s/2), lcMesh}; 	spline_2_points[j] = point_s_2;
		point_s_3 = newp; Point(point_s_3) = {i*lX+rMesh, j*lY, hMesh+pm*(rMesh+s/2), lcMesh}; 	spline_3_points[j] = point_s_3;
		point_s_4 = newp; Point(point_s_4) = {i*lX, j*lY, hMesh-rMesh+pm*(rMesh+s/2), lcMesh}; 	spline_4_points[j] = point_s_4;
		pm = pm*(-1);	
	EndFor

	c2center = newp; Point(c2center) = {i*lX, d, hMesh+pm*(rMesh+s/2), lcMesh};
	c2_1 = newp; Point(c2_1) = {i*lX-rMesh, d, hMesh+pm*(rMesh+s/2), lcMesh};
	c2_2 = newp; Point(c2_2) = {i*lY, d, hMesh+rMesh+pm*(rMesh+s/2), lcMesh};
	c2_3 = newp; Point(c2_3) = {i*lY+rMesh, d, hMesh+pm*(rMesh+s/2), lcMesh};
	c2_4 = newp; Point(c2_4) = {i*lY, d, hMesh-rMesh+pm*(rMesh+s/2), lcMesh};
	pm = pm*(-1);
	
	circle2_1 = newl; Circle(circle2_1) = {c2_1,c2center,c2_2};
	circle2_2 = newl; Circle(circle2_2) = {c2_2,c2center,c2_3};
	circle2_3 = newl; Circle(circle2_3) = {c2_3,c2center,c2_4};
	circle2_4 = newl; Circle(circle2_4) = {c2_4,c2center,c2_1};
	
	spline_1_points[linesX] = c2_1;
	spline_2_points[linesX] = c2_2;
	spline_3_points[linesX] = c2_3;
	spline_4_points[linesX] = c2_4;

	spline_1 = newl; Spline(spline_1) = {spline_1_points[]};
	spline_2 = newl; Spline(spline_2) = {spline_2_points[]};
	spline_3 = newl; Spline(spline_3) = {spline_3_points[]};  
	spline_4 = newl; Spline(spline_4) = {spline_4_points[]};

	axial_surface_1_loop = newll; Line Loop(axial_surface_1_loop) = {spline_1,circle2_1,-spline_2,-circle1_1};
	axial_surface_2_loop = newll; Line Loop(axial_surface_2_loop) = {spline_2,circle2_2,-spline_3,-circle1_2};
	axial_surface_3_loop = newll; Line Loop(axial_surface_3_loop) = {spline_3,circle2_3,-spline_4,-circle1_3};
	axial_surface_4_loop = newll; Line Loop(axial_surface_4_loop) = {spline_4,circle2_4,-spline_1,-circle1_4};
	end_surface_0_loop = newll; Line Loop(end_surface_0_loop) = {circle1_1,circle1_2, circle1_3,circle1_4};
	end_surface_N_loop = newll; Line Loop(end_surface_N_loop) = {-circle2_1,-circle2_4, -circle2_3,-circle2_2};
	
	axial_surface_1 = news; Ruled Surface(axial_surface_1) = {axial_surface_1_loop};
	axial_surface_2 = news; Ruled Surface(axial_surface_2) = {axial_surface_2_loop};
	axial_surface_3 = news; Ruled Surface(axial_surface_3) = {axial_surface_3_loop};
	axial_surface_4 = news; Ruled Surface(axial_surface_4) = {axial_surface_4_loop};
	end_surface_0 = news; Ruled Surface(end_surface_0) = {end_surface_0_loop};
	end_surface_N = news; Ruled Surface(end_surface_N) = {end_surface_N_loop};
	
	surfaceLoop_i = newsl; Surface Loop(surfaceLoop_i) = {axial_surface_1, axial_surface_2, axial_surface_3, axial_surface_4, end_surface_0, end_surface_N};
	volume_wire = newc; Volume(volume_wire) = {surfaceLoop_i};


	//Fill lists for later use
	border_mesh_Y0[i] = end_surface_0;
	border_mesh_YN[i] = end_surface_N;
	//surface_gas_X0[i] = end_surface_0_loop;
	//surface_gas_XN[i] = end_surface_N_loop;
	If(pos_end>0)
		surface_gas_Y0_down[index_Y0_down] = end_surface_0_loop;		//change i (now: empty indices)
		surface_gas_YN_down[index_YN_down] = end_surface_N_loop;
		index_Y0_down ++;
		index_YN_down ++;
	EndIf
	If(pos_end<0)
		surface_gas_Y0_up[index_Y0_up] = end_surface_0_loop;
		surface_gas_YN_up[index_YN_up] = end_surface_N_loop;
		index_Y0_up ++;
		index_YN_up ++;
	EndIf
	axialSurfaceLoop = newsl; Surface Loop(axialSurfaceLoop) = {axial_surface_1, axial_surface_2, axial_surface_3, axial_surface_4};
	wire_surface_loop[linesY-1+i] = axialSurfaceLoop;
	wire_volumes[linesY+1+i] = volume_wire;	

	wire_surface[4*linesY+4*i-2] = axial_surface_1;
	wire_surface[4*linesY+4*i-1] = axial_surface_2;
	wire_surface[4*linesY+4*i] = axial_surface_3;
	wire_surface[4*linesY+4*i+1] = axial_surface_4;
	//wire_surface[6*i+3] = end_surface_0;
	//wire_surface[6*i+4] = end_surface_N;
	
EndFor

//create first half wire at x=N;
i=linesX;	
c1center = newp; Point(c1center) = {i*lX, 0, hMesh+pm*(rMesh+s/2), lcMesh};
c1_1 = newp; Point(c1_1) = {i*lX-rMesh, 0, hMesh+pm*(rMesh+s/2), lcMesh};
c1_2 = newp; Point(c1_2) = {i*lX, 0, hMesh+rMesh+pm*(rMesh+s/2), lcMesh};
c1_4 = newp; Point(c1_4) = {i*lX, 0, hMesh-rMesh+pm*(rMesh+s/2), lcMesh};
pm = pm*(-1);
pos_end = pm;
	
circle1_1 = newl; Circle(circle1_1) = {c1_1,c1center,c1_2};
circle1_4 = newl; Circle(circle1_4) = {c1_4,c1center,c1_1};
border_line_1 = newl; Line(border_line_1) =  {c1_4,c1_2};
	
spline_1_points[0] = c1_1;
spline_2_points[0] = c1_2;
spline_4_points[0] = c1_4;	
	
	//create splines for the bend wires of the mesh 
For j In {1:linesY-1}
	point_s_1 = newp; Point(point_s_1) = {i*lX-rMesh, j*lX, hMesh+pm*(rMesh+s/2), lcMesh}; 	spline_1_points[j] = point_s_1;
	point_s_2 = newp; Point(point_s_2) = {i*lX, j*lX, hMesh+rMesh+pm*(rMesh+s/2), lcMesh}; 	spline_2_points[j] = point_s_2;
	point_s_4 = newp; Point(point_s_4) = {i*lX, j*lX, hMesh-rMesh+pm*(rMesh+s/2), lcMesh}; 	spline_4_points[j] = point_s_4;
	pm = pm*(-1);	
EndFor

c2center = newp; Point(c2center) = {i*lX, d, hMesh+pm*(rMesh+s/2), lcMesh};
c2_1 = newp; Point(c2_1) = {i*lX-rMesh, d, hMesh+pm*(rMesh+s/2), lcMesh};
c2_2 = newp; Point(c2_2) = {i*lY, d, hMesh+rMesh+pm*(rMesh+s/2), lcMesh};
c2_4 = newp; Point(c2_4) = {i*lY, d, hMesh-rMesh+pm*(rMesh+s/2), lcMesh};
pm = pm*(-1);
	
circle2_1 = newl; Circle(circle2_1) = {c2_1,c2center,c2_2};
circle2_4 = newl; Circle(circle2_4) = {c2_4,c2center,c2_1};
border_line_2 = newl; Line(border_line_2) =  {c2_4,c2_2};
	
spline_1_points[linesX] = c2_1;
spline_2_points[linesX] = c2_2;
spline_4_points[linesX] = c2_4;

spline_1 = newl; Spline(spline_1) = {spline_1_points[]};
spline_2 = newl; Spline(spline_2) = {spline_2_points[]};
spline_4 = newl; Spline(spline_4) = {spline_4_points[]};

axial_surface_1_loop = newll; Line Loop(axial_surface_1_loop) = {spline_1,circle2_1,-spline_2,-circle1_1};
axial_surface_4_loop = newll; Line Loop(axial_surface_4_loop) = {spline_4,circle2_4,-spline_1,-circle1_4};
axial_surface_border_loop =newll; Line Loop(axial_surface_border_loop) = {spline_4, border_line_2, -spline_2, -border_line_1};
end_surface_0_loop = newll; Line Loop(end_surface_0_loop) = {circle1_1,-border_line_1,circle1_4};
end_surface_N_loop = newll; Line Loop(end_surface_N_loop) = {-circle2_1,-circle2_4, border_line_2};
	
axial_surface_1 = news; Ruled Surface(axial_surface_1) = {axial_surface_1_loop};
axial_surface_4 = news; Ruled Surface(axial_surface_4) = {axial_surface_4_loop};
axial_surface_border = news; Ruled Surface(axial_surface_border) = {axial_surface_border_loop};
end_surface_0 = news; Ruled Surface(end_surface_0) = {end_surface_0_loop};
end_surface_N = news; Ruled Surface(end_surface_N) = {end_surface_N_loop};

surfaceLoop_i = newsl; Surface Loop(surfaceLoop_i) = {axial_surface_1,axial_surface_border,axial_surface_4,end_surface_0,end_surface_N};
volume_wire = newc; Volume(volume_wire) = {surfaceLoop_i};



//Fill lists for later use
border_mesh_XN[linesY+1] = axial_surface_border;
border_mesh_Y0[linesX] = end_surface_0;
border_mesh_YN[linesX] = end_surface_N;
XN_axial_surface_1 = axial_surface_1;
XN_axial_surface_2 = axial_surface_4;
XN_spline_1 = spline_2;
XN_spline_2 = spline_4;
Y0_circle_XN_1 = circle1_1;
Y0_circle_XN_2 = circle1_4;
YN_circle_XN_1 = circle2_1;
YN_circle_XN_2 = circle2_4;
XNY0_1 = c1_2;
XNY0_2 = c1_4;
XNYN_1 = c2_2;
XNYN_2 = c2_4;
wire_volumes[linesY+linesX+1] = volume_wire;
//wire_surface[4*linesY+4*linesX-2] = axial_surface_border;
//wire_surface[6*linesY] = end_surface_0;
//wire_surface[6*linesY+1] = end_surface_N;
wire_surface[4*linesY+4*linesX-2] = axial_surface_1;
wire_surface[4*linesY+4*linesX-1] = axial_surface_4;

// *******************************
// Points
// *******************************

p1 = newp; Point(p1) = {0, 0, hC, lcW};
p2 = newp; Point(p2) = {sW+sD, 0, hC, lcW};
p3 = newp; Point(p3) = {sW+sD, d, hC, lcW};
p4 = newp; Point(p4) = {0, d, hC, lcW};
p5 = newp; Point(p5) = {0, 0, 0, lcF};
p6 = newp; Point(p6) = {sW+sD, 0, 0, lcF};
p7 = newp; Point(p7) = {sW+sD, d, 0, lcF};
p8 = newp; Point(p8) = {0, d, 0, lcF};
p9 = newp; Point(p9) = {sD/2, 0, 0, lcF};
p10 = newp; Point(p10) = {sD/2+sW, 0, 0, lcF};
p11 = newp; Point(p11) = {sD/2+sW, 0, hS, lcF};
p12 = newp; Point(p12) = {sD/2, 0, hS, lcF};
p13 = newp; Point(p13) = {sD/2, d, 0, lcF};
p14 = newp; Point(p14) = {sD/2+sW, d, 0, lcF};
p15 = newp; Point(p15) = {sD/2+sW, d, hS, lcF};
p16 = newp; Point(p16) = {sD/2, d, hS, lcF};
p17 = newp; Point(p17) = {0, 0, -hPCB, lcW};
p18 = newp; Point(p18) = {sW+sD, 0, -hPCB, lcW};
p19 = newp; Point(p19) = {sW+sD, d, -hPCB, lcW};
p20 = newp; Point(p20) = {0, d, -hPCB, lcW};
pline17_1 = newp; Point(pline17_1) = {0,0,4*hMesh,lcW};		//Points to devide gas volume for meshing
pline18_1 = newp; Point(pline18_1) = {sW+sD,0,4*hMesh,lcW};
pline19_1 = newp; Point(pline19_1) = {sW+sD,d,4*hMesh,lcW};
pline20_1 = newp; Point(pline20_1) = {0,d,4*hMesh,lcW};
pline17_2 = newp; Point(pline17_2) = {0,0,0.5*hMesh,lcM};
pline18_2 = newp; Point(pline18_2) = {sW+sD,0,0.5*hMesh,lcM};
pline19_2 = newp; Point(pline19_2) = {sW+sD,d,0.5*hMesh,lcM};
pline20_2 = newp; Point(pline20_2) = {0,d,0.5*hMesh,lcM};


// *******************************
// Lines
// *******************************

l1 = newl; Line(l1) = {p1,p2};
l2 = newl; Line(l2) = {p2,p3};
l3 = newl; Line(l3) = {p4,p3};
l4 = newl; Line(l4) = {p1,p4};
l5 = newl; Line(l5) = {p5,p9};
l6 = newl; Line(l6) = {p9,p12};
l7 = newl; Line(l7) = {p12,p11};
l8 = newl; Line(l8) = {p10,p11};
l9 = newl; Line(l9) = {p10,p6};
l10 = newl; Line(l10) = {p6,p7};
l11 = newl; Line(l11) = {p8,p13};
l12 = newl; Line(l12) = {p13,p16};
l13 = newl; Line(l13) = {p16,p15};
l14 = newl; Line(l14) = {p14,p15};
l15 = newl; Line(l15) = {p14,p7};
l16 = newl; Line(l16) = {p5,p8};
//l17 = newl; Line(l17) = {p5,p1};
l17_0 = newl; Line(l17_0) = {pline17_1,p1};
l17_1 = newl; Line(l17_1) = {X0Y0_1,pline17_1}; 
l17_2 = newl; Line(l17_2) = {X0Y0_3, X0Y0_2};
l17_3 = newl; Line(l17_3) = {pline17_2,X0Y0_4};
l17_4 = newl; Line(l17_4) = {p5,pline17_2};
//l18 = newl; Line(l18) = {p6,p2};
l18_0 = newl; Line(l18_0) = {pline18_1,p2};
l18_1 = newl; Line(l18_1) = {XNY0_1,pline18_1};
l18_2 = newl; Line(l18_2) = {XNY0_3, XNY0_2};
l18_3 = newl; Line(l18_3) = {pline18_2,XNY0_4};
l18_4 = newl; Line(l18_4) = {p6,pline18_2};
//l19 = newl; Line(l19) = {p7,p3};
l19_0 = newl; Line(l19_0) = {pline19_1,p3};
l19_1 = newl; Line(l19_1) = {XNYN_1,pline19_1};
l19_2 = newl; Line(l19_2) = {XNYN_3, XNYN_2};
l19_3 = newl; Line(l19_3) = {pline19_2,XNYN_4};
l19_4 = newl; Line(l19_4) = {p7,pline19_2};
//l20 = newl; Line(l20) = {p8,p4};
l20_0 = newl; Line(l20_0) = {pline20_1,p4};
l20_1 = newl; Line(l20_1) = {X0YN_1,pline20_1};
l20_2 = newl; Line(l20_2) = {X0YN_3, X0YN_2};
l20_3 = newl; Line(l20_3) = {p8,X0YN_4};
l20_3 = newl; Line(l20_3) = {pline20_2,X0YN_4};
l20_4 = newl; Line(l20_4) = {p8,pline20_2};
l21 = newl; Line(l21) = {p9,p13};
l22 = newl; Line(l22) = {p10,p14};
l23 = newl; Line(l23) = {p11,p15};
l24 = newl; Line(l24) = {p12,p16};
l25 = newl; Line(l25) = {p9,p10};
l26 = newl; Line(l26) = {p13,p14};
l27 = newl; Line(l27) = {p17,p18};
l28 = newl; Line(l28) = {p18,p19};
l29 = newl; Line(l29) = {p20,p19};
l30 = newl; Line(l30) = {p17,p20};
l31 = newl; Line(l31) = {p17,p5};
l32 = newl; Line(l32) = {p18,p6};
l33 = newl; Line(l33) = {p19,p7};
l34 = newl; Line(l34) = {p20,p8};
l1718_1 = newl; Line(l1718_1) = {pline17_1,pline18_1};
l1819_1 = newl; Line(l1819_1) = {pline18_1,pline19_1};
l2019_1 = newl; Line(l2019_1) = {pline20_1,pline19_1};
l1720_1 = newl; Line(l1720_1) = {pline17_1,pline20_1};
/*l1718_2 = newl; Line(l1718_2) = {pline17_2,pline18_2};
l1819_2 = newl; Line(l1819_2) = {pline18_2,pline19_2};
l2019_2 = newl; Line(l2019_2) = {pline20_2,pline19_2};
l1720_2 = newl; Line(l1720_2) = {pline17_2,pline20_2};*/



// *******************************
// Surfaces
// *******************************

lp1 = newll; Line Loop(lp1) = {l1, l2, -l3, -l4};
s1 = news; Plane Surface(s1) = {lp1};
lp2 = newll; Line Loop(lp2) = {l5, l21, -l11,-l16};
s2 = news; Plane Surface(s2) = {lp2};
lp3 = newll; Line Loop(lp3) = {l6, l24, -l12, -l21};
s3 = news; Plane Surface(s3) = {lp3};
lp4 = newll; Line Loop(lp4) = {l7, l23, -l13, -l24};
s4 = news; Plane Surface(s4) = {lp4};
lp5 = newll; Line Loop(lp5) = {-l8, l22, l14, -l23};
s5 = news; Plane Surface(s5) = {lp5};
lp6 = newll; Line Loop(lp6) = {l9, l10, -l15, -l22};
s6 = news; Plane Surface(s6) = {lp6};
//lp7 = newll; Line Loop(lp7) = {l5, l6, l7, -l8, l9, l18, -l1, -l17};
//surface_gas_Y0[0] = lp7;
//s7 = news; Plane Surface(s7) = {surface_gas_Y0[]};
lp7_0 = newll; Line Loop(lp7_0) = {l1718_1, l18_0, -l1, -l17_0};
s7_0 = news; Plane Surface(s7_0) = {lp7_0};
lp7_1 = newll; Line Loop(lp7_1) = {Y0_spline_1, l18_2, Y0_circle_XN_2, Y0_circle_XN_1, l18_1, -l1718_1, -l17_1, Y0_circle_X0_1, Y0_circle_X0_2,-l17_2};
surface_gas_Y0_up[0] = lp7_1;
s7_1 = news; Plane Surface(s7_1) = {surface_gas_Y0_up[]};
lp7_2 = newll; Line Loop(lp7_2) = {l5,l6,l7,-l8,l9, l18_4, l18_3, -Y0_spline_2, -l17_3, -l17_4};
surface_gas_Y0_down[0] = lp7_2;
s7_2 = news; Plane Surface(s7_2) = {surface_gas_Y0_down[]};
/*lp7_3 = newll; Line Loop(lp7_3) = {l5,l6,l7,-l8,l9, l18_4, -l1718_2, -l17_4};
s7_3 = news; Plane Surface(s7_3) = {lp7_3};*/
//lp8 = newll; Line Loop(lp8) = {l11, l12, l13, -l14, l15, l19, -l3, -l20};
//surface_gas_YN[0] = lp8;
//s8 = news; Plane Surface(s8) = {surface_gas_YN[]};
lp8_0 = newll; Line Loop(lp8_0) = {l2019_1, l19_0, -l3, -l20_0};
s8_0 = news; Plane Surface(s8_0) = {lp8_0};
lp8_1 = newll; Line Loop(lp8_1) = {YN_spline_1, l19_2, YN_circle_XN_2, YN_circle_XN_1, l19_1,-l2019_1,-l20_1, YN_circle_X0_1, YN_circle_X0_2, -l20_2};
surface_gas_YN_up[0] = lp8_1;
s8_1 = news; Plane Surface(s8_1) = {surface_gas_YN_up[]};
lp8_2 = newll; Line Loop(lp8_2) = {l11, l12, l13, -l14, l15, l19_4, l19_3, -YN_spline_2, -l20_3,-l20_4};
surface_gas_YN_down[0] = lp8_2;
s8_2 = news; Plane Surface(s8_2) = {surface_gas_YN_down[]};
/*lp8_3 = newll; Line Loop(lp8_3) = {l11, l12, l13, -l14, l15, l19_4, -l2019_2, -l20_4};
s8_3 = news; Plane Surface(s8_3) = {lp8_3};*/
lp9_0 = newll; Line Loop(lp9_0) = {l1720_1, l20_0, -l4, -l17_0};
s9_0 = news; Plane Surface(s9_0) = {lp9_0};
lp9_1 = newll; Line Loop(lp9_1) = {X0_spline_1, l20_1, -l1720_1,-l17_1};
surface_gas_X0_up[0] = lp9_1;
s9_1 = news; Plane Surface(s9_1) = {surface_gas_X0_up[]};
lp9_2 = newll; Line Loop(lp9_2) = {l16, l20_4, l20_3, X0_circle_YN_2, X0_circle_YN_1, l20_2, -X0_spline_2, -l17_2, X0_circle_Y0_1, X0_circle_Y0_2, -l17_3, -l17_4};
surface_gas_X0_down[0] = lp9_2;
s9_2 = news; Plane Surface(s9_2) = {surface_gas_X0_down[]};
/*lp9_3 = newll; Line Loop(lp9_3) = {l16, l20_4, -l1720_2, -l17_4};
s9_3 = news; Plane Surface(s9_3) = {lp9_3};*/
lp10_0 = newll; Line Loop(lp10_0) = {l1819_1, l19_0, -l2, -l18_0};
s10_0 = news; Plane Surface(s10_0) = {lp10_0};
lp10_1 = newll; Line Loop(lp10_1) = {XN_spline_1, l19_1, -l1819_1, -l18_1};
surface_gas_XN_up[0] = lp10_1;
s10_1 = news; Plane Surface(s10_1) = {surface_gas_XN_up[]};
lp10_2 = newll; Line Loop(lp10_2) = {l10, l19_4, l19_3, XN_circle_YN_2, XN_circle_YN_1, l19_2, -XN_spline_2, -l18_2, XN_circle_Y0_1, XN_circle_Y0_2, -l18_3, -l18_4};
surface_gas_XN_down[0] = lp10_2;
s10_2 = news; Plane Surface(s10_2) = {lp10_2};
//s10_2 = news; Plane Surface(s10_2) = {surface_gas_XN_down[]};
/*lp10_3 = newll; Line Loop(lp10_3) = {l10, l19_4, -l1819_2, -l18_4};
s10_3 = news; Plane Surface(s10_3) = {lp10_3};*/
lp11 = newll; Line Loop(lp11) = {l27, l32, -l9,-l25, -l5, -l31};
s11 = news; Plane Surface(s11) = {lp11};
lp12 = newll; Line Loop(lp12) = {l29, l33, -l15, -l26, -l11, -l34};
s12 = news; Plane Surface(s12) = {lp12};
lp13 = newll; Line Loop(lp13) = {l28, l33, -l10, -l32};
s13 = news; Plane Surface(s13) = {lp13};
lp14 = newll; Line Loop(lp14) = {l30, l34, -l16, -l31};
s14 = news; Plane Surface(s14) = {lp14};
lp15 = newll; Line Loop(lp15) = {l27, l28, -l29, -l30};
s15 = news; Plane Surface(s15) = {lp15};
lp16 = newll; Line Loop(lp16) = {l25, l22, -l26, -l21};
s16 = news; Plane Surface(s16) = {lp16};
lp17 = newll; Line Loop(lp17) = {l25, l8, -l7, -l6};
s17 = news; Plane Surface(s17) = {lp17};
lp18 = newll; Line Loop(lp18) = {l26, l14, -l13, -l12};
s18 = news; Plane Surface(s18) = {lp18};


//Compound Surfaces for the boundaries of the cell
/*s7_up = news; Compound Surface(s7_up) = {s7_0,s7_1};
s7_down = news; Compound Surface(s7_down) = {s7_2,s7_3};
s8_up = news; Compound Surface(s8_up) = {s8_0,s8_1};
s8_down = news; Compound Surface(s8_down) = {s8_2,s8_3};
s9_up = news; Compound Surface(s9_up) = {s9_0,s9_1};
s9_down = news; Compound Surface(s9_down) = {s9_2,s9_3};
s10_up = news; Compound Surface(s10_up) = {s10_0,s10_1};
s10_down = news; Compound Surface(s10_down) = {s10_2,s10_3};*/


// *******************************
// Volumes
// *******************************

sl1 = newsl; Surface Loop(sl1) = {s2, -s3, s4, -s5, s6, s10_0,s10_1, X0_axial_surface_1, X0_axial_surface_2, s10_2, -s1, -s9_0, -s9_1, -XN_axial_surface_1, -XN_axial_surface_2, -s9_2,  s7_0, s7_1, Y0_axial_surface_1, Y0_axial_surface_2, s7_2, -s8_0, -s8_1,-YN_axial_surface_1, -YN_axial_surface_2, -s8_2};
wire_surface_loop[0] = sl1;
vGas =newv; Volume(vGas) = {wire_surface_loop[]};
sl2 = newsl; Surface Loop(sl2) = {s11, s13, -s12, -s14, s15, s2, s16, s6};
vPCB =newv; Volume(vPCB) = {sl2};
sl3 = newsl; Surface Loop(sl3) = {s17, s5, -s18, s3, s16, -s4};
vStrip =newv; Volume(vStrip) = {sl3};

// *******************************
// Physical Volumes
// *******************************

Physical Volume(1) = {vGas};
Physical Volume(2) = {vPCB};
Physical Volume(3) = {vStrip};
Physical Volume(4) = {wire_volumes[]};

// *******************************
// Physical Surfaces
// *******************************
//

Physical Surface(1) = {-s1};
Physical Surface(2) = {s2,s6};
Physical Surface(3) = {s3,-s4,s5,s16,s17,s18};
Physical Surface(4) = {s7_0,s7_1,s7_2,s11};
Physical Surface(5) = {s10_0,s10_1,s10_2,s13};
Physical Surface(6) = {s8_0,s8_1,s8_2,s12};
Physical Surface(7) = {s9_0,s9_1,s9_2,s14}; 
/*Physical Surface(4) = {s7_1,border_mesh_Y0[linesX+1],s7_2,s11};
Physical Surface(5) = {s10_1,border_mesh_XN[linesY+1],s10_2,s13};
Physical Surface(6) = {s8_1,border_mesh_YN[linesX+1],s8_2,s12};
Physical Surface(7) = {s9_1,border_mesh_X0[linesY+1],s9_2,s14}; 
/*Physical Surface(8) = {s7_2,s11};
Physical Surface(9) = {s10_2,s13};
Physical Surface(10) = {s8_2,s12};
Physical Surface(11) = {s9_2,s14}; */
/*Physical Surface(10) = {border_mesh_X0[]};
Physical Surface(11) = {border_mesh_XN[]};
Physical Surface(12) = {border_mesh_Y0[]};
Physical Surface(13) = {border_mesh_YN[]};*/
Physical Surface(8) = {wire_surface[],border_mesh_X0[],border_mesh_XN[],border_mesh_Y0[],border_mesh_YN[]};
Physical Surface(9) = {s15};			//bottom








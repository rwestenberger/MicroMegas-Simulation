// Mesh construction: Constructs wire mesh from elementary_mesh_unit elements.

// Parameters
mm = 1e-3;
x_0 = 0*mm; y_0 = 0*mm; z_0 = 0*mm;

number_of_wires = 4;
mesh_lattice_constant = 0.0625*mm;				// length of periodicity
wire_diameter = 0.03*mm;
wire_bend_amount = wire_diameter+1e-3*mm;		// amplitude of the wire bend

mesh_cl = 0.01*mm;			// characteristic length used for the mesh

If (Modulo(number_of_wires, 2))
	number_of_wires += 1;
	Printf("The number_of_wires must be even, set it to from %g to %g!", number_of_wires-1, number_of_wires);
EndIf

// create elementary mesh unit
Macro elementary_mesh_unit
	If (number_of_wires < 2)
		Printf("Must be at least 2 wires!"); Abort;
	EndIf

	If (wire_diameter > wire_bend_amount)
		Printf("Wire diameter must be smaller then wire bend amount!"); Abort;
	EndIf

	For i In {0:number_of_wires}
		If (i == 0 || i == number_of_wires) // end cap
			p_center = newp;
			If (!wire_rotated)
				Point(p_center) = {i*mesh_lattice_constant+x_0, Modulo(i, 2)*wire_bend_amount+y_0, Modulo(i+wire_mirrored, 2)*wire_bend_amount+z_0, mesh_cl};
			EndIf
			If (wire_rotated)
				Point(p_center) = {Modulo(i, 2)*wire_bend_amount+x_0, i*mesh_lattice_constant+y_0, Modulo(i+wire_mirrored, 2)*wire_bend_amount+z_0, mesh_cl};
			EndIf
		EndIf

		If (!wire_rotated)
			p_1 = newp; Point(p_1) = {i*mesh_lattice_constant+x_0,                  y_0, -wire_diameter/2 + Modulo(i+wire_mirrored, 2)*wire_bend_amount+z_0, mesh_cl};
			p_2 = newp; Point(p_2) = {i*mesh_lattice_constant+x_0, -wire_diameter/2+y_0,                  + Modulo(i+wire_mirrored, 2)*wire_bend_amount+z_0, mesh_cl};
			p_3 = newp; Point(p_3) = {i*mesh_lattice_constant+x_0,                  y_0, +wire_diameter/2 + Modulo(i+wire_mirrored, 2)*wire_bend_amount+z_0, mesh_cl};
		EndIf
		If (wire_rotated)
			p_1 = newp; Point(p_1) = {                 x_0, i*mesh_lattice_constant+y_0, -wire_diameter/2 + Modulo(i+wire_mirrored, 2)*wire_bend_amount+z_0, mesh_cl};
			p_2 = newp; Point(p_2) = {-wire_diameter/2+x_0, i*mesh_lattice_constant+y_0,                  + Modulo(i+wire_mirrored, 2)*wire_bend_amount+z_0, mesh_cl};
			p_3 = newp; Point(p_3) = {                 x_0, i*mesh_lattice_constant+y_0, +wire_diameter/2 + Modulo(i+wire_mirrored, 2)*wire_bend_amount+z_0, mesh_cl};
		EndIf

		If (i == 0 || i == number_of_wires) // end cap
			c_1 = newl; Circle(c_1) = {p_1, p_center, p_2};
			c_2 = newl; Circle(c_2) = {p_2, p_center, p_3};
			l = newl; Line(l) = {p_1, p_3};

			circle_1_circles[i] = c_1; circle_2_circles[i] = c_2;
			lines[i] = l;
		EndIf

		spline_1_points[i] = p_1;
		spline_2_points[i] = p_2;
		spline_3_points[i] = p_3;
	EndFor

	spline_1 = newl; Spline(spline_1) = {spline_1_points[]};
	spline_2 = newl; Spline(spline_2) = {spline_2_points[]};
	spline_3 = newl; Spline(spline_3) = {spline_3_points[]};

	axial_loop_1 = newll; Line Loop(axial_loop_1) = {spline_1, circle_1_circles[number_of_wires], -spline_2, -circle_1_circles[0]};
	surface_1 = news; Ruled Surface(surface_1) = {axial_loop_1};
	axial_loop_2 = newll; Line Loop(axial_loop_2) = {spline_2, circle_2_circles[number_of_wires], -spline_3, -circle_2_circles[0]};
	surface_2 = news; Ruled Surface(surface_2) = {axial_loop_2};
	radial_loop = newll; Line Loop(radial_loop) = {lines[0], spline_3, -lines[number_of_wires], -spline_1};
	surface_3 = news; Plane Surface(surface_3) = {radial_loop};
	end_cap_loop_1 = newll; Line Loop(end_cap_loop_1) = {circle_1_circles[0], circle_2_circles[0], -lines[0]};
	end_cap_1 = news; Plane Surface(end_cap_1) = {end_cap_loop_1};
	end_cap_loop_2 = newll; Line Loop(end_cap_loop_2) = {circle_1_circles[number_of_wires], circle_2_circles[number_of_wires], -lines[number_of_wires]};
	end_cap_2 = news; Plane Surface(end_cap_2) = {end_cap_loop_2};

	// half wire to full wire
	If (wire_rotated) tmp = x_0; x_0 = y_0;	y_0 = tmp; EndIf // swap x_0 and y_0
	surface_1_m = Symmetry{wire_rotated, !wire_rotated, 0, -y_0}{Duplicata{Surface{surface_1};}};
	surface_2_m = Symmetry{wire_rotated, !wire_rotated, 0, -y_0}{Duplicata{Surface{surface_2};}};
	end_cap_1_m = Symmetry{wire_rotated, !wire_rotated, 0, -y_0}{Duplicata{Surface{end_cap_1};}};
	end_cap_2_m = Symmetry{wire_rotated, !wire_rotated, 0, -y_0}{Duplicata{Surface{end_cap_2};}};

	full_wire_sur = newsl; Surface Loop(full_wire_sur) = {surface_1, surface_2, end_cap_1, end_cap_2, surface_1_m, surface_2_m, end_cap_1_m, end_cap_2_m};
	full_wire = newv; Volume(full_wire) = {full_wire_sur};
Return

// create the mesh with elementary_mesh_unit
For wire_num In {0:number_of_wires}
	// x wires
	x_0 = 0*mm; y_0 = wire_num*mesh_lattice_constant; z_0 = 0*mm;
	wire_rotated = 0; wire_mirrored = Modulo(wire_num+1,2);
	Call elementary_mesh_unit;
	wires_x[wire_num] = full_wire;

	// y wires
	x_0 = wire_num*mesh_lattice_constant; y_0 = 0*mm; z_0 = 0*mm;
	wire_rotated = 1; wire_mirrored = Modulo(wire_num,2);
	Call elementary_mesh_unit;
	wires_y[wire_num] = full_wire;
EndFor

mesh = newreg; Physical Volume(mesh) = {wires_y, wires_x};

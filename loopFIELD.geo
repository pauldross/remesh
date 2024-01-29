//mesh params

Mesh.MeshSizeExtendFromBoundary = 0;
Mesh.MeshSizeFromCurvature = 0;
Mesh.MshFileVersion = 2.2;    //  Fix the mesh version to 2.2 - this works best for TTH
Mesh.Algorithm = 5;
Mesh.Algorithm3D = 1;
Mesh.OptimizeNetgen = 1;  

//vars
zoff=0.2e-6; //thickness
R=%R; // loop radius
W=%W; // track width

//+
SetFactory("OpenCASCADE");
Circle(1) = {0, 0, -zoff/2, R, 0, 2*Pi};
//+
Circle(2) = {0, 0, -zoff/2, R-W, 0, 2*Pi};
//+
Curve Loop(1) = {1};
//+
Curve Loop(2) = {2};
//+
Plane Surface(1) = {1, 2};
//+
Extrude {0, 0, zoff} {
  Surface{1}; 
}

Point(17) = {0, 0, -1E-5, 1.5};
Point(18) = {0, 0, 1E-5, 1.5};
Line(25) = {17, 18};


Physical Surface("Fieldplane0", 26) = {4, 2, 3, 1};

Coherence;
Mesh 2;
Save "loopFIELD.msh";
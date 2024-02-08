## Remeshing module

### Command line options

Usage: remesh [--help] [--version] [-o VAR] [-mindj VAR] [-minchar VAR] [-debug] [-rg] vtkpath geopath

"remesh" is a module that allows the user to optimise the mesh based on the parameters described below

Positional arguments:
  vtkpath        The path to the VTK file containg the J field data for the squid washer [required]
  geopath        The path to the geometry file that is to be optimised [required]

Optional arguments:
  -h, --help     shows help message and exits 
  -v, --version  prints version information and exits 
  -o             The name of the optimised geo and msh file should be named (omit the .msh or .geo file extension) [nargs=0..1] [default: "opt"]
  -mindj         The minimum percentage change allowed between points in the mesh [nargs=0..1] [default: 0.1]
  -minchar       The minimum characteristic length (prevents over refinement set carefully!) [nargs=0..1] [default: 1e-08]
  -debug         When debug is added remesh will show the mesh in the GMSH window 
  -rg            Adding this tag stops the program from running GMSH 

### Example

#### Folder contents

  - loop.geo: This is the geometry file that generates a mesh file named loop.msh. The file describes the 3D volume of the SQUID washer.
  - loopFIELD.geo: Same as loop.geo but generates a loopFIELD.msh file. This is the mesh that defines the surface of the SQUID washer. It is used to specify where the B-feild must be calculated.
  - loopFIELDINIT.geo and loopINIT.geo: Same as loop.geo and loopFIELD.geo but generates a unrefined, course mesh to be used in the first iteration of the optimisation process. 

#### How to use

  1) Start by running the following commands to generate the necessary mesh files:
  ```console
  $ gmsh loopINIT.geo -0
  $ gmsh loopFIELDINIT.geo -0
  ```
  2) Run tth:
  ```console
  $ tth settings_washer.json
  ```
  3) An output folder should be generated. Now is a good time to look at the mesh and the results in the ouput folder.
  4) Run the following command to generate an optimised mesh (omit -debug if you do not wish to see the optimised mesh):
  ```console
  $ remesh ./output/J_washer.vtk loop.geo -o loop -mindj 0.1 -debug
  ```
  5) Optimise the field mesh:
  ```console
  $ remesh ./output/loop_test.vtk loopFIELD.geo -o loopFIELD -mindj 0.1 -debug
  ```
  6) Repeat step 2 - 5 however many times you like.
  7) Run the clean.py command to clean up.

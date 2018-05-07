# 3D Fractal Mountain Generator

[Read about our project.](https://vickymmcd.github.io/3DFractalMountains/)

### Setup
Install C++ and OpenGL.

Have a STL file viewer also installed.

### Clone Repo

`git clone https://github.com/vickymmcd/3DFractalMountains.git`

`cd 3DFractalMountains/code`

### Generate STL File of Landscape
`make stl_from_hm`

The STL file you generated will be titled "mountain.stl"

Open this file in an STL viewer such as MeshLab to see the terrain you generated.

### Visualize Landscape in OpenGL
`cd 3DFractalMountains/terrain`
`make`
`./tg`

The render can be controlled with mouse and arrow key input.


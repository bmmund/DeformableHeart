# DeformableHeart
CPSC 589 project for deformable heart models.

## Demo
Running Excetuable:
    double click "demo.exe" in the Bin folder

### User Interface:
    Left Click a Vertex - Selects the vertex, the selected vertex will be
                          highlighted to indicate it has been selected.
    Left Click a blank area/Right Click - De-select a vertex if a vertex is
                                          currently selected.
    Left Click and drag while holding shift - Rotate the heart mesh
    Scroll Wheel - zoom in and out of the scene
    Up/Down Arrows - Move the selected vertex back and forth along its normal.
    Right/Left Arrows - Shift the Camera around the heart in increments.
    S Key - Implement one level of loop subdivision.
    W Key - Enable/Disable Wireframe.
    L Key - Enable/Disable Lighting.
    R Key - Reset view to default position.

### Demo Procedure:
    1. Select vertex using left click
    2. use arrow up/down to move vertex in and out
    3. apply a level of subdivision with S
    4. repeat steps 1 - 3 as necessary

## Prerequisite
| Lib (URL)                             | Min Version | Note       |
| ------------------------------------- | ----------- | ---------- |
| CMake <br> http://www.cmake.org       | 3.1.3       | *Required* |
| GLM <br> http://glm.g-truc.net        | 0.9.7.5     | *Required* |
| GLFW <br> http://www.glfw.org         | 3.2         | *Required* |
| OpenMesh <br> http://www.glfw.org     | 6.1         | *Required* |

## Building Demo
Currently there are no platform independent instructions. It is recommended that
you do a "out-of-source" build. All this means is that you first create a folder
and run cmake out of there.

E.g:
```
mkdir build
cd build
cmake ..
./main
```


### Useful cmake options and environment variables

````
-DUSE_LOCAL_DEPENDANCIES=1  // Look for dependencies no local machine
````

## Credit:
    polyroots.cc/polyroots.hpp was provided in CPSC 453 by Erica Harrison

    heart icon by Charles Haitkin and is available at:
    https://dribbble.com/shots/2589899-Low-Poly-Series-Red-Heart-Icon

    heart model published by girolamo caiazzo:
    http://www.thingiverse.com/thing:1228829/#made

## Software3D - 3D for Game Editor

Software3D (a temporary name) is a 3D library for [Game Editor](http://game-editor.com/Main_Page).  
Opening the project (.ged file) requires Game Editor [version 1.4.0](http://game-editor.com/Download) or newer.

The engine is adapted from the following tutorial by David Rousset:  
https://www.davrous.com/2013/06/13/tutorial-series-learning-how-to-write-a-3d-soft-engine-from-scratch-in-c-typescript-or-javascript/

**Notice: Opening the project takes some time**  
The project contains 45 images of resolution 3360 x 4200. Loading these images causes Game Editor to
take a while opening the project. The program may become unresponsive and it may even look like it has frozen 
(Windows might even tell you so), **_but it will eventually get the job done_**. After that initial
loading phase Game Editor runs smoothly.

### Controls

- **F1**: toggle axis visualizers, default = on
- **F2/F3/F4**: toggle object rotation around x/y/z axis
    - current states shown as last three numbers on bottom left
- **F5**: reset object orientation
- **F6**: toggle object rotation rule (should only 1 axis be allowed to be rotated at a time, default = 0 (off)
    - current state shown as first number on last line on bottom left
- **F7**: cycle object visualizations (*mode*)
    - 0: vertices
    - 1: wireframe and vertices
    - 2: wireframe
    - 3: color only
- **F8**: toggle backface culling, default = off
- **WS/AD/QE**: move object (+-z, -+x, -+y)
- **↑↓/←→/LCTRL, SPACE**: move camera (+-z, -+x, -+Y)
- **§ (key above tab)**:
    - free allocated memory (on key down)
    - exit (on key up)

### YouTube preview (outdated)
[![Game Editor 3D YouTube video thumbnail](https://img.youtube.com/vi/8hE8w9jJIQw/maxresdefault.jpg)](https://www.youtube.com/watch?v=8hE8w9jJIQw)

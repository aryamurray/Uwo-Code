BUILDING AND RUNNING
    This project assumes a linux based enviroment (native, WSL, VM) with the libraries as outlined in the assignment page

    To build, just run "make" in the top level working directory with the source code and the Makefile
    To run, execute "./assignment4" WITHIN the bin directory. It will not work otherwise. Regardless, it only renders a black screen.
    Currently, nothing renders, but the entire pipeline is implimented. I am at a loss as to why it's not rendering.

STRUCTURE
    1. We start off by initiating our GLFW and GLEW instance and creating out window, specifing some parameters (size, name, etc.)
    2. We read in our face and vertex data from the bmp and ply files and hold them in instance variables. We also set the shaders in the constructor.
    2.5 Set all the opengl related variables by generating a binding buffers.
    3. Export the objects to the meshes array so that we can render it in the main loop
    4. Render the objects in the main loop

    Rest in contined in comments within the main source file.

BUILDING AND RUNNING
    This project assumes a linux based enviroment (native, WSL, VM) with the libraries as outlined in the assignment page

    To build, just run "make" in the appropreate working directory with the source code and the Makefile
    To run, execute "./main"

STRUCTURE
    1. We start off by initiating our GLFW instance and creating out window, specifing some parameters (size, name, etc.)
    2. Create the glfw context which does most of the heavy lifting with opengl
    3. Create our main while loop where we update the viewport every frame
    4. In this loop, we define our triangle and colour that gets rendered when we run the program.
    5. Finally, we just have a cleanup function and return from main
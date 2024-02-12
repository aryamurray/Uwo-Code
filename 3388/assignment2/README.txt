BUILDING AND RUNNING
    This project assumes a linux based enviroment (native, WSL, VM) with the libraries as outlined in the assignment page

    To build, just run "make" in the appropreate working directory with the source code and the Makefile
    To run, execute "./exercise1 or ./exercise2" to check both parts of the assigment.

STRUCTURE
    1. We start off by initiating our GLFW instance and creating out window, specifing some parameters (size, name, etc.)
    2. Create the glfw context which does most of the heavy lifting with opengl
    3. All of the code comments are within the source files that describe what the code is doing.
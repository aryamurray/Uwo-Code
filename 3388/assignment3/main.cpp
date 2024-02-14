#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>




int main(int argc, char*argv[]){
    int WIDTH = 1920;
    int HEIGHT = 1080;

    if (argc == 3){
        WIDTH = atoi(argv[1]);
        HEIGHT = atoi(argv[2]);
        printf("[Info] Setting window size to be %s by %s", WIDTH, HEIGHT);
    }

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WIDTH,HEIGHT, "Hello World", NULL, NULL);
    if (!window)
    {
        printf("[Error] Failed to create window");
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current and define view*/
    glfwMakeContextCurrent(window);
    glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);
    glViewport(0, 0, WIDTH, HEIGHT);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();
        
		/* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        
       
        
		/* Swap front and back buffers */
        glfwSwapBuffers(window);

    }

    glfwTerminate();
    return 0;
}
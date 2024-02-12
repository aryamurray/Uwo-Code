#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

static inline double frand()
{
    double x = ((double)rand()) / (double)RAND_MAX;
    if (rand() % 2)
    {
        x *= -1.0;
    }
    return x;
}

int main(int argc, char *argv[])
{

    if (argc != 4)
    {
        printf("Usage: %s <N> <width> <height>\n", argv[0]);
        return -1;
    }

    int N = atoi(argv[1]);
    int width = atoi(argv[2]);
    int height = atoi(argv[3]);

    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Assignment 2", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glOrtho(-1.1, 1.1, -1.1, 1.1, -1.0, 1.0); // Set the Orthographics as defined
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);     // Set background to white

    srand(time(NULL)); // Seed the random number generator

    float corners[4][2] = {{-1, -1}, {1, -1}, {1, 1}, {-1, 1}};
    float p[2] = {frand(), frand()};
    int cIndex = rand() % 4;

    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POINTS);          // Set the Draw Type
    glPointSize(2.0f);           // Set the point size
    glColor3f(0.0f, 0.0f, 0.0f); // Set point color to black

    for (int i = 0; i < N; i++)
    {
        // Choose a random corner not diagonally opposite to the previous one
        int newCIndex;
        do
        {
            newCIndex = rand() % 4;
        } while ((newCIndex + 2) % 4 == cIndex);
        cIndex = newCIndex;

        // Calculate the point halfway between p and the chosen corner
        p[0] = (p[0] + corners[cIndex][0]) / 2;
        p[1] = (p[1] + corners[cIndex][1]) / 2;

        // Draw the point
        glVertex2f(p[0], p[1]);
    }
    glfwSwapBuffers(window);

    printf("Now Reached Main Loop\n");
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <cmath>
#include <valarray>

#define DogData std::vector<float>

DogData readDogData()
{
    std::vector<float> coordinates;

    // Open the txt file for reading
    std::ifstream infile("dog.txt");

    // Check if the file is opened successfully
    if (infile.is_open())
    {
        // Declare a variable to store the value
        float value;

        // Read the file until the end
        while (infile >> value)
        {
            // Push the value to the vector
            coordinates.push_back(value);
        }

        // Close the file
        infile.close();
    }

    // Return the vector of coordinates
    return coordinates;
}

void drawDog(DogData dogData)
{
    glBegin(GL_LINE_STRIP);
    glLineWidth(2.0f);
    glColor3f(0, 0, 0);
    for (int i = 0; i < dogData.size(); i += 2)
    {
        glVertex2f(dogData[i], dogData[i + 1]);
    }
    glEnd();
}

int main(void)
{
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 640, "Spinning Dogs", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glOrtho(0, 60, 0, 60, -1, 1);
    // glClearColor(1.0, 1.0, 1.0, 1.0);

    DogData dogData = readDogData();
    float centerX = 30.0f, centerY = 30.0f, radius = 25.0f;
    float angleIncrement = 1.0f; // Rotation angle increment per frame

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();

        /* Render here */
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glColor3f(0.0f, 0.0f, 0.0f); // Set color to black

        for (int angle = 0; angle < 360; angle += 45)
        {
            glPushMatrix();
            glTranslatef(centerX + radius * cos(angle * M_PI / 180.0f), centerY + radius * sin(angle * M_PI / 180.0f), 0.0f);
            glRotatef(angleIncrement, 0.0f, 0.0f, 1.0f);
            drawDog(dogData);
            glPopMatrix();
        }

        angleIncrement += 1.0f; // Increment the rotation angle for the next frame

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
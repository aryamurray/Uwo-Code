#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>
#include <iostream>

#define DogData std::vector<std::pair<float, float>>

DogData readDogData(){
    DogData dogData;

    std::ifstream file("dog.txt");

    float x, y;

    while (file >> x >> y) {
        dogData.push_back(std::make_pair(x, y));
    }

    return dogData;
}

void drawDog(DogData dogData){
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i< dogData.size(); i++){
        glVertex2f(dogData[i].first, dogData[i].second);
    }
    glEnd();
}



int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280,1000, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    DogData dogData = readDogData();
  


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();
        
		/* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        drawDog(dogData);
        
        
		/* Swap front and back buffers */
        glfwSwapBuffers(window);

    }

    glfwTerminate();
    return 0;
}
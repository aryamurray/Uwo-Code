#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>
#include <vector>
#include "TexturedMesh.hpp"
#include "LoadBitmap.hpp"
#include "types.hpp"

// ... other necessary includes

// Camera variables
glm::vec3 cameraPosition = glm::vec3(0.5f, 0.4f, 0.5f);
glm::vec3 cameraLookDirection = glm::vec3(0.0f, 0.0f, -1.0f); // Normalized
float cameraSpeed = 0.05f;
float cameraRotation = 3.0f; 

// // Function to handle camera movement based on key input
// void handleCameraMovement(GLFWwindow* window) {
//     if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
//         cameraPosition += cameraSpeed * cameraLookDirection;
//     }
//     if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
//         cameraPosition -= cameraSpeed * cameraLookDirection;
//     }
//     if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
//         float radians = glm::radians(cameraRotation);
//         cameraLookDirection = glm::rotate(cameraLookDirection, radians, glm::vec3(0.0f, 1.0f, 0.0f));
//     }
//     if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
//         float radians = glm::radians(-cameraRotation); // Note: negative rotation
//         cameraLookDirection = glm::rotate(cameraLookDirection, radians, glm::vec3(0.0f, 1.0f, 0.0f));
//     }
// }

// // Function to calculate and update the view matrix
// void updateViewMatrix() {
//     glm::vec3 right = glm::normalize(glm::cross(cameraLookDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
//     glm::vec3 up = glm::normalize(glm::cross(right, cameraLookDirection));
//     glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraLookDirection, up);

//     // Get the uniform location of your view matrix in your shader
//     // and set its value
//     int viewMatrixLocation = glGetUniformLocation(yourShaderProgram, "view");
//     glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(view));
// }

void pushTexturedMeshes(std::vector<TexturedMesh>& meshes) {
    std::vector<std::string> plyFilenames = {"Bottles.ply","Curtains.ply","DoorBg.ply","Floor.ply","MetalObjects.ply","Patio.ply","Table.ply","Walls.ply","WindowBG.ply","WoodObjects.ply"}; // Array of PLY filenames
    std::vector<std::string> bmpFilenames = {"bottles.bmp","curtians.bmp","doorbg.bmp","floor.bmp","metalobjects.bmp","patio.bmp","table.bmp","walls.bmp","windowbg.bmp","woodobjects.bmp"}; // Array of texture filenames


    for (int i = 0; i < plyFilenames.size(); i++) {
        printf("[Init] Reading %s and %s ","assets/LinksHouse/" + plyFilenames[i], "assets/LinksHouse/" + bmpFilenames[i]);
        meshes.push_back(TexturedMesh("assets/LinksHouse/" + plyFilenames[i],"assets/LinksHouse/" + bmpFilenames[i]));
    }
}

int main(void) {
    //Declare needed variables
    GLFWwindow *window;
    std::vector<TexturedMesh> meshes;

    //Init the Meshes
    pushTexturedMeshes(meshes);
    printf("[Init] Finished Reading Textures");








    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_SAMPLES, 4); // Set the 4x Sampling
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 640, "Link's House", NULL, NULL);
    if (!window)
    {
        printf("[Error] Failed to create window");
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current and define view*/
    glfwMakeContextCurrent(window);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();

        /* Render here */
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
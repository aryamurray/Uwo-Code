#include "main.h"


// ... other necessary libraries

// Camera variables
glm::vec3 cameraPosition = glm::vec3(0.5f, 0.4f, 0.5f);  // Initial position
glm::vec3 cameraLookAt = glm::vec3(0.0f, 0.0f, -1.0f);    // Initial direction
float cameraSpeed = 0.05f;  // Movement speed per frame
float cameraRotation = 3.0f; // Rotation amount per frame (in degrees)

void handleCameraMovement(GLFWwindow* window) {
  // Check for key presses
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    cameraPosition += cameraSpeed * cameraLookAt;
  }
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    cameraPosition -= cameraSpeed * cameraLookAt;
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    // Convert degrees to radians for glm::rotate
    float radians = glm::radians(cameraRotation);
    glm::vec3 rotationAxis = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cameraLookAt);  // Rotate around Y-axis
    cameraLookAt = glm::rotate(cameraLookAt, radians, rotationAxis);
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    // Similar logic for right key press with negative rotation
  }
}

void updateViewMatrix() {
  // Calculate new view direction based on camera position and lookAt
  glm::vec3 viewDirection = cameraLookAt - cameraPosition;
  // Use glm::normalize to ensure proper direction vector
  viewDirection = glm::normalize(viewDirection);

  // Right vector (perpendicular to view direction and up vector)
  glm::vec3 right = glm::normalize(glm::cross(viewDirection, glm::vec3(0.0f, 1.0f, 0.0f)));

  // Up vector (perpendicular to right and view direction)
  glm::vec3 up = glm::normalize(glm::cross(right, viewDirection));

  // Construct view matrix
  glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + viewDirection, up);

  // ... use the view matrix for rendering
}

int main() {
  // ... other initialization code

  while (!glfwWindowShouldClose(window)) {
    // ... other rendering code

    handleCameraMovement(window);
    updateViewMatrix();

    // ... continue rendering using updated view matrix

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // ... other cleanup code

  return 0;
}

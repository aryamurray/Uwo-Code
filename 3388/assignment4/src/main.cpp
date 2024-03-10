#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>
#include <vector>
#include "LoadBitmap.hpp"

// Camera variables
glm::vec3 cameraPosition = glm::vec3(0.5f, 0.4f, 0.5f);
glm::vec3 cameraLookDirection = glm::vec3(0.0f, 0.0f, -1.0f); // Normalized
float cameraSpeed = 0.05f;
float cameraRotation = 3.0f;

struct VertexData
{
    float x, y, z;
    float nx = 0.0f, ny = 0.0f, nz = 0.0f; // Optional normal vector (default to 0)
    float r = 0.0f, g = 0.0f, b = 0.0f;    // Optional color (default to black)
    float u = 0.0f, v = 0.0f;              // Optional texture coordinates (default to 0)
};

// Struct to store face data (triangle indices)
struct TriData
{
    int indices[3]; // Array to store indices of 3 vertices
};

class TexturedMesh
{

private:
    // Bitmap and Ply variables
    std::vector<VertexData> vertices;
    std::vector<TriData> faces;
    unsigned char *texturePixels; // ARGB pixel data
    unsigned int textureWidth;
    unsigned int textureHeight;

    // OpenGL object IDs
    GLuint vertexBufferId;   // ID for vertex position VBO
    GLuint texCoordBufferId; // ID for texture coordinate VBO
    GLuint indexBufferId;    // ID for face index VBO
    GLuint textureId;        // ID for the texture object
    GLuint vaoId;            // ID for the Vertex Array Object
    GLuint shaderProgramId;  // ID for the shader program

public:
    TexturedMesh(const std::string &plyFilename, const std::string &textureFilename)
    {
        // Load geometry from PLY
        readPLYFile(plyFilename, vertices, faces);

        // Load Data from Bitmap
        loadARGB_BMP(plyFilename.c_str(), &texturePixels, &textureWidth, &textureHeight);

        // Initialize OpenGL objects
        glGenVertexArrays(1, &vaoId);
        glGenBuffers(1, &vertexBufferId);
        glGenBuffers(1, &texCoordBufferId);
        glGenBuffers(1, &indexBufferId);
        glGenTextures(1, &textureId);
    }

    void draw(glm::mat4 MVP)
    {
        // 1. Bind Shader Program
        glUseProgram(shaderProgramId);

        // 2. Set Uniform (MVP)
        GLint mvpUniformLocation = glGetUniformLocation(shaderProgramId, "MVP");
        glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, glm::value_ptr(MVP));

        // 3. Bind Texture
        glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
        glBindTexture(GL_TEXTURE_2D, textureId);

        // 4. Bind VAO
        glBindVertexArray(vaoId);

        // 5. Draw Elements
        glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, 0);

        // 6. Clean up (optional depending on your rendering loop)
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
    }

    // Accessors (you may want to add more)
    const std::vector<VertexData> &getVertices() const { return vertices; }
    const std::vector<TriData> &getFaces() const { return faces; }
    const unsigned char *getTexturePixels() const { return texturePixels; }
    int getTextureWidth() const { return textureWidth; }
    int getTextureHeight() const { return textureHeight; }
};

void readPLYFile(std::string fname, std::vector<VertexData> &vertices, std::vector<TriData> &faces)
{
    std::ifstream file(fname);

    if (!file.is_open())
    {
        std::cerr << "Error opening PLY file: " << fname << std::endl;
        return;
    }

    std::string line;

    // Skip header lines
    while (std::getline(file, line))
    {
        // Find first non-whitespace character
        size_t start = line.find_first_not_of(" \t\n\v\f\r");

        // Find last non-whitespace character
        size_t end = line.find_last_not_of(" \t\n\v\f\r");

        // Extract the trimmed substring (if line was not empty)
        if (start != std::string::npos)
        {
            std::string trimmed_line = line.substr(start, end - start + 1);

            if (trimmed_line == "end_header")
            {
                break;
            }
        }
    }

    // Read vertex data
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        VertexData vertex;

        // Mandatory properties
        if (!(iss >> vertex.x >> vertex.y >> vertex.z))
        {
            std::cerr << "Error reading vertex coordinates" << std::endl;
            return;
        }

        // Optional properties
        iss >> vertex.nx >> vertex.ny >> vertex.nz >> vertex.r >> vertex.g >> vertex.b >> vertex.u >> vertex.v;

        vertices.push_back(vertex);
    }

    // Read face data
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        TriData face;
        int numVertices;

        if (!(iss >> numVertices >> face.indices[0] >> face.indices[1] >> face.indices[2]))
        {
            std::cerr << "Error reading face data" << std::endl;
            return;
        }

        // Assuming triangles only (for now)
        faces.push_back(face);
    }
}

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

void pushTexturedMeshes(std::vector<TexturedMesh> &meshes)
{
    std::vector<std::string> plyFilenames = {"Bottles.ply", "Curtains.ply", "DoorBg.ply", "Floor.ply", "MetalObjects.ply", "Patio.ply", "Table.ply", "Walls.ply", "WindowBG.ply", "WoodObjects.ply"}; // Array of PLY filenames
    std::vector<std::string> bmpFilenames = {"bottles.bmp", "curtians.bmp", "doorbg.bmp", "floor.bmp", "metalobjects.bmp", "patio.bmp", "table.bmp", "walls.bmp", "windowbg.bmp", "woodobjects.bmp"}; // Array of texture filenames

    for (int i = 0; i < plyFilenames.size(); i++)
    {
        printf("[Init] Reading %s and %s ", "assets/LinksHouse/" + plyFilenames[i], "assets/LinksHouse/" + bmpFilenames[i]);
        meshes.push_back(TexturedMesh("assets/LinksHouse/" + plyFilenames[i], "assets/LinksHouse/" + bmpFilenames[i]));
    }
}

int main(void)
{
    // Declare needed variables
    GLFWwindow *window;
    std::vector<TexturedMesh> meshes;

    // Init the Meshes
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
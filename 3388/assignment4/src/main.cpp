#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "LoadBitmap.hpp"
#include "happly.h"


typedef struct VertexData
{
    float x, y, z;
    float nx = 0.0f, ny = 0.0f, nz = 0.0f; // Optional normal vector (default to 0)
    float r = 0.0f, g = 0.0f, b = 0.0f;    // Optional color (default to black)
    float u = 0.0f, v = 0.0f;              // Optional texture coordinates (default to 0)
} VertexData;

// Struct to store face data (triangle indices)
typedef struct TriData
{
    int indices[3]; // Array to store indices of 3 vertices
} TriData;

// Definitions
void readPLYFile(const std::string& filename, std::vector<VertexData>& vertices, std::vector<TriData>& faces);
// Camera variables
glm::vec3 cameraPosition = glm::vec3(0.5f, 0.4f, 0.5f);
glm::vec3 cameraLookDirection = glm::vec3(0.0f, 0.0f, -1.0f); // Normalized
float cameraSpeed = 0.05f;
float cameraRotation = 3.0f;

class TexturedMesh
{

private:
    // Bitmap and Ply variables
    std::vector<VertexData> vertices;
    std::vector<TriData> faces;
    unsigned char* texturePixels; // ARGB pixel data
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

        int j = 0;
        float strideverticies[vertices.size()*3];
        for (int i = 0; i < vertices.size(); i++){
            VertexData vert = vertices.at(i);

            strideverticies[j + 0] = vert.x;
            strideverticies[j + 1] = vert.y;
            strideverticies[j + 2] = vert.z;

            j+=3;
        }

        j = 0;
        float strideUV[vertices.size()*2];
        for (int i = 0; i < vertices.size(); i++){
            VertexData vert = vertices.at(i);

            strideUV[j + 0] = vert.u;
            strideUV[j + 1] = vert.v;

            j+=2;
        }


        // Load Data from Bitmap
        const char* texturebmp = textureFilename.c_str();
        printf("\n\n %s\n\n", texturebmp);
        loadARGB_BMP(texturebmp, &texturePixels, &textureWidth, &textureHeight);

        // Initialize OpenGL objects
        glGenVertexArrays(1, &vaoId);
        glGenBuffers(1, &vertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER,vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER,sizeof(strideverticies),strideverticies,GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3,GL_FLOAT,false,0,(GLvoid*)0);

        glGenBuffers(1, &texCoordBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, texCoordBufferId);
        glBufferData(GL_ARRAY_BUFFER,sizeof(strideUV),strideUV,GL_STATIC_DRAW);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,2,GL_FLOAT,false,0,(GLvoid*)0);

        glGenBuffers(1, &indexBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faces.data()),faces.data(), GL_STATIC_DRAW);
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId); // Bind the texture obj to GL_TEXTURE_2D
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, texturePixels);



        // Create the shaders
        GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
        std::string VertexShaderCode = "\
    	#version 330 core\n\
		// Input vertex data, different for all executions of this shader.\n\
		layout(location = 0) in vec3 vertexPosition;\n\
		layout(location = 1) in vec2 uv;\n\
		// Output data ; will be interpolated for each fragment.\n\
		out vec2 uv_out;\n\
		// Values that stay constant for the whole mesh.\n\
		uniform mat4 MVP;\n\
		void main(){ \n\
			// Output position of the vertex, in clip space : MVP * position\n\
			gl_Position =  MVP * vec4(vertexPosition,1);\n\
			// The color will be interpolated to produce the color of each fragment\n\
			uv_out = uv;\n\
		}\n";

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode = "\
		#version 330 core\n\
		in vec2 uv_out; \n\
		uniform sampler2D tex;\n\
		void main() {\n\
			gl_FragColor = texture(tex, uv_out);\n\
		}\n";
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    // Compile Fragment Shader
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    shaderProgramId = glCreateProgram();

    glAttachShader(shaderProgramId, VertexShaderID);
    glAttachShader(shaderProgramId, FragmentShaderID);
    glLinkProgram(shaderProgramId);

    }

    void draw(glm::mat4 MVP) const
    {
        
        // 1. Bind Shader Program
        glUseProgram(shaderProgramId);

        // 2. Set Uniform (MVP)
        GLint mvpUniformLocation = glGetUniformLocation(shaderProgramId, "MVP");
        glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, glm::value_ptr(MVP));

        // 3. Bind Texture
        glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
        glEnable(GL_TEXTURE_2D);
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

void readPLYFile(const std::string& filename, std::vector<VertexData>& vertices, std::vector<TriData>& faces) {
    happly::PLYData plyIn(filename);

    // Get vertex data 
    std::vector<std::array<double, 3>> positions = plyIn.getVertexPositions();
    std::vector<std::vector<size_t>> faceIndices = plyIn.getFaceIndices<size_t>(); 
    auto u = plyIn.getElement("vertex").getProperty<float>("u");
    auto v = plyIn.getElement("vertex").getProperty<float>("v");
    // Populate vertices
    int i = 0;
    for (const auto& pos : positions) {
        VertexData vertex;
        vertex.x = pos[0];
        vertex.y = pos[1];
        vertex.z = pos[2];
        vertex.u = u[i];
        vertex.v = v[i];
        i+=1;
        vertices.push_back(vertex);
    }

    // Populate faces 
    for (const auto& face : faceIndices) {
        TriData tri;
        tri.indices[0] = face[0];
        tri.indices[1] = face[1];
        tri.indices[2] = face[2];
        faces.push_back(tri);
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
    std::vector<std::string> plyFilenames = {"Bottles.ply", "Curtains.ply", "DoorBG.ply", "Floor.ply", "MetalObjects.ply", "Patio.ply", "Table.ply", "Walls.ply", "WindowBG.ply", "WoodObjects.ply"}; // Array of PLY filenames
    std::vector<std::string> bmpFilenames = {"bottles.bmp", "curtains.bmp", "doorbg.bmp", "floor.bmp", "metalobjects.bmp", "patio.bmp", "table.bmp", "walls.bmp", "windowbg.bmp", "woodobjects.bmp"}; // Array of texture filenames

    for (int i = 0; i < (int) plyFilenames.size(); i++)
    {
        std::cout << "[Info] Reading " << plyFilenames[i] << " and " << bmpFilenames[i].c_str();
        meshes.push_back(TexturedMesh( plyFilenames[i],bmpFilenames[i]));
    }
}

int main(void)
{
    // Declare needed variables
    GLFWwindow *window;
    std::vector<TexturedMesh> meshes;

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

    // Init GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Error initializing GLEW\n";
        return -1;
    }

    // Init the Meshes
    pushTexturedMeshes(meshes);
    printf("[Init] Finished Reading Textures\n");

    

    // Projection Matrix Setup 
    glm::mat4 projectionMatrix = glm::perspective(
        glm::radians(45.0f),    // 45-degree vertical field of view
        (float)640 / 640,  // Aspect ratio (assuming 'width' and 'height' of window)
        0.1f,                   // Near clipping plane
        100.0f                  // Far clipping plane
    );

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();

        /* Render here */
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        // Render each mesh
        for (const auto& mesh : meshes) {
            // Calculate Model-View-Projection (MVP) matrix
            glm::mat4 modelMatrix = glm::mat4(1.0f); // Adjust for mesh's position/rotation
            // glm::mat4 viewMatrix = ...;  // Update based on your camera 
            glm::mat4 MVP = projectionMatrix * modelMatrix;  

            // Draw the mesh
            mesh.draw(MVP); 
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
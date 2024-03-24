#include "TriTable.hpp"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <functional>
#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>


#define FRONT_TOP_LEFT     128
#define FRONT_TOP_RIGHT     64
#define BACK_TOP_RIGHT      32
#define BACK_TOP_LEFT       16
#define FRONT_BOTTOM_LEFT    8
#define FRONT_BOTTOM_RIGHT   4
#define BACK_BOTTOM_RIGHT    2
#define BACK_BOTTOM_LEFT     1

float scalarField(float x, float y, float z)
{
    return (pow(x, 2) + pow(y, 2) + pow(z, 2));
}

const char *texturedVertexShader = R"glsl(
#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
out vec3 normal;
out vec3 eye_direction;
out vec3 light_direction;
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 lightDir;
void main() {
    gl_Position =  MVP * vec4(vertexPosition,1);
    normal = mat3(V) * vertexNormal;
    eye_direction = vec3(5, 5, 5) - (V * vec4(-vertexPosition, 1)).xyz;
    light_direction = (mat3(V) * lightDir) + eye_direction;
}
)glsl";

const char *texturedFragShader = R"glsl(
#version 330 core
in vec3 normal;
in vec3 light_direction;
in vec3 eye_direction;
out vec4 color;
uniform vec4 modelColor;
void main() {
    vec4 diffuseColor = modelColor;
    vec4 ambient = vec4(0.2, 0.2, 0.2, 1) * diffuseColor; // Reduced ambient intensity
    vec4 specular = vec4(1, 1, 1, 1); // Reduced specular intensity
    float alpha = 64;
    vec3 n = normalize(normal);
    vec3 l = normalize(light_direction);
    vec3 e = normalize(eye_direction);
    vec3 r = reflect(-l, n);
    float cosTheta = clamp(dot(n, l), 0, 1);
    float cosAlpha = clamp(dot(e, r), 0, 1);
    color = ambient + (modelColor * cosTheta * 0.8) + (specular * pow(cosAlpha, alpha)); // Reduced diffuse intensity
}
)glsl";

std::vector<float> marching_cubes(std::function<float(float, float, float)> f, float isovalue, float min, float max, float stepsize)
{
}

class Camera
{

private:
    float moveSpeed = 0.05f;
    float rotateSpeed = 1.0f;

    glm::mat4 view = glm::lookAt(glm::vec3(5, 5, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.001f, 1000.0f);
    glm::mat4 model = glm::mat4(1.0f);

    glm::mat4 mvp = view * projection * model;

    float radius;
    float theta;
    float phi;

public:
    Camera(float initialRadius, float initialTheta, float initialPhi){
        radius = initialRadius;
        theta = initialTheta;
        phi = initialPhi;
    }

    void update(GLFWwindow *window);
    glm::mat4 getViewMatrix() const;
};

class Cube
{
    float minimum;
    float maximum;

    glm::vec3 xline = glm::vec3(1,0,0);
    glm::vec3 yline = glm::vec3(0,1,0);
    glm::vec3 zline = glm::vec3(0,0,1);

    public:
    Cube(float minimum, float maximum){
        minimum = minimum + 0.1;
        maximum = maximum + 0.1;
    }

    void draw(){


        glMatrixMode(GL_MODELVIEW);
    }

    
};


class MarchingCubes{
    std::function<float(float, float, float)> generationFunction;
    float isoValue = 0;     // the iso value for the mesh
    float min_coord = 0;    // the minimum coordinate of the mesh
    float max_coord = 1;    // the maximum coordinate of the mesh
    float stepSize = 0.1;   // the step size for the marching cube algorithm
    float current_iteration = 0;    // the current iteration of the marching cube algorithm
    std::vector<float> vertices;    // the vertices of the mesh

    void add_triangles(int* verts, float x, float y, float z) { // add the triangles to the vertices
        for (int i = 0; verts[i] >= 0; i += 3) {    // for each triangle
            for (int j = 0; j < 3; ++j) {   // for each vertex
                vertices.emplace_back(x + stepSize * vertTable[verts[i + j]][0]);   // add the vertex
                vertices.emplace_back(y + stepSize * vertTable[verts[i + j]][1]);
                vertices.emplace_back(z + stepSize * vertTable[verts[i + j]][2]);
            }
        }
    }
    void generate_iterative_new() { // generate the mesh
        float ftl, ftr, fbr, fbl, btl, btr, bbr, bbl;
        int which = 0;
        int *verts;
        for (float a = min_coord; a < max_coord; a += stepSize) {   // for each square
            for (float b = min_coord; b < max_coord; b += stepSize) {
                bbl = generationFunction(a, b, current_iteration);  // get the values of the corners
                btl = generationFunction(a, b + stepSize, current_iteration);
                btr = generationFunction(a + stepSize, b + stepSize, current_iteration);
                bbr = generationFunction(a + stepSize, b, current_iteration);
                fbl = generationFunction(a, b, current_iteration + stepSize);
                ftl = generationFunction(a, b + stepSize, current_iteration + stepSize);
                ftr = generationFunction(a + stepSize, b + stepSize, current_iteration + stepSize);
                fbr = generationFunction(a + stepSize, b, current_iteration + stepSize);
                which = 0;
                if (ftl < isoValue) {   // get the corners that are below the iso value
                    which |= FRONT_TOP_LEFT;
                }
                if (ftr < isoValue) {
                    which |= FRONT_TOP_RIGHT;
                }
                if (btr < isoValue) {
                    which |= BACK_TOP_RIGHT;
                }
                if (btl < isoValue) {
                    which |= BACK_TOP_LEFT;
                }
                if (fbl < isoValue) {
                    which |= FRONT_BOTTOM_LEFT;
                }
                if (fbr < isoValue) {
                    which |= FRONT_BOTTOM_RIGHT;
                }
                if (bbr < isoValue) {
                    which |= BACK_BOTTOM_RIGHT;
                }
                if (bbl < isoValue) {
                    which |= BACK_BOTTOM_LEFT;
                }
                verts = marching_cubes_lut[which];
                add_triangles(verts, a, b, current_iteration);
            }
        }
        current_iteration += stepSize;
        if (current_iteration > max_coord){
            finished = true;
        }
    }

std::vector<float> compute_normals(std::vector<float> vertices){    // compute the normals of the mesh
}

public:
bool finished = false;  // if the mesh generation is finished

MarchingCubes(std::function<float(float, float, float)> f, float isoval, float min, float max, float step)
: generationFunction(f), isoValue(isoval), min_coord(min), max_coord(max), stepSize(step), current_iteration(min) {}
void generate() {
    generate_iterative_new();
}
std::vector<float> get_vertices() {
    return vertices;
}
};

void writePLY(std::vector<float> vertex_data, std::vector<float> normal_data) { 

    FILE *plyFile = fopen("Rendering.ply", "w");
    if (plyFile == NULL) { // if the file could not be opened
        return;
    }

    int vertexLength = vertex_data.size();
    float* vertexData = vertex_data.data();
    float* normalData = normal_data.data();


    fprintf(plyFile, "ply\n"); // write the header
    fprintf(plyFile, "format ascii 1.0\n");
    fprintf(plyFile, "element vertex %d\n",  vertex_data.size());
    fprintf(plyFile, "property float x\n");
    fprintf(plyFile, "property float y\n");
    fprintf(plyFile, "property float z\n");
    fprintf(plyFile, "property float nx\n");
    fprintf(plyFile, "property float ny\n");
    fprintf(plyFile, "property float nz\n");
    fprintf(plyFile, "element face %d\n",  vertex_data.size()/3);
    fprintf(plyFile, "property list uchar uint vertex_indices\n");
    fprintf(plyFile, "end_header\n");


    for (int idx = 0; idx < vertexLength; idx += 3) { 
        fprintf(plyFile, "%f %f %f %f %f %f\n",
                *(vertexData + idx), *(vertexData + idx + 1), *(vertexData + idx + 2),
                *(normalData + idx), *(normalData + idx + 1), *(normalData + idx + 2));
    }

    // Write face data
    for (int idx = 0; idx < vertex_data.size(); idx += 3) {
        fprintf(plyFile, "3 %d %d %d\n", idx, idx + 1, idx + 2);
    }

    fclose(plyFile);
}

int programInit(GLFWwindow *window)
{
    /* Initialize the library */
    if (!glfwInit()){
        printf("[Error] Failed to init GLFW\n");
        return -1;
    }
        

    glfwWindowHint(GLFW_SAMPLES, 4); // Set the 4x Sampling
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1200, 1200, "Marching Cubes", NULL, NULL);
    if (!window)
    {
        printf("[Error] Failed to create window\n");
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

    return 0;
}

void initShaders(GLFWwindow* window, MarchingCubes marchingCubes,std::vector<float> normals,  GLuint program_ID, GLuint vao, GLuint vertex_VBO, GLuint normal_VBO)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vertex_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, &marchingCubes.get_vertices()[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        (void *)0);
    glGenBuffers(1, &normal_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, &normals, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        (void *)0);
    glBindVertexArray(0);

    GLuint vertex_shader_ID = glCreateShader(GL_VERTEX_SHADER); // create and compile shaders
    GLuint fragment_shader_ID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertex_shader_ID, 1, &texturedVertexShader, NULL);
    glCompileShader(vertex_shader_ID);
    glShaderSource(fragment_shader_ID, 1, &texturedFragShader, NULL);
    glCompileShader(fragment_shader_ID);
    program_ID = glCreateProgram();
    glAttachShader(program_ID, vertex_shader_ID);
    glAttachShader(program_ID, fragment_shader_ID);
    glLinkProgram(program_ID);
    glDetachShader(program_ID, vertex_shader_ID);
    glDetachShader(program_ID, fragment_shader_ID);
    glDeleteShader(vertex_shader_ID);
    glDeleteShader(fragment_shader_ID);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);

    return;
}

int main(void)
{
    // Declare needed variables
    GLFWwindow *window;

    if (programInit(window) != GLEW_OK)
    {
        std::cerr << "OpenGL failed to initialize.\n\n";
    }

    std::vector<float> normals;
    float step = 0.05;
    float min = -5.0f;
    float max = 5.0f;
    float isoval = -1.5;
    bool generateFile = true;


    MarchingCubes cubes(scalarField, isoval, min, max, step);
    Cube drawCube(min,max);

    GLuint program_ID, vao, vertex_VBO, normal_VBO;
    initShaders(window,cubes,normals,program_ID,vao,vertex_VBO,normal_VBO);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();

        /* Render here */
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
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



GLfloat scalarField(float x, float y, float z)
{
    return x*x - y*y -z*z -z;
}

std::vector<float> computeNormals(const std::vector<float>& vertices) {
    std::vector<float> normals;
    for (size_t i = 0; i < vertices.size(); i += 9) {
        glm::vec3 p1(vertices[i], vertices[i + 1], vertices[i + 2]);
        glm::vec3 p2(vertices[i + 3], vertices[i + 4], vertices[i + 5]);
        glm::vec3 p3(vertices[i + 6], vertices[i + 7], vertices[i + 8]);

        glm::vec3 edge1 = p2 - p1;
        glm::vec3 edge2 = p3 - p1;

        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        for (int j = 0; j < 3; ++j) {
            normals.insert(normals.end(), {normal.x, normal.y, normal.z});
        }
    }
    return normals;
}

class Camera
{

private:
    GLfloat rotateSpeed = 0.7f;
    GLfloat moveSpeed = 0.10f;
    

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

    void update(GLFWwindow *window){

        glm::vec3 camPos(radius * sin(glm::radians(theta)) * cos(glm::radians(phi)),radius * sin(glm::radians(phi)),radius * cos(glm::radians(theta)) * cos(glm::radians(phi)));
        
        static double last_xpos = -1, last_ypos = -1;   // last cursor position
        double xcursorpos, ycursorpos;
        glfwGetCursorPos(window, &xcursorpos, &ycursorpos);         // current cursor position
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { // rotate camera
            if (last_xpos >= 0 && last_ypos >= 0) { // if cursor was moved
                theta -= rotateSpeed * float(xcursorpos - last_xpos);    // rotate around y-axis
                phi += rotateSpeed * float(ycursorpos - last_ypos);    // rotate around x-axis
                phi = glm::clamp(phi, -89.0f, 89.0f);
            }
        }
        last_xpos = xcursorpos;   // update last cursor position
        last_ypos = ycursorpos;   // update last cursor position


        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {  
            theta += rotateSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {   
            radius = std::max(radius - moveSpeed, 0.1f);
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            theta -= rotateSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        { 
            radius += moveSpeed;
        }
        
        camPos.x = radius * sin(glm::radians(theta)) * cos(glm::radians(phi));
        camPos.y = radius * sin(glm::radians(phi));
        camPos.z = radius * cos(glm::radians(theta)) * cos(glm::radians(phi));
    
        view = glm::lookAt(camPos, camPos+glm::normalize(-camPos),glm::vec3(0,1,0));
    }

    glm::mat4* getMVP(){
        return &mvp;
    }

    glm::mat4* getView(){
        return &view;
    }

    glm::mat4* getProjection(){
        return &projection;
    }

};

class Container {
    float min;
    float max;

    float axisExtension = 1; 
    float arrowSize = 0.5f;  

public:
    Container(float min, float max) : min(min+0.1), max(max+0.1) {};
    
    void draw() {
 
        glMatrixMode(GL_MODELVIEW); 
        glPushMatrix();

        glLineWidth(3.0f);

        glBegin(GL_TRIANGLES);  // draw the arrows

        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        glVertex3f(max + axisExtension, min, min);
        glVertex3f(max + axisExtension - arrowSize, min + arrowSize/2, min);
        glVertex3f(max + axisExtension - arrowSize, min - arrowSize/2, min);

        glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
        glVertex3f(min, min, max + axisExtension);
        glVertex3f(min, min + arrowSize/2, max + axisExtension - arrowSize);
        glVertex3f(min, min - arrowSize/2, max + axisExtension - arrowSize);

        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        glVertex3f(min, max + axisExtension, min);
        glVertex3f(min + arrowSize/2, max + axisExtension - arrowSize, min);
        glVertex3f(min - arrowSize/2, max + axisExtension - arrowSize, min);

        

        glEnd();
        glBegin(GL_LINES);
        // draw the axis
        glColor4f(1.0, 0.0f, 0.0f, 1.0f);
        glVertex3f(min, min, min);
        glVertex3f(max + axisExtension, min, min);

        glColor4f(0.0, 1.0f, 0.0f, 1.0f);
        glVertex3f(min, min, min);
        glVertex3f(min, max + axisExtension, min);

        glColor4f(0.0, 0.0f, 1.0f, 1.0f);
        glVertex3f(min, min, min);
        glVertex3f(min, min, max + axisExtension);

        glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
        glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
        glVertex3f(min, max, min);
        glVertex3f(max, max, min);

        glVertex3f(max, min, min);
        glVertex3f(max, max, min);

        glVertex3f(max, min, min);
        glVertex3f(max, min, max);

        glVertex3f(max, min, max);
        glVertex3f(max, max, max);

        glVertex3f(max, max, min);
        glVertex3f(max, max, max);

        glVertex3f(min, max, min);
        glVertex3f(min, max, max);

        glVertex3f(min, max, max);
        glVertex3f(min, min, max);

        glVertex3f(min, min, max);
        glVertex3f(max, min, max);

        glVertex3f(min, max, max);
        glVertex3f(max, max, max);

        glEnd();


        glPopMatrix();
    }
};


class MarchingCubes {   
    std::function<float(float, float, float)> generationFunction;
    float isoValue = 0;     
    float min_coord = 0;    
    float max_coord = 1;    
    float stepSize = 0.1;   
    float iter = 0;    
    std::vector<float> vertices;  
    bool finished = false; 

    

   void genMesh()
{
    for (float i = min_coord; i < max_coord; i += stepSize)
    {
        for (float j = min_coord; j < max_coord; j += stepSize)
        {
            std::array<float, 8> cornerValues;
            // Get the values of the corners
            cornerValues[0] = generationFunction(i, j, iter);
            cornerValues[1] = generationFunction(i, j + stepSize, iter);
            cornerValues[2] = generationFunction(i + stepSize, j + stepSize, iter);
            cornerValues[3] = generationFunction(i + stepSize, j, iter);
            cornerValues[4] = generationFunction(i, j, iter + stepSize);
            cornerValues[5] = generationFunction(i, j + stepSize, iter + stepSize);
            cornerValues[6] = generationFunction(i + stepSize, j + stepSize, iter + stepSize);
            cornerValues[7] = generationFunction(i + stepSize, j, iter + stepSize);

            int which = 0;
            // Determine which corners are below the iso value
            for (int k = 0; k < 8; ++k)
            {
                if (cornerValues[k] < isoValue)
                {
                    which |= (1 << k);
                }
            }

            int *verts = marching_cubes_lut[which];
            addTri(verts, i, j, iter);
        }
    }

    iter += stepSize;
    if (iter > max_coord)
    {
        finished = true;
    }
}

    void addTri(int* verts, float x, float y, float z)
{
    int i = 0;
    while (verts[i] >= 0)
    {
        for (int j = 0; j < 3; ++j)
        {
            int vertIndex = verts[i + j];
            if (vertIndex >= 0)
            {
                float vx = x + stepSize * vertTable[vertIndex][0];
                float vy = y + stepSize * vertTable[vertIndex][1];
                float vz = z + stepSize * vertTable[vertIndex][2];
                vertices.push_back(vx);
                vertices.push_back(vy);
                vertices.push_back(vz);
            }
        }
        i += 3;
    }
}


    public:


    MarchingCubes(std::function<float(float, float, float)> f, float isoval, float min, float max, float step)
    : generationFunction(f), isoValue(isoval), min_coord(min), max_coord(max), stepSize(step), iter(min) {}

    void generate() {
        genMesh();
    }
    std::vector<float> getVert() {
        return vertices;
    }

    bool getFinished(){
        return finished;
    }
};


void writePLY(std::string filename,std::vector<float> vertex_data, std::vector<float> normal_data) { 

    FILE *plyFile = fopen(filename.c_str(), "w");
    if (plyFile == NULL) { // if the file could not be opened
        return;
    }

    int vertexLength = vertex_data.size();
    float* vertexData = vertex_data.data();
    float* normalData = normal_data.data();


    fprintf(plyFile, "ply\n"); // write the header
    fprintf(plyFile, "format ascii 1.0\n");
    fprintf(plyFile, "element vertex %d\n",vertex_data.size());
    fprintf(plyFile, "property float x\n");
    fprintf(plyFile, "property float y\n");
    fprintf(plyFile, "property float z\n");
    fprintf(plyFile, "property float nx\n");
    fprintf(plyFile, "property float ny\n");
    fprintf(plyFile, "property float nz\n");
    fprintf(plyFile, "element face %d\n",  vertex_data.size()/3);
    fprintf(plyFile, "property list uchar uint vertex_indices\n");
    fprintf(plyFile, "end_header\n");


    //Write Vertex Data
    for (int i = 0; i < vertexLength; i += 3) { 
        fprintf(plyFile, "%f %f %f %f %f %f\n",
                *(vertexData + i), *(vertexData + i + 1), *(vertexData + i + 2),
                *(normalData + i), *(normalData + i + 1), *(normalData + i + 2));
    }

    // Write face data
    for (int i = 0; i < vertex_data.size(); i += 3) {
        fprintf(plyFile, "3 %d %d %d\n", i, i + 1, i + 2);
    }

    fclose(plyFile);
}


int main(void)
{
    // Declare needed variables
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit()){
        printf("[Error] Failed to init GLFW\n");
        return -1;
    }
        
    glfwWindowHint(GLFW_SAMPLES, 8); 
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
    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        printf("Error initializing GLEW\n");
        return -1;
    }

    glClearColor(0.2, 0.2, 0.2, 0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    std::vector<float> normals;
    float step = 0.05;
    float min = -5.0f;
    float max = 5.0f;
    float isoval = -1.5;
    bool generateFile = true;

    glClearColor(0.2, 0.2, 0.2, 0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);


    MarchingCubes cubes(scalarField, isoval, min, max, step);
    Container container(min,max);
    Camera camera(30,45,45);

    bool wroteFile = false;

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.update(window);


        glm::mat4 mvp = *camera.getMVP();
        glm::mat4 view = *camera.getView();
        glm::mat4 projection = *camera.getProjection();

    
        if (!cubes.getFinished()) {  
            cubes.generate();
            std::vector<float> vertices = cubes.getVert();
            normals = computeNormals(vertices);

            glMatrixMode(GL_PROJECTION);    
            glPushMatrix();
            glLoadMatrixf(glm::value_ptr(projection));
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadMatrixf(glm::value_ptr(view));

            glPointSize(5.0);

            // Set color
            glColor3f(0.0,1.0,1.0); // Red color

            // Begin drawing points
            glBegin(GL_POINTS);

            for (int i = 0; i< vertices.size();i+=3){
                glVertex3d(vertices[i],vertices[i+1],vertices[i+2]);
            }


            // End drawing points
            glEnd();

        } else if (!wroteFile && generateFile) { // if finished, write file
            printf("Hit rendering finish");
            std::vector<float> vertices = cubes.getVert();
            normals = computeNormals(vertices);
            writePLY("rendering.ply", vertices, normals);
            wroteFile = true;
        }
        else if(cubes.getFinished()){
            std::vector<float> vertices = cubes.getVert();
            normals = computeNormals(vertices);

            glMatrixMode(GL_PROJECTION);    
            glPushMatrix();
            glLoadMatrixf(glm::value_ptr(projection));
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadMatrixf(glm::value_ptr(view));

            glPointSize(5.0);

            // Set color
            glColor3f(0.0,1.0,1.0); // Red color
    
            // Begin drawing points
            glBegin(GL_POINTS);
    
            for (int i = 0; i< vertices.size();i+=3){
                glVertex3d(vertices[i],vertices[i+1],vertices[i+2]);
            }
        
    
            // End drawing points
            glEnd();
        }

        glMatrixMode(GL_PROJECTION);    
        glPushMatrix();
        glLoadMatrixf(glm::value_ptr(projection));
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadMatrixf(glm::value_ptr(view));
        container.draw();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    return 0;
}
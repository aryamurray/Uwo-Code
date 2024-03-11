// Cole Sweet
// 251179642
// Assignment 4
// March 2023

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>   // Used for vectors
#include <string>   // Used for strings
#include <fstream>  // Used for file reading
#include <iostream> // Used for output

// Include GLEW
#include <GL/glew.h>
// Include GLUT
#include <GL/glut.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// GLFW Window reference
GLFWwindow *window;

// Given BMP file loader

/**
 * Given a file path imagepath, read the data in that bitmapped image
 * and return the raw bytes of color in the data pointer.
 * The width and height of the image are returned in the weight and height pointers,
 * respectively.
 *
 * usage:
 *
 * unsigned char* imageData;
 * unsigned int width, height;
 * loadARGB_BMP("mytexture.bmp", &imageData, &width, &height);
 *
 * Modified from https://github.com/opengl-tutorials/ogl.
 */
void loadARGB_BMP(const char *imagepath, unsigned char **data, unsigned int *width, unsigned int *height)
{

    printf("Reading image %s\n", imagepath);

    // Data read from the header of the BMP file
    unsigned char header[54];
    unsigned int dataPos;
    unsigned int imageSize;
    // Actual RGBA data

    // Open the file
    FILE *file = fopen(imagepath, "rb");
    if (!file)
    {
        printf("%s could not be opened. Are you in the right directory?\n", imagepath);
        getchar();
        return;
    }

    // Read the header, i.e. the 54 first bytes

    // If less than 54 bytes are read, problem
    if (fread(header, 1, 54, file) != 54)
    {
        printf("Not a correct BMP file1\n");
        fclose(file);
        return;
    }

    // Read the information about the image
    dataPos = *(int *)&(header[0x0A]);
    imageSize = *(int *)&(header[0x22]);
    *width = *(int *)&(header[0x12]);
    *height = *(int *)&(header[0x16]);
    // A BMP files always begins with "BM"
    if (header[0] != 'B' || header[1] != 'M')
    {
        printf("Not a correct BMP file2\n");
        fclose(file);
        return;
    }
    // Make sure this is a 32bpp file
    if (*(int *)&(header[0x1E]) != 3)
    {
        printf("Not a correct BMP file3\n");
        fclose(file);
        return;
    }
    // fprintf(stderr, "header[0x1c]: %d\n", *(int*)&(header[0x1c]));
    // if ( *(int*)&(header[0x1C])!=32 ) {
    //     printf("Not a correct BMP file4\n");
    //     fclose(file);
    //     return;
    // }

    // Some BMP files are misformatted, guess missing information
    if (imageSize == 0)
        imageSize = (*width) * (*height) * 4; // 4 : one byte for each Red, Green, Blue, Alpha component
    if (dataPos == 0)
        dataPos = 54; // The BMP header is done that way

    // Create a buffer
    *data = new unsigned char[imageSize];

    if (dataPos != 54)
    {
        fread(header, 1, dataPos - 54, file);
    }

    // Read the actual data from the file into the buffer
    fread(*data, 1, imageSize, file);

    // Everything is in memory now, the file can be closed.
    fclose(file);
}

//////////////////////////////////////////////////////////////////////////////
// Classes
//////////////////////////////////////////////////////////////////////////////

// VertexData class
// Stores vertex attributes
class VertexData
{
public:
    // x, y, z vertex position
    // nx, ny, nz vertex normal
    // r, g, b vertex color
    // u, v texture coordinates
    float x, y, z, nx, ny, nz, r, g, b, u, v;

    // Vertex data constructor
    // Initializes vertex attributes
    VertexData()
    {
        // Init position to (0,0,0)
        this->x = 0.0;
        this->y = 0.0;
        this->z = 0.0;

        // Init normal to (0,0,0)
        this->nx = 0.0;
        this->ny = 0.0;
        this->nz = 0.0;

        // Init color to (1,1,1) White
        this->r = 1.0;
        this->g = 1.0;
        this->b = 1.0;

        // Init texture coordinates to (0,0)
        this->u = 0.0;
        this->v = 0.0;
    }

    // Printing << operator overload
    friend std::ostream &operator<<(std::ostream &os, const VertexData &vd);
};

// Printing << operator overload implementation
std::ostream &operator<<(std::ostream &os, const VertexData &vd)
{
    os << "xyz(" << vd.x << ", " << vd.y << ", " << vd.z << ") "
       << "n(" << vd.nx << ", " << vd.ny << ", " << vd.nz << ") "
       << "rgb(" << vd.r << ", " << vd.g << ", " << vd.b << ") "
       << "uv(" << vd.u << ", " << vd.v << ")";
    return os;
}

// TriData class
// Stores 3 indices that makes up a triangle.
class TriData
{
public:
    // Vertex indices
    int vert1, vert2, vert3;

    // TriData constructors
    TriData(int _1, int _2, int _3)
    {
        this->vert1 = _1;
        this->vert2 = _2;
        this->vert3 = _3;
    }

    // Printing << operator overload
    friend std::ostream &operator<<(std::ostream &os, const TriData &td);
};

// Printing << operator overload implementation
std::ostream &operator<<(std::ostream &os, const TriData &td)
{
    os << "face(" << td.vert1 << ", " << td.vert2 << ", " << td.vert3 << ")";
    return os;
}

// Split a string into a vector of strings by a string delimiter
// converts "this is a test string" => std::vector("this", "is", "a", "test", "string")
std::vector<std::string> getLineAsVector(std::string line)
{
    // Vector of the split strings
    std::vector<std::string> lineValues;
    int pos = 0;
    std::string delimiter = " "; // Split the string by spaces " "
    std::string indexValue;

    // Loop through the string and parse into the vector
    while ((pos = line.find(delimiter)) != std::string::npos)
    {
        indexValue = line.substr(0, pos);
        lineValues.push_back(indexValue);
        line.erase(0, pos + delimiter.length());
    }

    // Get the last value in the string
    indexValue = line.substr(0, pos);
    lineValues.push_back(indexValue);

    // Return the vector strings.
    return lineValues;
}

// Read in a .ply file and parse it into vectors of VertexData and TriData
void readPLYFile(std::string fname, std::vector<VertexData> &vertices, std::vector<TriData> &faces)
{
    // Stores if the file header has been fully read and if the data portion has been reached in the file.
    bool dataBegan = false;
    // Stores the line position in the data.
    int dataPosition = 0;

    // Keep track of how many vertices and faces.
    int vertCount;
    int faceCount;

    // Open the file at the given filename parameter
    std::fstream newfile;
    newfile.open(fname, std::ios::in);

    // If the file is open, start parsing it.
    if (newfile.is_open())
    {
        std::string line;                        // The current line of the file being read.
        std::vector<std::string> vertProperties; // A list of the vertex properties in the data

        // Continue reading lines in the file until there are no lines left.
        while (getline(newfile, line))
        {
            // Get the line as a vector.
            std::vector<std::string> lineValues = getLineAsVector(line);

            // If the line has 3 words
            if (lineValues.size() == 3)
            {
                // If the line is an element line
                if (lineValues.at(0).compare("element") == 0)
                {
                    // If the element is a vertex
                    if (lineValues.at(1).compare("vertex") == 0)
                    {
                        // Parse the number of vertices and store it.
                        vertCount = stoi(lineValues.at(2));

                        // Read the next line
                        getline(newfile, line);
                        lineValues = getLineAsVector(line);

                        // Continue to read properties while the line is a property line
                        while (lineValues.at(0).compare("property") == 0)
                        {
                            // Read the property's name and put it at the back of the property vector
                            // property type name
                            // This stores the properties read in the correct order
                            vertProperties.push_back(lineValues.at(2));
                            // Read the next line
                            getline(newfile, line);
                            lineValues = getLineAsVector(line);
                        }
                    }
                    // If the element is a face
                    else if (lineValues.at(1).compare("face") == 0)
                    {
                        // Parse the number of faces and store it.
                        faceCount = stoi(lineValues.at(2));
                    }
                }
            }
            else
            {
                // If the line is the end of the header, the data has now started
                if (lineValues.at(0).compare("end_header") == 0)
                {
                    dataBegan = true;
                }
                else if (dataBegan)
                {

                    // Vert data
                    if (dataPosition < vertCount)
                    {
                        // Parse the data line
                        lineValues = getLineAsVector(line);
                        // Create a new VertexData object
                        VertexData v = VertexData();

                        // Loop through every vertex property.
                        for (int i = 0; i < vertProperties.size(); i++)
                        {

                            // If the current vertex property shares a name
                            // with the given properties, store it into the
                            // VertexData object.

                            // This will read the data from the line in the correct order.

                            if (vertProperties.at(i).compare("x") == 0)
                            {
                                v.x = stof(lineValues.at(i));
                            }
                            else if (vertProperties.at(i).compare("y") == 0)
                            {
                                v.y = stof(lineValues.at(i));
                            }
                            else if (vertProperties.at(i).compare("z") == 0)
                            {
                                v.z = stof(lineValues.at(i));
                            }

                            else if (vertProperties.at(i).compare("nx") == 0)
                            {
                                v.nx = stof(lineValues.at(i));
                            }
                            else if (vertProperties.at(i).compare("ny") == 0)
                            {
                                v.ny = stof(lineValues.at(i));
                            }
                            else if (vertProperties.at(i).compare("nz") == 0)
                            {
                                v.nz = stof(lineValues.at(i));
                            }

                            else if (vertProperties.at(i).compare("r") == 0)
                            {
                                v.r = stof(lineValues.at(i));
                            }
                            else if (vertProperties.at(i).compare("g") == 0)
                            {
                                v.g = stof(lineValues.at(i));
                            }
                            else if (vertProperties.at(i).compare("b") == 0)
                            {
                                v.b = stof(lineValues.at(i));
                            }

                            else if (vertProperties.at(i).compare("u") == 0)
                            {
                                v.u = stof(lineValues.at(i));
                            }
                            else if (vertProperties.at(i).compare("v") == 0)
                            {
                                v.v = stof(lineValues.at(i));
                            }
                        }

                        // Add the created VertexData object to the vertices vector.
                        vertices.push_back(v);
                    }

                    // Face data
                    if (dataPosition >= vertCount)
                    {
                        // Parse the line
                        lineValues = getLineAsVector(line);
                        // Create a TriData object and store it into the faces vector
                        faces.push_back(TriData(stoi(lineValues.at(1)), stoi(lineValues.at(2)), stoi(lineValues.at(3))));
                    }

                    // Increment the position of the data line to be read.
                    dataPosition++;
                }
            }
        }

        newfile.close(); // close the file object.
    }
}

// Class that creates Texture Objects, VBOs, VAO, for a single ply and single bmp file.
// This also provides a draw() method for the main draw-loop to call.
class TexturedMesh
{

private:
    std::string plyFilePath; // File path to the .ply mesh file.
    std::string bmpFilePath; // File path to the .bmp texture file.
    GLuint vaoID;            // ID of the VAO
    GLuint positionsID;      // ID of the VBO that stores all vertex position
    GLuint texCoordID;       // ID of the VBO that stores all texture coordinates
    GLuint normalsID;       // ID of the VBO that stores all texture coordinates
    GLuint eboID;            // ID of the EBO
    GLuint textureID;        // ID of the texture
    GLuint shaderID;         // ID of the shader program

    std::vector<VertexData> vertexList; // Stores vertex data from the ply file
    std::vector<TriData> triList;       // Stores tri/face data from the ply file
public:
    // Sets up the serverside texture object
    void setupTexture()
    {
        unsigned char *textureData; // Texture data in unsigned bytes
        unsigned int textureWidth;  // Width of the texture
        unsigned int textureHeight; // Height of the texture

        // Load the data and dimensions from file.
        loadARGB_BMP(this->bmpFilePath.c_str(), &textureData, &textureWidth, &textureHeight);

        glGenTextures(1, &textureID);            // Generate 1 serverside texture object
        glBindTexture(GL_TEXTURE_2D, textureID); // Bind the texture obj to GL_TEXTURE_2D
        // Store data from client data to server buffer:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, textureData);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glGenerateMipmap(GL_TEXTURE_2D); // Generate the mipmaps for the texture
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    }

    // This sets up the serverside position, uv, and index buffers.
    void setupVBOs()
    {

        // ===================================================
        // PARSING THEN STORING VERTEX POSITIONS IN A 1D ARRAY
        // ===================================================
        GLfloat vertex_buffer_data[vertexList.size() * 3];
        // [x1, y1, z1, x2, y2, z2, ...]

        int j = 0;
        for (int i = 0; i < vertexList.size(); i++)
        {
            VertexData v1 = vertexList.at(i);

            vertex_buffer_data[j + 0] = v1.x;
            vertex_buffer_data[j + 1] = v1.y;
            vertex_buffer_data[j + 2] = v1.z;
            j += 3;
        }

        // =================================================
        // PARSING THEN STORING UV COORDINATES IN A 1D ARRAY
        // =================================================
        GLfloat uv_buffer_data[vertexList.size() * 2];
        // [u1, v1, u2, v2, ...]

        j = 0;
        for (int i = 0; i < vertexList.size(); i++)
        {
            VertexData v1 = vertexList.at(i);

            uv_buffer_data[j + 0] = v1.u;
            uv_buffer_data[j + 1] = v1.v;
            j += 2;
        }


        // ===================================================
        // PARSING THEN STORING VERTEX POSITIONS IN A 1D ARRAY
        // ===================================================
        GLfloat normals_buffer_data[vertexList.size() * 3];
        // [x1, y1, z1, x2, y2, z2, ...]

        j = 0;
        for (int i = 0; i < vertexList.size(); i++)
        {
            VertexData v1 = vertexList.at(i);

            normals_buffer_data[j + 0] = v1.nx;
            normals_buffer_data[j + 1] = v1.ny;
            normals_buffer_data[j + 2] = v1.nz;
            j += 3;
        }

        // ===================================================
        // PARSING THEN STORING TRIANGLE INDICES IN A 1D ARRAY
        // ===================================================
        GLuint index_buffer_data[triList.size() * 3];
        // t => tri; v => vertex
        // [t1v1, t1v2, t1v3, t2v1, t2v2, t2v3, ...]

        j = 0;
        for (int i = 0; i < triList.size(); i++)
        {
            TriData triData = triList.at(i);

            index_buffer_data[j + 0] = triData.vert1;
            index_buffer_data[j + 1] = triData.vert2;
            index_buffer_data[j + 2] = triData.vert3;
            j += 3;
        }

        // ============================
        // CREATING VERTEX POSITION VBO
        // ============================
        glGenBuffers(1, &positionsID);              // Generate 1 serverside buffer object
        glBindBuffer(GL_ARRAY_BUFFER, positionsID); // Bind the object to GL_ARRAY_BUFFER
        // Store data from client data to server buffer:
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind the current GL_ARRAY_BUFFER buffer

        // =====================
        // CREATING UV COORD VBO
        // =====================
        glGenBuffers(1, &texCoordID);              // Generate 1 serverside buffer object
        glBindBuffer(GL_ARRAY_BUFFER, texCoordID); // Bind the object to GL_ARRAY_BUFFER
        // Store data from client data to server buffer:
        glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer_data), uv_buffer_data, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind the current GL_ARRAY_BUFFER buffer

        
        // ============================
        // CREATING VERTEX POSITION VBO
        // ============================
        glGenBuffers(1, &normalsID);              // Generate 1 serverside buffer object
        glBindBuffer(GL_ARRAY_BUFFER, normalsID); // Bind the object to GL_ARRAY_BUFFER
        // Store data from client data to server buffer:
        glBufferData(GL_ARRAY_BUFFER, sizeof(normals_buffer_data), normals_buffer_data, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind the current GL_ARRAY_BUFFER buffer

        // ============
        // CREATING EBO
        // ============
        glGenBuffers(1, &eboID);                      // Generate 1 serverside buffer object
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID); // Bind the object to GL_ELEMENT_ARRAY_BUFFER
        // Store data from client data to server buffer:
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // // Unbind the current GL_ELEMENT_ARRAY_BUFFER buffer
    }

    // Sets up the serverside VAO using the generated VBOs.
    // Must be called after setupVBO or else there will be no data in the VBOs.
    // (and no IDs will have been generated.)
    void setupVAO()
    {
        glGenVertexArrays(1, &vaoID); // Create 1 serverside VAO object
        glBindVertexArray(vaoID);     // Bind the current VAO object

        // Bind the EBO data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);

        // Bind the position data VBO
        glBindBuffer(GL_ARRAY_BUFFER, positionsID);
        glEnableVertexAttribArray(0); // activate vertex position attribute position
        // Store the data stored in the currently bound VBO
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (GLvoid *)0);
        // You could unbind here for safety but is isn't necessary.

        // Bind the uv data VBO
        glBindBuffer(GL_ARRAY_BUFFER, texCoordID);
        glEnableVertexAttribArray(1); // activate texture coords attribute position
        // Store the data stored in the currently bound VBO
        glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, (GLvoid *)0); // uv texture coordinates

        
        // Bind the normal data VBO
        glBindBuffer(GL_ARRAY_BUFFER, normalsID);
        glEnableVertexAttribArray(2);
        // Store the data stored in the currently bound VBO
        glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, (GLvoid *)0);

        glBindVertexArray(0);             // Unbind the current VAO
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind the current GL_ARRAY_BUFFER
    }

    // Sets the shader ID for the TexturedMesh.
    void setShaderID(GLuint programID)
    {
        this->shaderID = programID;
    }

    // Constructor
    // Takes in the file path of the ply file and the bmp file
    // Reads the ply file and stores the data into vertexList and triList
    // Sets up the Textures, VBOs, and VAO
    TexturedMesh(std::string plyFile, std::string bmpFile)
    {
        this->plyFilePath = plyFile;
        this->bmpFilePath = bmpFile;
        readPLYFile(plyFilePath, vertexList, triList);

        for(int i = 0; i < vertexList.size(); i++){
            VertexData v = vertexList.at(i);
        }

        setupTexture();
        setupVBOs();
        setupVAO();
    }

    // Draws the TexturedMesh
    void draw(glm::mat4 MVP, float Width, float Height, float Vig, float Fog, 
        glm::vec3 lightpos, glm::vec3 lightDir)
    {
        // Get a handle for the "MVP" uniform
        GLuint MatrixID = glGetUniformLocation(shaderID, "MVP");
        GLuint viewportSizeID = glGetUniformLocation(shaderID, "viewportSize");
        GLuint vigID = glGetUniformLocation(shaderID, "vig");
        GLuint fogID = glGetUniformLocation(shaderID, "fog");
        GLuint lightPosID = glGetUniformLocation(shaderID, "light_pos");
        GLuint lightDirID = glGetUniformLocation(shaderID, "light_dir");
        

        // Load the texture we set up in setupTexture() stored at textureID
        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Load the shader created in main (so we don't need to create the shader per object)
        glUseProgram(shaderID);
        // Load the MVP matrix into the program.
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniform2f(viewportSizeID, Width, Height);
        glUniform1f(vigID, Vig);
        glUniform1f(fogID, Fog);
        glUniform3f(lightDirID, lightDir.x, lightDir.y, lightDir.z);
        glUniform3f(lightPosID, lightpos.x, lightpos.y, lightpos.z);



        // Bind this TexturedMesh's VAO
        glBindVertexArray(vaoID);
        // Draw the mesh using all the info in the VAO
        glDrawElements(GL_TRIANGLES, triList.size() * 3, GL_UNSIGNED_INT, 0);
        // Unbind the VAO
        glBindVertexArray(0);

        glDisableVertexAttribArray(0); // Deactivate vertex position
        glDisableVertexAttribArray(1); // Deactivate texture coords

        //glUseProgram(0);                 // Deactivate the current shader program
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the current texture.
    }

    void drawDebug(){
        for(int i = 0; i < vertexList.size(); i++){
            VertexData v = vertexList.at(i);
        
        float mag = 0.02f;
        //std::cout << v.nx << "\n";
        float normalX = v.x + v.nx * mag;
        float normalY = v.y + v.ny * mag;
        float normalZ = v.z + v.nz * mag;

        glColor3f(v.nx, v.ny, v.nz);
        glBegin(GL_LINE_STRIP);
            glVertex3f(normalX, normalY, normalZ);
            glVertex3f(v.x, v.y, v.z);
        glEnd();


        glPointSize(5.0f);
        glColor3f(v.x, v.y, v.z);
        glBegin(GL_POINTS);
            glVertex3f(v.x, v.y, v.z);
        glEnd();

        }

        
        
    }
};

// Global bools to keep track of which buttons are pressed
bool rightArrowPressed = false;
bool leftArrowPressed = false;
bool upArrowPressed = false;
bool downArrowPressed = false;


float increment_move_speed = 1.0f;
float move_speed = 1.5f;
float increment_rot_speed = 30.0f;
float rot_speed = 70.0f;

float vig = 1.0;
float fog = 1.0;

// This method is called whenever a key event occurs.
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{

    
    if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS)
    {
        move_speed += increment_move_speed;
    }
    if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
    {
        move_speed -= increment_move_speed;
    }


    if (key == GLFW_KEY_V && action == GLFW_PRESS)
    {
        if(vig == 1) {vig = 0;} else {vig = 1;}
    }
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        if(fog == 1) {fog = 0;} else {fog = 1;}
    }

    if (key == GLFW_KEY_0 && action == GLFW_PRESS)
    {
        rot_speed += increment_rot_speed;
    }
    if (key == GLFW_KEY_9 && action == GLFW_PRESS)
    {
        rot_speed -= increment_rot_speed;
    }

    // RIGHT KEY
    // If pressed, set bool to true
    if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE && rightArrowPressed)
    {
        rightArrowPressed = false;
    }
    // If released, set bool to false
    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS && !rightArrowPressed)
    {
        rightArrowPressed = true;
    }

    // LEFT KEY
    // If pressed, set bool to true
    if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE && leftArrowPressed)
    {
        leftArrowPressed = false;
    }
    // If released, set bool to false
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS && !leftArrowPressed)
    {
        leftArrowPressed = true;
    }

    // UP KEY
    // If pressed, set bool to true
    if (key == GLFW_KEY_UP && action == GLFW_RELEASE && upArrowPressed)
    {
        upArrowPressed = false;
    }
    // If released, set bool to false
    else if (key == GLFW_KEY_UP && action == GLFW_PRESS && !upArrowPressed)
    {
        upArrowPressed = true;
    }

    // DOWN KEY
    // If pressed, set bool to true
    if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE && downArrowPressed)
    {
        downArrowPressed = false;
    }
    // If released, set bool to false
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS && !downArrowPressed)
    {
        downArrowPressed = true;
    }
}

//////////////////////////////////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{


    float previous_t = 0.0;
    
    float t = glutGet(GLUT_ELAPSED_TIME);
    float DeltaTime = (t - previous_t) / 1000.0;
    previous_t = t;

    ///////////////////////////////////////////////////////

    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);  
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    float screenW = 1400;
    float screenH = 900;
    window = glfwCreateWindow(screenW, screenH, "An Example", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    ///////////////////////////////////////////////////////

    // =============================
    // SETUP OPENGL RENDERING PARAMS
    // =============================

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Setup depth
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set up alpha transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ====================================
    // DECLARE MODEL VIEW PROJECTION MATRIX
    // ====================================
    glm::mat4 MVP;

    // ==============================
    // SETUP CAMERA AND MOVEMENT DATA
    // ==============================

    // Default starting position
    glm::vec3 eye = {0.5, 0.4, 0.5};
    // Default up vector
    glm::vec3 up = {0.0f, 1.0f, 0.0f};
    // Default center location
    glm::vec3 center = {0.0, 0.0, 0.0};
    // Default camera direction (-Z)
    glm::vec3 camera_dir = {0.0, 0.0, -1.0};

    // Movement speed
    float curr_move_speed = 0.0f;

    // Rotation speed
    float curr_rot_speed = 0.0f;

    // ================
    // SETUP KEY EVENTS
    // ================
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetKeyCallback(window, key_callback);

    // ==============
    // CREATE SHADERS
    // ==============

    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    std::string VertexShaderCode = "\
        #version 330 core\n\
        // Input vertex data, different for all executions of this shader.\n\
        layout(location = 0) in vec3 vertexPosition;\n\
        layout(location = 1) in vec2 uv;\n\
        layout(location = 2) in vec3 normals;\n\
        // Output data ; will be interpolated for each fragment.\n\
        out vec2 uv_out;\n\
        out float depth_out;\n\
        out vec4 position_out; \n\
        out vec3 normals_out; \n\
        // Values that stay constant for the whole mesh.\n\
        uniform mat4 MVP;\n\
        void main(){ \n\
            // Output position of the vertex, in clip space : MVP * position\n\
            gl_Position =  MVP * vec4(vertexPosition,1);\n\
            // The color will be interpolated to produce the color of each fragment\n\
            uv_out = uv;\n\
            normals_out = normals;\n\
            depth_out = gl_Position.w;\n\
            position_out = vec4(vertexPosition,1);\n\
} \n ";
    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode1 = "\
        #version 330 core\n\
        in vec2 uv_out; \n\
        in vec4 position_out; \n\
        in float depth_out; \n\
        in vec3 normals_out;\n\
        uniform sampler2D tex;\n\
        uniform vec2 viewportSize;\n\
        uniform float vig;\n\
        uniform float fog;\n\
        uniform vec3 light_pos;\n\
        uniform vec3 light_dir;\n\
        float map(float value, float min1, float max1, float min2, float max2) {\n\
            return min2 + (value - min1) * (max2 - min2) / (max1 - min1);\n\
        }\n\
        void main() {\n\
            float depth = clamp(depth_out / 4.0, 0.0, 1.0);\n\
            depth = pow(depth, 0.9);\n\
            if (fog == 0.0) {depth = 0.0;} \n\
            \n\
            vec3 fog_color = vec3(0.0, 0.0, 0.0);\n\
            \n\
            gl_FragColor = texture(tex, uv_out);\n\
            \n\
            float normalized_x = gl_FragCoord.x / viewportSize.x;\n\
            float normalized_y = gl_FragCoord.y / viewportSize.y;\n\
            \n\
            // Calculate distance\n\
            float pct = distance( \n\
                vec2(normalized_x, normalized_y), \n\
                vec2(0.5) );\n\
            \n\
            pct = pow(pct, 0.7);\n\
            vec3 light_color = vec3(5, 1, 5);\n\
            if(vig == 0.0) {pct = 0.0;}\n\
            // Lerp between fog and texture color\n\
            gl_FragColor.rgb = (1.0 - depth )*gl_FragColor.rgb + \n\
                                    (depth)*fog_color; \n\
            float dist_to_light = pow(distance(light_pos, position_out.xyz), 0.5);\n\
            dist_to_light = map(dist_to_light, 0.0, 2.0, 0.0, 1.0);\n\
            //gl_FragColor.rgb = gl_FragColor.rgb * (light_color * (0.9 - 0.9*dist_to_light + 0.1)); \n\
            // Multiply by vignette \n\
            gl_FragColor.rgb = gl_FragColor.rgb * (1.0-pct); \n\
            // Directional Light \n\
            //vec3 light_dir = vec3(1, 1.0, 1);\n\
            gl_FragColor.rgb = gl_FragColor.rgb * vec3(clamp(dot(normals_out.xyz, light_dir.xyz), 0.2, 1.0)); \n\
            //float diff = max(dot(normals_out, light_dir), 0.0);\n\
            //gl_FragColor.rgb = normalize(normals_out); \n\
        };\n ";

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode2 = "\
        #version 330 core\n\
        in vec2 uv_out; \n\
        in vec4 position_out; \n\
        in float depth_out; \n\
        in vec3 normals_out;\n\
        uniform sampler2D tex;\n\
        uniform vec2 viewportSize;\n\
        uniform float vig;\n\
        uniform float fog;\n\
        uniform vec3 light_pos;\n\
        uniform vec3 light_dir;\n\
        float map(float value, float min1, float max1, float min2, float max2) {\n\
            return min2 + (value - min1) * (max2 - min2) / (max1 - min1);\n\
        }\n\
        void main() {\n\
            float depth = clamp(depth_out / 4.0, 0.0, 1.0);\n\
            depth = pow(depth, 0.9);\n\
            if (fog == 0.0) {depth = 0.0;} \n\
            \n\
            vec3 fog_color = vec3(0.0, 0.0, 0.0);\n\
            \n\
            gl_FragColor = texture(tex, uv_out);\n\
            \n\
            float normalized_x = gl_FragCoord.x / viewportSize.x;\n\
            float normalized_y = gl_FragCoord.y / viewportSize.y;\n\
            \n\
            // Calculate distance\n\
            float pct = distance( \n\
                vec2(normalized_x, normalized_y), \n\
                vec2(0.5) );\n\
            \n\
            pct = pow(pct, 0.7);\n\
            vec3 light_color = vec3(5, 1, 5);\n\
            if(vig == 0.0) {pct = 0.0;}\n\
            // Lerp between fog and texture color\n\
            gl_FragColor.rgb = (1.0 - depth )*gl_FragColor.rgb + \n\
                                    (depth)*fog_color; \n\
            float dist_to_light = pow(distance(light_pos, position_out.xyz), 0.5);\n\
            dist_to_light = map(dist_to_light, 0.0, 2.0, 0.0, 1.0);\n\
            gl_FragColor.rgb = gl_FragColor.rgb * (light_color * (0.9 - 0.9*dist_to_light + 0.1)); \n\
            // Multiply by vignette \n\
            gl_FragColor.rgb = gl_FragColor.rgb * (1.0-pct); \n\
            // Directional Light \n\
            //vec3 light_dir = vec3(1, 1.0, 1);\n\
            //gl_FragColor.rgb = gl_FragColor.rgb * vec3(clamp(dot(normals_out.xyz, light_dir.xyz), 0.2, 1.0)); \n\
            //float diff = max(dot(normals_out, light_dir), 0.0);\n\
            //gl_FragColor.rgb = normalize(normals_out); \n\
        };\n ";


    std::string FragmentShaderCode = FragmentShaderCode2;
char const *VertexSourcePointer = VertexShaderCode.c_str();
glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
glCompileShader(VertexShaderID);

// Compile Fragment Shader
char const *FragmentSourcePointer = FragmentShaderCode.c_str();
glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
glCompileShader(FragmentShaderID);

GLuint ProgramID = glCreateProgram();
glAttachShader(ProgramID, VertexShaderID);
glAttachShader(ProgramID, FragmentShaderID);
glLinkProgram(ProgramID);

glDetachShader(ProgramID, VertexShaderID);
glDetachShader(ProgramID, FragmentShaderID);

glDeleteShader(VertexShaderID);
glDeleteShader(FragmentShaderID);

// ===========================
// CREATE TEXTUREDMESH OBJECTS
// ===========================

std::vector<TexturedMesh> tmList;
tmList.push_back(TexturedMesh("Bottles.ply", "bottles.bmp"));
tmList.push_back(TexturedMesh("Floor.ply", "floor.bmp"));
tmList.push_back(TexturedMesh("Patio.ply", "patio.bmp"));
tmList.push_back(TexturedMesh("TableSmooth.ply", "table.bmp"));
tmList.push_back(TexturedMesh("WallsSmooth.ply", "walls.bmp"));
tmList.push_back(TexturedMesh("WindowBG.ply", "windowbg.bmp"));
tmList.push_back(TexturedMesh("WoodObjectsSmooth.ply", "woodobjects.bmp"));
// Added last because they are transparent.
tmList.push_back(TexturedMesh("DoorBG.ply", "doorbg.bmp"));
tmList.push_back(TexturedMesh("MetalObjects.ply", "metalobjects.bmp"));
tmList.push_back(TexturedMesh("Curtains.ply", "curtains.bmp"));

float lightxpos = 0.0;
float lightypos = 0.5;
float lightzpos = 0.0;

glm::vec3 lightdir = glm::vec3(1.0, 1.0, 0.0);
lightdir = glm::normalize(lightdir);

float move_towards = 1.0;

// Setup each object and give it the ID of the shader program it should use.
for (int i = 0; i < tmList.size(); i++)
{
    tmList.at(i).setShaderID(ProgramID);
}

do
{
    

    t = glutGet(GLUT_ELAPSED_TIME);
    DeltaTime = (t - previous_t) / 1000.0;
    previous_t = t;

    // Clear the screen every loop
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup the projection matrix
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), screenW / screenH, 0.001f, 1000.0f);
    glLoadMatrixf(glm::value_ptr(Projection));

    // Setup the model view matrix
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // Create a rotation matrix to rotate the current camera direction around the up axis
    // which is y (0, 1, 0) in this case.
    // Rotate an angle determine by the different between if the right or left key are pressed
    float lerpIntensity = 10.0f * DeltaTime;
    float target_speed = ((float)rightArrowPressed - (float)leftArrowPressed) * rot_speed;
    curr_rot_speed = ((1.0 - lerpIntensity)*(curr_rot_speed) + (lerpIntensity)*target_speed);
    glm::mat4 rot_mat = glm::rotate(
        glm::mat4(1.0f),
        glm::radians(curr_rot_speed * DeltaTime),
        up);

    // Make the new camera direction equal to the current
    // camera direction multiplyed by the rotate matrix.
    camera_dir = glm::vec3(glm::vec4(camera_dir, 1.0f) * rot_mat);
    // Normalize the camera direction
    // This is not technically necessary because camera_dir started as a normalized vector
    // and rotating it should not change that, but we do it just in case.
    glm::normalize(camera_dir);

    // Displace the camera' position
    // lerpIntensity = 0.05f;
    target_speed = ((float)upArrowPressed - (float)downArrowPressed) * move_speed;
    curr_move_speed = ((1.0 - lerpIntensity)*(curr_move_speed) + (lerpIntensity)*target_speed);
    eye = eye + ((curr_move_speed * DeltaTime) * camera_dir);

    // Define the view matrix based on the lookAt function
    glm::mat4 V = glm::lookAt(eye, eye + camera_dir, up);
    // Have the model matrix be identify (models aren't translated, scaled, or rotated)
    glm::mat4 M = glm::mat4(1.0f);
    // Define the Modelview Matrix
    glm::mat4 MV = V * M;
    glLoadMatrixf(glm::value_ptr(V));

    // Define the Model View Projection Matrix
    MVP = Projection * V * M;

    float speed = 2.0f;
    lightxpos = lightxpos + speed * DeltaTime * move_towards;
    float mag = 3.0;
    if(lightxpos > mag && move_towards == 1.0){ move_towards = -1.0;}
    else if(lightxpos < (-1*mag) && move_towards == -1.0 ){ move_towards = 1.0;}
    lightzpos = lightxpos;

    glm::vec3 lightpos = glm::vec3(lightxpos, lightypos, lightzpos);

    rot_mat = glm::rotate(
        glm::mat4(1.0f),
        glm::radians(90.0f * DeltaTime),
        glm::vec3(0.0, 1.0, 0.0));
    lightdir = glm::vec3(glm::vec4(lightdir, 15.0f) * rot_mat);

    // glColor3f(1.0, 0.0, 0.0);
    // glBegin(GL_LINES);
    //     glVertex3f(lightdir. x, lightdir.y + 0.5f, lightdir. z);
    //     glVertex3f(0, lightdir.y, 0);
    // glEnd();

    printf("FPS: %f\n", 1.0f / DeltaTime);

    // Draw each object based on the MVP.
    for (int i = 0; i < tmList.size(); i++)
    {
        tmList.at(i).draw(MVP, screenW, screenH, vig, fog, lightpos, lightdir);
        //tmList.at(i).drawDebug();
    }

    // Swap buffers
    glfwSwapBuffers(window);

    glfwPollEvents();

} // Check if the ESC key was pressed or the window was closed
while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
       glfwWindowShouldClose(window) == 0);

// Close OpenGL window and terminate GLFW
glfwTerminate();

// Cleanup shader
glDeleteProgram(ProgramID);

return 0;
}

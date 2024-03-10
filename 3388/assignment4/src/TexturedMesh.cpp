#include "TexturedMesh.hpp"



class TexturedMesh {

private:
    //Bitmap and Ply variables
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
    TexturedMesh(const std::string& plyFilename, const std::string& textureFilename) {
        // Load geometry from PLY
        readPLYFile(plyFilename, vertices, faces);

        //Load Data from Bitmap
        loadARGB_BMP(plyFilename.c_str(), &texturePixels, &textureWidth,&textureHeight);


        // Initialize OpenGL objects 
        glGenVertexArrays(1, &vaoId);
        glGenBuffers(1, &vertexBufferId);
        glGenBuffers(1, &texCoordBufferId);
        glGenBuffers(1, &indexBufferId);
        glGenTextures(1, &textureId);
        
    }

    void draw(glm::mat4 MVP) {
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
    const std::vector<VertexData>& getVertices() const { return vertices; }
    const std::vector<TriData>& getFaces() const { return faces; } 
    const unsigned char* getTexturePixels() const { return texturePixels; }
    int getTextureWidth() const { return textureWidth; }
    int getTextureHeight() const { return textureHeight; }


};
#pragma once

#include <vector>
#include <string>
#include <GL/glew.h>
#include "LoadBitmap.hpp"
#include "types.hpp"
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "file.hpp"

// Forward declare your structures (if not defined elsewhere)
struct VertexData;
struct TriData;

class TexturedMesh {
public:
    TexturedMesh(const std::string& plyFilename, const std::string& textureFilename); 
    void draw(glm::mat4 MVP);

    // Accessors (you may want to add more)
    const std::vector<VertexData>& getVertices() const; 
    const std::vector<TriData>& getFaces() const; 
    const unsigned char* getTexturePixels() const;
    int getTextureWidth() const;
    int getTextureHeight() const;

private:
    // Bitmap and PLY variables
    std::vector<VertexData> vertices;
    std::vector<TriData> faces;
    unsigned char* texturePixels; // ARGB pixel data
    unsigned int textureWidth; 
    unsigned int textureHeight;

    // OpenGL object IDs
    GLuint vertexBufferId;   
    GLuint texCoordBufferId; 
    GLuint indexBufferId;   
    GLuint textureId;        
    GLuint vaoId;            
    GLuint shaderProgramId;  
};

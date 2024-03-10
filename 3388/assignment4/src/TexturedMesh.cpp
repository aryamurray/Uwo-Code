#pragma once

#include <vector>
#include <string>
#include "LoadBitmap.hpp"
#include "types.hpp"
#include <GLFW/glfw3.h>


class TexturedMesh {
public:
    TexturedMesh(const std::string& plyFilename, const std::string& textureFilename) {
        // Load PLY data
        std::vector<VertexData> vertices;
        std::vector<TriData> faces;
        readPLYFile(plyFilename, vertices, faces);

        // Load texture 
        TextureData textureData = loadARGB_BMP(textureFilename.c_str()); 

        // ... (Create VBOs, VAO, Texture Object using OpenGL functions)

        // ... (Create and compile shaders)
    }

    void draw(const glm::mat4& MVP) {
        glUseProgram(shaderProgram);

        // Bind VAO
        glBindVertexArray(VAO);

        // Bind texture
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set MVP uniform in shader if needed
        int mvpLocation = glGetUniformLocation(shaderProgram, "MVP");
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(MVP));

        // Render
        glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_INT, 0); 
    }

private:
    int vertexVBO;
    int texCoordVBO;
    int indexVBO;
    int textureID;
    int VAO;
    int shaderProgram;
    size_t numFaces; 
};
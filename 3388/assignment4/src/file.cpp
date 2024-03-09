#include <utility>
#include <vector>
#include <iostream>
#include "types.hpp"

std::pair<std::vector<VertexData>, std::vector<TriData>> readPLYFile(const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + filename);
  }

  std::string line;
  std::vector<VertexData> vertices;
  std::vector<TriData> faces;

  // Read header information
  int numVertices, numFaces;
  bool hasNormals = false, hasColors = false, hasTexCoords = false;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string token;
    if (iss >> token) {
      if (token == "element" && iss >> token && token == "vertex") {
        iss >> numVertices;
      } else if (token == "element" && iss >> token && token == "face") {
        iss >> numFaces;
      } else if (token == "property" && iss >> token) {
        if (token == "normal") {
          hasNormals = true;
        } else if (token == "color") {
          hasColors = true;
        } else if (token == "float" && iss >> token && token == "u") {
          hasTexCoords = true;
        }
      }
    }
  }

  // Read vertices
  for (int i = 0; i < numVertices; ++i) {
    VertexData vertex;
    file >> vertex.x >> vertex.y >> vertex.z;
    if (hasNormals) {
      file >> vertex.nx >> vertex.ny >> vertex.nz;
    }
    if (hasColors) {
      file >> vertex.r >> vertex.g >> vertex.b;
    }
    if (hasTexCoords) {
      file >> vertex.u >> vertex.v;
    }
    vertices.push_back(vertex);
  }

  // Read faces
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string token;
    iss >> token; // Skip the number of elements (3 for triangles)
    if (token == "face") {
      TriData face;
      iss >> face.indices[0] >> face.indices[1] >> face.indices[2];
      faces.push_back(face);
    }
  }

  file.close();
  return {vertices, faces};
}
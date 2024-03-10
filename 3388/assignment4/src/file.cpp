
#include "file.hpp"


void readPLYFile(std::string fname, std::vector<VertexData>& vertices, std::vector<TriData>& faces) {
    std::ifstream file(fname);

    if (!file.is_open()) {
        std::cerr << "Error opening PLY file: " << fname << std::endl;
        return;
    }

    std::string line;

    // Skip header lines
    while (std::getline(file, line)) {
        // Find first non-whitespace character
        size_t start = line.find_first_not_of(" \t\n\v\f\r"); 

        // Find last non-whitespace character
        size_t end = line.find_last_not_of(" \t\n\v\f\r");

        // Extract the trimmed substring (if line was not empty)
        if (start != std::string::npos) {
            std::string trimmed_line = line.substr(start, end - start + 1);

            if (trimmed_line == "end_header") {
                break;
            }
        }
    }

        // Read vertex data 
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        VertexData vertex;

        // Mandatory properties
        if (!(iss >> vertex.x >> vertex.y >> vertex.z)) { 
            std::cerr << "Error reading vertex coordinates" << std::endl;
            return;
        }

        // Optional properties
        iss >> vertex.nx >> vertex.ny >> vertex.nz 
            >> vertex.r >> vertex.g >> vertex.b
            >> vertex.u >> vertex.v;  

        vertices.push_back(vertex);
    }

    // Read face data 
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        TriData face;
        int numVertices; 

        if (!(iss >> numVertices >> face.indices[0] >> face.indices[1] >> face.indices[2])) {
            std::cerr << "Error reading face data" << std::endl;
            return; 
        }

        // Assuming triangles only (for now)
        faces.push_back(face);
    }
}

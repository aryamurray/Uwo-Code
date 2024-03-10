#pragma once

struct VertexData {
  float x, y, z;
  float nx = 0.0f, ny = 0.0f, nz = 0.0f; // Optional normal vector (default to 0)
  float r = 0.0f, g = 0.0f, b = 0.0f;   // Optional color (default to black)
  float u = 0.0f, v = 0.0f;             // Optional texture coordinates (default to 0)
};

// Struct to store face data (triangle indices)
struct TriData {
  int indices[3];  // Array to store indices of 3 vertices
};
#pragma once

#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include "types.hpp"

void readPLYFile(std::string fname, std::vector<VertexData>& vertices, std::vector<TriData>& faces);
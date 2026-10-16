#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "face.h"

bool loadOBJ(const std::string& path, std::vector<glm::vec3>& vertices, std::vector<Face>& faces);

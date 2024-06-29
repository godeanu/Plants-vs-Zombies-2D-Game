#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object2D
{
    Mesh* CreateSquare(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
    Mesh* CreateRectangle(const std::string& name, glm::vec3 leftBottomCorner, float width, float height, glm::vec3 color, bool fill = false);
    Mesh* CreateDiamond(const std::string& name, glm::vec3 leftDiamondCorner, float side, glm::vec3 color, bool fill = false);
    Mesh* CreateStar(const std::string& name, glm::vec3 center, float outerradius, float innerradius, glm::vec3 color, bool fill = false);
    Mesh* CreateHexagon(const std::string &name, glm::vec3 center, float outerRadius, glm::vec3 color, bool fill = false);
}
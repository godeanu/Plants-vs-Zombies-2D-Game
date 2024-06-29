#include "object_pvzgame.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"

Mesh* object2D::CreateHexagon(
    const std::string& name,
    glm::vec3 center,
    float outerRadius,
    glm::vec3 color,
    bool fill)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    const int numSides = 6; // Number of sides for a hexagon
    float angleBetweenVertices = 2 * M_PI / numSides;

    // Create the vertices for the outer hexagon
    for (int i = 0; i < numSides; ++i) {
        float angle = i * angleBetweenVertices;
        vertices.push_back(VertexFormat(center + glm::vec3(cos(angle) * outerRadius, sin(angle) * outerRadius, 0), color));
    }



    indices = {
		5,0,1,
        1,2,3,
        3,4,5,
        1,3,5
	};

    // Create hexagon mesh
    Mesh* hexagon = new Mesh(name);

    if (!fill) {
        hexagon->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        hexagon->SetDrawMode(GL_TRIANGLES);
    }

    hexagon->InitFromData(vertices, indices);
    return hexagon;
}


Mesh* object2D::CreateStar(
    const std::string& name,
    glm::vec3 center,
    float outerRadius,
    float innerRadius,
    glm::vec3 color,
    bool fill)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // We define the angle between star tips
    const int numSpikes = 5; // Number of spikes in the star.
    float angleBetweenSpikes = 2 * M_PI / numSpikes; // Angle between the spikes.

    // Create the outer and inner vertices of the star.
    for (int i = 0; i < numSpikes; ++i) {
        // Outer vertex of the spike.
        float outerAngle = i * angleBetweenSpikes;
        vertices.push_back(VertexFormat(center + glm::vec3(cos(outerAngle) * outerRadius, sin(outerAngle) * outerRadius, 0), color));

        // Inner vertex between spikes.
        float innerAngle = outerAngle + angleBetweenSpikes / 2.0f;
        vertices.push_back(VertexFormat(center + glm::vec3(cos(innerAngle) * innerRadius, sin(innerAngle) * innerRadius, 0), color));
    }
    // Calculate indices for the star (assuming GL_TRIANGLES drawing mode)
    indices = {
       8,2,5,
       4,0,7,
       6,9,2,
       8,4,1,
       6,3,0

    };
    // Create star mesh
    Mesh* star = new Mesh(name);

    if (!fill) {
        star->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // If filled, use the indices to create the triangles
        star->SetDrawMode(GL_TRIANGLES);
    }

    star->InitFromData(vertices, indices);
    return star;
}



Mesh* object2D::CreateDiamond(
    const std::string& name,
    glm::vec3 leftDiamondCorner,
    float side, //actually from corner to center
    glm::vec3 color,
    bool fill)

{
    glm::vec3 corner = leftDiamondCorner;
    // Define vertices
    std::vector<VertexFormat> vertices =

    {   
        VertexFormat(corner + glm::vec3(side, side * 1.5, 0), color),  // A
        VertexFormat(corner, color),    // B
        VertexFormat(corner + glm::vec3(side, -side*1.5, 0), color),  // C
        VertexFormat(corner + glm::vec3(side*1.8, 0.3*side, 0), color),// D
        VertexFormat(corner + glm::vec3(side*1.8, -0.3*side, 0), color),// E
        VertexFormat(corner + glm::vec3(2.6*side, 0.3*side, 0), color),// F
        VertexFormat(corner + glm::vec3(2.6*side, -0.3*side, 0), color) // G
        
        
    };

    // Define indices based on provided segments
    std::vector<unsigned int> indices =
    {
       0, 3, 1,
       3, 1, 4,
       1,4, 2,
       3, 4, 6,
       3, 6, 5
    };

    // Create diamond mesh
    Mesh* diamond = new Mesh(name);
    if (!fill) {
        diamond->SetDrawMode(GL_LINE_LOOP);
    }
    else {
    }
    diamond->InitFromData(vertices, indices);
    return diamond;
}

Mesh* object2D::CreateRectangle(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float width,
    float height,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(width, 0, 0), color),
        VertexFormat(corner + glm::vec3(width, height, 0), color),
        VertexFormat(corner + glm::vec3(0, height, 0), color)
    };

    Mesh* rectangle = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        rectangle->SetDrawMode(GL_LINE_LOOP);
    }
    else {

        indices.push_back(0);
        indices.push_back(2);
    }

    rectangle->InitFromData(vertices, indices);
    return rectangle;
}



Mesh* object2D::CreateSquare(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

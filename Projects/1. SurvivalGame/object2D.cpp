#include "object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"

Mesh* object2D::CreateSquare(
    const std::string &name,
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
    } else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}


Mesh* object2D::CreateCircle(std::string name, glm::vec3 center, float radius, glm::vec3 color) {
    Mesh* circle = new Mesh(name);
    std::vector<VertexFormat> vertices;
    std::vector<unsigned> indices;
    unsigned trianglesNo = 50;
    for(int i = 0; i < trianglesNo; i++) {
        float arg = 2 * M_PI * i / trianglesNo;
        vertices.emplace_back(glm::vec3(cos(arg) * radius, sin(arg) * radius, 0),
                              color);
        indices.push_back(i);
    }
    indices.push_back(trianglesNo);
    circle->InitFromData(vertices, indices);
    circle->SetDrawMode(GL_TRIANGLE_FAN);
    return circle;
}
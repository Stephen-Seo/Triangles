#include "triangle.hpp"

Tri::Triangle::Triangle() :
Shape(),
vertices{
    glm::vec2{0.0f, 0.0f},
    glm::vec2{1.0f, 0.0f},
    glm::vec2{0.0f, 1.0f}
}
{}

Tri::Triangle::Triangle(std::array<glm::vec2, 3> vertices) :
Shape(),
vertices(vertices)
{}

Tri::Triangle::Triangle(std::array<glm::vec2, 3> vertices, Color fillColor) :
Shape(fillColor),
vertices(vertices)
{}

Tri::Triangle::Triangle(std::array<glm::vec2, 3> vertices, Color fillColor, Color outlineColor) :
Shape(fillColor, outlineColor),
vertices(vertices)
{}

Tri::Shape& Tri::Triangle::draw() {
    std::array<glm::vec2, 3> transformed = {
        transform * glm::vec3(vertices[0].x, vertices[0].y, 1.0f),
        transform * glm::vec3(vertices[1].x, vertices[1].y, 1.0f),
        transform * glm::vec3(vertices[2].x, vertices[2].y, 1.0f)
    };
    DrawTriangle(
        {transformed[0].x, transformed[0].y},
        {transformed[1].x, transformed[1].y},
        {transformed[2].x, transformed[2].y},
        fillColor);
//    DrawTriangleLines(
//        {transformed[0].x, transformed[0].y},
//        {transformed[1].x, transformed[1].y},
//        {transformed[2].x, transformed[2].y},
//        outlineColor);
    return *this;
}

void Tri::Triangle::getVertices(std::vector<glm::vec2> &verticesOut) const {
    verticesOut.clear();
    for(const glm::vec2 &vertex : vertices) {
        verticesOut.push_back(vertex);
    }
}

void Tri::Triangle::getTransformedVertices(std::vector<glm::vec2> &verticesOut) const {
    verticesOut.clear();
    for(glm::vec2 vertex : vertices) {
        vertex = transform * glm::vec3(vertex.x, vertex.y, 1.0f);
        verticesOut.push_back(vertex);
    }
}

float Tri::Triangle::getRadius() const {
    return 0.0f;
}

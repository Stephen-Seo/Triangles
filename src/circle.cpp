#include "circle.hpp"

#include <cmath>

Tri::Circle::Circle() :
Shape(),
position{0.0f, 0.0f},
radius(4.0f)
{}

Tri::Circle::Circle(glm::vec2 pos) :
Shape(),
position(pos),
radius(4.0f)
{}

Tri::Circle::Circle(glm::vec2 pos, float radius) :
Shape(),
position(pos),
radius(radius)
{}

Tri::Circle::Circle(glm::vec2 pos, float radius, Color fillColor) :
Shape(fillColor),
position(pos),
radius(radius)
{}

Tri::Circle::Circle(glm::vec2 pos, float radius, Color fillColor, Color outlineColor) :
Shape(fillColor, outlineColor),
position(pos),
radius(radius)
{}

Tri::Shape& Tri::Circle::draw() {
    glm::vec2 transformed = transform * glm::vec3(position.x, position.y, 1.0f);
    transformed.x = std::roundf(transformed.x);
    transformed.y = std::roundf(transformed.y);
    DrawCircle(transformed.x, transformed.y, radius, fillColor);
    DrawCircleLines(transformed.x, transformed.y, radius, outlineColor);
    return *this;
}

void Tri::Circle::getVertices(std::vector<glm::vec2> &verticesOut) const {
    verticesOut.clear();
    verticesOut.push_back(position);
}

void Tri::Circle::getTransformedVertices(std::vector<glm::vec2> &verticesOut) const {
    verticesOut.clear();
    verticesOut.push_back(transform * glm::vec3{position.x, position.y, 1.0f});
}

float Tri::Circle::getRadius() const {
    return radius;
}

void Tri::Circle::setRadius(float radius) {
    this->radius = radius;
}

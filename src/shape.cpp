#include "shape.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#include <glm/ext/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>
#pragma GCC diagnostic pop

Tri::Shape::Shape() :
transform(glm::identity<glm::mat3>()),
fillColor(RAYWHITE),
outlineColor(BLACK)
{}

Tri::Shape::Shape(Color fillColor) :
transform(glm::identity<glm::mat3>()),
fillColor(fillColor),
outlineColor(BLACK)
{}

Tri::Shape::Shape(Color fillColor, Color outlineColor) :
transform(glm::identity<glm::mat3>()),
fillColor(fillColor),
outlineColor(outlineColor)
{}

Tri::Shape& Tri::Shape::resetTransform() {
    transform = glm::identity<glm::mat3>();
    return *this;
}

Tri::Shape& Tri::Shape::translate(const glm::vec2 move) {
    transform = glm::translate(transform, move);
    return *this;
}

Tri::Shape& Tri::Shape::rotate(const float angle) {
    transform = glm::rotate(transform, angle);
    return *this;
}

Tri::Shape& Tri::Shape::scale(const glm::vec2 scale) {
    transform = glm::scale(transform, scale);
    return *this;
}

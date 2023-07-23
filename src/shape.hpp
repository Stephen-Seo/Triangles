#ifndef TRIANGLES_SHAPE_HPP
#define TRIANGLES_SHAPE_HPP

#include <vector>

#pragma GCC diagnostic ignored "-Weffc++"
#include <raylib.h>
#include <glm/glm.hpp>
#pragma GCC diagnostic pop

namespace Tri {

struct Shape {
    Shape();
    Shape(Color fillColor);
    Shape(Color fillColor, Color outlineColor);
    virtual ~Shape() {}

    glm::mat3 transform;
    Color fillColor;
    Color outlineColor;

    Shape& resetTransform();
    Shape& translate(const glm::vec2 move);
    Shape& rotate(const float angle);
    Shape& scale(const glm::vec2 scale);

    virtual Shape& draw() = 0;

    virtual void getVertices(std::vector<glm::vec2> &verticesOut) const = 0;
    virtual void getTransformedVertices(std::vector<glm::vec2> &verticesOut) const = 0;

    virtual float getRadius() const = 0;
    virtual void setRadius(float radius) = 0;
}; // struct Shape

} // namespace Tri

#endif

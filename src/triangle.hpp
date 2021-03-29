#ifndef TRIANGLES_TRIANGLE_HPP
#define TRIANGLES_TRIANGLE_HPP

#include <array>

#include "shape.hpp"

namespace Tri {

struct Triangle : public Shape {
    Triangle();
    Triangle(std::array<glm::vec2, 3> vertices);
    Triangle(std::array<glm::vec2, 3> vertices, Color fillColor);
    Triangle(std::array<glm::vec2, 3> vertices, Color fillColor, Color outlineColor);

    std::array<glm::vec2, 3> vertices;

    virtual Shape& draw() override;

    virtual void getVertices(std::vector<glm::vec2> &verticesOut) const override;
    virtual void getTransformedVertices(std::vector<glm::vec2> &verticesOut) const override;

    virtual float getRadius() const override;
    virtual void setRadius(float /* radius */) override {};
}; // struct Triangle

} // namespace Tri

#endif

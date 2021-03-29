#ifndef TRIANGLES_CIRCLE_HPP
#define TRIANGLES_CIRCLE_HPP

#include "shape.hpp"

namespace Tri {

struct Circle : public Shape {
    Circle();
    Circle(glm::vec2 pos);
    Circle(glm::vec2 pos, float radius);
    Circle(glm::vec2 pos, float radius, Color fillColor);
    Circle(glm::vec2 pos, float radius, Color fillColor, Color outlineColor);

    glm::vec2 position;
    float radius;

    virtual Shape& draw() override;

    virtual void getVertices(std::vector<glm::vec2> &verticesOut) const override;
    virtual void getTransformedVertices(std::vector<glm::vec2> &verticesOut) const override;

    virtual float getRadius() const override;
    virtual void setRadius(float radius) override;
}; // struct Circle

} // namespace Tri

#endif

#include "catch.hpp"

#include <SFML/Graphics.hpp>

#include "helpers.hpp"

TEST_CASE("Test is_within_shape", "[Triangles]") {
    sf::ConvexShape shape;
    shape.setPointCount(3);
    shape.setPoint(0, {0.0f, 10.0f});
    shape.setPoint(1, {10.0f, 10.0f});
    shape.setPoint(2, {10.0f, 0.0f});

    CHECK(Tri::is_within_shape(shape, {2.0f, 2.0f}) == false);
    CHECK(Tri::is_within_shape(shape, {5.0f, 15.0f}) == false);
    CHECK(Tri::is_within_shape(shape, {15.0f, 5.0f}) == false);
    CHECK(Tri::is_within_shape(shape, {7.0f, 7.0f}) == true);
}

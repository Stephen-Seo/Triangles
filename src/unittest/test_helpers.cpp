#include "catch_amalgamated.hpp"

#include "helpers.hpp"
#include "triangle.hpp"

TEST_CASE("Test is_within_shape", "[Triangles]") {
    Tri::Triangle triangle({{
        {0.0f, 10.0f},
        {10.0f, 10.0f},
        {10.0f, 0.0f}
    }});

    CHECK(Tri::is_within_shape(triangle, {2.0f, 2.0f}) == false);
    CHECK(Tri::is_within_shape(triangle, {5.0f, 15.0f}) == false);
    CHECK(Tri::is_within_shape(triangle, {15.0f, 5.0f}) == false);
    CHECK(Tri::is_within_shape(triangle, {7.0f, 7.0f}) == true);
}

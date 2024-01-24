#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#include "catch_amalgamated.hpp"
#pragma GCC diagnostic pop

#include "helpers.hpp"
#include "triangle.hpp"

TEST_CASE("Test is_within_shape", "Helpers") {
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

TEST_CASE("Test make_counter_clockwise", "Helpers") {
    // Note that +x is right and +y is down.

    {
        // Clockwise triangle.
        std::array<glm::vec2, 3> tri{{
            {0.0F, 0.0F}, {1.0F, 0.0F}, {2.0F, 2.0F}
        }};

        Tri::make_counter_clockwise(tri);

        CHECK(tri.at(0).x == 2.0F);
        CHECK(tri.at(0).y == 2.0F);

        CHECK(tri.at(1).x == 1.0F);
        CHECK(tri.at(1).y == 0.0F);

        CHECK(tri.at(2).x == 0.0F);
        CHECK(tri.at(2).y == 0.0F);
    }

    {
        // Counter-Clockwise triangle.
        std::array<glm::vec2, 3> tri{{
            {2.0F, 0.0F}, {3.0F, 3.0F}, {4.0F, 1.0F}
        }};

        Tri::make_counter_clockwise(tri);

        CHECK(tri.at(0).x == 2.0F);
        CHECK(tri.at(0).y == 0.0F);

        CHECK(tri.at(1).x == 3.0F);
        CHECK(tri.at(1).y == 3.0F);

        CHECK(tri.at(2).x == 4.0F);
        CHECK(tri.at(2).y == 1.0F);
    }
}

TEST_CASE("Test invert_color", "Helpers") {
    Color c{
        255, 255, 255, 255
    };

    c = Tri::invert_color(c);

    CHECK(c.r == 0);
    CHECK(c.g == 0);
    CHECK(c.b == 0);
    CHECK(c.a == 255);
}

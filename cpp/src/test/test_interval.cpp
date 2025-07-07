#include "test/interval_test_case.hpp"
#include <catch2/catch_all.hpp>

INTERVAL_TEST_CASE("constructor") {
    SECTION("[1]") {
        REQUIRE_NOTHROW(Interval(1));
    }

    SECTION("[1,1]") {
        REQUIRE_NOTHROW(Interval(1, 1));
    }

    SECTION("[1,2]") {
        REQUIRE_NOTHROW(Interval(1, 2));
    }

    SECTION("[2,1]") {
        REQUIRE_THROWS_AS(Interval(2, 1), std::invalid_argument);
    }
}

INTERVAL_TEST_CASE("nan") {
    SECTION("[nan]") {
        REQUIRE(Interval::nan().is_nan());
    }

    SECTION("[1,2]") {
        REQUIRE_FALSE(Interval(1, 2).is_nan());
    }
}

INTERVAL_TEST_CASE("sign") {
    SECTION("[nan]") {
        const Interval interval = Interval::nan();
        REQUIRE_FALSE(interval.is_positive());
        REQUIRE_FALSE(interval.is_negative());
        REQUIRE_FALSE(interval.is_nonzero());
    }

    SECTION("[-2,-1]") {
        const Interval interval(-2, -1);
        REQUIRE_FALSE(interval.is_positive());
        REQUIRE(interval.is_negative());
        REQUIRE(interval.is_nonzero());
    }

    SECTION("[-1,0]") {
        const Interval interval(-1, 0);
        REQUIRE_FALSE(interval.is_positive());
        REQUIRE_FALSE(interval.is_negative());
        REQUIRE_FALSE(interval.is_nonzero());
    }

    SECTION("[0,1]") {
        const Interval interval(0, 1);
        REQUIRE_FALSE(interval.is_positive());
        REQUIRE_FALSE(interval.is_negative());
        REQUIRE_FALSE(interval.is_nonzero());
    }

    SECTION("[1,2]") {
        const Interval interval(1, 2);
        REQUIRE(interval.is_positive());
        REQUIRE_FALSE(interval.is_negative());
        REQUIRE(interval.is_nonzero());
    }
}

INTERVAL_TEST_CASE("comparison") {
    const Interval interval(-1, 1);

    SECTION("nan") {
        const Interval other = Interval::nan();
        REQUIRE_FALSE(interval > other);
        REQUIRE_FALSE(interval < other);
    }

    SECTION("[-3,-2]") {
        const Interval other(-3, -2);
        REQUIRE(interval > other);
        REQUIRE_FALSE(interval < other);
    }

    SECTION("[-2,-1]") {
        const Interval other(-2, -1);
        REQUIRE_FALSE(interval > other);
        REQUIRE_FALSE(interval < other);
    }

    SECTION("[-2,0]") {
        const Interval other(-2, 0);
        REQUIRE_FALSE(interval > other);
        REQUIRE_FALSE(interval < other);
    }

    SECTION("[-2,1]") {
        const Interval other(-2, 1);
        REQUIRE_FALSE(interval > other);
        REQUIRE_FALSE(interval < other);
    }

    SECTION("[-2,2]") {
        const Interval other(-2, 2);
        REQUIRE_FALSE(interval > other);
        REQUIRE_FALSE(interval < other);
    }

    SECTION("[-1,-1]") {
        const Interval other(-1, -1);
        REQUIRE_FALSE(interval > other);
        REQUIRE_FALSE(interval < other);
    }

    SECTION("[-1,0]") {
        const Interval other(-1, 0);
        REQUIRE_FALSE(interval > other);
        REQUIRE_FALSE(interval < other);
    }

    SECTION("[-1,1]") {
        const Interval other(-1, 1);
        REQUIRE_FALSE(interval > other);
        REQUIRE_FALSE(interval < other);
    }

    SECTION("[-1,2]") {
        const Interval other(-1, 2);
        REQUIRE_FALSE(interval > other);
        REQUIRE_FALSE(interval < other);
    }

    SECTION("[0,0]") {
        const Interval other(0, 0);
        REQUIRE_FALSE(interval > other);
        REQUIRE_FALSE(interval < other);
    }

    SECTION("[0,1]") {
        const Interval other(0, 1);
        REQUIRE_FALSE(interval > other);
        REQUIRE_FALSE(interval < other);
    }

    SECTION("[0,2]") {
        const Interval other(0, 2);
        REQUIRE_FALSE(interval > other);
        REQUIRE_FALSE(interval < other);
    }

    SECTION("[1,1]") {
        const Interval other(1, 1);
        REQUIRE_FALSE(interval > other);
        REQUIRE_FALSE(interval < other);
    }

    SECTION("[1,2]") {
        const Interval other(1, 2);
        REQUIRE_FALSE(interval > other);
        REQUIRE_FALSE(interval < other);
    }

    SECTION("[2,3]") {
        const Interval other(2, 3);
        REQUIRE_FALSE(interval > other);
        REQUIRE(interval < other);
    }
}

INTERVAL_TEST_CASE("to_float") {
    SECTION("[nan]") {
        const Interval interval = Interval::nan();
        REQUIRE(float_approx(interval.to_float()));
        REQUIRE(interval_approx(interval.to_floats()));
    }

    SECTION("[420]") {
        const Interval interval(420);
        REQUIRE(float_approx(interval.to_float(), 420.0));
        REQUIRE(interval_approx(interval.to_floats(), {420.0, 420.0}));
    }

    SECTION("[1,2]") {
        const Interval interval(1, 2);
        REQUIRE(float_approx(interval.to_float(), 1.5));
        REQUIRE(interval_approx(interval.to_floats(), {1.0, 2.0}));
    }
}

INTERVAL_TEST_CASE("min_max_mid_len_rad") {
    SECTION("[nan]") {
        const Interval interval = Interval::nan();
        REQUIRE(interval.min().is_nan());
        REQUIRE(interval.max().is_nan());
        REQUIRE(interval.mid().is_nan());
        REQUIRE(interval.len().is_nan());
        REQUIRE(interval.rad().is_nan());
    }

    SECTION("[6,9]") {
        const Interval interval(6, 9);
        REQUIRE(float_approx(interval.min().to_float(), 6.0));
        REQUIRE(float_approx(interval.max().to_float(), 9.0));
        REQUIRE(float_approx(interval.mid().to_float(), 7.5));
        REQUIRE(float_approx(interval.len().to_float(), 3.0));
        REQUIRE(float_approx(interval.rad().to_float(), 1.5));
    }
}

INTERVAL_TEST_CASE("basic_operations") {
    SECTION("nan") {
        const Interval interval = Interval::nan();
        REQUIRE(interval_approx((+interval).to_floats()));
        REQUIRE(interval_approx((-interval).to_floats()));
        REQUIRE(interval_approx((interval + interval).to_floats()));
        REQUIRE(interval_approx((interval - interval).to_floats()));
        REQUIRE(interval_approx((interval * interval).to_floats()));
        REQUIRE(interval_approx((interval / interval).to_floats()));
    }

    const Interval m2_m1(-2, -1);
    const Interval m1_z(-1, 0);
    const Interval z_z(0, 0);
    const Interval z_p1(0, 1);
    const Interval p1_p2(1, 2);
    const Interval m1_p1(-1, 1);

    SECTION("+x") {
        REQUIRE(interval_approx( (+m2_m1).to_floats(), {-2, -1}));
        REQUIRE(interval_approx( (+m1_z ).to_floats(), {-1, 0}));
        REQUIRE(interval_approx( (+z_z ).to_floats(), { 0, 0}));
        REQUIRE(interval_approx( (+z_p1 ).to_floats(), { 0, 1}));
        REQUIRE(interval_approx( (+p1_p2).to_floats(), { 1, 2}));
        REQUIRE(interval_approx( (+m1_p1).to_floats(), {-1, 1}));
    }

    SECTION("-x") {
        REQUIRE(interval_approx( (-m2_m1).to_floats(), { 1, 2}));
        REQUIRE(interval_approx( (-m1_z ).to_floats(), { 0, 1}));
        REQUIRE(interval_approx( (-z_z ).to_floats(), { 0, 0}));
        REQUIRE(interval_approx( (-z_p1 ).to_floats(), {-1, 0}));
        REQUIRE(interval_approx( (-p1_p2).to_floats(), {-2, -1}));
        REQUIRE(interval_approx( (-m1_p1).to_floats(), {-1, 1}));
    }

    SECTION("x+y") {
        REQUIRE(interval_approx( (m2_m1 + m2_m1).to_floats(), {-4, -2}));
        REQUIRE(interval_approx( (m2_m1 + m1_z ).to_floats(), {-3, -1}));
        REQUIRE(interval_approx( (m2_m1 + z_z ).to_floats(), {-2, -1}));
        REQUIRE(interval_approx( (m2_m1 + z_p1 ).to_floats(), {-2, 0}));
        REQUIRE(interval_approx( (m2_m1 + p1_p2).to_floats(), {-1, 1}));
        REQUIRE(interval_approx( (m2_m1 + m1_p1).to_floats(), {-3, 0}));

        REQUIRE(interval_approx( (m1_z + m2_m1).to_floats(), {-3, -1}));
        REQUIRE(interval_approx( (m1_z + m1_z ).to_floats(), {-2, 0}));
        REQUIRE(interval_approx( (m1_z + z_z ).to_floats(), {-1, 0}));
        REQUIRE(interval_approx( (m1_z + z_p1 ).to_floats(), {-1, 1}));
        REQUIRE(interval_approx( (m1_z + p1_p2).to_floats(), { 0, 2}));
        REQUIRE(interval_approx( (m1_z + m1_p1).to_floats(), {-2, 1}));

        REQUIRE(interval_approx( (z_z + m2_m1).to_floats(), {-2, -1}));
        REQUIRE(interval_approx( (z_z + m1_z ).to_floats(), {-1, 0}));
        REQUIRE(interval_approx( (z_z + z_z ).to_floats(), { 0, 0}));
        REQUIRE(interval_approx( (z_z + z_p1 ).to_floats(), { 0, 1}));
        REQUIRE(interval_approx( (z_z + p1_p2).to_floats(), { 1, 2}));
        REQUIRE(interval_approx( (z_z + m1_p1).to_floats(), {-1, 1}));

        REQUIRE(interval_approx( (z_p1 + m2_m1).to_floats(), {-2, 0}));
        REQUIRE(interval_approx( (z_p1 + m1_z ).to_floats(), {-1, 1}));
        REQUIRE(interval_approx( (z_p1 + z_z ).to_floats(), { 0, 1}));
        REQUIRE(interval_approx( (z_p1 + z_p1 ).to_floats(), { 0, 2}));
        REQUIRE(interval_approx( (z_p1 + p1_p2).to_floats(), { 1, 3}));
        REQUIRE(interval_approx( (z_p1 + m1_p1).to_floats(), {-1, 2}));

        REQUIRE(interval_approx( (p1_p2 + m2_m1).to_floats(), {-1, 1}));
        REQUIRE(interval_approx( (p1_p2 + m1_z ).to_floats(), { 0, 2}));
        REQUIRE(interval_approx( (p1_p2 + z_z ).to_floats(), { 1, 2}));
        REQUIRE(interval_approx( (p1_p2 + z_p1 ).to_floats(), { 1, 3}));
        REQUIRE(interval_approx( (p1_p2 + p1_p2).to_floats(), { 2, 4}));
        REQUIRE(interval_approx( (p1_p2 + m1_p1).to_floats(), { 0, 3}));

        REQUIRE(interval_approx( (m1_p1 + m2_m1).to_floats(), {-3, 0}));
        REQUIRE(interval_approx( (m1_p1 + m1_z ).to_floats(), {-2, 1}));
        REQUIRE(interval_approx( (m1_p1 + z_z ).to_floats(), {-1, 1}));
        REQUIRE(interval_approx( (m1_p1 + z_p1 ).to_floats(), {-1, 2}));
        REQUIRE(interval_approx( (m1_p1 + p1_p2).to_floats(), { 0, 3}));
        REQUIRE(interval_approx( (m1_p1 + m1_p1).to_floats(), {-2, 2}));
    }
    SECTION("x-y") {
        REQUIRE(interval_approx( (m2_m1 - m2_m1).to_floats(), {-1, 1}));
        REQUIRE(interval_approx( (m2_m1 - m1_z ).to_floats(), {-2, 0}));
        REQUIRE(interval_approx( (m2_m1 - z_z ).to_floats(), {-2, -1}));
        REQUIRE(interval_approx( (m2_m1 - z_p1 ).to_floats(), {-3, -1}));
        REQUIRE(interval_approx( (m2_m1 - p1_p2).to_floats(), {-4, -2}));
        REQUIRE(interval_approx( (m2_m1 - m1_p1).to_floats(), {-3, 0}));

        REQUIRE(interval_approx( (m1_z - m2_m1).to_floats(), { 0, 2}));
        REQUIRE(interval_approx( (m1_z - m1_z ).to_floats(), {-1, 1}));
        REQUIRE(interval_approx( (m1_z - z_z ).to_floats(), {-1, 0}));
        REQUIRE(interval_approx( (m1_z - z_p1 ).to_floats(), {-2, 0}));
        REQUIRE(interval_approx( (m1_z - p1_p2).to_floats(), {-3, -1}));
        REQUIRE(interval_approx( (m1_z - m1_p1).to_floats(), {-2, 1}));

        REQUIRE(interval_approx( (z_z - m2_m1).to_floats(), { 1, 2}));
        REQUIRE(interval_approx( (z_z - m1_z ).to_floats(), { 0, 1}));
        REQUIRE(interval_approx( (z_z - z_z ).to_floats(), { 0, 0}));
        REQUIRE(interval_approx( (z_z - z_p1 ).to_floats(), {-1, 0}));
        REQUIRE(interval_approx( (z_z - p1_p2).to_floats(), {-2, -1}));
        REQUIRE(interval_approx( (z_z - m1_p1).to_floats(), {-1, 1}));

        REQUIRE(interval_approx( (z_p1 - m2_m1).to_floats(), { 1, 3}));
        REQUIRE(interval_approx( (z_p1 - m1_z ).to_floats(), { 0, 2}));
        REQUIRE(interval_approx( (z_p1 - z_z ).to_floats(), { 0, 1}));
        REQUIRE(interval_approx( (z_p1 - z_p1 ).to_floats(), {-1, 1}));
        REQUIRE(interval_approx( (z_p1 - p1_p2).to_floats(), {-2, 0}));
        REQUIRE(interval_approx( (z_p1 - m1_p1).to_floats(), {-1, 2}));

        REQUIRE(interval_approx( (p1_p2 - m2_m1).to_floats(), { 2, 4}));
        REQUIRE(interval_approx( (p1_p2 - m1_z ).to_floats(), { 1, 3}));
        REQUIRE(interval_approx( (p1_p2 - z_z ).to_floats(), { 1, 2}));
        REQUIRE(interval_approx( (p1_p2 - z_p1 ).to_floats(), { 0, 2}));
        REQUIRE(interval_approx( (p1_p2 - p1_p2).to_floats(), {-1, 1}));
        REQUIRE(interval_approx( (p1_p2 - m1_p1).to_floats(), { 0, 3}));

        REQUIRE(interval_approx( (m1_p1 - m2_m1).to_floats(), { 0, 3}));
        REQUIRE(interval_approx( (m1_p1 - m1_z ).to_floats(), {-1, 2}));
        REQUIRE(interval_approx( (m1_p1 - z_z ).to_floats(), {-1, 1}));
        REQUIRE(interval_approx( (m1_p1 - z_p1 ).to_floats(), {-2, 1}));
        REQUIRE(interval_approx( (m1_p1 - p1_p2).to_floats(), {-3, 0}));
        REQUIRE(interval_approx( (m1_p1 - m1_p1).to_floats(), {-2, 2}));
    }

    SECTION("x*y") {
        REQUIRE(interval_approx( (m2_m1 * m2_m1).to_floats(), { 1, 4}));
        REQUIRE(interval_approx( (m2_m1 * m1_z ).to_floats(), { 0, 2}));
        REQUIRE(interval_approx( (m2_m1 * z_z ).to_floats(), { 0, 0}));
        REQUIRE(interval_approx( (m2_m1 * z_p1 ).to_floats(), {-2, 0}));
        REQUIRE(interval_approx( (m2_m1 * p1_p2).to_floats(), {-4, -1}));
        REQUIRE(interval_approx( (m2_m1 * m1_p1).to_floats(), {-2, 2}));

        REQUIRE(interval_approx( (m1_z * m2_m1).to_floats(), { 0, 2}));
        REQUIRE(interval_approx( (m1_z * m1_z ).to_floats(), { 0, 1}));
        REQUIRE(interval_approx( (m1_z * z_z ).to_floats(), { 0, 0}));
        REQUIRE(interval_approx( (m1_z * z_p1 ).to_floats(), {-1, 0}));
        REQUIRE(interval_approx( (m1_z * p1_p2).to_floats(), {-2, 0}));
        REQUIRE(interval_approx( (m1_z * m1_p1).to_floats(), {-1, 1}));

        REQUIRE(interval_approx( (z_z * m2_m1).to_floats(), { 0, 0}));
        REQUIRE(interval_approx( (z_z * m1_z ).to_floats(), { 0, 0}));
        REQUIRE(interval_approx( (z_z * z_z ).to_floats(), { 0, 0}));
        REQUIRE(interval_approx( (z_z * z_p1 ).to_floats(), { 0, 0}));
        REQUIRE(interval_approx( (z_z * p1_p2).to_floats(), { 0, 0}));
        REQUIRE(interval_approx( (z_z * m1_p1).to_floats(), { 0, 0}));

        REQUIRE(interval_approx( (z_p1 * m2_m1).to_floats(), {-2, 0}));
        REQUIRE(interval_approx( (z_p1 * m1_z ).to_floats(), {-1, 0}));
        REQUIRE(interval_approx( (z_p1 * z_z ).to_floats(), { 0, 0}));
        REQUIRE(interval_approx( (z_p1 * z_p1 ).to_floats(), { 0, 1}));
        REQUIRE(interval_approx( (z_p1 * p1_p2).to_floats(), { 0, 2}));
        REQUIRE(interval_approx( (z_p1 * m1_p1).to_floats(), {-1, 1}));

        REQUIRE(interval_approx( (p1_p2 * m2_m1).to_floats(), {-4, -1}));
        REQUIRE(interval_approx( (p1_p2 * m1_z ).to_floats(), {-2, 0}));
        REQUIRE(interval_approx( (p1_p2 * z_z ).to_floats(), { 0, 0}));
        REQUIRE(interval_approx( (p1_p2 * z_p1 ).to_floats(), { 0, 2}));
        REQUIRE(interval_approx( (p1_p2 * p1_p2).to_floats(), { 1, 4}));
        REQUIRE(interval_approx( (p1_p2 * m1_p1).to_floats(), {-2, 2}));

        REQUIRE(interval_approx( (m1_p1 * m2_m1).to_floats(), {-2, 2}));
        REQUIRE(interval_approx( (m1_p1 * m1_z ).to_floats(), {-1, 1}));
        REQUIRE(interval_approx( (m1_p1 * z_z ).to_floats(), { 0, 0}));
        REQUIRE(interval_approx( (m1_p1 * z_p1 ).to_floats(), {-1, 1}));
        REQUIRE(interval_approx( (m1_p1 * p1_p2).to_floats(), {-2, 2}));
        REQUIRE(interval_approx( (m1_p1 * m1_p1).to_floats(), {-1, 1}));
    }

    SECTION("x/y") {
        REQUIRE(interval_approx( (m2_m1 / m2_m1).to_floats(), { 0.5, 2.0}));
        REQUIRE((m2_m1 / m1_z ).is_nan());
        REQUIRE((m2_m1 / z_z ).is_nan());
        REQUIRE((m2_m1 / z_p1 ).is_nan());
        REQUIRE(interval_approx( (m2_m1 / p1_p2).to_floats(), {-2.0, -0.5}));
        REQUIRE((m2_m1 / m1_p1).is_nan());

        REQUIRE(interval_approx( (m1_z / m2_m1).to_floats(), {0.0, 1.0}));
        REQUIRE((m1_z / m1_z ).is_nan());
        REQUIRE((m1_z / z_z ).is_nan());
        REQUIRE((m1_z / z_p1 ).is_nan());
        REQUIRE(interval_approx( (m1_z / p1_p2).to_floats(), {-1.0, 0.0}));
        REQUIRE((m1_z / m1_p1).is_nan());

        REQUIRE(interval_approx( (z_z / m2_m1).to_floats(), {0.0, 0.0}));
        REQUIRE((z_z / m1_z ).is_nan());
        REQUIRE((z_z / z_z ).is_nan());
        REQUIRE((z_z / z_p1 ).is_nan());
        REQUIRE(interval_approx( (z_z / p1_p2).to_floats(), { 0.0, 0.0}));
        REQUIRE((z_z / m1_p1).is_nan());

        REQUIRE(interval_approx( (z_p1 / m2_m1).to_floats(), {-1.0, 0.0}));
        REQUIRE((z_p1 / m1_z ).is_nan());
        REQUIRE((z_p1 / z_z ).is_nan());
        REQUIRE((z_p1 / z_p1 ).is_nan());
        REQUIRE(interval_approx( (z_p1 / p1_p2).to_floats(), { 0.0, 1.0}));
        REQUIRE((z_p1 / m1_p1).is_nan());

        REQUIRE(interval_approx( (p1_p2 / m2_m1).to_floats(), {-2.0, -0.5}));
        REQUIRE((p1_p2 / m1_z ).is_nan());
        REQUIRE((p1_p2 / z_z ).is_nan());
        REQUIRE((p1_p2 / z_p1 ).is_nan());
        REQUIRE(interval_approx( (p1_p2 / p1_p2).to_floats(), { 0.5, 2.0}));
        REQUIRE((p1_p2 / m1_p1).is_nan());

        REQUIRE(interval_approx( (m1_p1 / m2_m1).to_floats(), {-1.0, 1.0}));
        REQUIRE((m1_p1 / m1_z ).is_nan());
        REQUIRE((m1_p1 / z_z ).is_nan());
        REQUIRE((m1_p1 / z_p1 ).is_nan());
        REQUIRE(interval_approx( (m1_p1 / p1_p2).to_floats(), {-1.0, 1.0}));
        REQUIRE((m1_p1 / m1_p1).is_nan());
    }

    SECTION("1/x") {
        REQUIRE(interval_approx( m2_m1.inv().to_floats(), {-1.0, -0.5}));
        REQUIRE(m1_z.inv().is_nan());
        REQUIRE(z_z.inv().is_nan());
        REQUIRE(z_p1.inv().is_nan());
        REQUIRE(interval_approx( p1_p2.inv().to_floats(), { 0.5, 1.0}));
        REQUIRE(m1_p1.inv().is_nan());
    }

    SECTION("x^2") {
        REQUIRE(interval_approx( m2_m1.sqr().to_floats(), {1, 4}));
        REQUIRE(interval_approx( m1_z.sqr().to_floats(), {0, 1}));
        REQUIRE(interval_approx( z_z.sqr().to_floats(), {0, 0}));
        REQUIRE(interval_approx( z_p1.sqr().to_floats(), {0, 1}));
        REQUIRE(interval_approx( p1_p2.sqr().to_floats(), {1, 4}));
        REQUIRE(interval_approx( m1_p1.sqr().to_floats(), {0, 1}));
    }

    SECTION("sqrt(x)") {
        REQUIRE(m2_m1.sqrt().is_nan());
        REQUIRE(m1_z.sqrt().is_nan());
        REQUIRE(interval_approx(z_z.sqrt().to_floats(), {0, 0}));
        REQUIRE(interval_approx( z_p1.sqrt().to_floats(), {0.0, 1.0}));
        REQUIRE(interval_approx( p1_p2.sqrt().to_floats(), {1.0, 1.4142135623730951}));
        REQUIRE(m1_p1.sqrt().is_nan());
    }
}

INTERVAL_TEST_CASE("trigonometric_functions") {
    SECTION("[nan]") {
        const Interval interval = Interval::nan();
        REQUIRE(interval.cos().is_nan());
        REQUIRE(interval.sin().is_nan());
        REQUIRE(interval.tan().is_nan());
        REQUIRE(interval.acos().is_nan());
        REQUIRE(interval.asin().is_nan());
        REQUIRE(interval.atan().is_nan());
    }

    SECTION("pi") {
        REQUIRE(interval_approx(Interval::pi().to_floats(), {std::numbers::pi_v<double>, std::numbers::pi_v<double>}));
    }

    SECTION("cos_sin_tan") {
        int from = GENERATE(range(-48, 49));
        int to = GENERATE_COPY(range(from, 49));

        const Interval interval = Interval(from, to) * Interval::pi() / Interval(12);

        double cos_min = std::numeric_limits<double>::infinity();
        double cos_max = -std::numeric_limits<double>::infinity();
        double sin_min = std::numeric_limits<double>::infinity();
        double sin_max = -std::numeric_limits<double>::infinity();
        bool tan_is_nan = false;
        double tan_min = std::numeric_limits<double>::infinity();
        double tan_max = -std::numeric_limits<double>::infinity();

        for(int value = from; value <= to; ++value) {
            tan_is_nan |= (value % 12 + 12) % 12 == 6;
            const double scaled_value = static_cast<double>(value) * std::numbers::pi_v<double> / 12;
            const double cos = std::cos(scaled_value);
            const double sin = std::sin(scaled_value);
            cos_min = std::min(cos_min, cos);
            cos_max = std::max(cos_max, cos);
            sin_min = std::min(sin_min, sin);
            sin_max = std::max(sin_max, sin);
            if(!tan_is_nan) {
                const double tan = std::tan(scaled_value);
                tan_min = std::min(tan_min, tan);
                tan_max = std::max(tan_max, tan);
            }
        }

        if(tan_is_nan) {
            REQUIRE(interval.tan().is_nan());
        } else {
            REQUIRE(interval_approx(interval.tan().to_floats(), {tan_min, tan_max}));
        }
    }

    SECTION("acos_asin_atan") {
        int from = GENERATE(range(-20, 21));
        int to = GENERATE_COPY(range(from, 21));

        const Interval interval = Interval(from, to) / Interval(10);

        bool acos_asin_are_nan = false;
        double acos_min = std::numeric_limits<double>::infinity();
        double acos_max = -std::numeric_limits<double>::infinity();
        double asin_min = std::numeric_limits<double>::infinity();
        double asin_max = -std::numeric_limits<double>::infinity();
        double atan_min = std::numeric_limits<double>::infinity();
        double atan_max = -std::numeric_limits<double>::infinity();

        for(int value = from; value <= to; ++value) {
            acos_asin_are_nan |= value <= -10 || 10 <= value;
            const double scaled_value = static_cast<double>(value) / 10;
            if(!acos_asin_are_nan) {
                const double acos_val = std::acos(scaled_value);
                const double asin_val = std::asin(scaled_value);
                acos_min = std::min(acos_min, acos_val);
                acos_max = std::max(acos_max, acos_val);
                asin_min = std::min(asin_min, asin_val);
                asin_max = std::max(asin_max, asin_val);
            }
            const double atan_val = std::atan(scaled_value);
            atan_min = std::min(atan_min, atan_val);
            atan_max = std::max(atan_max, atan_val);
        }

        if(acos_asin_are_nan) {
            REQUIRE(interval.acos().is_nan());
            REQUIRE(interval.asin().is_nan());
        } else {
            REQUIRE(interval_approx(interval.acos().to_floats(), {acos_min, acos_max}));
            REQUIRE(interval_approx(interval.asin().to_floats(), {asin_min, asin_max}));
        }
        REQUIRE(interval_approx(interval.atan().to_floats(), {atan_min, atan_max}));
    }
}

#include "test/interval_test_case.hpp"
#include <catch2/catch_approx.hpp>

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
        REQUIRE(std::isnan(interval.to_float()));
    }
    SECTION("[420]") {
        const Interval interval(420);
        REQUIRE(interval.to_float() == 420);
    }
}

INTERVAL_TEST_CASE("min_max_mid_len_rad") {
    SECTION("[nan]") {
        const Interval interval = Interval::nan();
        REQUIRE(std::isnan(interval.min().to_float()));
        REQUIRE(std::isnan(interval.max().to_float()));
        REQUIRE(std::isnan(interval.mid().to_float()));
        REQUIRE(std::isnan(interval.len().to_float()));
        REQUIRE(std::isnan(interval.rad().to_float()));
    }
}

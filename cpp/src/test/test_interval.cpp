#include "test/interval_test_case.hpp"

INTERVAL_TEST_CASE("[1,2] no throw") {
    REQUIRE_NOTHROW(Interval(1, 2));
}

// Test Includes
#include <catch2/catch_test_macros.hpp>

#include "tomorrow_framework/type_traits.hpp"

TEST_CASE("Type Traits tests")
{
    SECTION("is_valid_input_events_v")
    {
        struct ValidEvent {
            using data_type = int;
        };
        using valid_input_events = std::tuple<ValidEvent, ValidEvent, ValidEvent>;
        using invalid_input_events = std::tuple<ValidEvent, int, double, ValidEvent>;

        static_assert(tmrw::is_valid_input_events_v<valid_input_events>, "The `is_valid_input_events_v` is improperly claiming the `valid_input_events` is invalid.");
        static_assert(!tmrw::is_valid_input_events_v<invalid_input_events>, "The `is_valid_input_events_v` is improperly claiming the `invalid_input_events` is valid.");
    };
    REQUIRE(true);
};

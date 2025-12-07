// Test Includes
#include <catch2/catch_test_macros.hpp>

#include "tomorrow_framework/generic_module_data.hpp"
#include "tomorrow_framework/trace_context.hpp"

TEST_CASE("Generic Module Data tests")
{
    SECTION("Creating GMD without default Trace Context")
    {
        static constexpr auto expected_value = 5;
        static constexpr auto expected_trace_id = "";
        static constexpr auto expected_baggage = "";
        const auto actual = tmrw::make_gmd(expected_value);

        REQUIRE(*(std::get<int>(actual)) == expected_value);
        REQUIRE(std::get<double>(actual) == nullptr);

        const auto trace_context = std::get<tmrw::TraceContext>(actual);
        REQUIRE(trace_context[0] == expected_trace_id);
        REQUIRE(trace_context[1] == expected_baggage);
    };
    SECTION("Creating GMD with default Trace Context")
    {
        static constexpr auto expected_value = 6.0;
        static constexpr auto expected_trace_id = "my_trace_id";
        static constexpr auto expected_baggage = "my_baggage";
        const auto actual = tmrw::make_gmd(expected_value, {expected_trace_id, expected_baggage});

        REQUIRE(*(std::get<double>(actual)) == expected_value);
        REQUIRE(std::get<int>(actual) == nullptr);

        const auto trace_context = std::get<tmrw::TraceContext>(actual);
        REQUIRE(trace_context[0] == expected_trace_id);
        REQUIRE(trace_context[1] == expected_baggage);
    };
};

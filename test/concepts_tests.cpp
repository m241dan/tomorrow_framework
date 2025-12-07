// Test Includes
#include <catch2/catch_test_macros.hpp>

#include "tomorrow_framework/concepts.hpp"
#include "tomorrow_framework/event.hpp"
#include "tomorrow_framework/trace_context.hpp"

#include <any>
#include <map>

TEST_CASE("Concepts tests")
{
    SECTION("Events")
    {
        // Serves to also test the Event mixin
        struct ValidEvent : public tmrw::Event<int> {};

        SECTION("Input Event")
        {

            static_assert(tmrw::input_event<ValidEvent>, "The concept `input_event` is improperly claiming `ValidEvent` is invalid.");
            static_assert(!tmrw::input_event<int>, "The concept `input_event` is improperly claiming an `int` is valid.");
        }
        SECTION("Is Valid Input Events")
        {
            using valid_input_events = std::tuple<ValidEvent, ValidEvent, ValidEvent>;
            using invalid_input_events = std::tuple<ValidEvent, int, double, ValidEvent>;

            static_assert(tmrw::valid_input_events<valid_input_events>, "The concept `valid_input_events` is improperly claiming `valid_input_events` is invalid.");
            static_assert(!tmrw::valid_input_events<invalid_input_events>, "The concept `valid_input_events` is improperly claiming `invalid_input_events` is valid.");
        }
    }
    SECTION("Event Handler Checks")
    {
        struct EventHandlerWithTC {
            static auto process([[maybe_unused]] int x, [[maybe_unused]] tmrw::TraceContext tc) -> std::map<int, std::any> // NOLINT(performance-unnecessary-value-param)
            {
                return {};
            }
            static auto process([[maybe_unused]] std::string x, [[maybe_unused]] tmrw::TraceContext tc) -> std::map<int, std::any> // NOLINT(performance-unnecessary-value-param)
            {
                return {};
            }
        };

        struct EventHandlerWithoutTC {
            static auto process([[maybe_unused]] int x) -> std::map<int, std::any>
            {
                return {};
            }
            static auto process([[maybe_unused]] std::string dtc) -> std::map<int, std::any> // NOLINT(performance-unnecessary-value-param)
            {
                return {};
            }
        };

        struct EventHandlerWithBadReturnWithTC {
            auto process([[maybe_unused]] int x)
            {
            }
            static auto process([[maybe_unused]] std::string dtc) -> std::map<int, std::any> // NOLINT(performance-unnecessary-value-param)
            {
                return {};
            }
        };

        struct EventHandlerWithBadReturnWithoutTC {
            static auto process([[maybe_unused]] int x) -> std::map<int, std::any>
            {
                return {};
            }
            auto process([[maybe_unused]] std::string dtc) // NOLINT(performance-unnecessary-value-param)
            {
            }
        };

        SECTION("Supports Trace Context")
        {
            static_assert(tmrw::supports_trace_context<EventHandlerWithTC, int>, "The concept `supports_trace_context` is improperly claiming `EventHandlerWithTC` does not support a trace context.");
            static_assert(!tmrw::supports_trace_context<EventHandlerWithoutTC, int>, "The concept `supports_trace_context` is improperly claiming `EventHandlerWithoutTC` supports a trace context.");
        }

        SECTION("Supports Event")
        {
            static_assert(tmrw::supports_event<EventHandlerWithTC, int>, "The concept `supports_event` is improperly claiming `EventHandlerWithTC` does not support the given event.");
            static_assert(tmrw::supports_event<EventHandlerWithTC, std::string>, "The concept `supports_event` is improperly claiming `EventHandlerWithTC` does not support the given event.");
            static_assert(!tmrw::supports_event<EventHandlerWithTC, std::vector<int>>, "The concept `supports_event` is improperly claiming `EventHandlerWithTC` does support the given event.");
            static_assert(tmrw::supports_event<EventHandlerWithoutTC, int>, "The concept `supports_event` is improperly claiming `EventHandlerWithoutTC` does not support the given event.");
            static_assert(tmrw::supports_event<EventHandlerWithoutTC, std::string>, "The concept `supports_event` is improperly claiming `EventHandlerWithoutTC` does not support the given event.");
            static_assert(!tmrw::supports_event<EventHandlerWithoutTC, std::vector<int>>, "The concept `supports_event` is improperly claiming `EventHandlerWithoutTC` does support the given event.");
        }
        SECTION("Supports Events")
        {
            static_assert(tmrw::supports_events<EventHandlerWithTC, std::tuple<int, std::string>>, "The concept `supports_events` is improperly claiming `EventHandlerWithTC` does not support the given events.");
            static_assert(!tmrw::supports_events<EventHandlerWithTC, std::tuple<int, std::string, std::vector<int>>>, "The concept `supports_events` is improperly claiming `EventHandlerWithTC` does support the given events.");
            static_assert(tmrw::supports_events<EventHandlerWithoutTC, std::tuple<int, std::string>>, "The concept `supports_events` is improperly claiming `EventHandlerWithoutTC` does not support the given events.");
            static_assert(!tmrw::supports_events<EventHandlerWithoutTC, std::tuple<int, std::string, std::vector<int>>>, "The concept `supports_events` is improperly claiming `EventHandlerWithoutTC` does support the given events.");
            static_assert(!tmrw::supports_events<EventHandlerWithBadReturnWithoutTC, std::tuple<int, std::string>>, "The concept `supports_events` is improperly claiming `EventHandlerWithBadReturnWithoutTC` does support the given events.");
            static_assert(!tmrw::supports_events<EventHandlerWithBadReturnWithTC, std::tuple<int, std::string>>, "The concept `supports_events` is improperly claiming `EventHandlerWithBadReturnWithTC` does support the given events.");
        }
    }
    REQUIRE(true);
};

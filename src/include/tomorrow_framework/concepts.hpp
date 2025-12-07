#ifndef TOMORROW_FRAMEWORK_CONCEPTS_HPP
#define TOMORROW_FRAMEWORK_CONCEPTS_HPP

#include "tomorrow_framework/trace_context.hpp"
#include "tomorrow_framework/type_traits.hpp"

#include <any>
#include <map>
#include <type_traits>

namespace tmrw {

/**
 * A concept that checks if an EventHandler supports taking a TraceContext for a specific Event.
 */
template<typename EventHandler, typename Event>
concept supports_trace_context = requires(EventHandler eh, Event e) {
    { eh.process(e, std::declval<TraceContext>()) };
};

/**
 * A concept that checks if an EventHandler supports a given Event.
 */
template<typename EventHandler, typename Event>
concept supports_event = requires(EventHandler eh, Event e) {
    { eh.process(e, std::declval<TraceContext>()) };
} || requires(EventHandler eh, Event e) {
    { eh.process(e) } -> std::same_as<std::map<int, std::any>>;
};

namespace detail {
/**
 * A detail type_trait to support checking if the list of types in a tuple all work with a given event handler.
 * 
 * This type_trait is used to build a concept of the same name
 */
template<typename EventHandler, typename Events>
struct supports_events : std::false_type {};

template<typename EventHandler, typename... Events>
struct supports_events<EventHandler, std::tuple<Events...>>
    : std::bool_constant<(supports_event<EventHandler, Events> && ...)> {};
} // namespace detail

/**
 * A concept that checks to make sure an EventHandler can handle all of the events it should.
 */
template<typename EventHandler, typename InputEvents>
concept supports_events = detail::supports_events<EventHandler, InputEvents>::value;

/**
 * A concept that checks if an Event has the data_type member type.
 */
template<typename Event>
concept input_event = requires {
    typename Event::data_type;
};

/**
 * A concept that checks if a type is a valid set of input events.
 */
template<typename InputEvents>
concept valid_input_events = is_valid_input_events_v<InputEvents>;

} // namespace tmrw

#endif

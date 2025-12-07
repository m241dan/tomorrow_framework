#ifndef TOMORROW_FRAMEWORK_EVENT_HPP
#define TOMORROW_FRAMEWORK_EVENT_HPP

#include "tomorrow_framework/concepts.hpp"

#include <tuple>

namespace tmrw {

/**
 * This struct is a simple mixin that allows us to strictly define what an Event is.
 *
 * @nb This is "unit" tested by the Concepts tests.
 */
template<typename UnderlyingType>
struct Event {
    using data_type = UnderlyingType;
    data_type data;
};

/**
 * This type is a constrainted std::tuple for building the list of inputs that go into modules.
 *
 * @nb This is "unit" tested by the Module tests.
 */
template<input_event... Events>
using InputEvents = std::tuple<Events...>;

} // namespace tmrw

#endif

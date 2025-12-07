#ifndef TOMORROW_FRAMEWORK_TYPE_TRAITS_HPP
#define TOMORROW_FRAMEWORK_TYPE_TRAITS_HPP

#include <tuple>
#include <type_traits>

namespace tmrw {

/**
 * A type_trait for extracting an Events underlying data_type.
 */
template<typename T>
using event_data_t = typename T::data_type;

/**
 * The false type_trait for telling if a tuple of events is valid.
 */
template<typename T, typename = void>
struct is_valid_input_events : std::false_type {};

/**
 * The true type_trait for telling if a tuple of events is valid.
 */
template<typename... Events>
struct is_valid_input_events<std::tuple<Events...>, std::void_t<event_data_t<Events>...>> : std::true_type {};

/**
 * The boolean test trait for if a tuple of events is valid.
 */
template<typename T>
inline constexpr bool is_valid_input_events_v = is_valid_input_events<T>::value;

} // namespace tmrw

#endif

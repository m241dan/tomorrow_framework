#ifndef TOMORROW_FRAMEWORK_GENERIC_MODULE_DATA_HPP
#define TOMORROW_FRAMEWORK_GENERIC_MODULE_DATA_HPP

#include "trace_context.hpp"

#include <any>

namespace tmrw {

/**
 * The data struct that will be used by all Module Channels.
 */
struct GenericModuleData {
    TraceContext trace_context;
    std::any data;
};

/**
 * A helper function for creating GenericModuleData
 */
template<std::movable T>
auto make_gmd(T data, TraceContext trace_context = {}) -> GenericModuleData //NOLINT(performance-unnecessary-value-param)
{
    return {
        .trace_context = std::move(trace_context),
        .data = std::move(data) // ensure it uses the move constructor so that it can't throw
    };
}
} // namespace tmrw

// Overloads for std::get
namespace std {
template<>
struct tuple_size<tmrw::GenericModuleData> : std::integral_constant<std::size_t, 2> {
};

template<std::size_t I>
struct tuple_element<I, tmrw::GenericModuleData> {
    static_assert(I < tuple_size_v<tmrw::GenericModuleData>, "Index out of bounds for tmrw::GenericModuleData");
    using type = std::conditional_t<I == 0, tmrw::TraceContext,
                                    std::conditional_t<I == 1, std::any, void>>;
};

template<typename T>
auto get(tmrw::GenericModuleData& gmd) -> decltype(auto) // NOLINT(cert-dcl58-cpp)
{
    if constexpr (std::is_same_v<T, tmrw::TraceContext>)
    {
        return gmd.trace_context;
    }
    else
    {
        return std::any_cast<T>(&(gmd.data));
    }
}

template<typename T>
auto get(const tmrw::GenericModuleData& gmd) -> decltype(auto) // NOLINT(cert-dcl58-cpp)
{
    if constexpr (std::is_same_v<T, tmrw::TraceContext>)
    {
        return gmd.trace_context;
    }
    else
    {
        return std::any_cast<T>(&(gmd.data));
    }
}
} // namespace std

#endif

#ifndef TOMORROW_FRAMEWORK_ASIO_PROCESSOR_HPP
#define TOMORROW_FRAMEWORK_ASIO_PROCESSOR_HPP

#include "tomorrow_framework/channels.hpp"
#include "tomorrow_framework/concepts.hpp"
#include "tomorrow_framework/generic_module_data.hpp"

#include "boost/asio/co_spawn.hpp"
#include "boost/asio/detached.hpp"
#include <stdexcept>
#include <tuple>

namespace tmrw {

template<typename EventHandler, valid_input_events InputEvents>
    requires supports_events<EventHandler, InputEvents>
struct AsioProcessor {
    using EventHandler_t = EventHandler;
    using InputEvents_t = InputEvents;
    static constexpr auto number_of_input_events = std::tuple_size_v<InputEvents>;

    /**
     * A function that kicks off some compiletime recursion to spawn a processor for every input event.
     */
    template<typename Self>
    constexpr auto spawn_processors(this Self&& self) -> void
    {
        if (std::forward<Self>(self).channels.inputs.size() != number_of_input_events)
        {
            throw std::runtime_error{"Size mismatch with events and channels."};
        }

        std::forward<Self>(self).template spawn_processor<Self>();
    }

    /**
     * A function that spawns a processor for each input event recursively.
     *
     * It does this by spawning a coroutine for its given input event and then
     * if there are input events left in the tuple, it calls itself with the
     * next tuple index.
     *
     * Function is used by `spawn_processors`.
     */
    template<typename Self, std::size_t event_index = 0UL>
    constexpr auto spawn_processor(this Self&& self)
    {
        boost::asio::co_spawn(
            std::forward<Self>(self).executor,
            std::forward<Self>(self).template processor<Self, std::tuple_element_t<event_index, InputEvents_t>, event_index>(),
            boost::asio::detached);

        if constexpr (static constexpr auto next_event_index = event_index + 1; next_event_index < number_of_input_events)
        {
            std::forward<Self>(self).template spawn_processor<Self, next_event_index>();
        }
    }


    /**
     * A coroutine for processing an incoming event from a channel.
     */
    template<typename Self, input_event Event, std::size_t input_channel_index>
    auto processor(this Self&& self) -> boost::asio::awaitable<void> // NOLINT(cppcoreguidelines-avoid-reference-coroutine-parameters)
    {
        for (;;)
        {
            using data_type = event_data_t<Event>;

            // Asynchronously pull an event off the channel.
            auto [trace_context, data] = [](GenericModuleData incoming) static -> std::tuple<TraceContext, data_type> {
                return {
                    std::move(std::get<TraceContext>(incoming)),
                    std::move(*std::get<data_type>(incoming))};
            }(co_await std::forward<Self>(self).channels.inputs[input_channel_index]->async_receive()); // <- ** receive ** is here

            /////////////////////////////
            // call to start span here //
            /////////////////////////////

            auto [result, trace_context_to_send] = [&] {
                if constexpr (supports_trace_context<EventHandler_t, data_type>)
                {
                    return std::forward<Self>(self).process(Event{std::move(data)}, std::move(trace_context));
                }
                else
                {
                    return std::make_tuple(std::forward<Self>(self).process(Event{std::move(data)}), std::move(trace_context));
                }
            }();

            for (const auto& [output_index, value] : result)
            {
                for (auto& channel : std::forward<Self>(self).channels.outputs[output_index])
                {
                    using boost::system::error_code;
                    channel->async_send(error_code{}, make_gmd(value, trace_context_to_send), boost::asio::detached); // copies both per send :(
                }
            }
        }

        co_return;
    }

    boost::asio::any_io_executor executor;
    Channels channels;
};


} // namespace tmrw

#endif

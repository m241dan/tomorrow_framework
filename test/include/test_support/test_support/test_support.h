#ifndef TEST_SUPPORT_HPP
#define TEST_SUPPORT_HPP

#include "tomorrow_framework/asio_processor.hpp"
#include "tomorrow_framework/event.hpp"

#include <boost/asio/io_context.hpp>

#include <ranges>

namespace test_support {
/***************************************************************************
 * Section for a Test Module that is one to one and just doubles the ouput *
 ***************************************************************************/

// Event Handler
struct DoublingEvent : public tmrw::Event<int> {};
struct DoublingEventHandler {
    static auto process(DoublingEvent x) -> std::map<int, std::any>;
};

// Module Setup
using DoublingInputEvents = tmrw::InputEvents<DoublingEvent>;
struct DoublingModule : public DoublingEventHandler, public tmrw::AsioProcessor<DoublingEventHandler, DoublingInputEvents> {
    using EventHandler_t = DoublingEventHandler;
    using AsioProcessor_t = tmrw::AsioProcessor<DoublingEventHandler, DoublingInputEvents>;

    DoublingModule(boost::asio::any_io_executor ex, tmrw::Channels channels);
};

/***************************************************************************************************************
 * Section for a Test Module that is one to many where it just multiplies based on which output channel it is. *
 ***************************************************************************************************************/

// Event Handler
struct MultiplyingEvent : public tmrw::Event<int> {};
struct MultiplyingEventHandler {
    static auto process(MultiplyingEvent event) -> std::map<int, std::any>;
};

// Module Setup
using MultiplyingInputEvents = tmrw::InputEvents<MultiplyingEvent>;
struct MultiplyingModule : public MultiplyingEventHandler, public tmrw::AsioProcessor<MultiplyingEventHandler, MultiplyingInputEvents> {
    using EventHandler_t = MultiplyingEventHandler;
    using AsioProcessor_t = tmrw::AsioProcessor<MultiplyingEventHandler, MultiplyingInputEvents>;

    MultiplyingModule(boost::asio::any_io_executor ex, tmrw::Channels channels);
};

/*
 *
 */
// Event Handler
struct MultiInputEvent1 : public tmrw::Event<std::string> {};
struct MultiInputEvent2 : public tmrw::Event<std::string> {};
struct MultiInputEventHandler {
    static auto process(MultiInputEvent1 event) -> std::map<int, std::any>;
    static auto process(MultiInputEvent2 event) -> std::map<int, std::any>;
};

// Module Setup
using MultiInputEvents = tmrw::InputEvents<MultiInputEvent1, MultiInputEvent2>;
struct MultiInputModule : public MultiInputEventHandler, public tmrw::AsioProcessor<MultiInputEventHandler, MultiInputEvents> {
    using AsioProcessor_t = tmrw::AsioProcessor<MultiInputEventHandler, MultiInputEvents>;

    MultiInputModule(boost::asio::any_io_executor ex, tmrw::Channels channels);
};

/*********************
 * Asio Test Support *
 *********************/
constexpr auto run_ctx(boost::asio::io_context& ctx, int times) -> void
{
    for (const auto _ : std::views::iota(0, times))
    {
        ctx.run_one();
    }
};

/*************************
 * Tomorrow Test Support *
 *************************/
template<typename Input, typename Output>
struct TestData {
    using Input_t = Input;
    using Output_t = Output;
    std::tuple<Input_t, int> input_value_to_input_index;
    std::map<int, Output_t> output_indexes_to_output_values;
};

template<int case_number>
struct TestCase {};

template<typename T>
concept test_case = requires {
    T::number_of_input_channels;
    T::output_indexes_to_number_of_output_channels;
};

template<test_case TC>
constexpr auto setup_channels(boost::asio::any_io_executor e, int channel_size)
{
    auto channels = tmrw::Channels{};

    for (const auto _ : std::views::iota(0UL, TC::number_of_input_channels))
    {
        channels.inputs.push_back(std::make_shared<tmrw::Channel>(e, channel_size));
    }

    for (const auto& [index, num_channels] : TC::output_indexes_to_number_of_output_channels)
    {
        for (const auto _ : std::views::iota(0, num_channels))
        {
            channels.outputs[index].push_back(std::make_shared<tmrw::Channel>(e, channel_size));
        }
    }

    return channels;
}
} // namespace test_support

#endif

// Test Includes
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

#include "test_support/test_support.h"

#include "boost/asio/io_context.hpp"
#include "boost/asio/use_future.hpp"
#include <tuple>

using namespace std::chrono_literals;
using namespace std::string_view_literals;
using boost::system::error_code;

namespace test_support {
template<>
struct TestCase<1> {
    using Module = DoublingModule;
    static constexpr auto description = "Doubling module one input to one output"sv;
    static constexpr auto GIVEN = "An integer input value of `{}`"sv;
    static constexpr auto WHEN = "the input value is passed in the input channel"sv;
    static constexpr auto THEN = "the output value from the channel is doubled."sv;
    static constexpr auto number_of_input_channels = std::tuple_size_v<Module::AsioProcessor_t::InputEvents_t>;
    static constexpr auto output_indexes_to_number_of_output_channels = std::array{
        std::make_tuple(1, 1)};

    static constexpr auto get_data()
    {
        // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        return std::array{
            TestData<int, int>{
                .input_value_to_input_index = {10, 0},
                .output_indexes_to_output_values = {{1, 20}},
            },
            TestData<int, int>{
                .input_value_to_input_index = {20, 0},
                .output_indexes_to_output_values = {{1, 40}}}};
        // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    }
};

template<>
struct TestCase<2> {
    using Module = MultiplyingModule;
    static constexpr auto description = "Simple one to many multiplying module"sv;
    static constexpr auto GIVEN = "An integer input value of `{}`"sv;
    static constexpr auto WHEN = "we send the value on the input channel"sv;
    static constexpr auto THEN = "there will be an output value on each channel that is multiplied by its index."sv;
    static constexpr auto number_of_input_channels = std::tuple_size_v<Module::AsioProcessor_t::InputEvents_t>;
    static constexpr auto output_indexes_to_number_of_output_channels = std::array{
        std::make_tuple(1, 1),
        std::make_tuple(2, 1),
        std::make_tuple(3, 1),
    };

    static constexpr auto get_data()
    {
        // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        return std::array{
            TestData<int, int>{
                .input_value_to_input_index = {3, 0},
                .output_indexes_to_output_values = {{1, 3}, {2, 6}, {3, 9}}},
            TestData<int, int>{
                .input_value_to_input_index = {6, 0},
                .output_indexes_to_output_values = {{1, 6}, {2, 12}, {3, 18}}}};
        // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    }
};

template<>
struct TestCase<3> {
    using Module = MultiplyingModule;
    static constexpr auto description = "Simple one to many multiplying module with fanout on channel indexes"sv;
    static constexpr auto GIVEN = "An integer input value of `{}`"sv;
    static constexpr auto WHEN = "we send the value on the input channel"sv;
    static constexpr auto THEN = "there will be an output value all channels for a specific index."sv;
    static constexpr auto number_of_input_channels = std::tuple_size_v<Module::AsioProcessor_t::InputEvents_t>;
    static constexpr auto output_indexes_to_number_of_output_channels = std::array{
        std::make_tuple(1, 2),
        std::make_tuple(2, 3),
        std::make_tuple(3, 4),
    };

    static constexpr auto get_data()
    {
        // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        return std::array{
            TestData<int, int>{
                .input_value_to_input_index = {3, 0},
                .output_indexes_to_output_values = {{1, 3}, {2, 6}, {3, 9}}},
            TestData<int, int>{
                .input_value_to_input_index = {6, 0},
                .output_indexes_to_output_values = {{1, 6}, {2, 12}, {3, 18}}}};
        // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    }
};

} // namespace test_support


TEMPLATE_TEST_CASE("Testing TestCases", "[template]", test_support::TestCase<1>, test_support::TestCase<2>, test_support::TestCase<3>)
{
    // Setup Section
    auto ctx = boost::asio::io_context{};
    const auto test_data = GENERATE(from_range(TestType::get_data()));
    static constexpr auto channel_size = 10;
    auto channels = test_support::setup_channels<TestType>(ctx.get_executor(), channel_size);
    auto module = typename TestType::Module{ctx.get_executor(), channels};
    module.spawn_processors();

    // Test Values
    const auto [input_value, input_index] = test_data.input_value_to_input_index;

    // BDD Scenario
    INFO(TestType::description);
    GIVEN(std::format(TestType::GIVEN, input_value))
    {
        ctx.run_one(); // setup and start receiving

        WHEN(TestType::WHEN)
        {
            channels.inputs[input_index]->try_send(error_code{}, tmrw::make_gmd(input_value));
            ctx.run_one(); // receive event, process, and send to output channels

            THEN(TestType::THEN)
            {
                std::vector<std::tuple<std::future<tmrw::GenericModuleData>, int>> received;
                for (const auto [output_index, expected_value] : test_data.output_indexes_to_output_values)
                {
                    for (const auto& channel : channels.outputs[output_index])
                    {
                        received.emplace_back(channel->async_receive(boost::asio::use_future), expected_value);
                    }
                }

                test_support::run_ctx(ctx, static_cast<int>(received.size()) * 2); // one setup for each future and then another to put the value in it
                for (auto& [future, expected] : received)
                {
                    if (future.wait_for(0s) != std::future_status::ready)
                    {
                        FAIL("Futures were not ready when expected.");
                    }
                    const auto gmd = future.get();
                    const auto* actual = std::get<int>(gmd);

                    REQUIRE(*actual == expected);
                }
            }
        }
    }
}

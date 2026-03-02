// Test Includes
#include <catch2/catch_test_macros.hpp>

#include "tomorrow_framework/config.hpp"

#include <string_view>

using namespace std::literals;
//{
//  "thread_pool_size": 1,
//  "channels": [
//    {"name": "timer channel", "size": 1000},
//    {"name": "input channel", "size": 1000},
//    {"name": "output channel", "size": 1000},
//    {"name": "final channel", "size": 1000},
//    {"name": "synchronizer", "size": 0},
//    {"name": "lock", "size": 1}
//  ],
//  "timers": [
//    {
//      "name": "input timer",
//      "config": {},
//      "outputs": ["timer channel"]
//    }
//  ],
//  "strands": ["one", "two"],
//  "modules": [
//    {
//      "name": "input producer",
//      "config": {},
//      "inputs": ["timer channel"],
//      "outputs": {
//        1: ["input channel"]
//      }
//    },
//    {
//      "name": "output producer",
//      "config": {},
//      "inputs": ["input channel"],
//      "outputs": {
//        1: ["final channel"]
//      }
//    }
//  ]
//}
namespace {
constexpr auto not_json = R"not_json(}not json{)not_json"sv;
constexpr auto bad_field = R"bad_field(
{
    "a_random_field": 4
}
)bad_field"sv;
constexpr auto good_input = R"json(
{
   "thread_pool_size": 2,
   "channels": [
      {"name": "input channel", "size": 1000},
      {"name": "output channel", "size": 900}
   ]
}
        )json"sv;

} // namespace
TEST_CASE("Construct config from a string")
{
    SECTION("Receiving a bad json")
    {

        REQUIRE(not tmrw::make_config(not_json));
    }
    SECTION("Receiving a field that isn't present")
    {

        REQUIRE(not tmrw::make_config(bad_field));
    }
    SECTION("Creation of config is successful")
    {
        static constexpr auto expected_thread_pool_size = 2;
        const auto expected_channels = std::vector<tmrw::Config::Channel>{
            {.name = "input channel", .size = 1000},
            {.name = "output channel", .size = 900},
        };
        const auto config = tmrw::make_config(good_input);
        const auto& actual = config.value();

        REQUIRE(actual.thread_pool_size == expected_thread_pool_size);
        for (const auto& [actual, expected] : std::views::zip(actual.channels, expected_channels))
        {
            REQUIRE(actual.name == expected.name);
            REQUIRE(actual.size == expected.size);
        }
    }
}

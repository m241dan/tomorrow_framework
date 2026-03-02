// Test Includes
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

#include "tomorrow_framework/config.hpp"
#include "tomorrow_framework/factory.hpp"
#include "tomorrow_framework/module.hpp"

#include <stdexcept>

struct TestModule : public tmrw::Module {
    auto start() -> void override
    {
    }
};

//tmrw::RegisterModule("123test321", [](std::string_view /*config*/) -> std::unique_ptr<tmrw::Module> {
//    return std::make_unique<TestModule>();
//});

TEST_CASE("You can only have one Factory, there can only be one test case.")
{
    static auto factory = tmrw::Factory{};

    SECTION("Creating a second factory will throw.")
    {

        REQUIRE_THROWS_AS(tmrw::Factory{}, std::runtime_error);
    }
    SECTION("Attempting to create a thread_pool with a negative number of threads fails.")
    {
        const auto config = tmrw::Config{.thread_pool_size = -2};
        REQUIRE_THROWS_AS(factory.create_thread_pool(config), std::out_of_range);
    }
    SECTION("Creating a thread_pool with a valid number succeeds")
    {
        const auto config = tmrw::Config{.thread_pool_size = 4};
        REQUIRE_NOTHROW(factory.create_thread_pool(config));
    }
    SECTION("Tests that require a thread_pool")
    {
        auto thread_pool = boost::asio::thread_pool{1};

        SECTION("Attempting to create channels with one having a negative size fails.")
        {
            const auto config = tmrw::Config{
                .channels = {
                    {.name = "size negative", .size = -3},
                },
            };
            REQUIRE_THROWS_AS(factory.create_channels(config, thread_pool.get_executor()), std::out_of_range);
        }
        SECTION("Attempting to create channels with one having an empty string fails.")
        {
            const auto config = tmrw::Config{
                .channels = {
                    {.name = "", .size = 10},
                },
            };
            REQUIRE_THROWS_AS(factory.create_channels(config, thread_pool.get_executor()), std::invalid_argument);
        }
        SECTION("Creating channels with valid config is successful.")
        {
            const auto config = tmrw::Config{
                .channels = {
                    {.name = "input_channel", .size = 1000},
                    {.name = "output_channel", .size = 10},
                },
            };
            auto channels = factory.create_channels(config, thread_pool.get_executor());

            REQUIRE(not channels.empty());
            for (const auto& [name, channel] : channels)
            {
                REQUIRE(channel);
            }
        }
    }
}

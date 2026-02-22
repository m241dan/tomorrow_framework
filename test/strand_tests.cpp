// Test Includes
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include "boost/asio/any_io_executor.hpp"
#include "boost/asio/executor_work_guard.hpp"
#include "boost/asio/io_context.hpp"
#include "boost/asio/use_future.hpp"
#include "boost/system/detail/error_code.hpp"
#include "tomorrow_framework/asio_processor.hpp"
#include "tomorrow_framework/channels.hpp"
#include "tomorrow_framework/event.hpp"
#include "tomorrow_framework/generic_module_data.hpp"

#include <boost/asio/strand.hpp>
#include <boost/asio/thread_pool.hpp>
#include <thread>

using boost::system::error_code;
using namespace std::chrono_literals;
static constexpr auto channel_size = 10;

// Event Handler
struct TestEvent : public tmrw::Event<int> {};
struct TestEventHandler {
    static auto process(TestEvent /*unused*/) -> std::map<int, std::any>
    {
        return {{1, std::this_thread::get_id()}};
    }
};

using TestEvents = tmrw::InputEvents<TestEvent>;
struct TestEventModule : public TestEventHandler, public tmrw::AsioProcessor<TestEventHandler, TestEvents> {
    using AsioProcessor_t = tmrw::AsioProcessor<TestEventHandler, TestEvents>;

    TestEventModule(boost::asio::any_io_executor ex, tmrw::Channels channels)
        : TestEventHandler{}
        , AsioProcessor_t{.executor{std::move(ex)}, .channels{std::move(channels)}}
    {
    }
};


TEST_CASE("Strands are respected")
{
    /*
     * SETUP
     */
    auto ctx = boost::asio::io_context{};
    auto ctx_strand = boost::asio::io_context{};
    auto guard_one = boost::asio::make_work_guard(ctx);
    auto guard_two = boost::asio::make_work_guard(ctx_strand);
    auto strand = boost::asio::make_strand(ctx_strand);
    auto channels = tmrw::Channels{
        .inputs = {std::make_shared<tmrw::Channel>(ctx.get_executor(), channel_size)},
        .outputs = {
            {1, {std::make_shared<tmrw::Channel>(ctx.get_executor(), channel_size)}},
        },
    };
    auto module = TestEventModule{strand, channels};
    module.spawn_processors();

    auto thread_one = std::jthread{[&ctx](const std::stop_token& token) {
        while (not token.stop_requested())
        {
            ctx.run_for(1ms);
            std::this_thread::yield();
        }
    }};

    auto thread_two = std::jthread{[&ctx_strand](const std::stop_token& token) {
        while (not token.stop_requested())
        {
            ctx_strand.run_for(1ms);
            std::this_thread::yield();
        };
    }};

    /*
     * ACTUAL TEST
     */
    channels.inputs[0]->try_send(error_code{}, tmrw::make_gmd(1));
    auto future = channels.outputs.at(1)[0]->async_receive(boost::asio::use_future);

    if (future.wait_for(1s) != std::future_status::ready)
    {
        FAIL("Future not ready when it is expected to be.");
    }
    const auto gmd = future.get();
    const auto* actual = std::get<std::thread::id>(gmd);

    REQUIRE(thread_two.get_id() == *actual);
}

#include "test_support/test_support.h"
#include <cctype>

namespace test_support {

auto DoublingEventHandler::process(DoublingEvent x) -> std::map<int, std::any>
{
    return {
        {1, x.data * 2}};
}

DoublingModule::DoublingModule(boost::asio::any_io_executor ex, tmrw::Channels channels)
    : DoublingEventHandler{}
    , AsioProcessor_t{.executor = std::move(ex), .channels = std::move(channels)}
{
}

auto MultiplyingEventHandler::process(MultiplyingEvent event) -> std::map<int, std::any>
{
    return {
        {1, event.data * 1},
        {2, event.data * 2},
        {3, event.data * 3},
    };
};

MultiplyingModule::MultiplyingModule(boost::asio::any_io_executor ex, tmrw::Channels channels)
    : MultiplyingEventHandler{}
    , AsioProcessor_t{.executor = std::move(ex), .channels = std::move(channels)}

{
}

namespace {
auto capitalize(std::string to_cap) -> std::string
{
    to_cap[0] = static_cast<char>(std::toupper(to_cap[0]));
    return to_cap;
}
} // namespace
//
auto MultiInputEventHandler::process(MultiInputEvent1 event) -> std::map<int, std::any>
{
    return {{1, capitalize(std::move(event.data))}};
}

auto MultiInputEventHandler::process(MultiInputEvent2 event) -> std::map<int, std::any>
{
    return {{1, capitalize(std::move(event.data))}};
}

MultiInputModule::MultiInputModule(boost::asio::any_io_executor ex, tmrw::Channels channels)
    : MultiInputEventHandler{}
    , AsioProcessor_t{.executor = std::move(ex), .channels = std::move(channels)}
{
}


} // namespace test_support

#ifndef TOMORROW_FRAMEWORK_FACTORY_HPP
#define TOMORROW_FRAMEWORK_FACTORY_HPP

#include "tomorrow_framework/channels.hpp"
#include "tomorrow_framework/config.hpp"

#include "boost/asio/any_io_executor.hpp"
#include "boost/asio/thread_pool.hpp"

#include <algorithm>
#include <atomic>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string_view>

namespace tmrw {

class Module;

struct RegisterModule {
    RegisterModule(std::string_view, std::function<std::unique_ptr<Module>(std::string_view)>);
    RegisterModule(const RegisterModule&) = delete;
    RegisterModule(RegisterModule&&) = delete;
    auto operator=(const RegisterModule&) = delete;
    auto operator=(RegisterModule&&) = delete;
    ~RegisterModule() = default;
};

struct [[nodiscard("You only get one Factory, so you should probably capture it.")]]
Factory {
    Factory()
    {
        static std::atomic<bool> created{false};
        if (created.exchange(true))
        {
            throw std::runtime_error("You can only create ONE factory.");
        }
    }

    /**
     * Function to create an asio thread pool.
     *
     * nb: It is intentially NOT static.
     */
    [[nodiscard]]
    auto create_thread_pool(const tmrw::Config& config) -> boost::asio::thread_pool // NOLINT(*static*)
    {
        return boost::asio::thread_pool{static_cast<std::size_t>(config.thread_pool_size)};
    }

    [[nodiscard]]
    auto create_channels(const tmrw::Config& config, const boost::asio::any_io_executor& ex) -> ChannelMap // NOLINT(*static*)
    {
        const auto& create_channel = [&](const auto& channel) {
            if (channel.size < 0)
            {
                throw std::out_of_range{"Negative channel sizes are not allowed."};
            }
            if (channel.name.empty())
            {
                throw std::invalid_argument{"Empty strings are not valid channel names."};
            }

            return std::make_pair(channel.name, std::make_shared<Channel>(ex, channel.size));
        };

        // clang-format off
        return config.channels
            | std::views::transform(create_channel)
            | std::ranges::to<ChannelMap>();
        ;
        // clang-format on
    }
};

} // namespace tmrw
#endif

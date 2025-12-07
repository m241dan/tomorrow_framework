#ifndef TOMORROW_FRAMEWORK_CHANNELS_HPP
#define TOMORROW_FRAMEWORK_CHANNELS_HPP

#include "generic_module_data.hpp"

#include <boost/asio/experimental/channel.hpp>
#include <map>

namespace tmrw {

/**
 * Defines an Alias for all Module Channels
 */
using Channel = boost::asio::experimental::channel<void(boost::system::error_code, GenericModuleData)>;

/**
 * This struct represents the Channels that will be passed into any module.
 */
struct Channels {
    std::vector<std::shared_ptr<Channel>> inputs;
    std::map<int, std::vector<std::shared_ptr<Channel>>> outputs;
};

} // namespace tmrw

#endif

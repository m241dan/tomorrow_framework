#ifndef TOMORROW_FRAMEWORK_CONFIG_HPP
#define TOMORROW_FRAMEWORK_CONFIG_HPP

#include <glaze/core/buffer_traits.hpp>
#include <glaze/core/common.hpp>
#include <glaze/core/context.hpp>
#include <glaze/json/read.hpp>

#include <concepts>
#include <expected>
#include <map>

namespace tmrw {

struct Config {
    struct Channel {
        std::string name;
        int size;
    };
    struct Module {
        std::string name;
        std::string config;
        std::vector<std::string> inputs;
        std::map<int, std::string> outputs;
    };
    int thread_pool_size;
    std::vector<Channel> channels;
    std::vector<Module> modules;
};


template<std::convertible_to<std::string_view> Input>
auto make_config(const Input& input) -> std::expected<Config, glz::error_ctx> // leaks implementation details
{
    return glz::read_json<Config>(input);
}

} // namespace tmrw

#endif

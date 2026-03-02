#ifndef TOMORROW_FRAMEWORK_MODULE_HPP
#define TOMORROW_FRAMEWORK_MODULE_HPP

namespace tmrw {

struct Module { // NOLINT
    virtual auto start() -> void = 0;
    virtual ~Module() = default;
};

} // namespace tmrw
#endif

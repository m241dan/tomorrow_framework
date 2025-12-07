add_library(tomorrow_framework INTERFACE)
add_library(tomorrow_framework::tomorrow_framework ALIAS tomorrow_framework)

target_include_directories(tomorrow_framework
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(tomorrow_framework INTERFACE Boost::asio)

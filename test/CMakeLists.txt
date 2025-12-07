find_package(Catch2 REQUIRED)

add_library(test_support test_support.cpp)
add_library(test_support::test_support ALIAS test_support)
target_link_libraries(test_support
    PUBLIC
        tomorrow_framework::tomorrow_framework
)
target_include_directories(test_support
    PUBLIC
        $<INSTALL_INTERFACE:include/test_support>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/test_support>
)

add_executable(module_tests module_tests.cpp)
target_link_libraries(module_tests
    PRIVATE
        Catch2::Catch2WithMain
        tomorrow_framework::tomorrow_framework
        test_support::test_support
)
target_compile_features(module_tests INTERFACE cxx_std_23)
add_test(NAME ModuleTests COMMAND module_tests)

add_executable(type_traits_tests type_traits_tests.cpp)
target_link_libraries(type_traits_tests PRIVATE Catch2::Catch2WithMain tomorrow_framework::tomorrow_framework)
target_compile_features(type_traits_tests INTERFACE cxx_std_23)
add_test(NAME TypeTraitsTests COMMAND type_traits_tests)

add_executable(concepts_tests concepts_tests.cpp)
target_link_libraries(concepts_tests PRIVATE Catch2::Catch2WithMain tomorrow_framework::tomorrow_framework)
target_compile_features(concepts_tests INTERFACE cxx_std_23)
add_test(NAME ConceptsTests COMMAND concepts_tests)

add_executable(generic_module_data generic_module_data_tests.cpp)
target_link_libraries(generic_module_data PRIVATE Catch2::Catch2WithMain tomorrow_framework::tomorrow_framework)
target_compile_features(generic_module_data INTERFACE cxx_std_23)
add_test(NAME GenericModuleDataTests COMMAND generic_module_data)

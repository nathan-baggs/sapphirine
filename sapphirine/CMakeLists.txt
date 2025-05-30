set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
    capstone
    GIT_REPOSITORY https://github.com/capstone-engine/capstone
    GIT_TAG 5.0.6)
FetchContent_MakeAvailable(capstone)

add_library(sapphirine STATIC
    src/hello.cpp
)
add_library(sapphirine::sapphirine ALIAS sapphirine)
target_compile_features(sapphirine PUBLIC cxx_std_23)
target_link_libraries(sapphirine PRIVATE capstone)
target_compile_options(sapphirine PRIVATE /W4 /WX)
target_include_directories(sapphirine PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

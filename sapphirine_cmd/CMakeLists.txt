add_executable(sapphirine_cmd
    main.cpp
)
target_compile_options(sapphirine_cmd PRIVATE /W4 /WX)
target_link_libraries(sapphirine_cmd PRIVATE
    sapphirine::sapphirine
)


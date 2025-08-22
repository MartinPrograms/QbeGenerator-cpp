file(GLOB_RECURSE example_sources
    "*.cpp"
    "*.h"
)

add_executable(example ${example_sources})

target_link_libraries(example
    PRIVATE
        QbeIR
)
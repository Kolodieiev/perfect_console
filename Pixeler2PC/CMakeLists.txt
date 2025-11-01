cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

file(GLOB_RECURSE CPP_SOURCES
    "src/*.cpp"
    "src/**/*.cpp"
)

set(C_SOURCES
    "src/pixeler/driver/graphics/U8g2/u8g2_fonts.c"
    "src/pixeler/String/stdlib_noniso.c"
)

add_executable(main ${CPP_SOURCES} ${C_SOURCES}) 
set_source_files_properties(${C_SOURCES} PROPERTIES LANGUAGE C)

target_compile_features(main PRIVATE cxx_std_17)
target_link_libraries(main PRIVATE SFML::Graphics)

# Для збірки з локального пакету SFML можна використовувати скрипт нижче.

# cmake_minimum_required(VERSION 3.28)
# project(CMakeSFMLProject LANGUAGES CXX C)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# find_package(SFML 3.0.2 REQUIRED COMPONENTS Graphics)
# file(GLOB_RECURSE CPP_SOURCES
#     "src/*.cpp"
#     "src/**/*.cpp"
# )

# set(C_SOURCES
#     "src/graphics/U8g2/u8g2_fonts.c"
#     "src/pixeler/String/stdlib_noniso.c"
# )

# add_executable(main ${CPP_SOURCES} ${C_SOURCES}) 
# set_source_files_properties(${C_SOURCES} PROPERTIES LANGUAGE C)

# target_compile_features(main PRIVATE cxx_std_17)
# target_link_libraries(main PRIVATE SFML::Graphics)

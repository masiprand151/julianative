cmake_minimum_required(VERSION 3.21)

project(JuliaNative
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    Svg
    Multimedia
    MultimediaWidgets
)

set(APP_SOURCES
    app/main.cpp
    app/JuliaRuntime.cpp
    app/Style.cpp
    app/Media.cpp
    app/UiRenderer.cpp
)

# ---- release remove comments ----
# add_executable(JuliaNative WIN32
#     ${APP_SOURCES}
# )

# ---- DEBUG ----
add_executable(JuliaNative 
    ${APP_SOURCES}
)

target_link_libraries(JuliaNative PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Svg
    Qt6::Multimedia
    Qt6::MultimediaWidgets
)

target_include_directories(JuliaNative PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

set_target_properties(JuliaNative PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
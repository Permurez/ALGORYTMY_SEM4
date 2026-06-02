cmake_minimum_required(VERSION 3.16)
project(TicTacToe VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    main.cpp
    Board.cpp
    Game.cpp
    SI.cpp
    GUI.cpp
    Menu.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)
target_link_libraries(${PROJECT_NAME}
    SFML::Graphics
    SFML::Window
    SFML::System
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /O2)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -O2)
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/assets)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    )
endif()

cmake_minimum_required(VERSION 3.22)
project(GojoLimitlessSimulator)

set(CMAKE_CXX_STANDARD 17)

# try local SFML first, fall back to system install
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/SFML/lib/cmake/SFML")
    set(SFML_STATIC_LIBRARIES ON)
    set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SFML/lib/cmake/SFML")
endif()

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

add_executable(GojoLimitlessSimulator 
    src/main.cpp
    src/core/Utils.cpp
    src/gameplay/Enemy.cpp
    src/gameplay/EnemyManager.cpp
    src/gameplay/Particle.cpp
    src/gameplay/ParticleSystem.cpp
    src/systems/GameWorld.cpp
    src/systems/ScreenShake.cpp
    src/abilities/BlueAbility.cpp
    src/abilities/RedAbility.cpp
    src/abilities/PurpleAbility.cpp
)

target_include_directories(GojoLimitlessSimulator PRIVATE src)

target_link_libraries(GojoLimitlessSimulator PRIVATE SFML::Graphics SFML::Window SFML::System)
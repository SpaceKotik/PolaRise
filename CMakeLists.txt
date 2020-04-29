cmake_minimum_required(VERSION 3.15)
project(PolaRise)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
SET(PLATFORM_SPECIFIC_LIBS "-lpthread")
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
set(SFML_STATIC_LIBRARIES TRUE)

include_directories(.)
include_directories(include)

add_executable(PolaRise
        include/dump.hpp
        include/game.hpp
        include/player.hpp
        src/level.cpp
        include/level.hpp
        include/rayTracing.hpp
        include/tile.hpp
        src/consts.cpp
        src/dump.cpp
        src/game.cpp
        src/player.cpp
        src/main.cpp
        src/rayTracing.cpp
        src/tile.cpp

        SmartVector2/SmartVector2f.cpp
        SmartVector2/SmartVector2f.h
        src/lightEmitter.cpp
        include/lightEmitter.h
        include/lightScene.h
        src/lightScene.cpp
        include/consts.h
        util/FpsCounter/fpsCounter.cpp
        util/FpsCounter/fpsCounter.h
        include/emitterBehaviour.h
        src/emitterBehaviour.cpp
        util/eMath/eMath.cpp
        util/eMath/eMath.h
        include/lantern.h
        src/lantern.cpp
        util/ResourceHolders/TextureHolder/textureHolder.cpp
        util/ResourceHolders/TextureHolder/textureHolder.h util/ResourceHolders/resourceIdentifiers.h util/ResourceHolders/ShaderHolder/shaderHolder.cpp util/ResourceHolders/ShaderHolder/shaderHolder.h Game/sceneLoader.cpp Game/sceneLoader.h)
target_link_libraries(PolaRise sfml-graphics sfml-window sfml-system)

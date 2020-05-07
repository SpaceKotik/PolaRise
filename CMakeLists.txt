cmake_minimum_required(VERSION 3.15)
project(PolaRise)

#set(MALLOC_CHECK_ "0")
#-fno-stack-protector
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")
SET(PLATFORM_SPECIFIC_LIBS "-lpthread")
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
set(SFML_STATIC_LIBRARIES TRUE)

include_directories(.)
include_directories(include)

add_executable(PolaRise
        Game/include/dump.hpp
        Game/include/game.hpp
        Game/include/player.hpp
        Game/src/level.cpp
        Game/include/level.hpp
        LightSystem/include/rayTracing.hpp
        Game/include/tile.hpp
        Game/src/consts.cpp
        Game/src/dump.cpp
        Game/src/game.cpp
        Game/src/player.cpp
        Game/src/main.cpp
        LightSystem/src/rayTracing.cpp
        Game/src/tile.cpp

        util/SmartVector2/SmartVector2f.cpp
        util/SmartVector2/SmartVector2f.h
        LightSystem/src/lightEmitter.cpp
        LightSystem/include/lightEmitter.h
        LightSystem/include/lightScene.h
        LightSystem/src/lightScene.cpp
        Game/include/consts.h
        util/FpsCounter/fpsCounter.cpp
        util/FpsCounter/fpsCounter.h
        LightSystem/include/emitterBehaviour.h
        LightSystem/src/emitterBehaviour.cpp
        util/eMath/eMath.cpp
        util/eMath/eMath.h
        Game/include/lantern.h
        Game/src/lantern.cpp
        util/ResourceHolders/TextureHolder/textureHolder.cpp
        util/ResourceHolders/TextureHolder/textureHolder.h
        util/ResourceHolders/resourceIdentifiers.h
        util/ResourceHolders/ShaderHolder/shaderHolder.cpp
        util/ResourceHolders/ShaderHolder/shaderHolder.h
        Game/src/sceneLoader.cpp Game/include/sceneLoader.h
        GUI/Menu/MenuGUI.cpp GUI/Menu/MenuGUI.h GUI/Pause/PauseGUI.cpp GUI/Pause/PauseGUI.h GUI/BaseGUI/BaseGUI.cpp GUI/BaseGUI/BaseGUI.h)
target_link_libraries(PolaRise sfml-graphics sfml-window sfml-system -ltgui -lsfml-graphics -lsfml-window -lsfml-system)

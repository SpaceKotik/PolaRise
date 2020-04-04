#pragma once

/// Forward declaration of SFML classes
namespace sf {
    class Texture;
    class Shader;
}

namespace Textures {
    enum ID {
        TitleScreen,
        LoadingScreen,
        GameOverScreen,
        Title,
        Controls,
        GUI,
        TileMap,
        Player,
        Enemy,
        Key,
        PlayerMods,
        Background
    };
}

namespace Shaders {
    enum ID {
        Shadow,
        Blur,
        Pixel
    };
}

/// Forward declaration and a few type definitions
template <class Resource, class Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Texture, Textures::ID>   TextureHolder;
typedef ResourceHolder<sf::Shader, Shaders::ID>   ShaderHolder;
//typedef ResourceHolder<sf::Font, Fonts::ID>           FontHolder;
//typedef ResourceHolder<sf::SoundBuffer, Sounds::ID>   SoundHolder;

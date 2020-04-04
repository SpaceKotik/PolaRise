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

//typedef ResourceHolders<sf::Texture, Textures::ID>   TextureHolder;
///typedef ResourceHolders<sf::Shader, Shaders::ID>   ShaderHolder;
//typedef ResourceHolders<sf::Font, Fonts::ID>           FontHolder;
//typedef ResourceHolders<sf::SoundBuffer, Sounds::ID>   SoundHolder;

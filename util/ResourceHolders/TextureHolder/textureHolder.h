#pragma once
#include <memory>
#include <map>
#include <cassert>
#include "util/ResourceHolders/resourceIdentifiers.h"


class TextureHolder {
public:
    void load(Textures::ID id, const std::string& filename);
    sf::Texture& get(Textures::ID id);
    void setRepeat(Textures::ID id, bool repeat);
    const sf::Texture& get(Textures::ID id) const;

protected:
    void insertResource(Textures::ID, std::unique_ptr<sf::Texture> resource);

protected:
    std::map<Textures::ID, std::unique_ptr<sf::Texture>> mResourceMap;
};


#include "textureHolder.h"
#include "util/ResourceHolders/resourceIdentifiers.h"
#include <SFML/Graphics.hpp>

using namespace sf;

void TextureHolder::load(Textures::ID id, const std::string& filename){
    /// Create and load resource
    std::unique_ptr<Texture> resource(new Texture());
    if (!resource->loadFromFile(filename))
        throw std::runtime_error("ResourceHolders::load - Failed to load " + filename);

    /// If loading successful, insert resource to map
    insertResource(id, std::move(resource));
}

Texture& TextureHolder::get(Textures::ID id){
    auto found = mResourceMap.find(id);
    assert(found != mResourceMap.end());

    return *found->second;
}

void TextureHolder::setRepeat(Textures::ID id, bool repeat) {
    auto found = mResourceMap.find(id);
    assert(found != mResourceMap.end());

    found->second->setRepeated(repeat);
}

const Texture& TextureHolder::get(Textures::ID id) const {
    auto found = mResourceMap.find(id);
    assert(found != mResourceMap.end());

    return *found->second;
}

void TextureHolder::insertResource(Textures::ID id, std::unique_ptr<Texture> resource){
    /// Insert and check success
    auto inserted = mResourceMap.insert(std::make_pair(id, std::move(resource)));
    assert(inserted.second);
}


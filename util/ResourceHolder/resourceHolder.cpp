#include "resourceHolder.h"
#include "resourceIdentifier.h"
#include <SFML/Graphics.hpp>

template <class Resource, class Identifier>
void ResourceHolder<Resource, Identifier>::load(Identifier id, const std::string& filename){
    /// Create and load resource
    std::unique_ptr<Resource> resource(new Resource());
    if (!resource->loadFromFile(filename))
        throw std::runtime_error("ResourceHolder::load - Failed to load " + filename);

    /// If loading successful, insert resource to map
    insertResource(id, std::move(resource));
}

template <class Resource, class Identifier>
Resource& ResourceHolder<Resource, Identifier>::get(Identifier id){
    auto found = mResourceMap.find(id);
    assert(found != mResourceMap.end());

    return *found->second;
}

template <class Resource, class Identifier>
void ResourceHolder<Resource, Identifier>::setRepeat(Identifier id, bool repeat) {
    auto found = mResourceMap.find(id);
    assert(found != mResourceMap.end());

    found->second->setRepeated(repeat);
}



template <class Resource, class Identifier>
const Resource& ResourceHolder<Resource, Identifier>::get(Identifier id) const {
    auto found = mResourceMap.find(id);
    assert(found != mResourceMap.end());

    return *found->second;
}

template <class Resource, class Identifier>
void ResourceHolder<Resource, Identifier>::insertResource(Identifier id, std::unique_ptr<Resource> resource){
    /// Insert and check success
    auto inserted = mResourceMap.insert(std::make_pair(id, std::move(resource)));
    assert(inserted.second);
}

template class ResourceHolder<sf::Texture, Textures::ID>;

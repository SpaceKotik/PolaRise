#include "shaderHolder.h"
#include "util/ResourceHolders/resourceIdentifiers.h"
#include <SFML/Graphics.hpp>

using namespace sf;

void ShaderHolder::load(Shaders::ID id, const std::string& filename){
    /// Create and load resource
    std::unique_ptr<Shader> resource(new Shader());
    if (!resource->loadFromFile(filename, sf::Shader::Fragment))
        throw std::runtime_error("ResourceHolders::load - Failed to load " + filename);

    /// If loading successful, insert resource to map
    insertResource(id, std::move(resource));
}

Shader& ShaderHolder::get(Shaders::ID id){
    auto found = mResourceMap.find(id);
    assert(found != mResourceMap.end());

    return *found->second;
}

const Shader& ShaderHolder::get(Shaders::ID id) const {
    auto found = mResourceMap.find(id);
    assert(found != mResourceMap.end());

    return *found->second;
}

void ShaderHolder::insertResource(Shaders::ID id, std::unique_ptr<Shader> resource){
    /// Insert and check success
    auto inserted = mResourceMap.insert(std::make_pair(id, std::move(resource)));
    assert(inserted.second);
}

template<class T>
void ShaderHolder::setParam(Shaders::ID id, const std::string &param, T value) {
    auto found = mResourceMap.find(id);
    assert(found != mResourceMap.end());
    found->second->setUniform(param, value);
}

void ShaderHolder::setParam(Shaders::ID id, const std::string &param, Texture& value) {
    auto found = mResourceMap.find(id);
    assert(found != mResourceMap.end());
    found->second->setUniform(param, value);
}

template void ShaderHolder::setParam<float> (Shaders::ID, const std::string&, float);
template void ShaderHolder::setParam<Vector2f> (Shaders::ID, const std::string&, Vector2f);
template void ShaderHolder::setParam<Vector3f> (Shaders::ID, const std::string&, Vector3f);
//template void ShaderHolder::setParam<Texture> (Shaders::ID, const std::string&, Texture);

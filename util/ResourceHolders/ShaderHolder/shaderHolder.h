#pragma once
#include <memory>
#include <map>
#include <cassert>
#include "util/ResourceHolders/resourceIdentifiers.h"

class ShaderHolder {
public:
    void load(Shaders::ID id, const std::string& filename);
    sf::Shader& get(Shaders::ID id);
    const sf::Shader& get(Shaders::ID id) const;

    template<typename T>
    void setParam(Shaders::ID id, const std::string& param, T);

protected:
    void insertResource(Shaders::ID, std::unique_ptr<sf::Shader> resource);

protected:
    std::map<Shaders::ID, std::unique_ptr<sf::Shader>> mResourceMap;
};


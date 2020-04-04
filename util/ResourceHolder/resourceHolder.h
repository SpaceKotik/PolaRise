#pragma once
#include <memory>
#include <map>
#include <cassert>


template <class Resource, class Identifier>
class ResourceHolder
{
public:
    void load(Identifier id, const std::string& filename);
    Resource& get(Identifier id);
    void setRepeat(Identifier id, bool repeat);
    const Resource& get(Identifier id) const;

protected:
    void insertResource(Identifier id, std::unique_ptr<Resource> resource);

protected:
    std::map<Identifier, std::unique_ptr<Resource>> mResourceMap;
};



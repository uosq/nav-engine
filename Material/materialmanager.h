#pragma once
#include <map>
#include <string>
#include <memory>
#include <vector>
#include "material.h"

class MaterialManager {
private:
    std::map<std::string, std::unique_ptr<Material>> materials;
    
    MaterialManager() {}
    
public:
    static MaterialManager& GetInstance() {
        static MaterialManager instance;
        return instance;
    }
    
    Material* LoadMaterial(const std::string& name, const char* path) {
        auto it = materials.find(name);
        if (it != materials.end()) {
            return it->second.get();
        }
        
        auto material = std::make_unique<Material>(path);
        if (material->Load()) {
            Material* ptr = material.get();
            materials[name] = std::move(material);
            return ptr;
        }
        return nullptr;
    }
    
    Material* GetMaterial(const std::string& name) {
        auto it = materials.find(name);
        return (it != materials.end()) ? it->second.get() : nullptr;
    }
    
    std::vector<std::string> GetMaterialNames() const {
        std::vector<std::string> names;
        for (const auto& pair : materials) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    void UnloadAll() {
        materials.clear();
    }
};
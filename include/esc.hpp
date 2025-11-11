#pragma once
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <cstdint>

// 超最小ECS（学習用、PODコンポーネント想定）
using Entity = uint32_t;
const Entity INVALID_ENTITY = 0;

class IComponentArray { public: virtual ~IComponentArray() = default; };

template<typename T>
class ComponentArray : public IComponentArray {
public:
    std::vector<T> data;
    std::vector<Entity> entities; // parallel vectors (simple)
    void insert(Entity e, const T& comp) {
        entities.push_back(e);
        data.push_back(comp);
    }
    bool remove(Entity e) {
        for (size_t i = 0; i < entities.size(); ++i) {
            if (entities[i] == e) {
                std::swap(entities[i], entities.back());
                std::swap(data[i], data.back());
                entities.pop_back();
                data.pop_back();
                return true;
            }
        }
        return false;
    }
};

class World {
    Entity nextId = 1;
    std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> componentArrays;
public:
    Entity createEntity() { return nextId++; }

    template<typename T>
    void addComponent(Entity e, const T& comp) {
        auto ti = std::type_index(typeid(T));
        if (!componentArrays.count(ti)) {
            componentArrays[ti] = std::make_unique<ComponentArray<T>>();
        }
        auto arr = static_cast<ComponentArray<T>*>(componentArrays[ti].get());
        arr->insert(e, comp);
    }

    template<typename T>
    ComponentArray<T>* getComponentArray() {
        auto ti = std::type_index(typeid(T));
        if (!componentArrays.count(ti)) return nullptr;
        return static_cast<ComponentArray<T>*>(componentArrays[ti].get());
    }

    // シンプルな for_each（単一コンポーネント用）
    template<typename T, typename Func>
    void for_each(Func f) {
        auto arr = getComponentArray<T>();
        if (!arr) return;
        for (size_t i = 0; i < arr->entities.size(); ++i) {
            f(arr->entities[i], arr->data[i]);
        }
    }

    size_t entityCount() const { return nextId - 1; }
};
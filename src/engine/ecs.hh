#pragma once

#include <cstdint>
#include <unordered_map>
#include <map>
#include <memory>
#include <algorithm>

namespace ecs
{
class ECS;

using id_t = uint64_t;

class TypeIdGenerator
{
private:
    inline static id_t count_ = 1;

public:
    template <typename T>
    inline static const id_t id = count_++;
};

struct ComponentBase
{
    virtual void destroy_data(void* data) const = 0;
    virtual void move_data(void* src, void* dst) const = 0;
    virtual void construct_data(void* data) const = 0;
    virtual size_t size() const = 0;
};

template <typename C>
struct Component : public ComponentBase
{
    virtual void construct_data(void* data) const override;
    virtual void destroy_data(void* data) const override;
    virtual void move_data(void* src, void* dst) const override;
    virtual size_t size() const override;
    static id_t type_id();
};

class Entity
{
public:
    Entity() = delete;

    template <typename C, typename... Args>
    Entity& add(Args&&... args) const;

    id_t id() const;

private:
    Entity(ECS& ecs);

    id_t id_;
    ECS& ecs_;

    friend ECS;
};

struct Archetype
{
    std::vector<id_t> id;
    std::vector<id_t> entity_ids;
    std::vector<uint8_t*> component_data;
    std::vector<size_t> component_data_capacity;
};

class SystemBase
{
public:
private:
};

class System
{
public:
private:
};

class ECS
{
public:
    Entity new_entity();

    Archetype& get_archetype(const std::vector<id_t>& archetype_id);

    template <typename C>
    bool register_component();
    template <class C, typename... Args>
    C& add_component(id_t entity_id, Args&&... args);

    void register_system(size_t layer, SystemBase* system);
    void run_layer(size_t layer);
    void run_all();

    id_t new_id();

private:
    struct Record
    {
        Archetype* archetype = nullptr;
        size_t index = 0;
    };

    std::unordered_map<id_t, std::unique_ptr<ComponentBase>> component_map_;
    std::unordered_map<id_t, Record> entity_to_archetype_;
    std::vector<Archetype> archetypes_;
    std::map<size_t, std::vector<SystemBase*>> systems_;

    id_t entity_id_ = 1;
};

// Template implementations

template <typename C>
void Component<C>::construct_data(void* data) const
{
    new (dst) C();
}

template <typename C>
void Component<C>::destroy_data(void* data) const
{
    C* data_ptr = std::launder(reinterpret_cast<C*>(data));
    data_ptr->~C();
}

template <typename C>
void Component<C>::move_data(void* src, void* dst) const
{
    new (dst) C(std::move(*reinterpret_cast<C*>(src)));
}

template <typename C>
size_t Component<C>::size() const
{
    return sizeof(C);
}

template <typename C>
id_t Component<C>::type_id()
{
    return TypeIdGenerator::id<C>;
}

template <typename C, typename... Args>
Entity& Entity::add(Args&&... args) const
{
    ecs_.add_component<C, Args...>(id_, std::forward<Args>(args)...);
}

template <typename C>
bool ECS::register_component()
{
    auto comp_id = Component<C>::type_id();
    if (component_map_.contains(comp_id))
        return false;
    component_map_.emplace(comp_id, std::make_unique<C>());
    return true;
}

template <class C, typename... Args>
C& ECS::add_component(id_t entity_id, Args&&... args)
{
    register_component<C>();

    C* new_component = nullptr;
    id_t new_comp_id = Component<C>::type_id();
    size_t new_comp_size = component_map_[new_comp_id]->size();

    auto& record = entity_to_archetype_[entity_id];
    Archetype* new_archetype = nullptr;
    Archetype* old_archetype = record.archetype;

    if (old_archetype != nullptr)
    {
        for (size_t i = 0; i < old_archetype->id.size(); ++i)
        {
            if (old_archetype->id[i] == new_comp_id)
            {
                void* comp_addr = old_archetype->component_data[i][record.index * new_comp_size];
                new_component = new (comp_addr) C(std::forward<Args>(args)...);
                return *new_component;
            }
        }

        std::vector<id_t> new_archetype_id = old_archetype->id;
        auto it = std::upper_bound(new_archetype_id.begin(), new_archetype_id.end(), new_comp_id);
        new_archetype_id.insert(it, new_comp_id);
        new_archetype = &get_archetype(new_archetype_id);

        for (size_t i = 0; i < new_archetype_id.size(); ++i)
        {
            id_t comp_id = new_archetype_id[i];
            auto comp = component_map_[comp_id];
            size_t comp_size = comp->size();
            size_t current_size = new_archetype->entity_ids.size() * comp_size;
            size_t new_size = current_size + comp_size;

            if (new_size > new_archetype->component_data_capacity[i])
            {
                new_archetype->component_data_capacity[i] *= 2;
                uint8_t* new_data = new uint8_t[new_archetype->component_data_capacity[i]];
                std::memcpy(new_data, new_archetype->component_data[i], current_size);
                delete[] new_archetype->component_data[i];
                new_archetype->component_data[i] = new_data;
            }
        }
    }
    else
    {
        new_archetype = &get_archetype({new_comp_id});
        size_t current_size = new_archetype->entity_ids.size() * new_comp_size;
        size_t new_size = current_size + new_comp_size;

        if (new_size > new_archetype->component_data_capacity[0])
        {
            new_archetype->component_data_capacity[0] *= 2;
            uint8_t* new_data = new uint8_t[new_archetype->component_data_capacity[0]];
            std::memcpy(new_data, new_archetype->component_data[0], current_size);
            delete[] new_archetype->component_data[0];
            new_archetype->component_data[0] = new_data;
        }

        new_component = new (&new_archetype->component_data[0][current_size]) C(std::forward<Args>(args)...);
    }

    new_archetype->entity_ids.push_back(entity_id);
    record.index = new_archetype->entity_ids.size() - 1;
    record.archetype = new_archetype;

    return *new_component;
}

} // namespace ecs

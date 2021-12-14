#include "ecs.hh"

namespace ecs
{
Entity::Entity(ECS& ecs)
    : id_(ecs.new_id())
    , ecs_(ecs)
{}

id_t Entity::id() const
{
    return id_;
}

Entity ECS::new_entity()
{
    Entity e{*this};
    Record rec = {};
    entity_to_archetype_[e.id()] = rec;
    return e;
}

void ECS::register_system(size_t layer, SystemBase* system)
{
    systems_[layer].push_back(system);
}

void ECS::run_layer(size_t layer)
{
    for (SystemBase* system : systems_[layer])
    {
        const auto& archetype_key = system->archetype_key();
        for (const auto& archetype : archetypes_)
        {
            if (std::includes(archetype.id.begin(), archetype.id.end(), archetype_key.begin(), archetype_key.end()))
            {
                system->run(archetype);
            }
        }
    }
}

void ECS::run_all()
{
    for (const auto& [layer, systems] : systems_)
    {
        run_layer(layer);
    }
}

Archetype& ECS::get_archetype(const std::vector<id_t>& archetype_id)
{
    const auto& it = std::find_if(archetypes_.begin(), archetypes_.end(),
                                  [&](const auto& archetype) { return archetype.id == archetype_id; });
    if (it != archetypes_.end())
    {
        return *it;
    }

    auto& new_archetype = archetypes_.emplace_back();
    new_archetype.id = archetype_id;
    new_archetype.component_data.resize(archetype_id.size());
    new_archetype.component_data_capacity.resize(archetype_id.size());

    for (size_t i = 0; i < archetype_id.size(); ++i)
    {
        id_t comp_id = archetype_id[i];
        size_t comp_size = component_map_[comp_id]->size();
        new_archetype.component_data[i] = new uint8_t[comp_size];
        new_archetype.component_data_capacity[i] = comp_size;
    }

    return new_archetype;
}

id_t ECS::new_id()
{
    return entity_id_++;
}

} // namespace ecs

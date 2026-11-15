#ifndef DAMAGE_SYSTEM_HPP
#define DAMAGE_SYSTEM_HPP

#include "engine/ecs/blackboard.hpp"
#include "engine/ecs/component_storage.hpp"

class DamageSystem {
    public:
        void update(ComponentStorage& storage, Blackboard& blackboard);
};

#endif // DAMAGE_SYSTEM_HPP
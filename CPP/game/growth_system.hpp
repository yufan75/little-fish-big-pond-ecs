#ifndef GROWTH_SYSTEM_HPP
#define GROWTH_SYSTEM_HPP

#include "engine/ecs/blackboard.hpp"
#include "engine/ecs/component_storage.hpp"

class GrowthSystem {
    public:
        void update(ComponentStorage& storage, Blackboard& blackboard);
};

#endif // GROWTH_SYSTEM_HPP
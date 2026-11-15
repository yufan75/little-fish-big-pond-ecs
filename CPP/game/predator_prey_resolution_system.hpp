#ifndef PREDATOR_PREY_RESOLUTION_SYSTEM_HPP
#define PREDATOR_PREY_RESOLUTION_SYSTEM_HPP

#include "engine/ecs/blackboard.hpp"
#include "engine/ecs/component_storage.hpp"

/**
 * PredatorPreyReolutionSystem class
 * 
 */
class PredatorPreyResolutionSystem {
    public:
        void update(ComponentStorage& storage, Blackboard& blackboard);
};

#endif // PREDATOR_PREY_RESOLUTION_SYSTEM_HPP
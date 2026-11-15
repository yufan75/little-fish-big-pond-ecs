#include "player_control_system.hpp"

PlayerControlSystem::PlayerControlSystem(float speed)
    : speed_(speed) {
}

void PlayerControlSystem::update(ComponentStorage& storage, Blackboard& blackboard) {
    // Get all entities with Input components
    auto entities = storage.entities_with_component<Player>();
    
    // Update velocity for each entity that has both Input and Velocity
    for (Entity entity : entities) {
        // Get reference to Position
        auto pos_opt = storage.get_component<Position>(entity);
        if (!pos_opt.has_value()) {
            continue; // Skip entities without Position
        }

        // Check if entity has Velocity component
        auto vel_opt = storage.get_component<Velocity>(entity);
        if (!vel_opt.has_value()) {
            continue;  // Skip entities without Velocity
        }
        
        Position& pos = pos_opt->get();
        Velocity& vel = vel_opt->get();
        
        // Read World Position of the mouse from Blackboard
        double mx = blackboard.get_or<double>("mouse.x", 0.0);
        double my = blackboard.get_or<double>("mouse.y", 0.0);

        // Calculate the direction and distance from the mouse to the player entity
        float dx = mx - pos.x;
        float dy = my - pos.y;
        float dist = std::sqrt(dx*dx + dy*dy);

        // If the mouse is overlapped on the player entity
        if (dist < 1e-4f) {
            // Reset velocity to (0, 0) 
            // The player fish will not move in this frame
            vel.dx = 0.0f;
            vel.dy = 0.0f;
            continue; // return; is also fine, but continue; can prevent the potential bug in multi-mode
        }

        float speed = 0.0f;
        // If the mouse is far away
        int half_height = blackboard.get_or<int>("window_height", 600) / 2;
        if (dist > half_height) {
            speed = speed_ * 1.5f;
        } else if (dist > 80 && dist <= half_height) {
            speed = speed_ * 0.75f;
        } else if (dist <= 80) {
            speed = speed_ * 0.3f;
        }

        // Normalization
        vel.dx = (dx / dist) * speed;
        vel.dy = (dy / dist) * speed;
    }
}
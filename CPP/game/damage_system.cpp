#include "damage_system.hpp"

void DamageSystem::update(ComponentStorage& storage, Blackboard& blackboard) {
    // Read delta_time from blackboard
    double delta_time;
    try {
        delta_time = blackboard.get<double>("delta_time");
    } catch (const std::exception& e) {
        throw std::runtime_error("DamageSystem: delta_time not found in blackboard");
    }
    
    for (Entity player: storage.entities_with_component<Player>()) {
        if (!storage.has_component<Health>(player)) continue;

        auto health_opt = storage.get_component<Health>(player);
        if (!health_opt.has_value()) continue;

        Health& health = health_opt->get();

        // 1. Cool down each frame
        if (health.damage_cooldown > 0.0f) {
            health.damage_cooldown -= static_cast<float>(delta_time);
        }

        // 2. Death Condition
        if (health.hp <= 0) {
            blackboard.set("game.over", true);
            blackboard.set("game.health_text", std::string("Game Over"));
        }
    }
}
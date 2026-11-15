#include "predator_prey_resolution_system.hpp"

void PredatorPreyResolutionSystem::update(ComponentStorage& storage, Blackboard& blackboard) {
    // Loop each player with Playr component
    for (Entity player : storage.entities_with_component<Player>()) {

        // 1. Filter out non-collided
        if(!storage.has_component<CollidedWith>(player)) continue;

        // 2. Filter out no level
        if(!storage.has_component<FishLevel>(player)) continue;

        // 3. Get the player fish_level value
        int player_level = storage.get_component<FishLevel>(player)->get().level;

        // 4. Check and compare the fish_level that the player collides with
        auto collided_opt = storage.get_component<CollidedWith>(player);
        if(!collided_opt.has_value()) {
            continue;
        }

        const CollidedWith& collided = collided_opt->get();

        for (Entity other : collided.entities){
            if (!storage.has_component<FishLevel>(other)) continue;

            int other_level = storage.get_component<FishLevel>(other)->get().level;
            
            if (player_level >= other_level){
                storage.add_component<DestroyRequest>(other, DestroyRequest{});

                double score = blackboard.get_or<double>("game.score", 0.0);
                score += 1.0;
                blackboard.set("game.score", score);
                blackboard.set("game.score_text", "Score: " + std::to_string((int)score));
            } else {
                auto health_opt = storage.get_component<Health>(player);
                if (!health_opt.has_value()) continue;
                Health& player_health = health_opt->get();

                if (player_health.damage_cooldown <= 0.0f) {
                    player_health.hp -= 1;                                              // int
                    player_health.damage_cooldown = 1.0f;                               // 开冷却
                    blackboard.set("game.health_text",
                                "Health: " + std::to_string(player_health.hp));      // 字符串!
                }
            }
        }
    }
}
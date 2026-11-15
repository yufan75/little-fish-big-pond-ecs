#include "growth_system.hpp"
#include <iostream>

void GrowthSystem::update(ComponentStorage& storage, Blackboard& blackboard) {
    auto entities = storage.entities_with_component<Player>();

    for (Entity player : entities) {
        std::cout << "[Growth] found player" << std::endl;
        if (!storage.has_component<FishLevel>(player)) continue;
        

        auto fishLevel_opt = storage.get_component<FishLevel>(player);
        if (!fishLevel_opt.has_value()) continue;

        FishLevel& fish_level = fishLevel_opt->get();
        int player_level = fish_level.level;

        // Read score
        double score = blackboard.get_or<double>("game.score", 0.0);

        // Retrieve Threshold
        auto thresholds = blackboard.get_or<std::vector<int>>("game.level_thresholds", std::vector<int>{});
        if (thresholds.empty()) return;
        
        std::cout << "[Growth] level=" << player_level
                  << " score=" << score
                  << " thresholds.size=" << thresholds.size() << std::endl;

        // Get upgrade score
        if (player_level - 1 >= (int)thresholds.size()) {
            // Already the highest level (Level 3)
            // Win!
            blackboard.set("game.won", true);
            continue;
        }

        // player_level = 1 -> thresholds[0]
        // player_level = 2 -> thresholds[1]
        int request = thresholds[player_level-1];
        
        // Check if upgrade
        if (score >= request) {
            std::cout << "[Growth] LEVEL UP!" << std::endl;
            player_level += 1;
            fish_level.level = player_level;
            score = 0;
            blackboard.set("game.score", score);
            blackboard.set("game.score_text", std::string("Score: ") + std::to_string((int)score));
            blackboard.set("game.level_text", std::string("Level: ") + std::to_string((int)player_level));
        
            // Check if Win
            if (player_level - 1 >= (int)thresholds.size()) {   
                blackboard.set("game.won", true);
                blackboard.set("game.health_text", std::string("You Win"));
            }
        }
    }
}
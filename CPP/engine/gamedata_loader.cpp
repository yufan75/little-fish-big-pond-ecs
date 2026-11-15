#include "engine/gamedata_loader.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Helper: parse a single entity's "components" object and attach to ECS
// ---------------------------------------------------------------------------
static void parse_components(const json& components_obj,
                             Entity entity,
                             ComponentStorage& component_storage) {
    for (auto& [key, value] : components_obj.items()) {
        if (key == "position") {
            component_storage.add_component<Position>(entity, Position{
                value["x"].get<float>(),
                value["y"].get<float>()
            });
        } else if (key == "size") {
            component_storage.add_component<Size>(entity, Size{
                value["width"].get<float>(),
                value["height"].get<float>()
            });
        } else if (key == "color") {
            component_storage.add_component<Color>(entity, Color{
                value["r"].get<uint8_t>(),
                value["g"].get<uint8_t>(),
                value["b"].get<uint8_t>(),
                value["a"].get<uint8_t>()
            });
        } else if (key == "velocity") {
            component_storage.add_component<Velocity>(entity, Velocity{
                value["dx"].get<float>(),
                value["dy"].get<float>()
            });
        } else if (key == "input") {
            component_storage.add_component<Input>(entity, Input{});
        } else if (key == "images") {
            component_storage.add_component<Images>(entity, Images{
                value["names"].get<std::vector<std::string>>(),
                value["active_index"].get<size_t>()
            });
        } else if (key == "text") {
            auto& color_obj = value["color"];
            SDL_Color sdl_color{
                color_obj["r"].get<uint8_t>(),
                color_obj["g"].get<uint8_t>(),
                color_obj["b"].get<uint8_t>(),
                color_obj["a"].get<uint8_t>()
            };
            Text txt{
                value["content"].get<std::string>(),
                value["font_name"].get<std::string>(),
                value["font_size"].get<float>(),
                sdl_color
            };
            if (value.contains("blackboard_key")) {
                txt.blackboard_key = value["blackboard_key"].get<std::string>();
            }
            component_storage.add_component<Text>(entity, txt);
        } else if (key == "screen_position") {
            component_storage.add_component<ScreenPosition>(entity, ScreenPosition{
                value["x"].get<float>(),
                value["y"].get<float>()
            });
        } else if (key == "script") {
            component_storage.add_component<Script>(entity, Script{
                value["filename"].get<std::string>(),
                false
            });
        } else if (key == "collider") {
            component_storage.add_component<Collider>(entity, Collider{
                value["width"].get<float>(),
                value["height"].get<float>(),
                value.value("layer", static_cast<uint8_t>(0)),
                value.value("mask", static_cast<uint8_t>(0))
            });
        } else if (key == "player") {
            component_storage.add_component<Player>(entity, Player{});
        } else if (key == "fish_level") {
            component_storage.add_component<FishLevel>(entity, FishLevel{
                value["level"].get<int>()
            });
        } else if (key == "health") {
            component_storage.add_component<Health>(entity, Health{
                value["hp"].get<int>(),
                value["damage_cooldown"].get<float>()
            });
        }
        // Unrecognized keys are silently skipped
    }
}

// ---------------------------------------------------------------------------
// Helper: process an entity array ("entities" or "hud_entities")
// ---------------------------------------------------------------------------
static void process_entity_array(const json& arr,
                                 const std::string& array_name,
                                 EntityManager& entity_manager,
                                 ComponentStorage& component_storage,
                                 Blackboard& blackboard) {
    for (size_t i = 0; i < arr.size(); ++i) {
        const auto& element = arr[i];

        if (!element.contains("id")) {
            throw std::runtime_error(
                "Entity at index " + std::to_string(i) +
                " in '" + array_name + "' is missing 'id' field");
        }

        std::string id = element["id"].get<std::string>();
        Entity entity = entity_manager.create_entity();
        blackboard.set<Entity>("entity.id." + id, entity);

        if (element.contains("components")) {
            parse_components(element["components"], entity, component_storage);
        }
    }
}

// ---------------------------------------------------------------------------
// load_game_data
// ---------------------------------------------------------------------------
void load_game_data(const std::string& file_path,
                    EntityManager& entity_manager,
                    ComponentStorage& component_storage,
                    Blackboard& blackboard) {
    // 1. Read file
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + file_path);
    }

    // 2. Parse JSON
    json data;
    try {
        data = json::parse(file);
    } catch (const json::parse_error& e) {
        throw std::runtime_error(std::string("JSON parse error: ") + e.what());
    }

    // 3. Window configuration
    if (data.contains("window")) {
        const auto& win = data["window"];
        blackboard.set<int>("window_width", win["width"].get<int>());
        blackboard.set<int>("window_height", win["height"].get<int>());
    }

    // 4. Camera configuration (defaults if absent)
    if (data.contains("camera")) {
        const auto& cam = data["camera"];
        blackboard.set<float>("camera.lookat.x", cam.value("lookat_x", 0.0f));
        blackboard.set<float>("camera.lookat.y", cam.value("lookat_y", 0.0f));
        blackboard.set<float>("camera.zoom", cam.value("zoom", 1.0f));
    } else {
        blackboard.set<float>("camera.lookat.x", 0.0f);
        blackboard.set<float>("camera.lookat.y", 0.0f);
        blackboard.set<float>("camera.zoom", 1.0f);
    }

    // 4.5 World bounds (optional — used for debug border rendering)
    if (data.contains("world")) {
        const auto& world = data["world"];
        blackboard.set<float>("world.x", world.value("x", 0.0f));
        blackboard.set<float>("world.y", world.value("y", 0.0f));
        blackboard.set<float>("world.width", world.value("width", 0.0f));
        blackboard.set<float>("world.height", world.value("height", 0.0f));
    }

    // Game rules
    if (data.contains("game_rules")) {
        const auto& rules = data["game_rules"];
        if (rules.contains("level_thresholds")) {
            std::vector<int> thresholds = rules["level_thresholds"].get<std::vector<int>>();
            blackboard.set<std::vector<int>>("game.level_thresholds", thresholds);
        }
    }

    // 4.6 Debug configuration (optional)
    if (data.contains("debug")) {
        const auto& debug = data["debug"];
        blackboard.set<int>("debug.overlay_alpha", debug.value("overlay_alpha", 128));
    }

    // 5. Entities
    if (data.contains("entities")) {
        process_entity_array(data["entities"], "entities",
                             entity_manager, component_storage, blackboard);
    }

    // 6. HUD entities
    if (data.contains("hud_entities")) {
        process_entity_array(data["hud_entities"], "hud_entities",
                             entity_manager, component_storage, blackboard);
    }

    // 7-11: Game-specific entity loading removed (Asteroids code stripped for Class-070)
    // Game-specific loaders (ship, bullet, asteroids, spawn, lives) are added
    // by each game's main.cpp or a game-specific loader, not the engine.
}

// ---------------------------------------------------------------------------
// serialize_game_data
// ---------------------------------------------------------------------------
std::string serialize_game_data(
    const ComponentStorage& component_storage,
    const Blackboard& blackboard,
    const std::vector<std::pair<std::string, Entity>>& entity_ids) {

    json root;

    // Window
    root["window"] = {
        {"width",  blackboard.get<int>("window_width")},
        {"height", blackboard.get<int>("window_height")}
    };

    // Camera
    root["camera"] = {
        {"lookat_x", blackboard.get<float>("camera.lookat.x")},
        {"lookat_y", blackboard.get<float>("camera.lookat.y")},
        {"zoom",     blackboard.get<float>("camera.zoom")}
    };

    json entities_arr = json::array();
    json hud_arr      = json::array();

    for (const auto& [id, entity] : entity_ids) {
        json entry;
        entry["id"] = id;
        json comps = json::object();

        // Determine if this is a HUD entity (has both Text and ScreenPosition)
        bool is_hud = component_storage.has_component<Text>(entity) &&
                      component_storage.has_component<ScreenPosition>(entity);

        // Position
        if (component_storage.has_component<Position>(entity)) {
            auto pos = component_storage.get_component<Position>(entity);
            comps["position"] = {{"x", pos->get().x}, {"y", pos->get().y}};
        }

        // Size
        if (component_storage.has_component<Size>(entity)) {
            auto sz = component_storage.get_component<Size>(entity);
            comps["size"] = {{"width", sz->get().width}, {"height", sz->get().height}};
        }

        // Color
        if (component_storage.has_component<Color>(entity)) {
            auto c = component_storage.get_component<Color>(entity);
            comps["color"] = {
                {"r", c->get().r}, {"g", c->get().g},
                {"b", c->get().b}, {"a", c->get().a}
            };
        }

        // Velocity
        if (component_storage.has_component<Velocity>(entity)) {
            auto v = component_storage.get_component<Velocity>(entity);
            comps["velocity"] = {{"dx", v->get().dx}, {"dy", v->get().dy}};
        }

        // Input
        if (component_storage.has_component<Input>(entity)) {
            comps["input"] = json::object();
        }

        // Images
        if (component_storage.has_component<Images>(entity)) {
            auto img = component_storage.get_component<Images>(entity);
            comps["images"] = {
                {"names",        img->get().filenames},
                {"active_index", img->get().active_index}
            };
        }

        // Text
        if (component_storage.has_component<Text>(entity)) {
            auto t = component_storage.get_component<Text>(entity);
            comps["text"] = {
                {"content",   t->get().content},
                {"font_name", t->get().font_name},
                {"font_size", t->get().font_size},
                {"color", {
                    {"r", t->get().color.r}, {"g", t->get().color.g},
                    {"b", t->get().color.b}, {"a", t->get().color.a}
                }}
            };
            if (!t->get().blackboard_key.empty()) {
                comps["text"]["blackboard_key"] = t->get().blackboard_key;
            }
        }

        // ScreenPosition
        if (component_storage.has_component<ScreenPosition>(entity)) {
            auto sp = component_storage.get_component<ScreenPosition>(entity);
            comps["screen_position"] = {{"x", sp->get().x}, {"y", sp->get().y}};
        }

        entry["components"] = comps;

        if (is_hud) {
            hud_arr.push_back(entry);
        } else {
            entities_arr.push_back(entry);
        }
    }

    root["entities"]     = entities_arr;
    root["hud_entities"] = hud_arr;

    return root.dump(2);
}

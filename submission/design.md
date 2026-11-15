# Design Document — Class-110 Final Project

**Student**: Yufan Wang
**Date:** July 6, 2026
**Due:** Monday, July 6, 2026 (Deliverable 1)

---

## 1. Game description

*Little Fish, Big Pond* is a top-down 2D arcade survival and growth game set in a pond. The player controls a small fish from an overhead view, using mouse-directed steering to swim through the existing pond scene. The project is based on the Class-070 Lua pond assignment: it keeps the pond background, fish/frog/dragonfly/lily pad visual set, Lua scripts, blackboard-driven entity state, and C++/SDL3 ECS structure, but changes the interaction from the original mouse-repulsion duck behavior into direct player steering and predator-prey gameplay.

The player starts as a Level 1 fish. Other fish, dragonflies, frogs, and lily pads populate the pond. Fish exist in three levels: Level 3 fish are stronger than Level 2 and Level 1 fish, and Level 2 fish are stronger than Level 1 fish. The player grows by eating valid fish prey for score while avoiding stronger fish and later frogs. Dragonflies are collectible shield power-ups, but they do not give score; score only comes from eating valid fish.

When the game starts, the player sees a pond with water, fish, dragonflies, and lily pads. The player fish has a marker rendered above it so it remains readable even when the fish sprite is partly hidden by lily pads. NPC fish may use the same `fish.png` sprite at different scales, with small level markers or tuning values used to distinguish threat levels. The intended audience is casual arcade players who can quickly understand the central rule: eat safe fish, avoid dangerous pond creatures, collect shields, and grow before health runs out.

The game contains two stages. In Stage 1, the player starts as a Level 1 fish, eats Level 1 fish for score, collects dragonflies for shield, and avoids Level 2 and Level 3 fish. There are no frogs in Stage 1. When the Stage 1 score threshold is reached, the player evolves to Level 2 and enters Stage 2. In Stage 2, the player keeps the Level 2 state, can eat Level 1 and Level 2 fish, must avoid Level 3 fish and frogs, and wins by reaching the Stage 2 score threshold and evolving to Level 3.

---



## 2. Core loop

The player steers through the pond toward the mouse cursor, looks for fish that are safe to eat, and avoids fish or frogs that can damage them. Eating valid fish increases score; reaching the stage score threshold evolves the player to the next fish level. Dragonflies can be collected for shield, giving the player one defensive mistake without adding score. The loop is: steer toward safe prey, eat fish, dodge stronger enemies, collect shield when useful, and repeat until the player evolves or loses all health.

---



## 3. Controls and player goals

The player controls the fish with the mouse. The player fish moves in the direction from its current position to the mouse cursor. The player fish uses three discrete speed tiers based on the distance to the cursor, rather than a continuous speed curve: max_speed when the distance exceeds half the window height, normal_speed between 80px and half the window height, and min_speed below 80px. This tiered design gives the player clear, readable feedback about how their input maps to fish speed. The 80px and half-height thresholds are prototype estimates and will be tuned after playtesting for feel.

The player has 5 health points by default. Dangerous collisions subtract 1 health unless the player has an active shield. If a shield is active, the shield is consumed first and health is not reduced. A short damage cooldown prevents one overlapping collision from draining multiple health points across consecutive frames.

**Stage goals:**

- Stage 1: start as a Level 1 fish, eat Level 1 fish for score, collect dragonflies for shield, avoid Level 2 and Level 3 fish, and evolve to Level 2 when the score threshold is reached.
- Stage 2: continue as a Level 2 fish, eat Level 1 and Level 2 fish for score, collect dragonflies for shield, avoid Level 3 fish and frogs, and evolve to Level 3 to win the game.

**Failure condition:** the player loses if health reaches 0 during either stage.

Dragonflies grant shield only and do not give score. Lily pads are planned as optional visual-cover objects: fish render below lily pads, while frogs, dragonflies, the player marker, and HUD render above them. If this is too difficult to tune, lily pads will remain visual-only background objects.

---



## 4. Starting class

I will use Class-070, the Lua scripting and duck pond assignment, as my starting point. This class is the best fit because the repository already contains pond-themed assets (`pond_bg.png`, `fish.png`, `frog.png`, `dragonfly.png`, and `lilypad.png`), Lua scripts (`wander.lua`, `patrol.lua`, `cursor.lua`, `circle.lua`, `hop.lua`, and `score.lua`), and engine support for Lua through `lua_manager`, `lua_bindings`, and `script_system`. My final game keeps the top-down pond presentation and Lua-driven NPC idea, but replaces the mouse-repulsion duck interaction with a player-controlled fish and predator-prey collision rules. I also plan to adapt existing collision, movement, HUD, input, and render systems from the Class-070 codebase rather than writing a new engine from scratch.

---



## 5. Components

Most of the new components below are lightweight data/tag components (typically one to three fields, e.g. a level int, a health int, or a boolean flag) rather than new subsystems; the underlying behavior logic (movement, collision detection, scripting, rendering) continues to run through the reused Class-070 ECS framework and systems.


| Component                         | Purpose                                                                                                                                                                                                              | Source    | Status   |
| --------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------- | -------- |
| TransformComponent                | Stores entity position, rotation, and scale in the pond scene.                                                                                                                                                       | Class-070 | reused   |
| VelocityComponent                 | Stores movement velocity for player fish, NPC fish, frogs, and dragonflies.                                                                                                                                          | Class-070 | reused   |
| ScriptComponent                   | Attaches Lua scripts to NPC-controlled entities such as fish, frogs, and dragonflies.                                                                                                                                | Class-070 | reused   |
| Blackboard state                  | Stores script-controlled per-entity state such as patrol direction, flee timer, target point, or temporary behavior flags.                                                                                           | Class-070 | reused   |
| SpriteComponent / RenderComponent | Adds a layer/z-order field so RenderSystem can sort and draw entities in a fixed order (water background → fish → lily pads → frogs/dragonflies → player marker → HUD), instead of relying on entity creation order. | Class-070 | modified |
| CollisionComponent                | Circle collider used to detect overlap between fish, frogs, dragonflies, and optional lily pad obstacles; extended with a collision layer/mask (Class-050) so systems can filter which entity pairs they care about. | Class-070 | modified |
| PlayerComponent                   | Marks the player-controlled fish so input, camera/readability marker, and player-specific collision rules can find it.                                                                                               | —         | new      |
| FishLevelComponent                | Stores fish level 1, 2, or 3; used to decide which fish can be eaten or which fish are dangerous.                                                                                                                    | —         | new      |
| HealthComponent                   | Stores the player's health, defaulting to 5 points.                                                                                                                                                                  | —         | new      |
| ShieldComponent                   | Stores whether the player currently has a dragonfly shield charge.                                                                                                                                                   | —         | new      |
| DamageCooldownComponent           | Prevents repeated health loss from the same overlapping collision across multiple frames.                                                                                                                            | —         | new      |
| ScoreComponent                    | Stores the current stage score earned only from eating valid fish prey.                                                                                                                                              | —         | new      |
| StageComponent                    | Stores the current stage number, stage score threshold, and whether the player should advance or win.                                                                                                                | —         | new      |
| CollectibleComponent              | Marks dragonflies as collectible shield power-ups.                                                                                                                                                                   | —         | new      |
| FrogComponent / EnemyComponent    | Marks frogs as always-dangerous entities in Stage 2.                                                                                                                                                                 | —         | new      |
| LilyPadComponent                  | Marks lily pads as visual-cover objects and optional static obstacles if time permits.                                                                                                                               | —         | new      |
| PlayerMarkerComponent             | Marks a small visual indicator rendered above lily pads so the player can always locate their fish.                                                                                                                  | —         | new      |


---



## 6. Systems


| System                       | Purpose                                                                                                                                                                                                                                                                                                                                                                                                                                                       | Source    | Status   |
| ---------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------- | -------- |
| InputSystem                  | Reads mouse position for the player fish instead of using the original mouse-repulsion duck behavior.                                                                                                                                                                                                                                                                                                                                                         | Class-070 | modified |
| PlayerControlSystem          | Computes player velocity toward the mouse cursor using the three-tier speed logic (max_speed / normal_speed / min_speed) described in Section 3.                                                                                                                                                                                                                                                                                                              | Class-070 | modified |
| ScriptSystem                 | Runs Lua scripts that control NPC fish, frogs, and dragonflies. NPC Lua scripts only control movement (wander, patrol, chase/flee, drift) and never make predator-prey decisions.                                                                                                                                                                                                                                                                             | Class-070 | reused   |
| MovementSystem               | Updates entity positions from velocity and keeps moving entities inside the pond bounds.                                                                                                                                                                                                                                                                                                                                                                      | Class-070 | modified |
| RenderSystem                 | Draws the pond scene with explicit layering: water background, fish, lily pads, frogs/dragonflies, player marker, and HUD.                                                                                                                                                                                                                                                                                                                                    | Class-070 | modified |
| CollisionSystem              | Detects circular overlaps between player, fish, frogs, dragonflies, and optional lily pad colliders, and tags each collision pair with the layers of the two entities involved so downstream systems can filter relevant pairs.                                                                                                                                                                                                                               | Class-070 | modified |
| WrapSystem                   | Keeps scripted pond entities inside the playable area by wrapping them around pond edges.                                                                                                                                                                                                                                                                                                                                                                     | Class-070 | reused   |
| PredatorPreyResolutionSystem | A C++ system that reads FishLevelComponent and PlayerComponent directly from both colliding entities to decide predation outcome; only processes Player-vs-Fish or Player-vs-Frog pairs (filtered via collision layer/mask) and ignores Fish-vs-Fish or Fish-vs-LilyPad overlaps. In Stage 1, the Level 1 player can eat Level 1 fish and is damaged by Level 2/3 fish. In Stage 2, the Level 2 player can eat Level 1/2 fish and is damaged by Level 3 fish. | —         | new      |
| ShieldSystem                 | Grants a shield when the player collides with a dragonfly and consumes that shield before health loss on a dangerous collision.                                                                                                                                                                                                                                                                                                                               | —         | new      |
| DamageSystem                 | Subtracts health on dangerous collisions, respects shield, and applies damage cooldown.                                                                                                                                                                                                                                                                                                                                                                       | —         | new      |
| GrowthSystem                 | Checks whether the current stage score threshold has been reached and evolves the player from Level 1 to Level 2 or from Level 2 to Level 3.                                                                                                                                                                                                                                                                                                                  | —         | new      |
| StageSystem                  | Advances from Stage 1 to Stage 2 after Level 2 evolution, and ends the game after Level 3 evolution in Stage 2.                                                                                                                                                                                                                                                                                                                                               | —         | new      |
| HUDSystem / DebugHUDSystem   | Displays current stage, player level, score, score threshold, health, shield state, and win/loss state.                                                                                                                                                                                                                                                                                                                                                       | Class-070 | modified |
| SpawnSystem                  | Creates or respawns fish and dragonflies using stage data from GameData.json.                                                                                                                                                                                                                                                                                                                                                                                 | —         | new      |


---



## 7. Data

`GameData.json` will store the tunable values for both stages so that most gameplay balancing does not require recompiling. It will include:

- Stage count and current stage configuration.
- Score thresholds for Stage 1 and Stage 2 (e.g. `"stage1_score_threshold": 10`, int).
- Player starting level, starting health, speed tiers (`"max_speed"`, `"normal_speed"`, `"min_speed"`, floats), the two distance thresholds that separate the tiers (`"speed_tier_far_px"`, `"speed_tier_near_px"`, ints), and damage cooldown time (`"damage_cooldown_sec"`, float).
- Fish definitions: level, size/scale, speed, score value, collider radius, sprite image, and Lua script name.
- Stage population settings: number of Level 1 fish, Level 2 fish, Level 3 fish, dragonflies, frogs, and lily pads (ints).
- Dragonfly shield settings: shield charge count, respawn behavior, sprite, Lua script, and collider size.
- Frog settings: speed, collider size, sprite, Lua script, and spawn count for Stage 2.
- Lily pad settings: positions, render layer, sprite, and optional static collider radius if the obstacle version is implemented.
- HUD text positions and display options if the existing HUD system supports data-driven layout.

The values I expect to tune most frequently are score thresholds, fish counts, fish speeds, collider radii, the player's speed-tier thresholds and values, damage cooldown, health, and the number of dragonflies available in each stage.

---



## 8. Risks

1. **Risk:** The three-tier mouse steering (max/normal/min speed) may feel too abrupt at the tier boundaries, or the 80px and half-window-height thresholds may not match the intended feel.
  **Plan:** Prototype the player movement first using the tiered speed values, test how it feels crossing each threshold, and tune the threshold distances and tier speeds in `GameData.json` before adding the full stage rules.
2. **Risk:** Dangerous collisions could subtract all health immediately if the player overlaps an enemy for several frames.
  **Plan:** Implement `HealthComponent` and `DamageCooldownComponent` early, then test collision cases where the player remains inside a stronger fish or frog for multiple frames.
3. **Risk:** Lua NPC behavior may become difficult to debug if every fish has complex decision logic.
  **Plan:** Keep scripts simple: Level 1 fish wander, Level 2/3 fish patrol or chase/flee using simple distance checks, frogs use slow patrol behavior, and dragonflies drift. Predator-prey decisions themselves are never made in Lua — they are resolved in C++ by `PredatorPreyResolutionSystem` — so Lua only needs to stay simple enough to move entities believably.
4. **Risk:** Lily pad visual cover could make the game feel unfair if dangerous fish are hidden too often.
  **Plan:** Implement lily pads first as render-layer visual cover only, with the player marker, frogs, and dragonflies always rendered above them. If this hurts readability, lily pads will be downgraded to visual-only scenery with less overlap.


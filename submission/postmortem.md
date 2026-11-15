# Postmortem — Little Fish, Big Pond

**Student:** Yufan Wang
**Course:** CS 5850, Class-110 Final Project

*Little Fish, Big Pond* is a top-down, mouse-controlled "big fish eats little
fish" survival game built on my Class-070 (Duck Pond) engine. The player steers
a fish toward the cursor, eats fish of an equal-or-lower level to gain score,
grows to the next level when the score threshold is reached, takes damage from
larger fish, and either wins by reaching the maximum level or loses when health
reaches zero.

---

## 1. What worked

Three decisions I would make again:

**Building in vertical slices instead of by a component checklist.** I got the
player moving first, then added eating, scoring, damage, death, growth, and the
win state one layer at a time, keeping the game runnable after every step. This
meant I always had a demonstrable version, and it made bugs far easier to
localize because each change was small and isolated.

**Separating concerns into distinct systems.** Collision resolution
(`PredatorPreyResolutionSystem`) is event-driven — it only does work when a
collision occurs. The damage cooldown and death check (`DamageSystem`) must run
every frame, even when nothing collides. Because their responsibilities differ,
I split them into two systems rather than cramming everything into the collision
handler. This kept each system simple and prevented them from fighting over the
same state.

**Making the growth system data-driven.** Level-up thresholds live in
`GameData.json` under `game_rules.level_thresholds` (a vector of ints) rather
than hardcoded. `GrowthSystem` looks up the threshold for the current level;
running past the end of the table means the player has reached the top and wins.
The payoff is that adding more levels or stages later only requires appending to
the array — no logic changes. It cost almost the same effort as hardcoding but
left the door open for extension.

I would also keep the habit of **verifying against the source instead of
guessing.** Several times I checked the actual engine code (InputSystem's
coordinate conversion, cursor.lua, the blackboard's storage type) before
building on an assumption, which repeatedly saved me from rework.

---

## 2. What didn't work

Things I would do differently with another week:

**The loader is too fragile — a single mistyped key crashed the whole game.**
On startup the game aborted with a JSON `type_error.302`. I ran it under lldb,
set a breakpoint on the thrown exception, read the backtrace, and traced it into
the disassembly, where it was crashing while reading `font_size`. The real cause
was a mistyped HUD key — `font_names` instead of `font_name` — which sent the
loader down a wrong branch. A one-character typo took the entire game down with
SIGABRT. With more time I would add key validation and clearer error messages to
the loader.

**The blackboard's strong typing is a hidden landmine.** The same key
`game.health_text` was stored as both a `std::string` and a `const char*` in two
different places, and the game ran fine until the exact moment the player died,
then crashed with a type mismatch. Type mismatches on read also silently return
the default value (reading an `int` as a `float` quietly hands back the default).
These bugs don't raise errors — they just behave wrong — which makes them the
hardest to find.

**A whole family of "syntactically valid but logically inverted" bugs that the
compiler cannot catch.** I wrote `key = "..."` (assignment) where I meant `==`
(comparison); I omitted a `!` in a `has_component` guard, which inverted the
filter so the player was skipped and level-ups never fired; and I wrote the
freeze condition as `||` where it needed `&&` ("run gameplay only if not lost
AND not won"). All compiled cleanly. I could only locate them by adding prints
and distinguishing "the function was never called" from "it was called but the
loop ran zero times." With another week I would introduce logging and assertions
earlier instead of adding prints reactively.

**An off-by-one in the HUD level display.** It showed a level one lower than the
real one because I used the pre-upgrade `level` variable to update the HUD. The
root cause was mixing the old and new value. I fixed it by doing
`level += 1; fish_level.level = level;` so the two are always equal — filling in
the trap rather than carefully remembering which value to use each time.

---

## 3. What I cut

**Shield, frog enemy, layer/mask filtering, and lily-pad layering — cut as one
dependency unit.** I originally designed these as independent features, but while
implementing I realized they all depend on one unfinished foundation: automatic
NPC spawning and movement behavior (a `SpawnSystem`). The dragonflies were meant
to be collectible shields, the frog is a new spawning enemy, layer/mask matters
only once many entities exist, and layering only matters once those entities are
on screen. Because that shared foundation is a large piece of work and time was
limited, I chose to defer the entire group as a single unit rather than ship four
half-finished features. Recognizing them as one dependency tree — rather than
four separate to-dos — made the cut a single clear decision.

**Shield's interception point (PendingDamage) / a standalone
DamageCooldownComponent.** My design called for damage to be posted as a
`PendingDamage` marker that `DamageSystem` would read, with cooldown in its own
component. To save time, `PredatorPreyResolutionSystem` deducts health directly
and the cooldown lives inside `HealthComponent`. Functionally equivalent, but it
diverges from the layered design — and it's also why the shield can't be slotted
in: there is no "before health is deducted" interception point.

**Full stage transitions.** I implemented growth and a win-on-max-level, but not
switching fish sets or scenes between stages. Because the growth system is
data-driven, the hook for adding stages later is already in place.

**SpawnSystem (fish respawning).** Fish do not respawn, so eating depletes them.
As a result the level thresholds had to be set to low values matching the fixed
number of fish on screen; with respawning I could raise the thresholds for a
longer game.

Player-fish growing in size on level-up, and lily-pad layering (lily-pads above the player but below other fish and the frog), were not completed before submission. They are visual polish that were deprioritized in favor of the required deliverables, and the layering in particular is tied to the same NPC-spawning foundation described above.

The health values were brought back in line with the design document
(5 HP, 1 damage per hit), so there is no divergence to explain there.

---

## 4. Engine debt

What I would change about the engine now that I've built a real game on it:

**Adding one component touches at least three files and 6+ sites.** Definition in
`components.hpp`, a member in `component_storage.hpp`, the `get_storage`
specialization plus six explicit-instantiation lines in `component_storage.cpp`,
and a parse branch in `gamedata_loader.cpp`. Miss any one and the symptom
differs — compile error, link error, or silent no-op. I added three new
components this project and walked this gauntlet each time. I would replace it
with a single registration mechanism so adding a component needs one declaration.

**Blackboard keys are magic strings** — scattered across files, with no central
definition, and typos don't error. Worse, keys serve two different roles (HUD
display text that JSON binds via `blackboard_key`, versus internal state flags
read/written only in code) with nothing in the type system to distinguish them. I
would define keys as named constants so the compiler catches typos.

**Read and write paths must be kept in sync by hand.** `parse_components` (JSON →
components) and `serialize_game_data` (components → JSON) are separate; adding a
component and only updating one leaves dumps incomplete.

**The engine/game boundary blurs when a system is modified.**
`PlayerControlSystem` was changed to read a game-specific component and implement
this game's feel, yet it still lives under `engine/`. It really belongs in
`game/`, or engine systems should be forbidden from depending on game components.

**There is no game state machine.** `game.over` and `game.won` are scattered
booleans that, once set true, never reset (the game must be restarted to replay),
and the win path even reuses `game.over` to trigger the freeze — coupling "won"
and "lost." I would manage this with an explicit state machine
(Playing / Won / Lost).


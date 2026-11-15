#ifndef ENTITY_MANAGER_HPP
#define ENTITY_MANAGER_HPP

#include "components.hpp"
#include <vector>
#include <unordered_set>

class EntityManager {
    public:
        /**
         * Creates a new entity and returns its unique identifier.
         * 
         * The implementation uses an ID pool strategy:
         * - If there are recycled IDs available (from destroyed entities), reuse one
         * - Otherwise, generate a new ID by incrementing the counter
         * 
         * This ensures IDs are unqiue among active entities while allowing efficient reuse.
         * 
         * @return A unique Entity ID that is not currently in use
         */
        Entity create_entity();

        /**
         * Marks an entity as destroyed, allowing its ID to be reused.
         * 
         * After calling this method:
         * - is_alive(entity) will return false
         * - The ID may be returned by a future create_entity() call
         * - Components attached to this entity are NOT automatically removed
         *   (that's the responsibility of ComponentStorage or the caller)
         * 
         * If the entity ID is not currently active, this is a no-op (safe to call).
         * 
         * @param entity The entity ID to destroy
         */
        void destroy_entity(Entity entity);

        /**
         * Checks if an entity ID is currently active.
         * 
         * An entity is "alive" if it has been created and not yet destroyed.
         * This should be checked before using an entity ID to add/get components.
         * 
         * @param entity The entity ID to check
         * @return true if the entity is currently active, false otherwise
         */
        bool is_alive(Entity entity) const;

        /**
         * Returns the numver of currently active entities
         * 
         * This count should always equal: (total creates) - (total destroys)
         * This invariant is verified by property-based tests.
         * 
         * @return The count of active entities
         */
        size_t active_count() const;

    private:
        /**
         * Pool of entity IDs that have been destroyed and are avaliable for reuse.
         * When create_entity() is called, it first checks this pool before generating a new ID.
         * This prevents ID exhaustion in long-running applications.
         */
        std::vector<Entity> available_ids_;

        /**
         * Set of entity IDs that are currently active (alive)
         * Used for fase O(1) lookup in is_alive() and to track active count
         */
        std::unordered_set<Entity> active_entities_;

        /**
         * Counter for generating new entity IDs
         * Incremented each time a new ID is needed (when available_ids_ is empty)
         * Starts at 0, so the first entity created will have ID 0.
         */
        Entity next_id_ = 0;
};

#endif // ENTITY_MANAGER_HPP
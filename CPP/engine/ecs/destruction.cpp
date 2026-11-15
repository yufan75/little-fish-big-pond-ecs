#include "destruction.hpp"
#include "entity_manager.hpp"
#include "component_storage.hpp"

void destroy_marked_entities(EntityManager& em, ComponentStorage& storage) {
    // Get a snapshot (vector copy) of all entities marked for destruction.
    // Iterating a copy is safe even though we modify the maps during the loop.
    auto marked = storage.entities_with_component<DestroyRequest>();

    for (Entity entity : marked) {
        // Remove ALL 17 component types — each call is a no-op if the
        // entity doesn't have that particular component.
        storage.remove_component<Position>(entity);
        storage.remove_component<Size>(entity);
        storage.remove_component<Color>(entity);
        storage.remove_component<Input>(entity);
        storage.remove_component<Velocity>(entity);
        storage.remove_component<Images>(entity);
        storage.remove_component<Text>(entity);
        storage.remove_component<ScreenPosition>(entity);
        storage.remove_component<Rotation>(entity);
        storage.remove_component<Collider>(entity);
        storage.remove_component<CircleCollider>(entity);
        storage.remove_component<OBBCollider>(entity);
        storage.remove_component<Lifetime>(entity);
        storage.remove_component<WrapAround>(entity);
        storage.remove_component<CollidedWith>(entity);
        storage.remove_component<Script>(entity);
        storage.remove_component<DestroyRequest>(entity);

        em.destroy_entity(entity);
    }
}
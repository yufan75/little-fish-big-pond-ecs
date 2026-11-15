#ifndef COMPONENT_STORAGE_HPP
#define COMPONENT_STORAGE_HPP

#include "components.hpp"
#include <unordered_map>
#include <optional>
#include <functional>
#include <vector>

/**
 * ComponentStorage class
 * 
 * Manages the storage and retrieval of components in the ECS architecture.
 * This class demonstrates a key ECS principle: components (data) are stored separately from entities (IDs) and systems (logic).
 * 
 * The ComponentStorage associates component data with entity IDs using a "table per component type" approach.
 * Each component type (Position, Size, Color, etc.) has its own std::unordered_map that maps Entity IDs to component instances.
 * 
 * Key Design Decisions:
 * - Template methods allow type-safe component operations
 * - std::optional<std::reference_wrapper<T>> for get_component() avoids copying components and and clearly indicates when a component doesn't exist
 * - Each entity can have at most one component of each type
 * - Adding a component when one already exists replaces the old value
 * 
 * Component Lifecycle:
 * - add_component<T>() associates a component with an entity
 * - get_component<T>() retrieves a reference to the component if it exists, or std::nullopt if it doesn't
 * - remove_component<T>() removes the association
 * - has_component<T>() checks if the association exists
 * - entities_with_component<T>() returns a list of all entities that have a component of type T
 */
class ComponentStorage {
    public:
        /**
         * Adds a component to an entity
         * 
         * If the entity already has a component of this type, the old value is replaced.
         * This is the expected behavior for component updates - you don't need to remove the old component first.
         * 
         * The component is copied into storage, so the original can be safely discarded.
         * 
         * @tparam T The type of the component to add
         * @param entity The entity ID to associate the component with
         * @param component The component instance to add
         */
        template <typename T>
        void add_component(Entity entity, const T& component);

        /**
         * Retrieves a component for an entity (mutable version)
         * 
         * Returns a reference wrapper to the component if it exists, allowing modification of the component data.
         * Returns std::nullopt if the entity doesn't have this component type.
         * 
         * Using std::optional<std::reference_wrapper<T>> avoids copying the component and clearly indicates the "not found" case without exceptions or null pointers.
         * 
         * Example usage:
         * auto pos = storage.get_component<Position>(entity);
         * if (pos.has_value()) {
         *     pos->get().x += 10; // Modify the component directly
         * }
         * 
         * @tparam T The type of the component to retrieve
         * @param entity The entity ID to retrieve the component for
         * @return std::optional containing a reference to the component or std::nullopt if it doesn't exist
         */
        template <typename T>
        std::optional<std::reference_wrapper<T>> get_component(Entity entity);

        /**
         * Retrieves a component for an entity (const version).
         * 
         * Same as the mutable version, but returns a const reference for read-only access.
         * Used when you need to read component data without modifying it.
         * 
         * @tparam T The type of the component to retrieve
         * @param entity The entity ID to retrieve the component for
         * @return std::optional containing a const reference to the component or std::nullopt
         */
        template <typename T>
        std::optional<std::reference_wrapper<const T>> get_component(Entity entity) const;

        /**
         * Removes a component from an entity
         * 
         * After calling this method, has_component<T>(entity) will return false,
         * and get_component<T>(entity) will return std::nullopt.
         * 
         * If the entity doesn't have this component type, this is a no-op (safe to call).
         * This simplifies cleanup code - You don't need to check has_component() first.
         * 
         * @tparam T The type of the component to remove
         * @param entity The entity ID to remove the component from
         */
        template <typename T>
        void remove_component(Entity entity);

        /**
         * Checks if an entity has a specific component type.
         * 
         * This is a convenience method that's equivalent to checking if get_component<T>(entity).has_value(), but more readable.
         * 
         * @tparam T The type of the component to check for
         * @param entity The entity ID to check
         * @return true if the entity has the component, false otherwise
         */
        template <typename T>
        bool has_component(Entity entity) const;

        /**
         * Returns all entities that have a specific component type.
         * 
         * This is used by systems to iterate over entities with required components.
         * For example, the RenderSystem uses this to find all entities with Position, Size, and Color components.
         * 
         * The returned vector contains entity IDs in an unspecified but consistent order.
         * 
         * @tparam T The type of the component to filter by
         * @return A vector of entity IDs that have the specified component type
         */
        template <typename T>
        std::vector<Entity> entities_with_component() const;

    private:
        /**
         * Storage for Position components.
         * Maps entity IDs to Position data. Only entities that have a Position
         * component will have an entry in this map.
         */
        std::unordered_map<Entity, Position> positions_;

        /**
         * Storage for Size components.
         * Maps entity IDs to Size data. Only entities that have a Size
         * component will have an entry in this map.
         */
        std::unordered_map<Entity, Size> sizes_;

        /**
         * Storage for Color components.
         * Maps entity IDs to Color data. Only entities that have a Color
         * component will have an entry in this map.
         */
        std::unordered_map<Entity, Color> colors_;

        /**
         * Storage for Velocity components.
         * Maps entity IDs to Velocity data. Only entities that have a Velocity
         * component will have an entry in this map.
         */
        std::unordered_map<Entity, Velocity> velocities_;

        /**
         * Storage for Input components.
         * Maps entity IDs to Input data. Only entities that have an Input
         * component will have an entry in this map.
         */
        std::unordered_map<Entity, Input> inputs_;

        /**
         * Storage for Images components.
         * Maps entity IDs to Images data. Only entities that have an Images
         * component will have an entry in this map.
         */
        std::unordered_map<Entity, Images> images_;

        /**
         * Storage for Text components.
         * Maps entity IDs to Text data. Only entities that have a Text
         * component will have an entry in this map.
         */
        std::unordered_map<Entity, Text> texts_;
        
        /**
         * Storage for ScreenPosition components.
         * Maps entity IDs to ScreenPosition data. Only entities that have a ScreenPosition
         * component will have an entry in this map.
         */
        std::unordered_map<Entity, ScreenPosition> screen_positions_;
        
        /**
         * Storage for Rotation components.
         * Maps entity IDs to Rotation data. Only entities that have a Rotation
         * component will have an entry in this map.
         */
        std::unordered_map<Entity, Rotation> rotations_;
        
        /**
         * Storage for Collider components.
         * Maps entity IDs to Collider data. Only entities that have a Collider
         * component will have an entry in this map.
         */
        std::unordered_map<Entity, Collider> colliders_;

        /**
         * Storage for CircleCollider components.
         * Maps entity IDs to CircleCollider data. Only entities that have a CircleCollider
         * component will have an entry in this map.
         */
        std::unordered_map<Entity, CircleCollider> circle_colliders_;

        /**
         * Storage for OBBCollider components.
         * Maps entity IDs to OBBCollider data. Only entities that have an OBBCollider
         * component will have an entry in this map.
         */
        std::unordered_map<Entity, OBBCollider> obb_colliders_;

        /**
         * Storage for Lifetime components.
         * Maps entity IDs to Lifetime data. Only entities that have a Lifetime
         * component will have an entry in this map.
         */
        std::unordered_map<Entity, Lifetime> lifetimes_;
        
        /**
         * Storage for WrapAround components.
         * Maps entity IDs to WrapAround data. Only entities that have a WrapAround
         * component will have an entry in this map.
         */
        std::unordered_map<Entity, WrapAround> wrap_arounds_;
        
        /**
         * Storage for DestroyRequest components.
         * Maps entity IDs to DestroyRequest data. Only entities that have a DestroyRequest
         * component will have an entry in this map.
         */
        std::unordered_map<Entity, DestroyRequest> destroy_requests_;
        
        /**
         * Storage for CollidedWith components.
         * Maps entity IDs to CollidedWith data. Only entities that have a CollidedWith
         * component will have an entry in this map.
         */
        std::unordered_map<Entity, CollidedWith> collided_withs_;
        
        /**
         * Storage for Script components.
         * Maps entity IDs to Script data. Only entities that have a Script
         * component will have an entry in this map.
         */
        std::unordered_map<Entity, Script> scripts_;

        /**
         * Storage for Player components
         * Maps entity IDs to Player data. Only entities that have a Player
         * component will have an entry in this map.
         */
        std::unordered_map<Entity, Player> player_components_;

        /**
         * Storage for FishLevel components
         * Maps entity IDs to FishLevel data. Only entities that have a FishLevel
         * component will have an entry in this map.
         */
        std::unordered_map<Entity, FishLevel> fish_levels_;

        /**
         * Storage for Health components
         * Maps entity IDs to Health data. Only entities that have a Health
         * component will have an entry in this map.
         */
        std::unordered_map<Entity, Health> healths_;

        /**
         * Helper method to get the storage map for a specific component type.
         * This allows the template methods to work uniformly across all component types.
         * 
         * @tparam T The type of the component
         * @return A reference to the unordered_map that stores components of type T
         */
        template<typename T>
        std::unordered_map<Entity, T>& get_storage();

        /**
         * Const version of get_storage() for read-only access.
         * 
         * @tparam T The type of the component
         * @return Const reference to the storage map for that type
         */
        template<typename T>
        const std::unordered_map<Entity, T>& get_storage() const;
};

// Decalre explicit specializations of get_storage
template<> std::unordered_map<Entity, Position>& ComponentStorage::get_storage<Position>();
template<> std::unordered_map<Entity, Size>& ComponentStorage::get_storage<Size>();
template<> std::unordered_map<Entity, Color>& ComponentStorage::get_storage<Color>();
template<> std::unordered_map<Entity, Velocity>& ComponentStorage::get_storage<Velocity>();
template<> std::unordered_map<Entity, Input>& ComponentStorage::get_storage<Input>();
template<> std::unordered_map<Entity, Images>& ComponentStorage::get_storage<Images>();
template<> std::unordered_map<Entity, Text>& ComponentStorage::get_storage<Text>();
template<> std::unordered_map<Entity, ScreenPosition>& ComponentStorage::get_storage<ScreenPosition>();
template<> std::unordered_map<Entity, Rotation>& ComponentStorage::get_storage<Rotation>();
template<> std::unordered_map<Entity, Collider>& ComponentStorage::get_storage<Collider>();
template<> std::unordered_map<Entity, CircleCollider>& ComponentStorage::get_storage<CircleCollider>();
template<> std::unordered_map<Entity, OBBCollider>& ComponentStorage::get_storage<OBBCollider>();
template<> std::unordered_map<Entity, Lifetime>& ComponentStorage::get_storage<Lifetime>();
template<> std::unordered_map<Entity, WrapAround>& ComponentStorage::get_storage<WrapAround>();
template<> std::unordered_map<Entity, DestroyRequest>& ComponentStorage::get_storage<DestroyRequest>();
template<> std::unordered_map<Entity, CollidedWith>& ComponentStorage::get_storage<CollidedWith>();
template<> std::unordered_map<Entity, Script>& ComponentStorage::get_storage<Script>();
template<> std::unordered_map<Entity, Player>& ComponentStorage::get_storage<Player>();
template<> std::unordered_map<Entity, FishLevel>& ComponentStorage::get_storage<FishLevel>();
template<> std::unordered_map<Entity, Health>& ComponentStorage::get_storage<Health>();

template<> const std::unordered_map<Entity, Position>& ComponentStorage::get_storage<Position>() const;
template<> const std::unordered_map<Entity, Size>& ComponentStorage::get_storage<Size>() const;
template<> const std::unordered_map<Entity, Color>& ComponentStorage::get_storage<Color>() const;
template<> const std::unordered_map<Entity, Velocity>& ComponentStorage::get_storage<Velocity>() const;
template<> const std::unordered_map<Entity, Input>& ComponentStorage::get_storage<Input>() const;
template<> const std::unordered_map<Entity, Images>& ComponentStorage::get_storage<Images>() const;
template<> const std::unordered_map<Entity, Text>& ComponentStorage::get_storage<Text>() const;
template<> const std::unordered_map<Entity, ScreenPosition>& ComponentStorage::get_storage<ScreenPosition>() const;
template<> const std::unordered_map<Entity, Rotation>& ComponentStorage::get_storage<Rotation>() const;
template<> const std::unordered_map<Entity, Collider>& ComponentStorage::get_storage<Collider>() const;
template<> const std::unordered_map<Entity, CircleCollider>& ComponentStorage::get_storage<CircleCollider>() const;
template<> const std::unordered_map<Entity, OBBCollider>& ComponentStorage::get_storage<OBBCollider>() const;
template<> const std::unordered_map<Entity, Lifetime>& ComponentStorage::get_storage<Lifetime>() const;
template<> const std::unordered_map<Entity, WrapAround>& ComponentStorage::get_storage<WrapAround>() const;
template<> const std::unordered_map<Entity, DestroyRequest>& ComponentStorage::get_storage<DestroyRequest>() const;
template<> const std::unordered_map<Entity, CollidedWith>& ComponentStorage::get_storage<CollidedWith>() const;
template<> const std::unordered_map<Entity, Script>& ComponentStorage::get_storage<Script>() const;
template<> const std::unordered_map<Entity, Player>& ComponentStorage::get_storage<Player>() const;
template<> const std::unordered_map<Entity, FishLevel>& ComponentStorage::get_storage<FishLevel>() const;
template<> const std::unordered_map<Entity, Health>& ComponentStorage::get_storage<Health>() const;

// Prevent implicit instantiation - definitions are in component_storage.cpp
extern template void ComponentStorage::add_component<Position>(Entity, const Position&);
extern template void ComponentStorage::add_component<Size>(Entity, const Size&);
extern template void ComponentStorage::add_component<Color>(Entity, const Color&);
extern template void ComponentStorage::add_component<Velocity>(Entity, const Velocity&);
extern template void ComponentStorage::add_component<Input>(Entity, const Input&);
extern template void ComponentStorage::add_component<Images>(Entity, const Images&);
extern template void ComponentStorage::add_component<Text>(Entity, const Text&);
extern template void ComponentStorage::add_component<ScreenPosition>(Entity, const ScreenPosition&);
extern template void ComponentStorage::add_component<Rotation>(Entity, const Rotation&);
extern template void ComponentStorage::add_component<Collider>(Entity, const Collider&);
extern template void ComponentStorage::add_component<CircleCollider>(Entity, const CircleCollider&);
extern template void ComponentStorage::add_component<OBBCollider>(Entity, const OBBCollider&);
extern template void ComponentStorage::add_component<Lifetime>(Entity, const Lifetime&);
extern template void ComponentStorage::add_component<WrapAround>(Entity, const WrapAround&);
extern template void ComponentStorage::add_component<DestroyRequest>(Entity, const DestroyRequest&);
extern template void ComponentStorage::add_component<CollidedWith>(Entity, const CollidedWith&);
extern template void ComponentStorage::add_component<Script>(Entity, const Script&);
extern template void ComponentStorage::add_component<Player>(Entity, const Player&);
extern template void ComponentStorage::add_component<FishLevel>(Entity, const FishLevel&);
extern template void ComponentStorage::add_component<Health>(Entity, const Health&);

extern template std::optional<std::reference_wrapper<Position>> ComponentStorage::get_component<Position>(Entity);
extern template std::optional<std::reference_wrapper<Size>> ComponentStorage::get_component<Size>(Entity);
extern template std::optional<std::reference_wrapper<Color>> ComponentStorage::get_component<Color>(Entity);
extern template std::optional<std::reference_wrapper<Velocity>> ComponentStorage::get_component<Velocity>(Entity);
extern template std::optional<std::reference_wrapper<Input>> ComponentStorage::get_component<Input>(Entity);
extern template std::optional<std::reference_wrapper<Images>> ComponentStorage::get_component<Images>(Entity);
extern template std::optional<std::reference_wrapper<Text>> ComponentStorage::get_component<Text>(Entity);
extern template std::optional<std::reference_wrapper<ScreenPosition>> ComponentStorage::get_component<ScreenPosition>(Entity);
extern template std::optional<std::reference_wrapper<Rotation>> ComponentStorage::get_component<Rotation>(Entity);
extern template std::optional<std::reference_wrapper<Collider>> ComponentStorage::get_component<Collider>(Entity);
extern template std::optional<std::reference_wrapper<CircleCollider>> ComponentStorage::get_component<CircleCollider>(Entity);
extern template std::optional<std::reference_wrapper<OBBCollider>> ComponentStorage::get_component<OBBCollider>(Entity);
extern template std::optional<std::reference_wrapper<Lifetime>> ComponentStorage::get_component<Lifetime>(Entity);
extern template std::optional<std::reference_wrapper<WrapAround>> ComponentStorage::get_component<WrapAround>(Entity);
extern template std::optional<std::reference_wrapper<DestroyRequest>> ComponentStorage::get_component<DestroyRequest>(Entity);
extern template std::optional<std::reference_wrapper<CollidedWith>> ComponentStorage::get_component<CollidedWith>(Entity);
extern template std::optional<std::reference_wrapper<Script>> ComponentStorage::get_component<Script>(Entity);
extern template std::optional<std::reference_wrapper<Player>> ComponentStorage::get_component<Player>(Entity);
extern template std::optional<std::reference_wrapper<FishLevel>> ComponentStorage::get_component<FishLevel>(Entity);
extern template std::optional<std::reference_wrapper<Health>> ComponentStorage::get_component<Health>(Entity);

extern template std::optional<std::reference_wrapper<const Position>> ComponentStorage::get_component<Position>(Entity) const;
extern template std::optional<std::reference_wrapper<const Size>> ComponentStorage::get_component<Size>(Entity) const;
extern template std::optional<std::reference_wrapper<const Color>> ComponentStorage::get_component<Color>(Entity) const;
extern template std::optional<std::reference_wrapper<const Velocity>> ComponentStorage::get_component<Velocity>(Entity) const;
extern template std::optional<std::reference_wrapper<const Input>> ComponentStorage::get_component<Input>(Entity) const;
extern template std::optional<std::reference_wrapper<const Images>> ComponentStorage::get_component<Images>(Entity) const;
extern template std::optional<std::reference_wrapper<const Text>> ComponentStorage::get_component<Text>(Entity) const;
extern template std::optional<std::reference_wrapper<const ScreenPosition>> ComponentStorage::get_component<ScreenPosition>(Entity) const;
extern template std::optional<std::reference_wrapper<const Rotation>> ComponentStorage::get_component<Rotation>(Entity) const;
extern template std::optional<std::reference_wrapper<const Collider>> ComponentStorage::get_component<Collider>(Entity) const;
extern template std::optional<std::reference_wrapper<const CircleCollider>> ComponentStorage::get_component<CircleCollider>(Entity) const;
extern template std::optional<std::reference_wrapper<const OBBCollider>> ComponentStorage::get_component<OBBCollider>(Entity) const;
extern template std::optional<std::reference_wrapper<const Lifetime>> ComponentStorage::get_component<Lifetime>(Entity) const;
extern template std::optional<std::reference_wrapper<const WrapAround>> ComponentStorage::get_component<WrapAround>(Entity) const;
extern template std::optional<std::reference_wrapper<const DestroyRequest>> ComponentStorage::get_component<DestroyRequest>(Entity) const;
extern template std::optional<std::reference_wrapper<const CollidedWith>> ComponentStorage::get_component<CollidedWith>(Entity) const;
extern template std::optional<std::reference_wrapper<const Script>> ComponentStorage::get_component<Script>(Entity) const;
extern template std::optional<std::reference_wrapper<const Player>> ComponentStorage::get_component<Player>(Entity) const;
extern template std::optional<std::reference_wrapper<const FishLevel>> ComponentStorage::get_component<FishLevel>(Entity) const;
extern template std::optional<std::reference_wrapper<const Health>> ComponentStorage::get_component<Health>(Entity) const;

extern template void ComponentStorage::remove_component<Position>(Entity);
extern template void ComponentStorage::remove_component<Size>(Entity);
extern template void ComponentStorage::remove_component<Color>(Entity);
extern template void ComponentStorage::remove_component<Input>(Entity);
extern template void ComponentStorage::remove_component<Velocity>(Entity);
extern template void ComponentStorage::remove_component<Images>(Entity);
extern template void ComponentStorage::remove_component<Text>(Entity);
extern template void ComponentStorage::remove_component<ScreenPosition>(Entity);
extern template void ComponentStorage::remove_component<Rotation>(Entity);
extern template void ComponentStorage::remove_component<Collider>(Entity);
extern template void ComponentStorage::remove_component<CircleCollider>(Entity);
extern template void ComponentStorage::remove_component<OBBCollider>(Entity);
extern template void ComponentStorage::remove_component<Lifetime>(Entity);
extern template void ComponentStorage::remove_component<WrapAround>(Entity);
extern template void ComponentStorage::remove_component<DestroyRequest>(Entity);
extern template void ComponentStorage::remove_component<CollidedWith>(Entity);
extern template void ComponentStorage::remove_component<Script>(Entity);
extern template void ComponentStorage::remove_component<Player>(Entity);
extern template void ComponentStorage::remove_component<FishLevel>(Entity);
extern template void ComponentStorage::remove_component<Health>(Entity);

extern template bool ComponentStorage::has_component<Position>(Entity) const;
extern template bool ComponentStorage::has_component<Size>(Entity) const;
extern template bool ComponentStorage::has_component<Color>(Entity) const;
extern template bool ComponentStorage::has_component<Input>(Entity) const;
extern template bool ComponentStorage::has_component<Velocity>(Entity) const;
extern template bool ComponentStorage::has_component<Images>(Entity) const;
extern template bool ComponentStorage::has_component<Text>(Entity) const;
extern template bool ComponentStorage::has_component<ScreenPosition>(Entity) const;
extern template bool ComponentStorage::has_component<Rotation>(Entity) const;
extern template bool ComponentStorage::has_component<Collider>(Entity) const;
extern template bool ComponentStorage::has_component<CircleCollider>(Entity) const;
extern template bool ComponentStorage::has_component<OBBCollider>(Entity) const;
extern template bool ComponentStorage::has_component<Lifetime>(Entity) const;
extern template bool ComponentStorage::has_component<WrapAround>(Entity) const;
extern template bool ComponentStorage::has_component<DestroyRequest>(Entity) const;
extern template bool ComponentStorage::has_component<CollidedWith>(Entity) const;
extern template bool ComponentStorage::has_component<Script>(Entity) const;
extern template bool ComponentStorage::has_component<Player>(Entity) const;
extern template bool ComponentStorage::has_component<FishLevel>(Entity) const;
extern template bool ComponentStorage::has_component<Health>(Entity) const;

extern template std::vector<Entity> ComponentStorage::entities_with_component<Position>() const;
extern template std::vector<Entity> ComponentStorage::entities_with_component<Size>() const;
extern template std::vector<Entity> ComponentStorage::entities_with_component<Color>() const;
extern template std::vector<Entity> ComponentStorage::entities_with_component<Input>() const;
extern template std::vector<Entity> ComponentStorage::entities_with_component<Velocity>() const;
extern template std::vector<Entity> ComponentStorage::entities_with_component<Images>() const;
extern template std::vector<Entity> ComponentStorage::entities_with_component<Text>() const;
extern template std::vector<Entity> ComponentStorage::entities_with_component<ScreenPosition>() const;
extern template std::vector<Entity> ComponentStorage::entities_with_component<Rotation>() const;
extern template std::vector<Entity> ComponentStorage::entities_with_component<Collider>() const;
extern template std::vector<Entity> ComponentStorage::entities_with_component<CircleCollider>() const;
extern template std::vector<Entity> ComponentStorage::entities_with_component<OBBCollider>() const;
extern template std::vector<Entity> ComponentStorage::entities_with_component<Lifetime>() const;
extern template std::vector<Entity> ComponentStorage::entities_with_component<WrapAround>() const;
extern template std::vector<Entity> ComponentStorage::entities_with_component<DestroyRequest>() const;
extern template std::vector<Entity> ComponentStorage::entities_with_component<CollidedWith>() const;
extern template std::vector<Entity> ComponentStorage::entities_with_component<Script>() const;
extern template std::vector<Entity> ComponentStorage::entities_with_component<Player>() const;
extern template std::vector<Entity> ComponentStorage::entities_with_component<FishLevel>() const;
extern template std::vector<Entity> ComponentStorage::entities_with_component<Health>() const;
#endif // COMPONENT_STORAGE_HPP
# engine.ability

Reference documentation of the `engine.ability` module of the openage modding API.

## ability.Ability

```python
Ability(Entity):
    properties : dict(abstract(children(AbilityProperty)), children(AbilityProperty))
```

Generalization object for all abilities. Abilities define what game entities can *do* and what they *are*, respectively. They can be considered passive and active traits.

**properties**
Further specializes the ability beyond the standard behaviour.

The engine expects objects from the namespace `engine.ability.property.type` as keys. Values must always be an instance of the object used as key.

Standard behavior without properties:

* Abilities in the `abilities` set of `GameEntity` are considered enabled as soon as the game entity is created, unless a `StateChager` object disables them.
* No ability is explicitly required to be animated or to play a sound. For this purpose, ability properties are used.
* A game entity's abilities are available to **all** players, not just the owner. This can be limited to players with specific diplomatic stances towards the owner by assigning the `Diplomatic` property.

Properties:

* `Animated`: Assigns an animation that is played while the ability is active.
* `AnimationOverride`: Overrides the animations of specified animated abilities temporarily.
* `CommandSound`: Assigns a sound that is played once when the player orders the game entity to use the ability.
* `Diplomatic`: Makes the ability only accessible for players when the owner of the game entity has the specified diplomatic stances towards them.
* `ExecutionSound`: Assigns a sound that is played while the ability is active.
* `Lock`: Assigns the ability to a lock pool.

## ability.property.AbilityProperty

```python
AbilityProperty(Entity):
    pass
```

Generalization object for all properties of abilities.

## ability.property.type.Animated

```python
Animated(AbilityProperty):
    animations : set(Animation)
```

Abilities with this property will play an animation while they are active. Whether the animation loops is defined in the `.sprite` file linked in the `Animation` object.

**animations**
The animation(s) played while the ability is active. If more than one animation is defined, the engine picks one of them at random.

## ability.property.type.AnimationOverride

```python
AnimationOverride(AbilityProperty):
    overrides : set(AnimationOverride)
```

Specifies a set of animation overrides that are applied when the ability is used.

*Usage example*: Consider a unit with two attacks. One of them is animated by having the unit wield a sword, while the other one is an animation in which the unit uses a bow. What we want is that the animation of the `Move` and `Idle` abilities correspond to the attack that was used last, e.g. after a sword attack, the movement animation show the unit moving around with a sword in its hand, and after a bow attack, the movement animation show the unit moving around with a bow. To accomplish this, we would add the `AnimationOverride(AbilityProperty)` to both attack abilities and specify `AnimationOverride` objects for them which target `Move` and `Idle`.

**overrides**
Stores the animations and abilities that will be overridden temporarily as `AnimationOverride` objects.

## ability.property.type.CommandSound

```python
CommandSound(AbilityProperty):
    sounds : set(Sound)
```

Abilities with this property will play a sound once when the player orders the game entity to use them.

**sounds**
The sound(s) played when the order to use the ability is given. If more than one sound is defined, the engine picks one of them at random.

## ability.property.type.Diplomatic

```python
Diplomatic(AbilityProperty):
    stances : set(DiplomaticStance)
```

Restricts the players who can access the ability. Access is given to a player when the owner of the game entity has one of the specified stances towards this player. Note that when using this property, the access must also be explicitly allowed for the owner of the game entity by adding the `Self` stance to the set.

*Usage example*: Consider a trade post like the marketplace from AoE games. Players can send trade carts between trade posts to generate resources. Trade post behavior is activated by assigning the `TradePost` ability to a game entity. Without the `Diplomatic` property, all player can trade with this trade post, including its owner. However, if we want to limit the trade post to only be accessible for allies, we assign `Diplomatic`as a property and add the `Allied` stance to the `stances` set.

**stances**
The stances the owner needs to have towards another player to make the ability accessible for the latter.

## ability.property.type.ExecutionSound

```python
ExecutionSound(AbilityProperty):
    sounds : set(Sound)
```

Abilities with this property will play a sound while they are active.

**sounds**
The sound(s) played while the ability is active. If more than one sound is defined, the engine picks one of them at random.

## ability.property.type.Lock

```python
Lock(AbilityProperty):
    lock_pool : LockPool
```

Abilities with this property require a free slot in the specified lock pool to become active. While they are active, the occupy one slot in the pool.

**lock_pool**
The lock pool used by the ability. This lock pool should be assigned to a `Lock` ability that is assigned to the game entity.

If the lock pool

* is not assigned to a `Lock` ability of the game entity **or**
* the corresponding `Lock` ability is disabled **or**
* all slots are occupied by other abilities at runtime

the ability with this property cannot become active.

## ability.type.ActiveTransformTo

```python
TransformTo(Ability):
    target_state       : StateChanger
    transform_time     : float
    transform_progress : set(TransformProgress)
```

Activates a state change for a game entity.

**target_state**
Target state change that activates when the time defined in `transform_time` has passed.

**transform_time**
The time for the transformation to complete.

**transform_progress**
A set of `TransformProgress` objects that can activate state changes and animation overrides while the transformation progresses.

## ability.type.ApplyContinuousEffect

```python
ApplyContinuousEffect(Ability):
    effects              : set(ContinuousEffect)
    application_delay    : float
    allowed_types        : set(GameEntityType)
    blacklisted_entities : set(GameEntity)
```

Applies a batch of continuous effects on another game entity.

**effects**
The applied continuous effect definitions.

**application_delay**
Time between the initiation of the ability and the application of the effects.

**allowed_types**
Whitelist of game entity types that can be targeted with the ability.

**blacklisted_entities**
Blacklist for specific game entities that would be covered by `allowed_types`, but should be explicitly excluded.

## ability.type.ApplyDiscreteEffect

```python
ApplyDiscreteEffect(Ability):
    batches              : set(EffectBatch)
    reload_time          : float
    application_delay    : float
    allowed_types        : set(GameEntityType)
    blacklisted_entities : set(GameEntity)
```

Applies batches of discrete effects on a game entity.

**effects**
The batches of discrete effect which are applied when the ability is used.

**reload_time**
Minimum time between two applications of the effect batches.

**application_delay**
Time between the initiation of the ability and first application of the effects.

**allowed_types**
Whitelist of game entity types that can be targeted with the ability.

**blacklisted_entities**
Blacklist for specific game entities that would be covered by `allowed_types`, but should be explicitly excluded.

## ability.type.AttributeChangeTracker

```python
AttributeChangeTracker(Ability):
    attribute       : Attribute
    change_progress : set(DamageProgress)
```

Allows the alteration of the state of the game entity when its attribute values increase or decrease.

**attribute**
The attribute which is monitored.

**change_progress**
Set of `AttributeChangeProgress` objects that can activate state changes and animation overrides when the current attribute increases or decreases.

## ability.type.Cloak

```python
Cloak(Ability):
    interrupted_by     : set(Ability)
    interrupt_cooldown : float
```

Makes the unit invisible to game entity's of other players.

**interrupted_by**
Abilities of the game entity that interrupt the cloak when used.

**interrupt_cooldown**
Time needed to reactivate the cloak after an ability from `interrupted_by` has been used.

## ability.type.CollectStorage

```python
CollectStorage(Ability):
    container        : Container
    storage_elements : set(GameEntity)
```

Allows a game entity to insert specified game entities into one of its containers.

**container**
The container the target game entity will be inserted into. A `Storage` ability with this container must be enabled.

**storage_elements**
The set of game entities that can be inserted into the container. The container must allow the `GameEntity` objects.

## ability.type.Constructable

```python
Constructable(Ability):
    starting_progress     : int
    construction_progress : set(ConstructionProgress)
```

Makes the game entity constructable via `Effect` types.

**starting_progress**
The construction progress when the game entity is created.

**construction_progress**
Set of `ConstructionProgress` objects that can activate state changes and animation overrides when the construction progresses.

## ability.type.Create

```python
Create(Ability):
    creatables : set(CreatableGameEntity)
```

Allows a game entity to spawn new instances of game entities.

**creatables**
Stores the reference to the creatable game entities as well as the preconditions that have to be fulfilled in order to spawn a new instance.

## ability.type.Despawn

```python
Despawn(Ability):
    activation_condition : set(LogicElement)
    despawn_condition    : set(LogicElement)
    despawn_time         : float
    state_change         : optional(StateChanger)
```

Allows a "clean exit" that removes the game entity from the current game. By default, `Despawn` is inactive until triggered by `activation_condition`. Once activated, at least one of the elements in `despawn_condition` must be `True` to trigger the despawning.

**activation_condition**
Activates (i.e. *primes*) the ability once one of the elements is `True`. After activation, the ability will wait for an element in `despawn_condition` to be `True`.

**despawn_condition**
Triggers the despawning once one of the elements is `True`.

**despawn_time**
Time until the game entity is removed from the game. The removal is permanent.

**state_change**
Alters the abilities and modifiers of a game entity after the `despawn_condition` is fulfilled. The state change stays active until the game entity is removed from the game.

## ability.type.DetectCloak

```python
DetectCloak(Ability):
    range                : float
    allowed_types        : set(GameEntityType)
    blacklisted_entities : set(GameEntity)
```

Enables the game entity to decloak other game entities which use the `Cloak` ability.

**range**
The range in which other game entities will be decloaked.

**allowed_types**
Whitelist of game entity types that can be detected.

**blacklisted_entities**
Blacklist for specific game entities that would be covered by `allowed_types`, but should be explicitly excluded.

## ability.type.DropResources

```python
DropResources(Ability):
    containers           : set(ResourceContainer)
    search_range         : float
    allowed_types        : set(GameEntityType)
    blacklisted_entities : set(GameEntity)
```

Allows a game entity to drop off resources at other game entities that have the `DropSite` ability. The resources are transfered to the player's global resource storage. Game entities with the `ResourceStorage` ability will automatically use this ability when their resource container capacity is reached.

**containers**
Containers whose resources are deposited.

**search_range**
The range in which the unit will search for a resource drop site when the ability is not used manually.

**allowed_types**
Whitelist of game entity types that can be used as drop sites. They must have a `DropSite` ability accepting the corresponding resource.

**blacklisted_entities**
Blacklist for specific game entities that would be covered by `allowed_types`, but should be explicitly excluded.

## ability.type.DropSite

```python
DropSite(Ability):
    accepts_from : set(ResourceContainer)
```

Allows a game entity to act as a resource drop off point for game entities with the `DropResources` ability. The resources are transfered to the player's global resource storage.

**accepts_from**
Resource containers that can be emptied at the game entity.

## ability.type.EnterContainer

```python
EnterContainer(Ability):
    allowed_containers   : set(Container)
    allowed_types        : set(GameEntityType)
    blacklisted_entities : set(GameEntity)
```

Allows a game entity to enter specified containers of another game entity's `Storage` ability.

**allowed_containers**
The containers that can be entered.

**allowed_types**
Whitelist of game entity types that can be targeted with the ability.

**blacklisted_entities**
Blacklist for specific game entities that would be covered by `allowed_types`, but should be explicitly excluded.

## ability.type.ExchangeResources

```python
ExchangeResources(Ability):
    resource_a     : Resource
    resource_b     : Resource
    exchange_rate  : ExchangeRate
    exchange_modes : set(ExchangeMode)
```

Exchanges a resource for another resource. The nature of the exchange depends on the exchange mode. Players can exchange resources with themselves or other players.

**resource_a**
First resource used for the exchange.

**resource_b**
Second resource used for the exchange.

**exchange_rate**
Defines an exchange rate for the exchange.

**exchange_modes**
Defines how the resources can be exchanged.

## ability.type.ExitContainer

```python
ExitContainer(Ability):
    allowed_containers : set(Container)
```

Allows a game entity to exit specified containers of another game entity's `Storage` ability when they are in the container.

**allowed_containers**
The containers that can be exited.

## ability.type.Fly

```python
Fly(Ability):
    height : float
```

Allows a game entity fly at a fixed height.

**height**
The height at which the game entity flies. This value is always relative to the ground below.

## ability.type.FormFormation

```python
Formation(Ability):
    formations : set(GameEntityFormation)
```

Allows a game entity to be part of certain formations.

**formations**
Formation types that the game entity can be part of as well as the corresponding subformation that is used.

## ability.type.Foundation

```python
Foundation(Ability):
    foundation_terrain : Terrain
```

Changes the terrain under a game entity when it is created.

**foundation_terrain**
Replacement terrain that is placed on the tiles the game entity occupies.

## ability.type.GameEntityStance

```python
GameEntityStance(Ability):
    stances: set(GameEntityStance)
```

Defines activity stances for a game entity. These stances define which abilities will be used without player interaction when the game entity is idle.

**stances**
Stance definitions for the game entity as `aux.game_entity_stance.GameEntityStance` objects.

## ability.type.Gather

```python
Gather(Ability):
    auto_resume         : bool
    resume_search_range : float
    targets             : set(ResourceSpot)
    gather_rate         : ResourceRate
    container           : ResourceContainer
```

Allows a game entity to gather from resource spots defined the `Harvestable` abilities of other game entities.

**auto_resume**
Determines whether the game entity will automatically resume gathering after the resource spot is depleted. When enabled, the game entity will check if it can refill the depleted resource spot with one of its `Restock` abilities. Otherwise, the game entity will search for a new resource spot it can access with this ability.

**resume_search_range**
The range in which the game entity searches for a new resource spot if `auto_resume` is enabled.

**targets**
Resource spots that can be accessed with this ability.

**gather_rate**
The rate at which the game entity collects resources from the resource spot.

**container**
Resource container of the game entity where the gathered resources are stored. The resource container must be referenced by the game entity in a `ResourceStorage` ability.

## ability.type.Harvestable

```python
Harvestable(Ability):
    resources              : ResourceSpot
    harvest_progress       : set(HarvestProgress)
    restock_progress       : set(RestockProgress)
    gatherer_limit         : int
    harvestable_by_default : bool
```

Assigns a game entity a resource spot that can be harvested by other game entities with the `Gather` ability.

**resource_spot**
The resource spot as a `ResourceSpot` object. It defines the contained resource type and capacity.

**harvest_progress**
Can alter the game entity when the percentage of harvested resources reaches defined progress intervals.

**restock_progress**
Can alter the game entity when the percentage of restocked resources reaches defined progress intervals.

**gatherer_limit**
Limits the amount of gatherers that can access the resource spot simultaneously.

**harvestable_by_default**
Determines whether the resource spot is harvestable when it is created. If `True`, the resource spot will be accessible without any conditions as long as the corresponding `Harvestable` ability of the game entity is enabled. When set to `False`, the resource spot must be activated with a `MakeHarvestable` effect. The conditions under which the activation succeeds are decided by the `MakeHarvestable` resistance of the game entity the ability belongs to.

## ability.type.Herd

```python
Herd(Ability):
    range                : float
    strength             : int
    allowed_types        : set(GameEntityType)
    blacklisted_entities : set(GameEntity)
```

Allows a game entity to change the ownership of other game entities with the `Herdable` ability.

**range**
Minimum distance to a herdable game entity to make it change ownership.

**strength**
Comparison value for situations when the game entity competes with other game entities for a herdable. The game entity with the highest `strength` value will always be prefered, even if other game entities fulfill the condition set by `mode` in `Hardable` better.

**allowed_types**
Whitelist of game entity types that can be herded.

**blacklisted_entities**
Blacklist for specific game entities that would be covered by `allowed_types`, but should be explicitly excluded.

## ability.type.Herdable

```python
Herdable(Ability):
    adjacent_discover_range : float
    mode                    : HerdableMode
```

Makes the game entity switch ownership when it is in range of another game entity with a `Herd` ability. The new owner is the player who owns the game entity with the `Herd` ability. If the herdable game entity is in range of two or more herding game entities, the ownership changes to the owner of the game entity which is closest.

**adjacent_discover_range**
When the game entity is herded, other herdables in this range are also herded. The rules in `Herd` of the herding game entity apply.

**mode**
Determines who gets ownership of the herdable game entity when multiple game entities using `Herd` are in range.

## ability.type.Hitbox

```python
Hitbox(Ability):
    hitbox : Hitbox
```

Adds a hitbox to a game entity that is used for collision with other game entities.

**hitbox**
Defines the size (x, y, z) of the hitbox.

## ability.type.Idle

```python
Idle(Ability):
    pass
```

Used for assigning animations and sounds to game entities in an idle state. In contrast to the other abilities, game entities always have an implicit idle state, even if they have no `Idle` ability defined.

## ability.type.LineOfSight

```python
LineOfSight(Ability):
    range : float
```

Reveals an area around the game entity.

**range**
Determines the radius of the circular area around the game entity that is revealed.

## ability.type.Live

```python
Live(Ability):
    attributes : set(AttributeSetting)
```

Assigns attributes to a game entity. The properties of the attributes are defined in `AttributeSetting` objects.

**attributes**
Configures the attribute values of the game entity via `AttributeSetting` objects.

## ability.type.Lock

```python
Lock(Ability):
    lock_pools : set(LockPool)
```

Defines lock pools for the abilities of the game entities.

**lock_pools**
Set of lock pools usable by abilities of the game entity with the `Lock` property.

## ability.type.Move

```python
Move(Ability):
    speed : float
    modes : set(MoveMode)
```

Allows a game entity to move around the map.

**speed**
Speed of movement.

**modes**
Movement modes that can be used by the game entity.

## ability.type.Named

```python
Named(Ability):
    name             : TranslatedString
    description      : TranslatedMarkupFile
    long_description : TranslatedMarkupFile
```

Assigns a game entity name and descriptions.

**name**
The name of the game entity as a translatable string.

**description**
A description of the game entity as a translatable markup file.

**long_description**
A longer description of the game entity as a translatable markup file.

## ability.type.Passable

```python
Passable(Ability):
    hitbox : Hitbox
    mode   : PassableMode
```

Deactivates the specified hitbox of the game entity for movement of other game entities. The hitbox is still relevant for the game entity's own movement.

**hitbox**
Reference to the hitbox that should be deactivated.

**mode**
Defines the game entities for which the hitbox is deactivated.

## ability.type.PassiveTransformTo

```python
PassiveTransformTo(Ability):
    condition          : set(LogicElement)
    transform_time     : float
    target_state       : StateChanger
    transform_progress : set(TransformProgress)
```

Activates a defined state when a condition is fulilled.

**condition**
Triggers the transformation once one element is `True`.

**transform_time**
Time until the target state is reached.

**target_state**
State change activated after `transform_time` has passed.

**transform_progress**
Can alter the game entity while the transformation progresses.

## ability.type.ProductionQueue

```python
ProductionQueue(Ability):
    size             : int
    production_modes : set(children(ProductionMode))
```

Enables a game entity to queue production of `CreatableGameEntity` and `ResearchableTech`.

**size**
Maximum number of production items in the queue.

**production_modes**
Defines the production items which can be added to the queue.

## ability.type.Projectile

```python
Projectile(Ability):
    arc               : int
    accuracy          : set(Accuracy)
    target_mode       : TargetMode
    ignored_types     : set(GameEntityType)
    unignore_entities : set(GameEntity)
```

Gives a game entity projectile behaviour. A projectile is always spawned with another `GameEntity` object as a target. The engine calculates a parabolic path to the target, using the `arc`, `accuracy` and `target_mode` members. While moving along this path, the game entity can use other abilities.

**arc**
The starting arc of the projectile as value between 0 and 360.

**accuracy**
The accuracy of the projectile as an `Accuracy` object.

**target_mode**
Determines the end point of the projectile path with a `TargetMode` object. The end point can be at the expected position of the target, when the projectile hits, or at the position of the target, when the projectile spawns.

**ignore_types**
The projectile will not use abilities on game entities with these types and ignore their hitbox, while moving along the path. However, the projectile will always use abilities on its target, even if it has one of they types in this set.

**unignore_entities**
Whitelist of game entities who have a type that is listed in `ignore_types`, but should not ne ignored.

## ability.type.ProvideContingent

```python
ProvideContingent(Ability):
    amount : set(ResourceAmount)
```

Provides a temporary resource amount to a `ResourceContingent`. The amount is provided until the ability is disabled.

**amount**
Temporary resource amount that is provided by the game entity.

## ability.type.RallyPoint

```python
RallyPoint(Ability):
    pass
```

Allows a game entity to set a rally point on the map. Game entities spawned by the `Create` ability or ejected from a container will move to the rally point location. The rally point can be placed on another game entity. In that case, the game entities moving there will try to use an appropriate ability on it.

## ability.type.RangedContinuousEffect

```python
RangedContinuousEffect(ApplyContinuousEffect):
    min_range : int
    max_range : int
```

Applies a batch of discrete effects on another game entity. This specialization of `ApplyContinuousEffect` allows ranged application.

**min_range**
Minimum distance to target.

**max_range**
Maximum distance to the target.

## ability.type.RangedDiscreteEffect

```python
RangedDiscreteEffect(ApplyDiscreteEffect):
    min_range : int
    max_range : int
```

Applies a batch of discrete effects on another game entity. This specialization of `ApplyDiscreteEffect` allows ranged application.

**min_range**
Minimum distance to target.

**max_range**
Maximum distance to the target.

## ability.type.RegenerateAttribute

```python
RegenerateAttribute(Ability):
    rate : AttributeRate
```

Regenerate attribute points at a defined rate. The game entity must have the attribute in its `Live` ability.

**rate**
Regeneration rate as an `AttributeRate` object.

## ability.type.RegenerateResourceSpot

```python
RegenerateResourceSpot(Ability):
    rate : ResourceRate
    resource_spot : ResourceSpot
```

Regenerate the available resources of a game entity's resource spot at a defined rate.

**rate**
Regeneration rate as an `ResourceRate` object.

**resource_spot**
Resource spot that is refilled. The game entity must have a `Harvestable` ability that contains this resource spot.

## ability.type.RemoveStorage

```python
RemoveStorage(Ability):
    container        : Container
    storage_elements : set(GameEntity)
```

Allows a game entity to remove specified game entities from one of its containers.

**container**
The container the target game entity will be removed from. A `Storage` ability with this container must be enabled.

**storage_elements**
The set of game entities that can be removed from the container.

## ability.type.Research

```python
Research(Ability):
    researchables : set(ResearchableTech)
```

Allows a game entity to research a `Tech` object. Initiating a research will lock the technology for other game entities that can research it. The lock is lifted when the research process of the `Tech` is cancelled.

**researchables**
Stores the reference to the researchable techs as well as the preconditions that have to be fulfilled in order to research them.

## ability.type.Resistance

```python
Resistance(Ability):
    resistances : set(Resistance)
```

Assigns a game entity resistances to effects applied by other game entities.

**resistances**
Set of resistances.

## ability.type.ResourceStorage

```python
ResourceStorage(Ability):
    containers : set(ResourceContainer)
```

Assigns a game entity containers for storing resources until they are dropped at a drop site.

**containers**
Set of containers which specify how resources can be stored.

## ability.type.Restock

```python
Restock(Ability):
    auto_restock : bool
    target       : ResourceSpot
    restock_time : float
    manual_cost  : Cost
    auto_cost    : Cost
    amount       : int
```

Refills a resource spot with a defined amount of resources.

**auto_restock**
Determines whether the game entity will automatically restock a resource spot that it helped deplete.

**target**
The resource spot that can be restocked with this ability.

**restock_time**
Time until the restocking finishes.

**manual_cost**
Cost of restocking when the ability is used manually by a player.

**auto_cost**
Cost of restocking when the ability is automatically used by the game entity.

**amount**
The amount of resources that is refilled.

## ability.type.Selectable

```python
Selectable(Ability):
    selection_box : SelectionBox
```

Makes the game entity selectable by players.

**selection_box**
Defines the area which has to be clicked by the player to select the game entity.

## ability.type.SendBackToTask

```python
SendBackToTask(Ability):
    allowed_types        : set(GameEntityType)
    blacklisted_entities : set(GameEntity)
```

Empties the containers of the `Storage` abilities of a game entity and lets the ejected game entities resume their previous tasks.

**allowed_types**
Whitelist of game entity types that can will be affected.

**blacklisted_entities**
Blacklist for specific game entities that would be covered by `allowed_types`, but should be explicitly excluded.

## ability.type.ShootProjectile

```python
ShootProjectile(Ability):
    projectiles              : orderedset(Projectile)
    min_projectiles          : int
    max_projectiles          : int
    min_range                : int
    max_range                : int
    reload_time              : float
    spawn_delay              : float
    projectile_delay         : float
    require_turning          : bool
    manual_aiming_allowed    : bool
    spawning_area_offset_x   : float
    spawning_area_offset_y   : float
    spawning_area_offset_z   : float
    spawning_area_width      : float
    spawning_area_height     : float
    spawning_area_randomness : float
    allowed_types            : set(GameEntityType)
    blacklisted_entities     : set(GameEntity)
```

Spawns projectiles that have a `GameEntity` object as a target. Projectiles are essentially fire-and-forget objects and only loosely depend on the game entity they originate from. The game entity using `ShootProjectile` will forward its `max_range` value and the target's ID to the spawned projectile object, so that it can calculate the projectile path.

**projectiles**
`Projectile` game entities that can be spawned by the ability. The projectiles are fired in the order they have in the set. The amount of spawned projectiles is `min_projectiles` by default and can be increased by `Modifier` objects up to an upper limit of `max_projectiles`. The *last* `Projectile` is reused when the number of projectiles supposed to be fired is greater than the size of the set.

**min_projectiles**
Minimum amount of projectiles spawned.

**max_projectiles**
Maximum amount of projectiles spawned.

**min_range**
Minimum distance the game entity has to have to the target.

**max_range**
Maximum distance at which the game entity can initiate the ability. Projectiles will be fired at targets further than this distance if the target was in range of the shooting game entity when the ability was initiated.

**reload_time**
Time until the ability can be used again. The timer starts after the *last* projectile has been fired.

**spawn_delay**
Time between the initiation of the ability and first projectile being fired.

**projectile_delay**
Time until the next projectile is spawned when multiple projectiles are fired.

**require_turning**
Determines whether the game entity must turn towards the target.

**manual_aiming_allowed**
Determines whether the ability can be aimed at a coordinate instead of a `GameEntity` object.

**spawning_area_offset_x**
Part of the spawn location coordinates. The spawn location is an offset from the anchor point of the game entity.

**spawning_area_offset_y**
Part of the spawn location coordinates. The spawn location is an offset from the anchor point of the game entity.

**spawning_area_offset_z**
Part of the spawn location coordinates. The spawn location is an offset from the anchor point of the game entity.

**spawning_area_width**
Determines the spawn area in which the projectile can be spawned randomly.

**spawning_area_height**
Determines the spawn area in which the projectile can be spawned randomly.

**spawning_area_randomness**
Value between 0 and 1 that determines far the spawn location can be from the spawn area center. The engine chooses random (x,y) coordinates in the spawn area and multiplies them by this value in order to determine the spawn location.

**allowed_types**
Whitelist of game entity types that can be targeted with the ability.

**blacklisted_entities**
Blacklist for specific game entities that would be covered by `allowed_types`, but should be explicitly excluded.

## ability.type.Stop

```python
Stop(Ability):
    pass
```

Stops all current tasks and returns the game entity to an idle state.

## ability.type.Storage

```python
Storage(Ability):
    container       : Container
    empty_threshold : set(LogicElement)
```

Enables a game entity to store other game entities. The stored game entities can influence the state of the storing game entity.

**container**
Defines which and how many game entities can be stored.

**empty_threshold**
Empties the storage if one of the elements is `True`.

## ability.type.TerrainRequirement

```python
TerrainRequirement(Ability):
    allowed_types        : set(TerrainType)
    blacklisted_terrains : set(Terrain)
```

Makes a game entity require specific terrains to be placed or moved on.

**allowed_types**
Whitelist of terrain types that the game entity can be placed or moved on.

**blacklisted_terrains**
Blacklist for specific terrains that would be covered by `allowed_types`, but should be explicitly excluded.

## ability.type.Trade

```python
Trade(Ability):
    trade_routes : set(TradeRoute)
```

Allows a game entity to trade with other game entities that have the `TradePost` ability.

**trade_routes**
Trade routes that can be established with trade posts. The `TradeRoute` object defines rules and traded resources for the trade.

## ability.type.TradePost

```python
TradePost(Ability):
    trade_routes : set(TradeRoute)
```

Makes a game entity a trade post that other game entities can trade with using their `Trade` ability.

**trade_routes**
Trade routes that can be established with this trade post. The `TradeRoute` object defines rules and traded resources for the trade.

## ability.type.TransferStorage

```python
TransferStorage(Ability):
    storage_element  : GameEntity
    source_container : Container
    target_container : Container
```

Transfers one game entity from a container to another.

**storage_element**
Game entity that is transferred.

**source_container**
Container the game entity is stored in when the ability is used.

**target_container**
Container that the game entity is inserted into. The target container can belong to the same game entity.

## ability.type.Turn

```python
Fly(Ability):
    turn_speed : float
```

Allows a game entity to turn on the spot.

**turn_speed**
Speed at which the game entity turns.

## ability.type.UseContingent

```python
UseContingent(Ability):
    amount : set(ResourceAmount)
```

Reserves a temporary resource amount of a `ResourceContingent`. Game entities produced via the `Create` ability only spawn the the contingent has enough space to support them. The amount is reserved until the ability is disabled.

**amount**
Temporary resource amount that is reserved by the game entity.

## ability.type.Visibility

```python
Visibility(Ability):
    visible_in_fog : bool
```

Configures a game entity's visibility in the fog of war.

**visible_in_fog**
Determines whether the game entity is visible in the fog of war.

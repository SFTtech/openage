# engine.modifier

Reference documentation of the `engine.modifier` module of the openage modding API.

## modifier.Modifier

```python
Modifier(Object):
    properties : dict(abstract(ModifierProperty), ModifierProperty) = {}
```

Generalization object for all modifiers. Modifiers change the behavior of abilities at for general and edge cases. They can influence more than one ability at a time.

**properties**
Further specializes the modifier beyond the standard behaviour.

The engine expects objects from the namespace `engine.modifier.property.type` as keys. Values must always be an instance of the object used as key.

Standard behavior without properties:

* Modifiers in the `modifiers` set of `GameEntity` are considered enabled as soon as the game entity is created, unless a `StateChager` object disables them.
* If the modifier is assigned to a `GameEntity` object, the modifications only apply to this game entity.
* If the modifier is assigned to a `Civilization` object, the modifications apply to all game entities of this civilization.
* Modifiers stack by default.

Properties:

* `Multiplier`: The associated member value in the ability targeted by the modifier is multiplied with a defined factor.
* `Scoped`: The modifier is applied to a defined set of game entities, regardless of where it is assigned.
* `Stacked`: Allows defining a limit on how often the modifier effect is stacked.

## modifier.property.ModifierProperty

```python
ModifierProperty(Object):
    pass
```

Generalization object for all properties of modifiers.

## modifier.property.type.Multiplier

```python
Multiplier(ModifierProperty):
    multiplier : float
```

Multiples the value of the associated ability member with a defined factor.

**multiplier**
Multiplication factor.

## modifier.property.type.Scoped

```python
Scoped(ModifierProperty):
    stances : set(children(DiplomaticStance))
    scope   : children(ModifierScope)
```

Applies the modifier to a defined set of game entities. The modifier affects these game entities as long as it stays enabled for the game entity it is assigned to.

**diplomatic_stances**
Applies the modifiers to players that the owner of the game entity has these diplomatic stances with.

**scope**
Defines the game entities affected by this modifier.

## modifier.property.type.Stacked

```python
Stacked(ModifierProperty):
    stack_limit : int
```

Defines how often a modifier can be applied to the same game entity.

**stack_limit**
Maximum number of times the modifier can be stacked.

## modifier.effect.flat_attribute_change.type.ElevationDifferenceHigh

```python
ElevationDifferenceHigh(Modifier):
    min_elevation_difference : optional(float) = None
```

Changes the cumulated *change value* of `FlatAtttributeChange` effects when the effector containing this modifier is located *higher* than the targeted resistor.

**min_elevation_difference**
The minimum elevation difference between effector and resistor.

## modifier.effect.flat_attribute_change.type.ElevationDifferenceLow

```python
ElevationDifferenceLow(Modifier):
    min_elevation_difference : optional(float) = None
```

Changes the cumulated *change value* of `FlatAtttributeChange` effects when the effector containing this modifier is located *lower* than the targeted resistor.

**min_elevation_difference**
The minimum elevation difference between effector and resistor.

## modifier.effect.flat_attribute_change.type.Flyover

```python
Flyover(Modifier):
    relative_angle       : float
    flyover_types        : set(children(GameEntityType))
    blacklisted_entities : set(GameEntity)
```

Changes the accumulated *applied change value* of `FlatAtttributeChange` effects of a projectile's attack if the projectile path went over specified game entity types.

**relative_angle**
Maximum difference between the relative angle of the effector and the flyover entity in degrees.

**flyover_types**
Whitelist of game entity types that must be under the patch of the projectile. The game entities must have the `Hitbox` ability.

**blacklisted_entities**
Blacklist for specific game entities that would be covered by `flyover_types`, but should be excplicitly excluded.

## modifier.effect.flat_attribute_change.type.Terrain

```python
Terrain(Modifier):
    terrain : Terrain
```

Changes the accumulated *applied change value* of `FlatAtttributeChange` effects when the target game entity is on a specified terrain.

**terrain**
Terrain the targeted game entity must stand on.

## modifier.effect.flat_attribute_change.type.Unconditional

```python
Unconditional(Modifier):
    pass
```

Changes the accumulated *applied change value* of `FlatAtttributeChange` effects without any conditions.

## modifier.effect.type.TimeRelativeAttributeChange

```python
TimeRelativeAttributeChange(Modifier):
    pass
```

Changes the `total_change_time` member of `TimeRelativeAttributeChange` effects.

## modifier.effect.type.TimeRelativeProgressChange

```python
TimeRelativeProgressChange(Modifier):
    pass
```

Changes the `total_change_time` member of `TimeRelativeProgress` effects.

## modifier.resistance.flat_attribute_change.type.ElevationDifferenceHigh

```python
ElevationDifferenceHigh(Modifier):
    min_elevation_difference : optional(float) = None
```

Changes the cumulated *change value* of `FlatAtttributeChange` resistances when the resistor containing this modifier is located *higher* than the effector.

**min_elevation_difference**
The minimum elevation difference between effector and resistor.

## modifier.resistance.flat_attribute_change.type.ElevationDifferenceLow

```python
ElevationDifferenceLow(Modifier):
    min_elevation_difference : optional(float) = None
```

Changes the cumulated *change value* of `FlatAtttributeChange` resistances when the resistor containing this modifier is located *lower* than the effector.

**min_elevation_difference**
The minimum elevation difference between effector and resistor.

## modifier.resistance.flat_attribute_change.type.Stray

```python
Stray(Modifier):
    pass
```

Changes the cumulated *applied change value* of `FlatAtttributeChange` resistances for a projectile when the resistor was not the intended target.

## modifier.resistance.flat_attribute_change.type.Terrain

```python
Terrain(Modifier):
    terrain : Terrain
```

Changes the cumulated *applied change value* of `FlatAtttributeChange` resistances when the resisting game entity is on a specified terrain.

**terrain**
The terrain the game entity must stand on.

## modifier.resistance.flat_attribute_change.type.Unconditional

```python
Unconditional(Modifier):
    pass
```

Changes the accumulated *applied change value* of `FlatAtttributeChange` resistances without any conditions.

## modifier.type.AbsoluteProjectileAmount

```python
AbsoluteProjectileAmount(Modifier):
    amount : float
```

Increases the projectile amount of `ShootProjectile` abilities by a fixed value. The total amount is limited by the `max_projectiles` member in `ShootProjectile`.

**amount**
Amount of projectiles added.

## modifier.type.AoE2ProjectileAmount

```python
AoE2ProjectileAmount(Modifier):
    provider_abilities : set(ApplyDiscreteEffect)
    receiver_abilities : set(ApplyDiscreteEffect)
    change_types       : set(children(AttributeChangeType))
```

Compares the raw change value of two sets of `ApplyDiscreteEffect` abilities. The final amount is calculated by using this formula:

```math
amount = (change\_value\_provider / reload\_time\_provider) / change\_value\_receiver
```

**provider_abilities**
The abilities of the *provider*, i.e. the game entity that provides the projectile amount to a receiving game entity. These abilities do not have to be assigned to a game entity.

**receiver_abilities**
The abilities of the *receiver*, i.e. the game entity that has its projectile amount changed. These abilities do not have to be assigned to a game entity.

**change_types**
The change types of the abilities that are considered as `AttributeChangeType` objects.

## modifier.type.AttributeSettingsValue

```python
AttributeSettingsValue(Modifier):
    attribute : Attribute
```

Changes the `starting_value` **and** `max_value` members of an `AttributeSettings` object in the `Live` ability.

**attribute**
`AttributeSettings` objects with this attribute are considered.

## modifier.type.EntityContainerCapacity

```python
EntityContainerCapacity(Modifier):
    container : EntityContainer
```

Changes the `size` member of an `EntityContainer` object in a `Storage` ability. Resulting values are floored.

**container**
The container which is considered.

## modifier.type.ContinuousResource

```python
ContinuousResource(Modifier):
    rates : set(ResourceRate)
```

Provides a continuous trickle of resources while the modifier is enabled.

**rates**
The resource rates as `ResourceRate` objects.

## modifier.type.CreationAttributeCost

```python
CreationAttributeCost(Modifier):
    attributes : set(Attribute)
    creatables : set(CreatableGameEntity)
```

Changes the attribute amount of `AttributeCost` objects in `CreatableGameEntity` objects.

**attributes**
Limits the modifier to `AttributeAmount` objects referencing attributes from this set.

**creatables**
`CreatableGameEntity` objects that are considered.

## modifier.type.CreationResourceCost

```python
CreationResourceCost(Modifier):
    resources  : set(Resource)
    creatables : set(CreatableGameEntity)
```

Changes the resource amount of `ResourceCost` objects in `CreatableGameEntity` objects.

**resources**
Limits the modifier to `ResourceAmount` objects referencing resources from this set.

**creatables**
These `CreatableGameEntity` objects are considered.

## modifier.type.CreationTime

```python
CreationTime(MultiplierModifier):
    creatables : set(CreatableGameEntity)
```

Changes the `creation_time` member of a `CreatableGameEntity` object in the `Create` ability.

**creatables**
`CreatableGameEntity` objects that are considered.

## modifier.type.DepositResourcesOnProgress

```python
DepositResourcesOnProgress(Modifier):
    progress_status      : ProgressStatus
    resources            : set(Resource)
    affected_types       : set(children(GameEntityType))
    blacklisted_entities : set(GameEntity)
```

Deposits resources of a game entity into the players resource pool if the game entity was involved in advancing the progress via `TimeRelativeProgressChange` effects.

**progress_status**
Progress status at which the resources are deposited. The `TimeRelativeProgress` effect must be applied at the exact same time as the progress is reached.

**resources**
Resources that are dropped off.

**affected_types**
Whitelist of game entity types that can trigger the deposit action when the progress status is reached. The game entities do not need a `DropSite` ability.

**blacklisted_game_entities**
Blacklist for specific game entities that would be covered by `affected_types`, but should be excplicitly excluded.

## modifier.type.DiplomaticLineOfSight

```python
DiplomaticLineOfSight(Modifier):
    diplomatic_stance : children(DiplomaticStance)
```

Activates line of sight for game entities of players with the specified diplomatic stance are also visible to owner of the modifier.

**diplomatic_stance**
Players with these stances share ther line of sight with the modifier owner.

## modifier.type.GatheringEfficiency

```python
GatheringEfficiency(Modifier):
    resource_spot : ResourceSpot
```

Changes the amount of resources that are removed from a specific resource spot's resource amount while gathering.

*Example*: Consider a gold resource spot containing 100 gold and a game entity with a `GatheringEfficiency` modifier for this resource spot with multiplier `0.8`. For an amount of 10 gold that the game entity gathers, the resource spot will remove only `0.8` times this amount, i.e. the resource spot only loses 8 gold. This effectively increases the yield of the resource spot to 125 gold for the game entity.

**resource_spot**
Resource spot for which the efficiency is changed.

## modifier.type.GatheringRate

```python
GatheringRate(Modifier):
    resource_spot : ResourceSpot
```

Changes the gathering rate in a game entity's `Gather` ability for a specific resource spot.

**resource_spot**
Resource spot for which the gathering rate is changed.

## modifier.type.InContainerContinuousEffect

```python
InContainerContinuousEffect(Modifier):
    containers : set(EntityContainer)
    ability    : ApplyContinuousEffect
```

Makes a game entity apply continuous effects on itself while stored in specified containers.

**containers**
The containers where the continuous effects are applied.

**ability**
Ability that is used to apply the effects. It does not have to be an ability of the game entity. Other modifiers of the game entity apply while using the ability.

## modifier.type.InContainerDiscreteEffect

```python
InContainerDiscreteEffect(Modifier):
    containers : set(EntityContainer)
    ability    : ApplyDiscreteEffect
```

Makes a game entity apply discrete effects on itself while stored in specified containers.

**containers**
The containers where the discrete effects are applied.

**ability**
Ability that is used to apply the effects. It does not have to be an ability of the game entity. Other modifiers of the game entity apply while using the ability.

## modifier.type.InstantTechResearch

```python
InstantTechResearch(Modifier):
    tech      : Tech
    condition : set(LogicElement)
```

Instantly unlocks a `Tech` and applies its patches when the condition is fulfilled.

**tech**
Technology that is researched.

**condition**
Condition that need to be fulfilled to trigger the research.

## modifier.type.MoveSpeed

```python
MoveSpeed(MultiplierModifier):
    pass
```

Changes the `move_speed` member of the `Move` ability.

## modifier.type.RefundOnCondition

```python
RefundOnCondition(Modifier):
    refund_amount : set(ResourceAmount)
    condition     : set(LogicElement)
```

Returns a fixed amount of resources back to the player after the condition has been fulfilled.

**refund_amount**
Amount of resources that are added to the player's resource pool.

**condition**
Condition that triggers the refund.

## modifier.type.ReloadTime

```python
ReloadTime(Modifier):
    pass
```

Changes the `reload_time` member of `ApplyDiscreteEffect` and `ShootProjectile` abilities.

## modifier.type.ResearchAttributeCost

```python
ResearchAttributeCost(Modifier):
    attributes    : set(Attribute)
    researchables : set(CreatableGameEntity)
```

Changes the attribute amount of `AttributeCost` objects in `ResearchableTech` objects.

**attributes**
Limits the modifier to `AttributeAmount` objects referencing `Attribute` objects from this set.

**researchables**
`ResearchableTech` objects that are considered.

## modifier.type.ResearchResourceCost

```python
ResearchResourceCost(Modifier):
    resources     : set(Resource)
    researchables : set(ResearchableTech)
```

Changes the resource amount of `ResourceCost` objects in `ResearchableTech` objects.

**resources**
Limits the modifier to `ResourceAmount` objects referencing `Resource` objects from this set.

**researchables**
`ResearchableTech` objects that are considered.

## modifier.type.ResearchTime

```python
ResearchTime(Modifier):
    researchables : set(ResearchableTech)
```

Changes the `research_time` member of a `ResearchableTech` object in the `Research` ability.

**researchables**
`ResearchableTech` objects that are considered.

## modifier.type.Reveal

```python
Reveal(Modifier):
    line_of_sight        : float
    affected_types       : set(children(GameEntityType))
    blacklisted_entities : set(GameEntity)
```

Reveals an area around specified game entities.

**line_of_sight**
Radius of the visible area around the game entity.

**affected_types**
Whitelist of game entity types that the modifier should apply to.

**blacklisted_entities**
Blacklist for specific game entities that would be covered by `affected_types`, but should be excplicitly excluded.

## modifier.type.StorageElementCapacity

```python
StorageElementCapacity(Modifier):
    storage_element : StorageElementDefinition
```

Changes the `elements_per_slot` member of a `StorageElementDefinition` object in a container. Resulting values are floored.

**storage_element**
Storage element which is considered.

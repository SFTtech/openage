# Reference

Reference documentation of the `engine.modifier` module of the openage modding API.

## modifier.Modifier

```python
Modifier(Entity):
```

Generalization object for all modifiers. Modifiers change the behavior of abilities at for general and edge cases. They can influence more than one ability at a time.

Standard behavior without specializations:

* Modifiers in the `modifiers` set of `GameEntity` are considered enabled as soon as the game entity is created, unless a `StateChager` object disables them.
* If the modifier is assigned to a `GameEntity` object, the modifications only apply to this game entity.
* If the modifier is assigned to a `Civilization` object, the modifications apply to all game entities of this civilization.
* Modifiers stack by default.

Specializations:

* `ScopeModifier`: Instead of using standard behavior, the modifier is applied to a defined set of game entities.

## modifier.specialization.ScopeModifier

```python
ScopeModifier(Modifier):
    diplomatic_stances : set(DiplomaticStance)
    scope              : ModifierScope
```

Overrides the standard behavior of modifiers and applies the modifier to a defined set of game entities. The modifiers affect the game entities as long as it stays enabled for the game entity that it has been assigned to.

**diplomatic_stances**
Applies the modifiers to players that the owner of the game entity has these diplomatic stances with.

**scope**
Defines the game entities affected by this modifier.

## modifier.multiplier.MultiplierModifier

```python
MultiplierModifier(Modifier):
    multiplier : float
```

A subclass of modifiers that act as multipliers for specific values of abilities.

**multiplier**
Multiplier for a value. Which value is multiplied depends on the specific type of `MultiplierModifier`.

## modifier.multiplier.effect.EffectMultiplierModifier

```python
EffectMultiplierModifier(MultiplierModifier):
```

A subclass of `MultiplierModifier` that handles multipliers for `Effect` objects.

## modifier.multiplier.effect.flat_attribute_change.FlatAtttributeChangeModifier

```python
FlatAtttributeChangeModifier(EffectMultiplierModifier):
```

A subclass of `EffectMultiplierModifier` that handles multipliers for `FlatAtttributeChange` effects.

## modifier.multiplier.effect.flat_attribute_change.type.ElevationDifferenceHigh

```python
ElevationDifferenceHigh(FlatAtttributeChangeModifier):
    elevation_difference : float
```

Applies a multiplier on the cumulated *change value* of `FlatAtttributeChange` effects when the effector containing this modifier is located *higher* than the targeted resistor.

**elevation_difference**
The minimum elevation difference between effector and resistor.

## modifier.multiplier.effect.flat_attribute_change.type.Flyover

```python
Flyover(FlatAtttributeChangeModifier):
    flyover_types             : set(GameEntityType)
    blacklisted_game_entities : set(GameEntity)
```

Applies a multiplier on the cumulated *applied change value* of `FlatAtttributeChange` effects of a projectile's attack if the projectile path went over specified game entity types.

**flyover_types**
Whitelist of game entity types that must be under the patch of the projectile. The game entities must have the `Hitbox` ability.

**blacklisted_game_entities**
Blacklist for specific game entities that would be covered by `flyover_types`, but should be excplicitely excluded.

## modifier.multiplier.effect.flat_attribute_change.type.Terrain

```python
Terrain(FlatAtttributeChangeModifier):
    terrain : Terrain
```

Applies a multiplier on the cumulated *applied change value* of `FlatAtttributeChange` effects when the target game entity is on a specified terrain.

**terrain**
The terrain the targeted game entity must stand on.

## modifier.multiplier.effect.flat_attribute_change.type.Unconditional

```python
Unconditional(FlatAtttributeChangeModifier):
```

Applies a multiplier on the cumulated *applied change value* of `FlatAtttributeChange` effects without any conditions.

## modifier.multiplier.effect.type.TimeRelativeAttributeChangeTime

```python
TimeRelativeAttributeChangeTime(EffectMultiplierModifier):
```

Applies a multiplier on the `total_change_time` member of `TimeRelativeAttributeChange` effects.

## modifier.multiplier.effect.type.TimeRelativeProgressTime

```python
TimeRelativeProgressTime(EffectMultiplierModifier):
```

Applies a multiplier on the `total_change_time` member of `TimeRelativeProgress` effects.

## modifier.multiplier.resistance.ResistanceMultiplierModifier

```python
ResistanceMultiplierModifier(MultiplierModifier):
```

A subclass of `MultiplierModifier` that handles multipliers for `Resistance` resistances.

## modifier.multiplier.resistance.flat_attribute_change.FlatAtttributeChangeModifier

```python
FlatAtttributeChangeModifier(ResistanceMultiplierModifier):
```

A subclass of `ResistanceMultiplierModifier` that handles multipliers for `FlatAtttributeChange` resistances.

## modifier.multiplier.resistance.flat_attribute_change.type.ElevationDifferenceLow

```python
ElevationDifferenceLow(FlatAtttributeChangeModifier):
    elevation_difference : float
```

Applies a multiplier on the cumulated *change value* of `FlatAtttributeChange` resistances when the resistor containing this modifier is located *lower* than the effector.

**elevation_difference**
The minimum elevation difference between effector and resistor.

## modifier.multiplier.resistance.flat_attribute_change.type.Stray

```python
Stray(FlatAtttributeChangeModifier):
```

Applies a multiplier on the cumulated *applied change value* of `FlatAtttributeChange` resistances for a projectile when the resistor was not the intended target.

## modifier.multiplier.resistance.flat_attribute_change.type.Terrain

```python
Terrain(FlatAtttributeChangeModifier):
    terrain : Terrain
```

Applies a multiplier on the cumulated *applied change value* of `FlatAtttributeChange` resistances when the resisting game entity is on a specified terrain.

**terrain**
The terrain the game entity must stand on.

## modifier.multiplier.resistance.flat_attribute_change.type.Unconditional

```python
Unconditional(FlatAtttributeChangeModifier):
```

Applies a multiplier on the cumulated *applied change value* of `FlatAtttributeChange` resistances without any conditions.

## modifier.multiplier.type.AttributeSettingsValue

```python
AttributeSettingsValue(MultiplierModifier):
    attribute : Attribute
```

Applies a multiplier on the `starting_value` **and** `max_value` members of an `AttributeSettings` object in the `Live` ability.

**attribute**
`AttributeSettings` objects with this attribute are considered.

## modifier.multiplier.type.ContainerCapacity

```python
ContainerCapacity(MultiplierModifier):
    container : Container
```

Applies a multiplier to the `size` member of a `Container` object in a `Storage` ability. Resulting values are floored.

**container**
The container which is considered.

## modifier.multiplier.type.CreationAttributeCost

```python
CreationAttributeCost(MultiplierModifier):
    attributes : set(Attribute)
    creatables : set(CreatableGameEntity)
```

Applies a multiplier on the attribute amount of `AttributeCost` objects in `CreatableGameEntity` objects.

**attributes**
Limits the modifier to `AttributeAmount` objects referencing attributes from this set.

**creatables**
These `CreatableGameEntity` objects are considered.

## modifier.multiplier.type.CreationResourceCost

```python
CreationResourceCost(MultiplierModifier):
    resources  : set(Resource)
    creatables : set(CreatableGameEntity)
```

Applies a multiplier on the resource amount of `ResourceCost` objects in `CreatableGameEntity` objects.

**resources**
Limits the modifier to `ResourceAmount` objects referencing resources from this set.

**creatables**
These `CreatableGameEntity` objects are considered.

## modifier.multiplier.type.CreationTime

```python
CreationTime(MultiplierModifier):
    creatables : set(CreatableGameEntity)
```

Applies a multiplier on the `creation_time` member of a `CreatableGameEntity` object in the `Create` ability.

**creatables**
These `CreatableGameEntity` objects are considered.

## modifier.multiplier.type.GatheringEfficiency

```python
GatheringEfficiency(MultiplierModifier):
    resource_spot : ResourceSpot
```

Applies a multiplier to the amount of resources that are removed from a specific resource spot's resource amount while gathering.

*Example*: Consider a gold resource spot containing 100 gold and a game entity with a `GatheringEfficiency` modifier for this resource spot with multiplier `0.8`. For an amount of 10 gold that the game entity gathers, the resource spot will remove only `0.8` times this amount, i.e. the resource spot only loses 8 gold. This effectively increases the yield of the resource spot to 125 gold for the game entity.

**resource_spot**
The resource spot for which the effiency multiplier is applied.

## modifier.multiplier.type.GatheringRate

```python
GatheringRate(MultiplierModifier):
    resource_spot : ResourceSpot
```

Applies a multiplier to the amount of resources that are gathered from a specific resource spot.

**resource_spot**
The resource spot for which the rate multiplier is applied.

## modifier.multiplier.type.MoveSpeed

```python
MoveSpeed(MultiplierModifier):
```

Applies a multiplier on the `move_speed` member of the `Move` ability.

## modifier.multiplier.type.ReloadTime

```python
ReloadTime(MultiplierModifier):
```

Applies a multiplier on the `reload_time` member of `ApplyDiscreteEffect` and `ShootProjectile` abilities.

## modifier.multiplier.type.ResearchAttributeCost

```python
ResearchAttributeCost(MultiplierModifier):
    attributes    : set(Attribute)
    researchables : set(CreatableGameEntity)
```

Applies a multiplier on the attribute amount of `AttributeCost` objects in `ResearchableTech` objects.

**attributes**
Limits the modifier to `AttributeAmount` objects referencing attributes from this set.

**researchables**
These `ResearchableTech` objects are considered.

## modifier.multiplier.type.ResearchResourceCost

```python
ResearchResourceCost(MultiplierModifier):
    resources     : set(Resource)
    researchables : set(ResearchableTech)
```

Applies a multiplier on the resource amount of `ResourceCost` objects in `ResearchableTech` objects.

**resources**
Limits the modifier to `ResourceAmount` objects referencing resources from this set.

**researchables**
These `ResearchableTech` objects are considered.

## modifier.multiplier.type.ResearchTime

```python
ResearchTime(MultiplierModifier):
    researchables : set(ResearchableTech)
```

Applies a multiplier on the `research_time` member of a `ResearchableTech` object in the `Research` ability.

**researchables**
These `ResearchableTech` objects are considered.

## modifier.multiplier.type.StorageElementCapacity

```python
StorageElementCapacity(MultiplierModifier):
    storage_element : StorageElement
```

Applies a multiplier to the `elements_per_slot` member of a `StorageElement` object in a container. Resulting values are floored.

**storage_element**
The storage element which is considered.

## modifier.relative_projectile_amount.AoE2ProjectileAmount

```python
AoE2ProjectileAmount(Modifier):
    provider_abilities : set(ApplyDiscreteEffect)
    receiver_abilities : set(ApplyDiscreteEffect)
    change_types       : set(AttributeChangeType)
```

Compares the raw change value of two sets of `ApplyDiscreteEffect` abilities. The final amount is calculated by using this formula:

```
amount = (change_value_provider / reload_time_provider) / change_value_receiver
```

**provider_abilities**
The abilities of the *provider*, i.e. the game entity that provides the projectile amount to a receiving game entity. These abilities do not have to be assigned to a game entity.

**receiver_abilities**
The abilities of the *receiver*, i.e. the game entity that has its projectile amount changed. These abilities do not have to be assigned to a game entity.

**change_types**
The change types of the abilities that are considered as `AttributeChangeType` objects.

## modifier.relative_projectile_amount.type.

```python
RelativeProjectileAmountModifier(Modifier):
```

Generalization object for modifiers that change the projectile amount by comparing two sets of values.

## modifier.type.AbsoluteProjectileAmount

```python
AbsoluteProjectileAmount(Modifier):
    amount : float
```

Increases the projectile amount of `ShootProjectile` abilities by a fixed value. The total amount is limited by the `max_projectiles` member in `ShootProjectile`.

**amount**
The amount of projectiles added.

## modifier.type.AlwaysHerd

```python
AlwaysHerd(Modifier):
```

The game entity will always herd a herdable game entity in in range, even if other game entities with `Herd` abilities are closer. If two game entities with this modifier are in range, the closest one of them will herd the herdable.

## modifier.type.ContinuousResource

```python
ContinuousResource(Modifier):
    rates : set(ResourceRate)
```

Provides a continuous trickle of resources while the modifier is enabled.

**rates**
The resource rates as `ResourceRate` objects.

## modifier.type.DepositResourcesOnProgress

```python
DepositResourcesOnProgress(Modifier):
    progress_status      : ProgressStatus
    resources            : set(Resource)
    affected_types       : set(GameEntityType)
    blacklisted_entities : set(GameEntity)
```

Deposits resources of a game entity into the players resource pool if the game entity was involved in advancing the progress via `TimeRelativeProgress` effects.

**progress_status**
Progress status at which the resources are deposited. The `TimeRelativeProgress` effect must be applied at the exact same time as the progress is reached.

**resources**
Resources that are dropped off.

**affected_types**
Whitelist of game entity types that can trigger the deposit action when the progress status is reached. The game entities do not need a `DropSite` ability.

**blacklisted_game_entities**
Blacklist for specific game entities that would be covered by `affected_types`, but should be excplicitely excluded.

## modifier.type.DiplomaticLineOfSight

```python
DiplomaticLineOfSight(Modifier):
    diplomatic_stance : DiplomaticStance
```

Activates line of sight for game entities of players that the owner of the game entity has a specified diplomatic stance towards.

**diplomatic_stance**
Stance towards other players which should be visible.

## modifier.type.InContainerContinuousEffect

```python
InContainerContinuousEffect(Modifier):
    containers : set(Container)
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
    containers : set(Container)
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
    tech         : Tech
    requirements : set(AvailabilityRequirement)
```

Instantly researches a `Tech` and applies its patches for a player when the listed requirements are fulfilled.

**tech**
The technology that is researched.

**requirements**
Requirements that need to be fulfilled to trigger the research.

## modifier.type.RefundOnDeath

```python
RefundOnDeath(Modifier):
    refund_amount : set(ResourceAmount)
```

Returns a fixed amount of resources back to the player after the game entity has despawned with a `Despawn` ability.

**refund_amount**
Amount of resources that are added to the player's resource pool.

## modifier.type.Reveal

```python
Reveal(Modifier):
    line_of_sight        : float
    affected_types       : set(GameEntityType)
    blacklisted_entities : set(GameEntity)
```

Reveals an area around specified game entities.

**line_of_sight**
The radius of the visible area around the game entity.

**affected_types**
Whitelist of game entity types that the modifier should apply to.

**blacklisted_game_entities**
Blacklist for specific game entities that would be covered by `affected_types`, but should be excplicitely excluded.

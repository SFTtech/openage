# Reference

## Entity

```
Entity():
```

Root object of the API. All other objects inherit from it, either explicitely or implicitely.

## effect.Effect

```python
Effect(Entity):
```

Generalization object for all effects.

Standard behavior without specializations:

* Effects are **only applied** if the resistor has a **matching** `Resistance` object. Matching can be further defined depending on the concrete effect.
* Applying them **costs nothing**.
* Will be applied to targets **with any diplomatic stance**.
* Only affect the target chosen with the `ApplyContinuousEffect`/`ApplyDiscretEffect` ability.

Specializations:

* `AreaEffect`: Effects are applied to other game entities in a circular area around the target.
* `CostEffect`: Makes effects cost attribute points or resources.
* `DiplomaticEffect`: Effects only apply to specified diplomatic stances.

## effect.continuous.ContinuousEffect

```python
ContinuousEffect(Effect):
```

Generalization object for effects that are applied at a per-second rate.


## effect.continuous.flat_attribute_change.FlatAttributeChange

```python
FlatAttributeChange(ContinuousEffect):
    type              : AttributeChangeType
    min_change_rate   : optional(AttributeRate)
    max_change_rate   : optional(AttributeRate)
    change_rate       : AttributeRate
    ignore_protection : set(ProtectingAttribute)
```

Generalization object for effects that change the resistor's current attribute values at a flat per-second rate. The change value can optionally be limited to an interval with `min_change_rate` as the lower bound and `max_change_rate` as the upper bound.

Note that you cannot use this effect object directly and have to choose one of the specializations `FlatAttributeChangeDecrease` or `FlatAttributeChangeIncrease`.

**type**
The effect will be matched with a `Resistance.ContinuousResistance.FlatAttributeChange` object that stores the same `AttributeChangeType` object in its `type` member. Otherwise, the effect will not be applied.

**min_change_rate (optional)**
The applied change rate can never go lower than the specified rate. Does not have to be set.

**max_change_rate (optional)**
The applied change rate can never go higher than the specified rate. Does not have to be set.

**change_rate**
The gross per-second rate at which the attribute points of the resistor change. The net change rate (applied rate) is calculated by subtracting the effector's `change_rate` from the resistor's `block_rate`.

```math
applied_rate = change_rate - block_rate
```

The applied rate is further bound by the interval defined by `min_change_rate` and `max_change_rate`, if these members are set.

**ignore_protection**
Ignores the `ProtectingAttribute`s in the set when changing the attributes of the target.

## effect.continuous.flat_attribute_change.type.FlatAttributeChangeDecrease

```python
FlatAttributeChangeDecrease(FlatAttributeChange):
```

Specialization of the continuous `FlatAttributeChange` effect that decreases the resistor's current attribute value at a per-second rate.


## effect.continuous.flat_attribute_change.type.FlatAttributeChangeIncrease

```python
FlatAttributeChangeIncrease(FlatAttributeChange):
```

Specialization of the continuous `FlatAttributeChange` effect that increases the resistor's current attribute value at a per-second rate.

## effect.continuous.type.Lure

```python
Lure(ContinuousEffect):
    type                        : LureType
    destination                 : set(GameEntity)
    min_distance_to_destination : float
```

Makes the target move to another game entity.

**type**
The effect will be matched with a `Resistance.ContinuousResistance.Lure` object that stores the same `LureType` object in its `type` member. Otherwise, the effect will not be applied.

**destination**
The possible destinations the target can move to. Whichever game entity is closest will be chosen.

**min_range_to_destination**
The minimum distance to the destination the target has to have before it will stop.

## effect.continuous.time_relative_attribute_change.TimeRelativeAttributeChange

```python
TimeRelativeAttributeChange(ContinuousEffect):
    type              : AttributeChangeType
    total_change_time : float
    ignore_protection : set(ProtectingAttribute)
```

Generalization object for effects that change the resistor's current attribute values at a  per-second rate relative to the `max_value` of their attribute settings. The change rate is scaled such that it would increase the attribute points of the resistor from 0 to `max_value` (or decrease them from `max_value` to 0, respectively) in a fixed amount of time. The current attribute points are not considered. Calculating the change rate can be done by using this formula:

```
applied_rate = resistor_max_value / total_change_time
```

*Example*: Consider a resistor with 50 maximum HP and an effector with a `TimeRelativeAttributeDecrease` time of 5 seconds. The per-second change rate is calculated by dividing the maximum HP value by the time requirement of the effect. Hence, the change rate is 10HP/s. This rate is fix as long as the maximum HP value does not change. If the resistor currently has 30 HP, it would arrive at 0 HP in 3 seconds.

**type**
The effect will be matched with a `Resistance.ContinuousResistance.TimeRelativeAttributeChange` object that stores the same `AttributeChangeType` object in its `type` member. Otherwise, the effect will not be applied.

**total_change_time**
The total time needed to change the resistors attribute points from `max_value` to 0 (for `TimeRelativeAttributeDecrease`) or from 0 to `max_value` (for `TimeRelativeAttributeIncrease`).

**ignore_protection**
Ignores the `ProtectingAttribute`s in the set when changing the attributes of the target.

## effect.continuous.time_relative_attribute_change.type.TimeRelativeAttributeDecrease

```python
TimeRelativeAttributeDecrease(TimeRelativeAttributeChange):
```

Specialization of the continuous `TimeRelativeAttributeChange` effect that decreases the resistor's current attribute value in a fixed amount of time relative to their attribute's `max_value`.

## effect.continuous.time_relative_attribute_change.type.TimeRelativeAttributeIncrease

```python
TimeRelativeAttributeIncrease(TimeRelativeAttributeChange):
```

Specialization of the continuous `TimeRelativeAttributeChange` effect that increases the resistor's current attribute value in a fixed amount of time relative to their attribute's `max_value`.

## effect.continuous.time_relative_progress.TimeRelativeProgress

```python
TimeRelativeProgress(ContinuousEffect):
    type              : ProgressType
    total_change_time : float
```

Generalization object for effects that changes a resistor's current progress amount at a per-second rate relative to 100%. The change rate is scaled such that it would increase the specified progress amount of the resistor from 0% to 100% (or decrease it from 100% to 0%, respectively) in a fixed amount of time. The current progress amount is not considered. Calculating the change rate can be done by using this formula:

```
applied_rate = 100 / total_change_time
```

*Example*: Consider a constructable resistor and an effector with a `TimeRelativeProgressIncrease` time of 10 seconds. The per-second change rate is calculated by dividing 100% by the time requirement of the effect. Hence, the change rate is 10%/s. This rate is fix. If the resistor currently has 30% construction progress, it would be constructed in 7 seconds.

**type**
The effect will be matched with a `Resistance.ContinuousResistance.TimeRelativeProgress` object that stores the same `ProgressType` object in its `type` member. Otherwise, the effect will not be applied.

**total_change_time**
The total time needed to change the resistors attribute points from 100% to 0 (for `TimeRelativeProgressDecrease`) or from 0 to 100% (for `TimeRelativeProgressIncrease`).

## effect.continuous.time_relative_progress.type.TimeRelativeProgressDecrease

```python
TimeRelativeProgressDecrease(TimeRelativeProgress):
```

Specialization of the continuous `TimeRelativeProgress` effect that decreases the resistor's progress amount in a fixed amount of time relative to 100%.

## effect.continuous.time_relative_progress.type.TimeRelativeProgressIncrease

```python
TimeRelativeProgressIncrease(TimeRelativeProgress):
```

Specialization of the continuous `TimeRelativeProgress` effect that increases the resistor's progress amount in a fixed amount of time relative to 100%.
´
## effect.discrete.DiscreteEffect

```python
DiscreteEffect(Effect):
```

Generalization object for effects that are applied immediately.

## effect.discrete.convert.Convert

```python
Convert(DiscreteEffect):
    type               : ConvertType
    min_chance_success : optional(float)
    max_chance_success : optional(float)
    chance_success     : float
    cost_fail          : optional(Cost)
```

Change the owner of the target unit to the player who owns the effector game entity.

**type**
The effect will be matched with a `Resistance.DiscreteResistance.Convert` object that stores the same `ConvertType` object in its `type` member. Otherwise, the effect will not be applied.

**min_chance_success (optional)**
The applied chance can never go lower than the specified percentage. Does not have to be set.

**max_chance_success (optional)**
The applied chance can never go higher than the specified percentage. Does not have to be set.

**chance_success**
Gross chance for the conversion to succeed as a percentage chance. The percentage should be stored as a float value between *0.0* and *1.0*. The net chance (applied chance) of success is calculated by subtracting the effector's `chance_success` from the resistor's `chance_resist`.

```math
applied_chance = chance_success - chance_resist
```

Any value below *0.0* is an automatic fail, while any value above *1.0* is an automatic success. The applied chance is further bound by the interval defined by `min_chance_success` and `max_chance_success`, if these members are set.

**cost_fail**
The amount of attribute points or resources removed from the effector if the conversion fails. Does not have to be set.

## effect.discrete.convert.type.AoE2Convert

```python
AoE2Convert(Convert):
    skip_guaranteed_rounds : int
    skip_protected_rounds  : int
```

Specialized conversion effect that is implemented in AoE2. The convert chance at the start is guaranteed to be *0.0* for X rounds and guaranteed to be *1.0* after Y rounds (both defined by the `Restistance.DiscreteResistance.Convert.AoE2Convert` object).

When the effector stops applying the effect, the resistor's protected rounds are increased until they reach their maximum value again. Running out of the range of the effector does not count as stopping the effect application.

**skip_guaranteed_rounds**
Lowers the number of rounds that the resistor is guaranteed to resist every time.

**skip_protected_rounds**
Lowers the number of rounds that are needed for the success chance to always be *1.0*.

## effect.discrete.flat_attribute_change.FlatAttributeChange

```python
FlatAttributeChange(DiscreteEffect):
    type              : AttributeChangeType
    min_change_rate   : optional(AttributeAmount)
    max_change_rate   : optional(AttributeAmount)
    change_rate       : AttributeAmount
    ignore_protection : set(ProtectingAttribute)
```

Generalization object for effects that change the resistor's current attribute values by a flat amount. The change value can optionally be limited to an interval with `min_change_value` as the lower bound and `max_change_value` as the upper bound.

Note that you cannot use this effect object directly and have to choose one of the specializations `FlatAttributeChangeDecrease` or `FlatAttributeChangeIncrease`.

**type**
The effect will be matched with a `Resistance.DiscreteResistance.FlatAttributeChange` object that stores the same `AttributeChangeType` object in its `type` member. Otherwise, the effect will not be applied.

**min_change_value (optional)**
The applied change value can never go lower than the specified amount. Does not have to be set.

**max_change_value (optional)**
The applied change value can never go higher than the specified amount. Does not have to be set.

**change_value**
The gross amount by that the attribute points of the resistor change. The net change value (applied value) is calculated by subtracting the effector's `change_value` from the resistor's `block_value`.

```math
applied_value = change_value - block_value
```

The applied rate is further bound by the interval defined by `min_change_value` and `max_change_value`, if these members are set.

**ignore_protection**
Ignores the `ProtectingAttribute`s in the set when changing the attributes of the target.

## effect.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease

```python
FlatAttributeChangeDecrease(FlatAttributeChange):
```

Specialization of the discrete `FlatAttributeChange` effect that decreases the resistor's current attribute value by a flat amount.


## effect.discrete.flat_attribute_change.type.FlatAttributeChangeIncrease

```python
FlatAttributeChangeIncrease(FlatAttributeChange):
```

Specialization of the discrete `FlatAttributeChange` effect that increases the resistor's current attribute value by a flat amount.

## effect.discrete.type.MakeHarvestable

```python
MakeHarvestable(DiscreteEffect):
    resource_spot : ResourceSpot
```

Makes a resource spot harvestable, if it is not already harbestable by default.

**resource_spot**
Resource spot that should be made harvestable. The effect will be matched with a `Resistance.DiscreteResistance.MakeHarvestable` object that stores the same `ResourceSpot` object in its `resource_spot` member. Additionally, the target needs to have a `Harvestable` ability that contains the resource spot.

## effect.discrete.type.SendToContainer

```python
SendToContainer(DiscreteEffect):
    type     : SendToStorageType
    storages : set(Container)
```

Makes the target move to and enter the nearest game entity where it can be stored. The resistor needs an `EnterContainer` ability for at least one of the containers for this to work.

**type**
The effect will be matched with a `Resistance.DiscreteResistance.SendToStorage` object that stores the same `SendToStorageType` object in its `type` member. Otherwise, the effect will not be applied.

**storages**
Set of containers the target should enter. The target will choose the nearest one for which it has an `EnterContainer` ability.

## effect.specialization.AreaEffect

```python
AreaEffect(Effect):
    range   : float
    dropoff : DropoffType
```

Can be inherited to apply the effect to game entities in a circular area around the target.

**range**
The radius of the area in which game entities are affected.

**dropoff**
Changes the effectiveness of the effect based on the distance to the center of the circle (see `DropoffType`).

## effect.specialization.CostEffect

```python
AttributeCostEffect(Effect):
    cost : Cost
```

Can be inherited to make the effect cost attribute points or resources.

**cost**
The costs for the effect as a `Cost` object.

## effect.specialization.DiplomaticEffect

```python
DiplomaticEffect(Effect):
    stances : set(DiplomaticStance)
```

Can be inherited to make the effect only applicable to game entities of a player with a specified diplomatic stance.

**stances**
If the target is owned by a player has *any* of the specified diplomatic stances, the effect is applied.

## ability.Ability

```python
Ability(Entity):
```

Generalization object for all abilities. Abilities define what game entities can *do* and what they *are*, respectively. They can be considered passive and active traits.

Standard behavior without specializations:

* Abilities in the `abilities` set of `GameEntity` are considered enabled as soon as the game entity is created, unless a `StateChager` object disables them.
* No ability explicitely requires sounds or to be animated. For this purpose, ability specializations are used.
* A game entity's abilities are available to **all** players, not just the owner. This can be limited to players with specific diplomatic stances towards the owner by inheriting `DiplomaticAbility`.

Specializations:

* `AnimatedAbility`: Assigns an animation that is played while the ability is active.
* `AnimationOverrideAbility`: Overrides the animations of specified animated abilities temporarily.
* `CommandSoundAbility`: Assigns a sound that is played once when the player orders the game entity to use the ability.
* `DiplomaticAbility`: Makes the ability only accessible for players when the owner of the game entity has the specified diplomatic stances towards them.
* `ExecutionSoundAbility`: Assigns a sound that is played while the ability is active.

## ability.specialization.AnimatedAbility

```python
AnimatedAbility(Ability):
    animations : set(Animation)
```

Abilities of this type will play an animation while they are active. Whether the animation loops is defined in the `.sprite` file linked in the `Animation` object.

**animations**
The animation(s) played while the ability is active. If more than one animation is defined, the engine picks one of them at random.

## ability.specialization.AnimationOverrideAbility

```python
AnimationOverrideAbility(Ability):
    overrides : set(AnimationOverride)
```

Specifies a set of animation overrides that are applied when the ability is used.

*Usage example*: Consider a unit with two attacks. One of them is animated by having the unit wield a sword, while the other one is an animation in which the unit uses a bow. What we want is that the animation of the `Move` and `Idle` abilities correspond to the attack that was used last, e.g. after a sword attack, the movement animation show the unit moving around with a sword in its hand, and after a bow attack, the movement animation show the unit moving around with a bow. To accomplish this, we would let both attack abilities inherit from `AnimationOverrideAbility` and specify `AnimationOverride` objects for them that target `Move` and `Idle`.

**overrides**
Stores the animations and abilities that will be overriden temporarily as `AnimationOverride` objects.

## ability.specialization.CommandSoundAbility

```python
CommandSoundAbility(Ability):
    sounds : set(Sound)
```

Abilities of this type will play a sound once when the player orders the game entity to use them.

**sounds**
The sound(s) played when the order to use the ability is given. If more than one sound is defined, the engine picks one of them at random.

## ability.specialization.DiplomaticAbility

```python
DiplomaticAbility(Ability):
    stances : set(DiplomaticStance)
```

Restricts the players who can access the ability. Access is given to a player when the owner of the game entity has one of the specified stances towards this player. Note that when inheriting from this specilization, the access must also be explicitely allowed for the owner of the game entity by adding the `Self` stance to the set.

*Usage example*: Consider a trade post like the marketplace from AoE games. Players can send trade carts between trade posts to generate resources. Trade post behavior is activated by assigning the `TradePost` ability to a game entity. Without inheriting from `DiplomaticAbility`, all player can trade with this trade post, including its owner. However, if we want to limit the trade post to only be accessible for allies, we can let the ability inherit from `DiplomaticAbility` and add the `Allied` stance to the `stances` set.

**stances**
The stances the owner needs to have towards another player to make the ability accessible for the latter.

## ability.specialization.ExecutionSoundAbility

```python
ExecutionSoundAbility(Ability):
    sounds : set(Sound)
```

Abilities of this type will play a sound while they are active.

**sounds**
The sound(s) played while the ability is active. If more than one sound is defined, the engine picks one of them at random.

## ability.type.ApplyContinuousEffect

```python
ApplyContinuousEffect(Ability):
    effects                   : set(ContinuousEffect)
    application_delay         : float
    allowed_types             : set(GameEntityType)
    blacklisted_game_entities : set(GameEntity)
```

Applies a batch of continuous effects on another game entity.

**effects**
The applied continuous effect definitions.

**application_delay**
Time between the initiation of the ability and the application of the effects.

**allowed_types**
Whitelist of game entity types that can be targeted with the ability.

**blacklisted_game_entities**
Blacklist for specific game entities that would be covered by `allowed_types`, but should be excplicitely excluded.

## ability.type.ApplyDiscreteEffect

```python
ApplyDiscreteEffect(Ability):
    effects                   : set(ContinuousEffect)
    reload_time               : float
    application_delay         : float
    allowed_types             : set(GameEntityType)
    blacklisted_game_entities : set(GameEntity)
```

Applies a batch of discrete effects on another game entity.

**effects**
The applied discrete effect definitions.

**reload_time**
Minimum time between two applications of the batch of effects.

**application_delay**
Time between the initiation of the ability and first application of the effects.

**allowed_types**
Whitelist of game entity types that can be targeted with the ability.

**blacklisted_game_entities**
Blacklist for specific game entities that would be covered by `allowed_types`, but should be excplicitely excluded.

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
The set of game entities that can be inserted into the container. The container must define a corresponding `StorageElement` for the `GameEntity` objects.

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
A set of `ConstructionProgress` objects that can activate state changes and animation overrides when the construction progresses.

## ability.type.Create

```python
Create(Ability):
    creatables : set(CreatableGameEntity)
```

Allows a game entity to spawn new instances of game entities.

**creatables**
Stores the reference to the creatable game entities as well as the preconditions that have to be fulfilled in order to spawn a new instance.

## ability.type.Damageable

```python
Damageable(Ability):
    attribute       : Attribute
    damage_progress : set(DamageProgress)
```

Allows the alteration of the base state of the game entity when its attribute values increase or decrease.

**attribute**
The attribute which is monitored.

**damage_progress**
A set of `DamageProgress` objects that can activate state changes and animation overrides when the current attribute increases or decreases.

## ability.type.Deletable

```python
Deletable(Ability):
```

Makes the game entity deletable via manual command. This will trigger the currently active `Die` ability without the need to fulfill the conditions for death. If the game entity does not have an active `Die` ability, the engine will try to trigger `Despawn` instead. If this ability is also not present, the game entity is instantly removed from the game.

## ability.type.DepositResources

```python
DepositResources(Ability):
    search_range              : float
    allowed_types             : set(GameEntityType)
    blacklisted_game_entities : set(GameEntity)
```

Allows a game entity to dropoff resources at other game entities that have the `DropSite` ability. Game entities with the `Gather` ability will automatically use this ability when their gathering capacity is reached.

**search_range**
The range in which the unit will search for a resource drop site when the ability is not used manually.

**allowed_types**
Whitelist of game entity types that can be used as drop sites. They must have a `DropSite` ability accepting the corresponding resource.

**blacklisted_game_entities**
Blacklist for specific game entities that would be covered by `allowed_types`, but should be excplicitely excluded.

## ability.type.Despawn

```python
Despawn(Ability):
    despawn_condition : set(DespawnCondition)
    despawn_time      : float
    state_change      : StateChanger
```

Second stage of a "clean exit" that removes the game entity from the current game. The ability is can be used when a `Die` ability was executed by the same game entity or if the game entity has no `Die` ability. `Despawn` is triggered if at least one of the conditions in `despawn_conditions` is fulfilled. There can only be one `Despawn` ability active at the same time. If more than one `Despawn` abilities are enabled, the least recently enabled one of them is used.

**despawn_conditions**
Conditions that trigger the ability. Only one of them needs to be fulfilled. If the set contains no conditions, the game entity is despawned immediately.

**despawn_time**
Time until the game entity is removed from the game. The removal is permanent.

**state_change**
Alters the base abilities and modifiers of a game entity after the ability is executed. The state change stays active until the game entity is removed from the game.

## ability.type.Die

```python
Die(Ability):
    death_conditions : set(DeathCondition)
    death_time       : float
    state_change     : StateChanger
```

First stage of a "clean exit" that removes the game entity from the current game. The ability is triggered if at least one of the conditions in `death_conditions` is fulfilled. All current tasks of the game entity will be disabled. There can only be one `Die` ability active at the same time. If more than one `Die` abilities are enabled, the least recently enabled one of them is used.

After `Die` has finished, the `Despawn` ability is activated. `Despawn` must be triggered by another condition. Until then, the game entity will be assigned the 'dead' status by the engine. If no `Despawn` is present for the game entity, it will immediately be removed from the game.

`Die` can be combined with `ApplyDiscreteEffect` for an ability by inheriting from both API objects. The resulting ability can be used in either ways. If the player uses this ability as an `ApplyDiscreteEffect` ability, the game entity will die on use. Similarly, if one of the death conditions is triggered, effects will be applied. This can be used to model self destruct behavior.

**death_conditions**
Conditions that trigger the ability. Only one of them needs to be fulfilled. If the set contains no conditions, the game entity dies immediately.

**death_time**
Time until `Despawn` can be used.

**state_change**
Alters the base abilities and modifiers of a game entity after the ability is executed. The state change stays active until the game entity is removed from the game.

## ability.type.DropSite

```python
DropSite(Ability):
    accepts : set(Resource)
```

Allows a game entity to act as a resource dropoff site for game entities with the `DepositResources` ability.

**accepts**
Resources that can be dropped off at the game entity.

## ability.type.EnterContainer

```python
EnterContainer(Ability):
    allowed_containers        : set(Container)
    allowed_types             : set(GameEntityType)
    blacklisted_game_entities : set(GameEntity)
```

Allows a game entity to enter specified containers of another game entity's `Storage` ability.

**allowed_containers**
The containers that can be entered.

**allowed_types**
Whitelist of game entity types that can be targeted with the ability.

**blacklisted_game_entities**
Blacklist for specific game entities that would be covered by `allowed_types`, but should be excplicitely excluded.

## ability.type.ExchangeResources

```python
ExchangeResources(Ability):
    source_resources : set(ResourceAmount)
    target_resources : set(ResourceAmount)
    source_fee       : float
    target_fee       : float
    exchange_mode    : ExchangeMode
```

Exhanges a fixed amount of resources for another fixed amount of resources. Players can exchange resources with themselves or other players.

**source_resources**
Resource amount sent by the player initiating the ability to the targeted player (this is omitted if the player trades with themselves). This removes an amount of

`remove_amount = source_resources * source_fee`

from the player's resource pool. The resource amount must be available at the time of initiation.

**target_resources**
Resource amount removed from the targeted player's resource pool (this is omitted if the player trades with themselves). The resource amount must be available at the time of initiation. The initiating player receives an amount of

`receive_amount = target_resources * target_fee`

that is added to their resource pool.

**source_fee**
A multiplier for the amount of resources the initiating player has to spent.

**target_resources**
A multiplier for the amount of resources the initiating player receives.

**exchange_mode**
Configures advanced exchange behavior as an `ExchangeMode` object.

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
    flatten_ground     : bool
```

Changes the terrain under a game entity when it is created. Only works if a `TileRequirement` ability is enabled.

**foundation_terrain**
Replacement terrain that is placed on the tiles the game entity occupies.

**flatten_ground**
Determines whether the ground under the game entity is flattened when it is placed on it. The height level at the center of the game entity is used as orientation.

## ability.type.GameEntityStance

```python
GameEntityStance(Ability):
    stances: set(GameEntityStance)
```

Defines activity stances for a game entity. These stances define which abilities will be used without player interaction when the game entity is idle.

**stances**
Stance definitions for the game entity as `aux.game_entity_stance.GameEntityStance` objects.

## ability.type.Gate

```python
Gate(Ability):
    range                     : float
    allowed_types             : set(GameEntityType)
    blacklisted_game_entities : set(GameEntity)
```

Deactivates the hitbox of the game entity for movement of other game entities (like `Passable`) when specified game entities are in range. The hitbox is still relevant for the game entity's own movement.

**range**
Minimum distance to one of the specified game entities at which the game entity becomes passable.

**allowed_types**
Whitelist of game entity types that makes this game entity passable when they are in range.

**blacklisted_game_entities**
Blacklist for specific game entities that would be covered by `allowed_types`, but should be excplicitely excluded.

## ability.type.Gather

```python
Gather(Ability):
    auto_resume         : bool
    resume_search_range : float
    targets             : set(ResourceSpot)
    carry_capacity      : int
    gather_rate         : ResourceRate
    carry_progress      : set(CarryProgress)
```

Allows a game entity to gather from resource spots defined the `Harvestable` abilities of other game entities.

**auto_resume**
Determines whether the game entity will automatically resume gathering after the resource spot is depleted. When enabled, the game entity will check if it can refill the depleted resource spot with one of its `Restock` abilities. Otherwise, the game entity will search for a new resource spot it can access with this ability.

**resume_search_range**
The range in which the game entity searches for a new resource spot if `auto_resume` is enabled.

**targets**
Resource spots that can be accessed with this ability.

**carry_capacity**
Maximum carry capacity for resources gathered with this ability. If the stored amount of resources reaches this value or the resource spot is depleted, the game entity will use the `DropResources` ability.

**gather_rate**
The rate at which the game entity collects resources from the resource spot,

**carry_progress**
A set of `CarryProgress` objects that can activate state changes and animation overrides when the carried amount of resources increases.

## ability.type.Harvestable

```python
Harvestable(Ability):
    resources : ResourceSpot
```

Assigns a game entity a resource spot that can be harvested by other game entities with the `Gather` ability.

**resource_spot**
The resource spot as a `ResourceSpot` object. It defines the contained resource and capacity.

## ability.type.Herd

```python
Herd(Ability):
    range                     : float
    allowed_types             : set(GameEntityType)
    blacklisted_game_entities : set(GameEntity)
```

Allows a game entity to change the ownership of other game entities with the `Herdable` ability.

**range**
Minimum distance to a herdable game entity to make it change ownership.

**allowed_types**
Whitelist of game entity types that can be herded.

**blacklisted_game_entities**
Blacklist for specific game entities that would be covered by `allowed_types`, but should be excplicitely excluded.

## ability.type.Herdable

```python
Herdable(Ability):
    adjacent_discover_range : float
```

Makes the game entity switch ownership when it is in range of another game entity with a `Herd` ability. The new owner is the player who owns the game entity with the `Herd` ability. If the herdable game entity is in range of two or more herding game entities, the ownership changes to the owner of the game entity which is closest.

**adjacent_discover_range**
When the game entity is herded, other herdables in this range are also herded. The rules in `Herd` of the herding game entity apply.

## ability.type.Hitbox

```python
Hitbox(Ability):
    radius_x : float
    radius_y : float
    radius_z : float
```

Defines the hitbox of a game entity that is used for collision with other game entities.

**radius_x**
The size of the hitbox in the x axis direction.

**radius_y**
The size of the hitbox in the y axis direction.

**radius_z**
The size of the hitbox in the z axis direction.

## ability.type.Idle

```python
Idle(Ability):
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
```

Deactivates the hitbox of the game entity for movement of other game entities. However, the hitbox  is still relevant for the game entity's own movement.

## ability.type.Projectile

```python
Projectile(Ability):
    arc               : int
    accuracy          : set(Accuracy)
    target_mode       : TargetMode
    ignored_types     : set(GameEntityType)
    unignore_entities : set(GameEntity)
```

Gives a game entity projectile behavior. A projectile is always spawned with another `GameEntity` object as a target. The engine calculates a parabolic path to the target, using the `arc`, `accuracy` and `target_mode` members. While moving along this path, the game entity can use other abilities.

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
    indicator : Ambient
```

Allows a game entity to set a rally point on the map. Game entities spawned by the `Create` ability or ejected from a container will move to the rally point location. The rally point can be placed on another game entity. In that case, the game entities moving there will try to use an appropriate ability on it.

**indicator**
Indicator showing the location of the rally point on the map as an `Ambient` game entity object. It's `Visibility` ability is only active if the game entity using the rally point is selected.

## ability.type.RangedContinuousEffect

```python
RangedContinuousEffect(ApplyContinuousEffect):
    min_range : int
    max_range : int
```

Applies a batch of discrete effects on another game entity. This specialization of `ApplyContinuousEffect` allows ranged application.

**min_range**
Minumum distance to target.

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
Minumum distance to target.

**max_range**
Maximum distance to the target.

## ability.type.RegenerateAttribute

```python
RegenerateAttribute(Ability):
    rate : AttributeRate
```

Regenerate attribute points at a defined rate. The game entity must have the attribute in its `Live` ability.

**rate**
Regeration rate as an `AttributeRate` object.

## ability.type.RegenerateResourceSpot

```python
RegenerateResourceSpot(Ability):
    rate : ResourceRate
    resource_spot : ResourceSpot
```

Regenerate the available resources of a game entity's resource spot at a defined rate.

**rate**
Regeration rate as an `ResourceRate` object.

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
The set of game entities that can be removed from the container. The container must define a corresponding `StorageElement` for the `GameEntity` objects.

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
A set of resistances as `Resistance` objects.

## ability.type.Restock

```python
Restock(Ability):
    auto_restock : bool
    target       : RestockableResourceSpot
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
```

Makes the game entity selectable by players.

## ability.type.SendBackToTask

```python
SendBackToTask(Ability):
    allowed_types : set(GameEntityType)
    blacklisted_game_entities : set(GameEntity)
```

Empties the containers of the `Storage` abilities of a game entity and lets the ejected game entities resume their previous tasks.

**allowed_types**
Whitelist of game entity types that can will be affected.

**blacklisted_game_entities**
Blacklist for specific game entities that would be covered by `allowed_types`, but should be excplicitely excluded.

## ability.type.ShootProjectile

```python
ShootProjectile(Ability):
    projectiles               : orderedset(Projectile)
    min_projectiles           : int
    max_projectiles           : int
    min_range                 : int
    max_range                 : int
    reload_time               : float
    spawn_delay               : float
    projectile_delay          : float
    require_turning           : bool
    manual_aiming_allowed     : bool
    spawning_area_offset_x    : float
    spawning_area_offset_y    : float
    spawning_area_offset_z    : float
    spawning_area_width       : float
    spawning_area_height      : float
    spawning_area_randomness  : float
    allowed_types             : set(GameEntityType)
    blacklisted_game_entities : set(GameEntity)
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

**blacklisted_game_entities**
Blacklist for specific game entities that would be covered by `allowed_types`, but should be excplicitely excluded.

## ability.type.Stop

```python
Stop(Ability):
```

Stops all current tasks and returns the game entity to an idle state.

## ability.type.Storage

```python
Storage(Ability):
    container       : Container
    empty_threshold : AttributeAmount
```

Enables a game entity to store other game entities. The stored game entities can influence the state of the storing game entity.

**container**
Defines which and how many game entities can be stored.

**empty_threshold**
Empties the storage if the current value of the referenced attribute falls below the defined value.

## ability.type.TileRequirement

```python
TileRequirement(Ability):
    tiles_x : int
    tiles_y : int
```

Makes a game entity require a rectangular set of tiles on the terrain grid.

**tiles_x**
Number of tile required on the x axis of the grid.

**tiles_y**
Number of tile required on the y axis of the grid.

## ability.type.TerrainRequirement

```python
TerrainRequirement(Ability):
    terrain_requirement : set(Terrain)
```

Makes a game entity require a specific terrain to move and be placed on.

**terrain_requirement**
Set of terrains the game entity requires.

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
Game entity that is transfered.

**source_container**
Container the game entity is stored in when the ability is used.

**target_container**
Container that the game entity is inserted into. The target container can belong to the same game entity.

## ability.type.Transform

```python
Transform(Ability):
    states        : set(StateChanger)
    initial_state : StateChanger
```

Defines a set of predefined state changes that a game entity can activate by using the `TransformTo` ability. Only one `StateChanger` object per `Transform` ability can be active at the same time.

**states**
State changes that can be activated. `TransformTo` can switch to more states than those which are defined in this set. However, it is recommend to list all state changes reachable by transformation in here to allow easy access to them in scripts.

**initial_state**
Activates automatically when the ability is enabled.

## ability.type.TransformTo

```python
TransformTo(Ability):
    target_state       : StateChanger
    transform_time     : float
    transform_progress : set(TransformProgress)
```

Activates a state change for a game entity. A `Transform` ability must be assigned to the game entity. The previous state change will be deactivated when the transformation has finished.

**target_state**
Target state change that activates when the time defined in `transform_time` has passed.

**transform_time**
The time for the transformation to complete.

**transform_progress**
A set of `TransformProgress` objects that can activate state changes and animation overrides while the transformation progresses.

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

Configures a game entity's visiblity in the fog of war.

**visible_in_fog**
Determines whether the game entity is visible in the fog of war.

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

## resistance.Resistance

```python
Resistance(Entity):
```

Generalization object for all resistances.

Standard behavior without specializations:

* Effects are **only applied** if the resistor has a **matching** `Resistance` object. Matching can be further defined depending on the concrete effect.
* Resisting **costs no attribute points**.

## resistance.continuous.ContinuousResistance

```python
ContinuousResistance(Resistance):
```

Generalization object for resistances to continuous effects.

## resistance.continuous.flat_attribute_change.FlatAttributeChange

```python
FlatAttributeChange(ContinuousResistance):
    type       : AttributeChangeType
    block_rate : set(AttributeRate)
```

Generalization object for resistances to continuous `FlatAttributeChange` effects.

Note that you cannot use this resistance object directly and have to choose one of the specializations `FlatAttributeChangeDecrease` or `FlatAttributeChangeIncrease`.

**type**
Effects of type `Effect.ContinuousEffect.FlatAttributeChange` are matched to this resistance if they store the same `AttributeChangeType` object in their `type` member. Otherwise, the effect will not be applied.

**block_rate**
The per-second attribute rate that is blocked by the resistor. The net change rate (applied rate) is calculated by subtracting the effector's `change_rate` from the resistor's `block_rate`.

```math
applied_rate = change_rate - block_rate
```

The applied rate is further bound by the interval defined by `min_change_rate` and `max_change_rate` in the effect, if these members are set.

## resistance.continuous.flat_attribute_change.type.FlatAttributeChangeDecrease

```python
FlatAttributeChangeDecrease(FlatAttributeChange):
```

Specialization of the continuous `FlatAttributeChange` resistance that blocks the effector's attribute decrease rate.


## resistance.continuous.flat_attribute_change.type.FlatAttributeChangeIncrease

```python
FlatAttributeChangeIncrease(FlatAttributeChange):
```

Specialization of the continuous `FlatAttributeChange` resistance that blocks the effector's attribute increase rate.

## resistance.continuous.type.Lure

```python
Lure(ContinuousResistance):
    type : LureType
```

Resistance to the `Lure` effect.

**type**
Effects of type `Effect.ContinuousEffect.Lure` are matched to this resistance if they store the same `LureType` object in their `type` member. Otherwise, the effect will not be applied.

## resistance.continuous.time_relative_attribute_change.TimeRelativeAttributeChange

```python
TimeRelativeAttributeChange(ContinuousResistance):
    type : AttributeChangeType
```

Generalization object for resistances to continuous `TimeRelativeAttributeChange` effects.

Note that you cannot use this resistance object directly and have to choose one of the specializations `TimeRelativeAttributeDecrease` or `TimeRelativeAttributeIncrease`.

**type**
Effects of type `Effect.ContinuousEffect.TimeRelativeAttributeChange` are matched to this resistance if they store the same `AttributeChangeType` object in their `type` member. Otherwise, the effect will not be applied.

## resistance.continuous.time_relative_attribute_change.type.TimeRelativeAttributeDecrease

```python
TimeRelativeAttributeDecrease(TimeRelativeAttributeChange):
```

Specialization of the continuous `TimeRelativeAttributeChange` resistance that decreases the resistor's current attribute value in a fixed amount of time relative to their attribute's `max_value`.

## resistance.continuous.time_relative_attribute_change.type.TimeRelativeAttributeIncrease

```python
TimeRelativeAttributeIncrease(TimeRelativeAttributeChange):
```

Specialization of the continuous `TimeRelativeAttributeChange` resistance that increases the resistor's current attribute value in a fixed amount of time relative to their attribute's `max_value`.

## resistance.continuous.time_relative_progress.TimeRelativeProgress

```python
TimeRelativeProgress(ContinuousResistance):
    type : ProgressType
```

Generalization object for resistances to continuous `TimeRelativeProgress` effects.

Note that you cannot use this resistance object directly and have to choose one of the specializations `TimeRelativeProgressDecrease` or `TimeRelativeProgressIncrease`.

**type**
Effects of type `Effect.ContinuousEffect.TimeRelativeProgress` are matched to this resistance if they store the same `ProgressType` object in their `type` member. Otherwise, the effect will not be applied.

## resistance.continuous.time_relative_progress.type.TimeRelativeProgressDecrease

```python
TimeRelativeProgressDecrease(TimeRelativeProgress):
```

Specialization of the continuous `TimeRelativeProgress` resistance that decreases the resistor's progress amount in a fixed amount of time relative to 100%.

## resistance.continuous.time_relative_progress.type.TimeRelativeProgressIncrease

```python
TimeRelativeProgressIncrease(TimeRelativeProgress):
```

Specialization of the continuous `TimeRelativeProgress` resistance that increases the resistor's progress amount in a fixed amount of time relative to 100%.

## resistance.discrete.DiscreteResistance

```python
DiscreteResistance(Resistance):
```

Generalization object for resistances to discrete effects.

## resistance.discrete.convert.Convert

```python
Convert(DiscreteResistance):
    type          : ConvertType
    chance_resist : float
```

Resistance to the `Convert` effect.

**type**
Effects of type `Effect.ContinuousEffect.Convert` are matched to this resistance if they store the same `ConvertType` object in their `type` member. Otherwise, the effect will not be applied.

**chance_resist**
Percentage amount subtracted from the effector's success chance. The percentage should be stored as a float value between *0.0* and *1.0*. The net chance (applied chance) of success is calculated by subtracting the effector's `chance_success` from the resistor's `chance_resist`.

```math
applied_chance = chance_success - chance_resist
```

Any value below *0.0* is an automatic fail, while any value above *1.0* is an automatic success. The applied chance is further bound by the interval defined by `min_chance_success` and `max_chance_success` in the effect, if these members are set.

## resistance.discrete.convert.type.AoE2Convert

```python
AoE2Convert(Convert):
    guaranteed_resist_rounds       : int
    protected_rounds               : int
    protection_round_recharge_time : float
```

Resistance to the `AoE2Convert` effect.

**guaranteed_resist_rounds**
Number of rounds at the start where the success chance is guaranteed to be *0.0*.

**protected_rounds**
Number of rounds that are needed for the success chance to always be *1.0*.

**protection_round_recharge_time**
Time it takes until one protected round is added back to the number of protected rounds.

When the effector stops applying the effect, the number of protected rounds is increased until they reach their maximum value again. Running out of the range of the effector does not count as stopping the effect application.

## resistance.discrete.flat_attribute_change.FlatAttributeChange

```python
FlatAttributeChange(DiscreteResistance):
    type        : AttributeChangeType
    block_value : set(AttributeAmount)
```

Generalization object for resistances to discrete `FlatAttributeChange` effects.

Note that you cannot use this resistance object directly and have to choose one of the specializations `FlatAttributeChangeDecrease` or `FlatAttributeChangeIncrease`.

**type**
Effects of type `Effect.DiscreteEffect.FlatAttributeChange` are matched to this resistance if they store the same `AttributeChangeType` object in their `type` member. Otherwise, the effect will not be applied.

**change_value**
The flat attribute amount that is blocked by the resistor. The net change value (applied value) is calculated by subtracting the effector's `change_value` from the resistor's `block_value`.

```math
applied_value = change_value - block_value
```

The applied value is further bound by the interval defined by `min_change_value` and `max_change_value` in the effect, if these members are set.

## resistance.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease

```python
FlatAttributeChangeDecrease(FlatAttributeChange):
```

Specialization of the discrete `FlatAttributeChange` resistance that blocks the effector's attribute decrease value.


## resistance.discrete.flat_attribute_change.type.FlatAttributeChangeIncrease

```python
FlatAttributeChangeIncrease(FlatAttributeChange):
```

Specialization of the discrete `FlatAttributeChange` resistance that blocks the effector's attribute increase value.

## resistance.discrete.type.MakeHarvestable

```python
MakeHarvestable(DiscreteResistance):
    resource_spot      : ResourceSpot
    harvest_conditions : set(HarvestableRequirement)
```

Resistance to the `MakeHarvestable` effect.

**resource_spot**
Resource spot that should be made harvestable. Effects of type `Effect.DiscreteEffect.MakeHarvestable` are matched to this resistance if they store the same `ResourceSpot` object in their `resource_spot` member. Additionally, the target needs to have a `Harvestable` ability that contains the resource spot.

**harvest_conditions**
The requirements under which the resource spots will be made harvestable.

## resistance.discrete.type.SendToContainer

```python
SendToContainer(DiscreteResistance):
    type              : SendToContainerType
    search_range      : float
    ignore_containers : set(Container)
```

Resistance to the `SendToContainer` effect.

**type**
Effects of type `Effect.DiscreteEffect.SendToContainer` are matched to this resistance if they store the same `SendToContainerType` object in their `type` member. Otherwise, the effect will not be applied.

**search_range**
The range in which the resistor will search for a container.

**ignore_containers**
Excludes the containers from the set when searching for a target container.

## resistance.specialization.CostResistance

```python
AttributeCostResistance(Resistance):
    cost : Cost
```

Can be inherited to make the resistance cost attribute points or resources.

**cost**
The amount of attribute points or resources removed from the resistor.

## aux.accuracy.Accuracy

```python
Accuracy(Entity):
    accuracy : float
    accuracy_dispersion : float
    dispersion_dropoff : DropOffType
    target_types : set(GameEntityType)
    blacklisted_entities : set(GameEntity)
```

Stores information for the accuracy calculation of a game entity with `Projectile` ability.

**accuracy**
The chance for the projectile to land at the "perfect" position to hit its target as a value between 0 and 1.

**accuracy_dispersion**
The maximum accuracy dispersion when the projectile fails the accuracy check.

**dispersion_dropoff**
Multiplies the maximum dispersion with a dropoff factor. The dropoff depends on the distance of the projectile spawning game entity in relation to the `max_range` of its `ShootProjectile` ability.

**target_types**
Lists the game entities types for which the accuracy value can be used.

**blacklisted_entities**
Used to blacklist game entities that have one of the types listed in `target_types`, but should not be covered by this `Accuracy` object.

## aux.animation_override.AnimationOverride

```python
AnimationOverride(Entity):
    ability    : Ability
    animations : set(Animation)
    priority   : int
```

Internally overrides the animations used for an ability. The ability must be an `AnimatedAbility` when the override occurs. The override stops when

* Another override of the same ability with a *greater than or equal to* (>=) is initiated
* The overriden ability is deactivated
* The override animation would be the same as the standard animation

**ability**
The ability whos animations should be overriden. This member can reference a specific ability of the game entity or an API ability. If an API ability is referenced, all its instances in the ability set of the game entity will be overriden.

**animations**
The replacement animations of the override.

**priority**
Priority of the override. Overrides are only executed if their priority is *greater than or equal to* (>=) an already existing override. The default animation of an ability always has a priority of 0.

## aux.attribute.Attribute

```python
Attribute(Entity):
    name         : TranslatedString
    abbreviation : TranslatedString
```

Defines an attribute that can be assigned a value range by `AttributeSetting`. Attributes are used for interaction between game entities (e.g. health) and as payment for effects.

**name**
The name of the attribute as a translated string.

**abbreviation**
Short version of the names as a translated string.

## aux.attribute.AttributeAmount

```python
AttributeAmount(Entity):
    type   : Attribute
    amount : int
```

A fixed amount of a certain attribute.

**type**
The attribute.

**amount**
Amount of attribute points.

## aux.attribute.AttributeRate

```python
AttributeRate(Entity):
    type   : Attribute
    rate   : float
```

A per second rate of a certain attribute.

**type**
The attribute.

**rate**
Rate of attribute points.

## aux.attribute.AttributeSetting

```python
AttributeSetting(Entity):
    attribute      : Attribute
    min_value      : int
    max_value      : int
    starting_value : int
```

Assigns an interval for an attribute of a game entity. The game entity can have a *current attribute value* while it exists. Attribute values can be changed by abilities or effects.

**attribute**
Attribute the interval is defined for.

**min_value**
Minimum value the current attribute value can have. Can be negative.

**max_value**
Maximum value the current attribute value can have.

**starting_value**
The current attribute value when the game entity is created.

## aux.attribute.ProtectingAttribute

```python
ProtectingAttribute(Attribute):
    protects : Attribute
```

Protects an attribute by preventing its value to be decreased by another game entity. Attribute increases by other game entities, costs and regeneration are not affected.

**protects**
The attribute that is protected.

## aux.attribute_change_type.AttributeChangeType

```python
AttributeChangeType(Entity):
```

Used by `FlatAttributeChange` effects and resistances for matching.

## aux.attribute_change_type.type.Fallback

```python
Fallback(AttributeChangeType):
```

A special type for `FlatAttributeChange`. Effects with this type are only evaluated if the accumalated applied value or applied rate of all other effects is outside of a specified interval. The interval is defined by the `FlatAttributeChange` object that has its `type` member set to `Fallback`. Upper and lower bounds are `[min_change_rate,max_change_rate]` (continuous effects) and `[min_change_value,max_change_value]` (discrete effects). The fallback effect is also evaluated if no other `FlatAttributeChange` effect is present or matched to any resistances. However, fallback effects still needs to be matched against a resistance object with `type` set to `Fallback`.

For example, effects that utilize fallback can be used to model minimum or maximum damage of a game entity.

## aux.availability_prerequisite.AvailabilityPrerequisite

```python
AvailabilityPrerequisite(Entity):
    mode : bool
```

*This object is part of a mechanic for unlocking game entities (see `CreatableGameEntity`) through specifying a set of requirements.* (TODO: explain)

Generalization object for a condition that can be either true or false.

**mode**
Determines whether the condition defined by the prerequisite should be negated.

* `mode = true`: The prerequisite must be true
* `mode = false`: The prerequisite must be false.

## aux.availability_prerequisite.type.TechResearched

```python
TechResearched(AvailabilityPrerequisite):
    tech : Tech
```

Is true when the technology has been researched by the player.

**tech**
The technology that has to be researched.

## aux.availability_prerequisite.type.GameEntityProgress

```python
GameEntityProgress(AvailabilityPrerequisite):
    game_entity : GameEntity
    status   ´  : ProgressStatus
```

Is true when an instance of a game entity owned by the player has reached a certain progress.

**game_entity**
The game entity that should have progressed this far.

**status**
Current status of a game entity as a `ProgressStatus` object.

## aux.availability_requirement.AvailabilityRequirement

```python
AvailabilityRequirement(Entity):
    requirements : set(AvailabilityPrerequisite)
```

*This object is part of a mechanic for unlocking game entities (see `CreatableGameEntity`) through specifying a set of requirements.* (TODO: explain)

To evaluate to `true`, all of the `Prerequisites` in the `AvailabilityRequirement` must be true as well.

**requirements**
A number of prerequisites that can be true or false. If all of the prerequisites are true, the availability requirement is fulfilled.

## aux.cheat.Cheat

```python
Cheat(Entity):
    activation_message : text
    display_message    : text
    changes            : orderedset(Patch)
```

Cheats are a predefined gameplay change, often in favor and to the amusement of the player. They are deactivated by default in multiplayer. Advanced cheating behavior can be realized by attaching scripts to the `Cheat` object.

**activation_message**
The activation message that has to be typed into the chat console.

**display_message**
The displayed message after the cheat is activated as a `TranslatedString`.

**changes**
Changes to API objects.

## aux.civilization.Civilization

```python
Civilization(Entity):
    name               : TranslatedString
    description        : TranslatedMarkupFile
    long_description   : TranslatedMarkupFile
    leader_names       : set(TranslatedString)
    modifiers          : set(Modifier)
    starting_resources : set(ResourceAmount)
    civ_setup          : orderedset(Patch)
```

Civilization customize the base state of the game for a player. This includes availabilitty of units, buildings and techs as well as changing their abilities and modifiers plus their individual members.

**name**
The name of the civilization as a translatable string.

**description**
A description of the civilization as a translatable markup file.

**long_description**
A longer description of the civilization as a translatable markup file. Used for the tech tree help of the civilization.

**leader_names**
Names for the leader of the civilizations that are displayed with the score.

**modifiers**
Modifiers for game entities of the civilization. By default, these modifiers apply to **all** game entities belonging to the player. For example, an `AttributeModifier` with `multiplier = 1.2` for the attribute `Health` will increase the maximum HP of every unit owned by the player by 20\%. If you want the modifier to only affect specific game entities, you have to use `ScopedModifier` or assign `Modifier` objects to individual game entities using `civ_setup`.

**starting_resources**
The resources of the civilization at the start of a game.

**civ_setup**
Customizes the base state of the game through patches. Any members and objects can be patched. Normal `Patch` objects will only be applied to the player. To apply patches to other player with specific diplomatic stances, use `DiplomaticPatch`.

## aux.container_type.SendToContainerType

```python
SendToContainerType(Entity):
```

Used by `SendToContainer` effects and resistances for matching.

## aux.convert_type.ConvertType

```python
ConvertType(Entity):
```

Used by `Convert` effects and resistances for matching.

## aux.cost.Cost

```python
Cost(Entity):
    payment_mode : PaymentMode
```

Generalization object for resource and attribute costs.

**payment_mode**
Determines when the costs have to be payed as `PaymentMode` objects.

## aux.cost.type.AttributeCost

```python
AttributeCost(Cost):
    amount : set(AttributeAmount)
```

Defines the cost as an amount of attribute points that is removed from the game entity's current attribute value.

**amount**
Amounts of attribute points as `AttributeAmount` objects.

## aux.cost.type.ResourceCost

```python
ResourceCost(Cost):
    amount : set(ResourceAmount)
```

Defines the cost as an amount of resources that is removed from the player's resource pool.

**amount**
Amounts of resources as `ResourceAmount` objects.

## aux.create.CreatableGameEntity

```python
CreatableGameEntity(Entity):
    game_entity    : GameEntity
    cost           : Cost
    creation_time  : float
    creation_sound : Sound
    requirements   : set(AvailabilityRequirement)
    placement_mode : PlacementMode
```

Defines preconditions, placement and spawn configurations for a new instance of a game entity created by a `Create` ability.

**game_entity**
Reference to the `GameEntity` object that is spawned.

**cost**
Resource amount spent to intiate creation. Cancelling the creation results in a refund of the spent resources.

**creation_time**
Time until the game entity is spawned from the creating game entity.

**creation_sound**
Sounds that are played on creating an instance of the game entity.

**requirements**
A set of requirements that unlock the creatable game entity for the creating game entity. Only one requirement needs to be fulfilled to trigger the unlock. If the set is empty, the game entity is considered available from the start for the creating game entity.

**placement_mode**
Decides where and how the game entity instance is spawned as a `PlacementMode` object.

## aux.death_condition.DeathCondition

```python
DeathCondition(Entity):
```

Generalization object for all death conditions that are usable by the `Die` ability.

## aux.death_condition.type.AttributeInterval

```python
AttributeInterval(DeathCondition):
    attribute : Attribute
    min_value : optional(AttributeAmount)
    max_value : optional(AttributeAmount)
```

Triggers when the current attribute value of a game entity is outside of a defined interval. At least one of the members `min_value` and `max_value` has to be set.

**attribute**
Attribute of the game entity.

**min_value**
Lower bound of the interval. If the current attribute value is *lower than or equal to* (<=) this bound, the condition will trigger.

**max_value**
Upper bound of the interval. If the current attribute value is *greater than or equal to* (>=) this bound, the condition will trigger.

## aux.death_condition.type.ProjectileHit

```python
ProjectileHit(DeathCondition):
```

Triggers when a game entity hitbox collides with that of another game entity.

## aux.death_condition.type.ProjectileHitTerrain

```python
ProjectileHitTerrain(DeathCondition):
```

Triggers when a game entity's hitbox collides with the terrain of the map.

## aux.death_condition.type.ProjectilePassThrough

```python
ProjectilePassThrough(DeathCondition):
    pass_through_range : int
```

Triggers when the distance to the spawn location of the game entity becomes greater than a defined value.

**pass_through_range**
Distance to the spawn location. Only (x,y) coordinates are considered.

## aux.despawn_condition.DespawnCondition

```python
DespawnCondition(Entity):
```

Generalization object for all despawn conditions that are usable by the `Despawn` ability.

## aux.despawn_condition.type.ResourceSpotsDepleted

```python
ResourceSpotsDepleted(DespawnCondition):
    only_enabled : bool
```

Triggers when all resource spots in `Harvestable` abilities have been depleted.

**only_enabled**
The condition only considers resource spots of enabled `Harvestable` abilities.

## aux.despawn_condition.type.Timer

```python
Timer(DespawnCondition):
    time : float
```

Triggers after a specified amount of time has passed after the `Despawn` ability has been activated. If the `Despawn` ability is diabled before the timer has finished, it will be reset.

**time**
Time that has to pass after the activation of the corresponding `Despawn` ability.

## aux.diplomatic_stance.DiplomaticStance

```python
DiplomaticStance(Entity):
```

Generalization object for diplomaic stances that can be used for diplomacy ingame. Diplomatic stances also define which player can use the abilities, modifiers and effects of a game entity.

## aux.diplomatic_stance.type.Self

```python
Self(DiplomaticStance):
```

The diplomatic stance of a player towards themselves.

## aux.dropoff_type.DropoffType

```python
DropoffType(Entity):
```

Used for calculating the effectiveness over distance of `AreaEffect`s and `Accuracy` of projectiles. The dropoff modifier is always relational to the maximum range.

## aux.dropoff_type.type.InverseLinear

```python
InverseLinear(DropoffType):
```

The effectiveness starts at 0\% (for zero distance) and linearly increases to 100\% (for maximum distance).

## aux.dropoff_type.type.Linear

```python
Linear(DropoffType):
```

The effectiveness starts at 100\% (for zero distance) and linearly decreases to 0\% (for maximum distance).

## aux.dropoff_type.type.NoDropoff

```python
NoDropoff(DropoffType):
```

The effectiveness is constant and independent from the range to the target.

## aux.exchange_mode.ExchangeMode

```python
ExchangeMode(Entity):
```

Generalization object for exchange modes of the `ExchangeResource` ability.

## aux.exchange_mode.type.Fixed

```python
Fixed(ExchangeMode):
```

Applies no additional rules to the ability.

## aux.exchange_mode.volatile.Volatile

```python
Volatile(ExchangeMode):
    source_min_amount : int
    source_max_amount : int
    target_min_amount : int
    target_max_amount : int
    scope             : optional(ExchangeScope)
```

Generalization object for *volatile* exchange modes. Adjusts the transfered resource amounts after every transaction.

**source_min_amount**
Lower bound for the source resource amount.

**source_max_amount**
Upper bound for the source resource amount.

**target_max_amount**
Upper bound for the source resource amount.

**target_max_amount**
Upper bound for the target resource amount.

**scope**
An optional scope for which the adjustments are made. When a scope is defined, the transfered resource amounts are changed for *all* abilities that reference the same scope object in their exchange mode.

## aux.exchange_mode.volatile.VolatileFlat

```python
VolatileFlat(Volatile):
    change_source_amount : int
    change_target_amount : int
```

Changes the transfered resource amounts by a fixed amount after every transaction.

**change_source_amount**
Changes the current source resource amount by this value.

**change_target_amount**
Changes the current target resource amount by this value.

## aux.exchange_scope.ExchangeScope

```python
ExchangeScope(Entity):
```

Generalization object for volatile exchange modes.

## aux.formation.Formation

```python
Formation(Entity):
    subformations : set(Subformation)
```

Organizational structure for multiple game entities.

**subformations**
Subdivisions of the formation. Game entities are sorted into one of the subformations.

## aux.formation.Subformation

```python
Subformation(Entity):
```

Subdivision of a formation. It defines the structure and placement of game entities when the formation is formed.

## aux.formation.PreceedingSubformation

```python
PreceedingSubformation(Entity):
    preceeds : Subformation
```

Links the individual subformations of a formation together. By using this object, the subformations form a linked list that represents their order in the parent formation.

**preceeds**
Subformation that should be in front of this subformation.

## aux.game_entity.GameEntity

```python
GameEntity(Entity):
    types     : set(GameEntityType)
    variants  : set(Variant)
    abilities : set(Ability)
    modifiers : set(Modifier)
```

For definition of all ingame objects, including units, buildings, items, projectiles and ambience. Their capabilities are handled through `Ability` and `Modifier` API objects stored in the members.

**types**
Classification of the game entity via `GameEntityType` objects.

**variants**
Variants can alter the game entity before they are created. The requirement and extent of the changes depends on the `Variant` object.

**abilities**
Define what the game entity *does* and *is* through `Ability` objects.

**modifiers**
Change the stats of abilities belonging to the game entity. Mostly used to give boni or mali in certain situations (see `Modifier`).

## aux.game_entity.type.Ambient

```python
Ambient(GameEntity):
```

Specialization of `GameEntity` for objects in the scenery (cliffs, rocks, trees).

## aux.game_entity.type.Building

```python
Building(GameEntity):
```

Specialization of `GameEntity` for buildable objects.

## aux.game_entity.type.BuildingPart

```python
BuildingPart(Building):
    offset_x : int
    offset_y : int
```

A subpart of a `MultiPartBuilding`. Works essentially like any other `Building` game entity.

**offset_x**
Offset on the x-axis relative to the `MultiPartBuilding` container.

**offset_y**
Offset on the y-axis relative to the `MultiPartBuilding` container.

## aux.game_entity.type.MultiPartBuilding

```python
MultiPartBuilding(Building):
    building_parts : set(BuildingPart)
```

A building that also is container for several other buildings with own abilities and modifiers.

**building_parts**
The buildings managed by the container (see `BuildingPart`).

## aux.game_entity.type.Item

```python
Item(GameEntity):
```

Specialization of `GameEntity` for items.

## aux.game_entity.type.Unit

```python
Unit(GameEntity):
```

Specialization of `GameEntity` for military and civilian units.

## aux.game_entity_formation.GameEntityFormation

```python
GameEntityFormation(Entity):
    formation    : Formation
    subformation : Subformation
```

Defines a placement in a formation for the `Formation` ability.

**formation**
The formation the game entity is placed in.

**subformation**
Subformation inside the subformation that the game entity is inserted into.

## aux.game_entity_stance.GameEntityStance

```python
GameEntityStance(Entity):
    ability_preference : orderedset(Ability)
    type_preference    : orderedset(GameEntityType)
```

Generalization object for activity stances for the `GameEntityStance` ability.

**ability_preference**
The abilities which the game entity will execute or search targets for. Their order in the set defines the priority of usage.

**type_preference**
Determines which game entity types are prioritized as targets. Their order in the set defines the priority of usage. Game entities with types that are not in the set will be ignored.

## aux.game_entity_stance.type.Agressive

```python
Agressive(GameEntityStance):
```

The game entity will use ranged abilities or move to the nearest target in its line of sight to use other abilities. If the target gets out of the line of sight, the game entity searches for a new target. When no new target can be found, the game entity stops moving and returns to an idle state.

## aux.game_entity_stance.type.Defensive

```python
Defensive(GameEntityStance):
```

The game entity will use ranged abilities or move to the nearest target in its line of sight to use other abilities. If the target gets out of range or the line of sight, the game entity searches for a new target. When no new target can be found, the game entity returns to its original position and returns to an idle state.

## aux.game_entity_stance.type.StandGround

```python
StandGround(GameEntityStance):
```

The game entity will stay at its current position.

## aux.game_entity_stance.type.Passive

```python
Passive(GameEntityStance):
```

The game entity will stay at its current position and only reacts to manual commands given by players. Abilities in `ability_preference` will be ignored.

## aux.graphics.Animation

```python
Animation(Entity):
    sprite : file
```

Points to a openage sprite definition file in the `.sprite` format. The specified aniimation can be used by `AnimatedAbility` objects.

## aux.graphics.Terrain

```python
Terrain(Entity):
    sprite : file
```

Points to a openage terrain definition file in the `.terrain` format. This object is used by the `terrain.Terrain` object that defines properties of ingame terrain.

## aux.language.Language

```python
Language(Entity):
    ietf_string : text
```

A language definition. Languages are used for translated strings, markup files and sounds.

**ietf_string**
The IETF identification tag of the language. [See here](https://tools.ietf.org/html/rfc4646) for more information on how the tags are established.

## aux.language.LanguageMarkupPair

```python
LanguageMarkupPair(Entity):
    language    : Language
    markup_file : file
```

Defines the translation of a longer text from a markup file in a certain language.

**language**
The language used in the markup file.

**markup_file**
File descriptor of the markup file. Has to be relative to the `.nyan` file where the `LanguageMarkupPair` is defined.

## aux.language.LanguageSoundPair

```python
LanguageSoundPair(Entity):
    language : Language
    sound    : Sound
```

Defines the translation of a sound in a certain language.

**language**
The language used for the sound.

**sound**
References the `Sound` object for the translation.

## aux.language.LanguageTextPair

```python
LanguageTextPair(Entity):
    language : Language
    string   : text
```

Defines the translation of a string in a certain language.

**language**
The language used for the string.

**string**
The translated string.

## aux.lure_type.LureType

```python
LureType(Entity):
```

Used by `Lure` effects and resistances for matching.

## aux.mod.Mod

```python
Mod(Entity):
    patches : orderedset(Patch)
```

A set of patches that will be automatically applied when the modpack is loaded. It is recommended to only use one `Mod` object per modpack since the correct order of patches cannot be guaranteed otherwise.

**patches**
Changes the game state through patches. Any members and objects can be patched. Every `Patch` will be applied to all players. `DiplomaticPatch` objects will also be applied to all players.

## aux.modifier_scope.ModifierScope

```python
ModifierScope(Entity):
```

Generalization object for scopes of a `ScopeModifier` object.

## aux.modifier_scope.type.GameEntityScope

```python
GameEntityScope(ModifierScope):
    affected_types            : set(GameEntityType)
    blacklisted_game_entities : set(GameEntity)
```

Defines a scope of game entities the modifier should apply to.

**affected_types**
Whitelist of game entity types that the modifier should apply to.

**blacklisted_game_entities**
Blacklist for specific game entities that would be covered by `affected_types`, but should be excplicitely excluded.

## aux.modifier_scope.type.Standard

```python
Standard(ModifierScope):
```

Makes the modifier behave as if standard rules would apply, i.e. as if the modifier had no `ScopeModifier` specialization type.

## aux.move_mode.MoveMode

```python
MoveMode(Entity):
```

Generalization object for move modes for the `Move` ability.

## aux.move_mode.type.AttackMove

```python
AttackMove(MoveMode):
```

Move to a position on the map. Stances from `GameEntityStance` ability are considered during movement.

## aux.move_mode.type.Follow

```python
Follow(MoveMode):
    range : float
```

Follow another game entity at a defined range. The movement speed is adjusted to the followed game entity, but cannot go higher than the `speed` value from `Move`. Stances from `GameEntityStance` ability are ignored during movement. Following is stopped when the followed game entity gets out of the line of sight of the following game entity.

**range**
The range at which the other game entity is followed.

## aux.move_mode.type.Normal

```python
AttackMove(MoveMode):
```

Move to a position on the map. Stances from `GameEntityStance` ability are ignored during movement.

## aux.move_mode.type.Patrol

```python
Patrol(MoveMode):
```

Lets player set two or more waypoints that the game enity will follow. Stances from `GameEntityStance` ability are considered during movement.

## aux.patch.Patch

```python
Patch(Entity):
```

Generalization object for all nyan patchesv. Let nyan patches inherit from this object to make them usable for the openage API.

## aux.patch.type.DiplomaticPatch

```python
DiplomaticPatch(Patch):
    stances : set(DiplomaticStance)
```

A patch that is applied to all players that have the specified diplomatic stances from the viewpoint of the patch's initiator.

**stances**
The diplomatic stances of the players the patch should apply to.

## aux.payment_mode.PaymentMode

```python
PaymentMode(Entity):
```

Generalization object for the payment options of a `Cost` object.

## aux.payment_mode.type.Adaptive

```python
Adaptive(PaymentMode):
```

The cost is handled as *running costs*. Payment is progressive (relative to a value, e.g. time) while an action is executed. The action halts if the costs cannot be payed anymore.

## aux.payment_mode.type.Advance

```python
Advance(PaymentMode):
```

The cost is handled as *payment in advance*. Actions require payment first before they can be executed.

## aux.payment_mode.type.Arrear

```python
Arrear(PaymentMode):
```

The cost is handled as *payment in arrear*. Actions that require payment are executed first and payed afterwards.

## aux.placement_mode.PlacementMode

```python
PlacementMode(Entity):
```

Generalization object for all placement modes that are configurable for a `CreatableGameEntity` object.

## aux.placement_mode.type.Eject

```python
Eject(PlacementMode):
```

The game entity is ejected from the creating game entity. Ejecting considers the game entity's `Hitbox`, `TileRequirement` and `TerrainRequirement` abilities for the ejection location.

## aux.placement_mode.type.Place

```python
Place(PlacementMode):
    allow_rotation : bool
```

The game entity can be placed on the map after its creation. Placement considers the game entity's `Hitbox`, `TileRequirement` and `TerrainRequirement` abilities for the placement location.

**allow_rotation**
The player can cycle through the `PerspectiveVariant` variants of the game entity before placement.

## aux.progress.Progress

```python
Progress(Entity):
    progress : int
```

Generalization object for progression types. Certain properties of a game entity can progress from 0% to 100%. These are for example construction or harvesting progress. `Progress` objects can define what happens at after a property has reached a specific point of progress.

A `Progress` object is always tied to an ability.

Specializations:

* `AnimatedProgress`: Overrides the animation of an ability.
* `TerrainProgress`: Changes the underlying terrain of the game entity.
* `TerrainOverlayProgress`: Changes terrain overlays of a game entity.
* `StateChangeProgress`: Alters the base abilities and modifiers of the game entity through `StateChanger` objects.

**progress**
An integer between 0 and 100 that represents the percentage of progress after which the progress object is used.

## aux.progress.specialization.AnimatedProgress

```python
AnimatedProgress(Progress):
    progress_sprite : AnimationOverride
```

Overrides the animation of an ability when the specified progress has been reached.

**progress_sprite**
The new animation as an `AnimationOverride` object.

## aux.progress.specialization.StateChangeProgress

```python
StateChangeProgress(Progress):
    state_change : StateChanger
```

Alters the base abilities and modifiers of a game entity when the specified progress has been reached.

**state_change**
The state modifications as a `StateChanger` object.

## aux.progress.specialization.TerrainOverlayProgress

```python
TerrainOverlayProgress(Progress):
    terrain_overlay : Terrain
```

Changes overlayed terrain of a game entity when the specified progress has been reached. The game entity needs an enabled `OverlayTerrain` ability for this to work.

**terrain**
Overrides the overlayed terrain of the currently enabled `OverlayTerrain` ability of the game entity. The override stops when another terrain overlay override is initiated, the overriden `OverlayTerrain` ability is deactivated or the override terrain is the same as the default overlayed terrain of the ability.

## aux.progress.specialization.TerrainProgress

```python
TerrainProgress(Progress):
    terrain : Terrain
```

Changes the underlying terrain of a game entity when the specified progress has been reached.

**terrain**
The new terrain that will be permanently placed under the game entity.

## aux.progress.type.CarryProgress

```python
CarryProgress(Progress):
```

Monitors the occupied storage space of a `Storage` or `Gather` ability. An empty storage has a progress of 0\% and a full storage a progress of 100\%.

## aux.progress.type.ConstructionProgress

```python
ConstructionProgress(Progress):
```

Monitors the construction progress of a game entity with `Contructable` ability. An unconstructed game entity has a progress of 0\% and a fully constructed game entity a progress of 100\%.

## aux.progress.type.DamageProgress

```python
DamageProgress(Progress):
```

Compares the current attribute value in relation to the `max_value` of an attribute of a game entity. The `Progress` objects are stored in a `Damageable` ability which specifies the type of attribute that is monitored. When the attribute value is equal to `max_value` of the attribute defined by the game entity, the progress is 0\%. Once the attribute value reaches the `min_value`, the progress is 100\%.

## aux.progress.type.HarvestProgress

```python
HarvestProgress(Progress):
```

Monitors the harvesting progress of a resource spot stored by a `Harvestable` ability. A resource spot with full capacity has a progress of 0\% and a fully harvested resource spot a progress of 100\%.

## aux.progress.type.RestockProgress

```python
RestockProgress(Progress):
```

Monitors the restock progress of a restockable resource spot stored by a `Harvestable` ability. The restocking progress is intiated by the `Restock` ability of another game entity. At the start of the restocking process, the progress is 0\%. After the restocking has finished, the progress is 100\%.

Restocking progress is only tracked between the start and end of restock process. Therefore, state changes initiated by `RestockProgress` objects of type `StateChangerProgress` will be deactivated after the progress reaches 100\%.

## aux.progress.type.TransformProgress

```python
TransformProgress(Progress):
```

Monitors the progress of a trasformation initiated by a `TransformTo` ability. At the start of the transformation, the progress is 0\%. After the transformation has finished, the progress is 100\%.

Transformation progress is only tracked between the start and end of transformation. Therefore, state changes initiated by `TransformProgress` objects of type `StateChangerProgress` will be deactivated after the progress reaches 100\%.

## aux.progress_status.ProgressStatus

```python
ProgressStatus(Entity):
    progress_type : ProgressType
    progress      : float
```

Generalization object for progress status objects required by `GameEntityProgress`.

**progress_type**
The type of progress.

**progress**
Minimum amount of progress that has to be reached.

## aux.progress_type.ProgressType

```python
ProgressType(Entity):
```

Used by `Convert` effects and resistances for matching.

## aux.progress_type.type.Construct

```python
Construct(ProgressType):
```

A progress type that covers construction progress.

## aux.research.ResearchableTech

```python
ResearchableTech(Entity):
    tech           : Tech
    cost           : Cost
    research_time  : float
    research_sound : Sound
    requirements   : set(AvailabilityRequirement)
```

Defines preconditions for researching a technology with the `Research` ability.

**tech**
Reference to the `Tech` object that is researched.

**cost**
Resource amount spent to intiate the research. Cancelling the research results in a refund of the spent resources.

**research_time**
Time until the `Tech` object's patches are applied.

**research_sound**
Sounds that are played when the research finishes.

**requirements**
A set of requirements that unlock the technology for the researching game entity. Only one requirement needs to be fulfilled to trigger the unlock. If the set is empty, the technology is considered available from the start for the researching game entity.

## aux.resource.Resource

```python
Resource(Entity):
    name        : TranslatedString
    max_storage : int
```

Defines a resource that can be used in the game. Adding a resources will give an amount of 0 of that resource to all players. The current amount of resources can be influenced by the abilities and modifiers of game entities.

**name**
The name of the resource as a translatable string.

**max_storage**
Maximum amount of resources that can be stored at a time by the player.

## aux.resource.ResourceContingent

```python
ResourceContingent(Resource):
    min_amount : int
    max_amount : int
```

A `Resource` that creates a *contingent* which is temporarily usable by game entities. The size of the contingent is determined by two values:

* Static amounts can be aquired like normal resources
* Temporary amounts that can be provided by game entities with `ProvideContingent`

By using the contingent (see `UseContingent` ability), the current amount of resources is not reduced. Instead, the game entity will reserve parts of the contingent until it loses the ability or dies. When the whole contingent is reserved, no more game entities using it can be created.

Contingents can be utilized to implement mechanics like Population Space (AoE2) or Supply (Starcraft).

Note that it is also allowed to spend the static amounts determining the contingent size like normal resources.

**min_amount**
The minimum contingent size. Static and temporary amounts will be added to this value.

**max_amount**
The maximum contingent size.

## aux.resource.ResourceAmount

```python
ResourceAmount(Entity):
    type   : Resource
    amount : int
```

A fixed amount of a certain resource.

**type**
The resource.

**amount**
Amount of the resource.

## aux.resource.ResourceRate

```python
ResourceRate(Entity):
    type : Resource
    rate : float
```

A per-second rate of a certain resource.

**type**
The resource.

**rate**
Rate of the resource.

## aux.resource_spot.ResourceSpot

```python
ResourceSpot(Entity):
    resource               : Resource
    capacity               : int
    starting_amount        : int
    decay_rate             : float
    harvest_progress       : set(HarvestProgress)
    gatherer_limit         : int
    harvestable_by_default : bool
```

Amount of resources that is gatherable through the `Harvestable` ability of a game entity.

**resource**
Type of resource that can be harvested.

**capacity**
Maximum resource capacity of the resource spot.

**starting_amount**
Gatherable amount when the resource spot is created.

**decay_rate**
Determines how much resources are lost each second after the resource spot is activated (see `harvestable_by_default` for details).

**harvest_progress**
Can alter the game entity the resource spot belongs to after certain percentages of the  resource amount are harvested.

**gatherer_limit**
Limits the amount of gatherers that can access the resource spot simultaneously.

**harvestable_by_default**
Determines whether the resource spot is harvestable when it is created. If `true`, the resource spot will be accessible without any conditions as long as the corresponding `Harvestable` ability of the game entity is enabled. When set to false, the resource spot must be activated with a `MakeHarvestable` effect. The conditions under which the activation succeeds are decided by the `MakeHarvestable` resistance of the game entity the resource spot belongs to.

## aux.resource_spot.RestockableResourceSpot

```python
RestockableResourceSpot(ResourceSpot):
    destruction_time_limit : float
    restock_progress       : set(RestockProgress)
```

Resource spot that can be restocked after the contained resource amount reaches 0.

**destruction_time_limit**
Time until the resource spot is made permanently inaccessible. Afterwards, restocking is impossible.

**restock_progress**
Can alter the game entity the resource spot belongs to after certain percentages of the  resource amount are restocked.

## aux.sound.Sound

```python
Sound(Entity):
    play_delay : float
    sounds     : orderedset(file)
```

A collection of sound files that can be played by abilities of a game entity.

**play_delay**
Delay when the sound loops.

**sounds**
A set of sound files that are played in the order they are stored in the set.

## aux.state_machine.StateChanger

```python
StateChanger(Entity):
    enable_abilities  : set(Ability)
    disable_abilities : set(Ability)
    enable_modifiers  : set(Modifier)
    disable_modifiers : set(Modifier)
    priority          : int
```

State changes alter the *base state* of a game entity which is defined by the abilities and modifers stored in a `GameEntity` object. They are allowed to enable new and disable existing abilities as well as modifiers. Multiple state changes can be applied at once. Only abilities and modifiers with a priority *lower than or equal to* (<=) the one defined in the `StateChanger` object will be disabled.

**enable_abilities**
A set of abilities that are enabled when the state change is active.

**disable_abilities**
A set of abilities that are disabled when the state change is active and the abilities have a priority *lower than or equal to* (<=) the `priority` in the disabling `StateChanger` object.

**enable_modifiers**
A set of modifiers that are enabled when the state change is active.

**disable_modifiers**
A set of modifiers that are disabled when the state change is active and the modifiers have a priority *lower than or equal to* (<=) the `priority` in the disabling `StateChanger` object.

**priority**
Priority of a state change. The value can be negative. Abilities and modifiers belonging to the base state have an implicit priority of 0.

## aux.storage.Container

```python
Container(Entity):
    storage_elements : set(StorageElement)
    slots            : int
    carry_progress   : set(CarryProgress)
```

Used by the `Storage` ability to store definitions of how the stored game entities influence the storing game entity.

**storage_elements**
Contains the definitions for handling stored game entities as `StorageElement` objects.

**slots**
Defines how many slots for game entities the container has. Multiple game entities may be stacked in one slot depending on the `elements_per_slot` member in `StorageElement`.

**carry_progress**
A set of `CarryProgress` objects that can activate state changes and animation overrides when the container is filled.

## aux.storage.StorageElement

```python
StorageElement(Entity):
    storage_element   : GameEntity
    elements_per_slot : int
    conflicts         : set(StorageElement)
    state_change      : StateChanger
```

Defines how a stored game entity influences its storing game entity.

**storage_element**
The stored game entity to which this definition applies.

**elements_per_slot**
Defines how many game entities of the type referenced in `storage_element` can be stacked in one slot.

**conflicts**
A set of storage elements which cannot be in the container at the same time as this storage element.

**state_change**
Alters the base abilities and modifiers of the storing game entity when at least one game entity of the type referenced in `storage_element` is present in the container.

## aux.target_mode.TargetMode

```python
TargetMode(Entity):
```

Generalization object for target modes used by projectiles.

## aux.target_mode.type.CurrentPosition

```python
CurrentPosition(TargetMode):
```

Makes the projectile path end at the current postion of the target when the projectile spawned.

## aux.target_mode.type.ExpectedPosition

```python
ExpectedPosition(TargetMode):
```

Makes the projectile path end at the postion where the target is expected to be when the projectile is supposed to hit it.

## aux.taunt.Taunt

```python
Taunt(Entity):
    activation_message : text
    display_message    : TranslatedString
    sound              : Sound
```

A predefined message players can send to each other.

**activation_message**
The activation message that has to be typed into the chat console.

**display_message**
The displayed message after the taunt is activated as a `TranslatedString`.

**sound**
Sounds that are played after the taunt is activated.

## aux.tech.Tech

```python
Tech(Entity):
    name             : TranslatedString
    description      : TranslatedMarkupFile
    long_description : TranslatedMarkupFile
    updates          : orderedset(Patch)
```

An object that can apply changes through patching. It follows the standard implementation from most other strategy games. The `Tech` object only stores the patches that change the game state, while cost, research time, and requirements are decided by the `Research` ability of a game entity. By default, technologies can only be applied once and researched by one game entity at a time. Afterwards, the engine sets a flag that the `Tech` was applied and automatically forbids researching it again.

**name**
The name of the technology as a translatable string.

**description**
A description of the technology as a translatable markup file.

**long_description**
A longer description of the technology as a translatable markup file.

**updates**
Changes the game state through patches. Any members and objects can be patched. Normal `Patch` objects will only be applied to the player. To apply patches to other player with specific diplomatic stances, use `DiplomaticPatch`.

## aux.terrain.Terrain

```python
Terrain(Entity):
    name                      : TranslatedString
    terrain_graphic           : Terrain
    sound                     : Sound
    allowed_types             : set(GameEntityType)
    blacklisted_game_entities : set(GameEntity)
    ambience                  : set(TerrainAmbient)
```

Terrains define the properties of the ground which the game entities are placed on.

**name**
The name of the terrain as a translatable string.

**terrain_graphic**
Texture of the terrain (see `Terrain`). openage uses 3D terrain on which the texture is painted on.

**sound**
Ambient sound played when the camera of the player is looking onto the terrain.

**allowed_types**
Game entities that have one of the specified types (see `GameEntityType`) are allowed to traverse or be placed on the terrain.

**blacklisted_game_entities**
Used to blacklist game entities that have one of the types listed in `allowed_types`, but must not traverse or be placed on the terrain.

**ambience**
Defines ambient objects placed on the terrain through a set of `TerrainAmbient` objects.

## aux.terrain.TerrainAmbient

```python
TerrainAmbient(Entity):
    object      : Ambient
    max_density : int
```

An ambient game entity that is placed randomly on a chunk of terrain (10x10 tiles).

**object**
The game entity placed on the terrain.

**max_density**
Defines how many ambient objects are allowed to be placed on a chunk at maximum.

## aux.trade_route.TradeRoute

```python
TradeRoute(Entity):
    trade_resource    : Resource
    start_trade_post  : GameEntity
    end_trade_post    : GameEntity
```

Generalization object that defines a trade route between two game entities.

**trade_resource**
Resource that is traded on this trade route. The traded amount depends on the trade route type.

**start_trade_post**
The game entity where the traded resource is collected. The game entity must have a `TradePost` ability that contains this `TradeRoute` object.

**end_trade_post**
The game entity the traded resource is delivered to. The game entity must have a `TradePost` ability that contains this `TradeRoute` object.

## aux.trade_route.type.AoE1TradeRoute

```python
AoE1TradeRoute(Entity):
    exchange_resources : set(Resource)
    trade_amount       : int
```

Uses Age of Empires 1 rules for trading.

**exchange_resources**
The trading game entity exchanges `trade_amount` of a selected resource in this set by `trade_amount` of the resource defined by `trade_resource`.

**trade_amount**
Amount of resources traded each time.

## aux.trade_route.type.AoE2TradeRoute

```python
AoE2TradeRoute(Entity):
```

Uses Age of Empires 2 rules for trading. The trading game entity chooses the nearest possible `end_trade_post` from the `start_trade_post`. Calculation of the traded resource amount is based on this formula:

```
trade_amount = 0.46 * tiles_distance * ((tiles_distance / map_size) + 0.3)
```

## aux.translated.TranslatedObject

```python
TranslatedObject(Entity):
```

Generalization object for any objects that are or should be different depending on the language. Currently we support translations for strings, markup files and sounds.

## aux.translated.type.TranslatedMarkupFile

```python
TranslatedMarkupFile(TranslatedObject):
    translations : set(LanguageMarkupPair)
```

The translated versions of a longer text stored in markup files.

**translations**
All translations of the makrup files as language-file pairs (see `LanguageMarkupPair`).

## aux.translated.type.TranslatedSound

```python
TranslatedSound(TranslatedObject):
    translations : set(LanguageSoundPair)
```

The translated versions of a sound.

**translations**
All translations of the sound as language-sound pairs (see `LanguageSoundPair`).

## aux.translated.type.TranslatedString

```python
TranslatedString(TranslatedObject):
    translations : set(LanguageTextPair)
```

The translated versions of a string.

**translations**
All translations of the string as language-text pairs (see `LanguageTextPair`).

## aux.variant.Variant

```python
Variant(Entity):
    changes  : orderedset(Patch)
    priority : int
```

Variants can change the game entity when it is created. When variants are chosen depends on their type.

**changes**
The changes to the game entity as a set of patches. Only the created game entity is affected.

**priority**
When many variants are chosen, this member influences the order in which the patches from their `changes` member are applied. Patches from variants with higher priority value are applied first.

## aux.variant.type.AdjacencyVariant

```python
AdjacentTilesVariant(Variant):
    north      : optional(GameEntity)
    north_east : optional(GameEntity)
    east       : optional(GameEntity)
    south_east : optional(GameEntity)
    south      : optional(GameEntity)
    south_west : optional(GameEntity)
    west       : optional(GameEntity)
    north_west : optional(GameEntity)
```

A variant that is chosen based on adjacent game entities. Both the created game entity and the adjacent game entities must have the `TileRequirement` ability. From all `AdjacentVariant` variants the one with the most matches in all directions is chosen.

**north**
The desired game entity north of the created game entity. Does not have to be set.

**north_east**
The desired game entity north-east of the created game entity. Does not have to be set.

**east**
The desired game entity east of the created game entity. Does not have to be set.

**south_east**
The desired game entity south-east of the created game entity. Does not have to be set.

**south**
The desired game entity south of the created game entity. Does not have to be set.

**south_west**
The desired game entity south-west of the created game entity. Does not have to be set.

**west**
The desired game entity west of the created game entity. Does not have to be set.

**north_west**
The desired game entity north-west of the created game entity. Does not have to be set.

## aux.variant.type.RandomVariant

```python
RandomVariant(Variant):
    chance_share : float
```

From all variants of this type in the `variants` member of the game entity, one will be picked at random.

**chance_share**
The relative chance of the variant to be picked. Note that this is **not** a percentage chance. The value defines how likely it is for the variant to be chosen relative to the other `RandomVariant` objects.

**Example:**

* Random variant 1 with `chance_share = 1.0`
* Random variant 2 with `chance_share = 4.0`

The second variant is four times as likely to be picked. The absolute chances are:

* Random variant 1: 20\%
* Random variant 2: 80\%

## aux.variant.type.PerspectiveVariant

```python
PerspectiveVariant(Variant):
    angle : int
```

Variant depending on the placement angle of the game entity. Cuurently only works with the `PlacementMode` of type `Place` with the `allow_ratation` member set to true.

**angle**
Angle of the game entity. An angle of *0* points to the south-west direction.
# engine.effect

Reference documentation of the `engine.effect` module of the openage modding API.

## effect.Effect

```python
Effect(Object):
    properties : dict(abstract(EffectProperty), EffectProperty) = {}
```

Generalization object for all effects.

**properties**
Further specializes the effect beyond the standard behaviour.

The engine expects objects from the namespace `engine.effect.property.type` as keys. Values must always be an instance of the object used as key.

Standard behavior without properties:

* Effects are **only applied** if the resistor has a **matching** `Resistance` object. Matching can be further defined depending on the concrete effect.
* Applying them **costs nothing**.
* Will be applied to targets **with any diplomatic stance**.
* They only affect the target chosen with the `ApplyContinuousEffect`/`ApplyDiscretEffect` ability.
* Effects have no preferred order of application.

Properties:

* `Area`: Effects are applied to all game entities in a circular area around the target.
* `Cost`: Makes effects cost attribute points or resources.
* `Diplomatic`: Effects only apply to game entities with specified diplomatic stances.
* `Priority`: Sets the preferred order of application in comparison to other effects.

## effect.property.EffectProperty

```python
EffectProperty(Object):
    pass
```

Generalization object for all properties of effects.

## effect.property.type.Area

```python
Area(EffectProperty):
    range   : float
    dropoff : DropoffType
```

Apply the effect to game entities in a circular area around the target.

**range**
The radius of the area in which game entities are affected.

**dropoff**
Changes the effectiveness of the effect based on the distance to the center of the circle (see `DropoffType`).

## effect.property.type.Cost

```python
Cost(EffectProperty):
    cost : Cost
```

Make the effect cost attribute points or resources.

**cost**
The costs for the effect as a `Cost` object.

## effect.property.type.Diplomatic

```python
Diplomatic(EffectProperty):
    stances : set(children(DiplomaticStance))
```

Make the effect only applicable to game entities of players with a specified diplomatic stance.

**stances**
If the target is owned by a player has *any* of the specified diplomatic stances, the effect is applied.

## effect.property.type.Priority

```python
Priority(EffectProperty):
    priority : int
```

Assigns a priority to the effect. Priorities determine the order of application when multiple effects are applied via an `EffectBatch`.

**priority**
Priority value used for comparison with other effects. Effects with higher values are preferred before effects with lower values.

## effect.continuous.ContinuousEffect

```python
ContinuousEffect(Effect):
    pass
```

Generalization object for effects that are applied at a per-second rate.

## effect.continuous.flat_attribute_change.FlatAttributeChange

```python
FlatAttributeChange(ContinuousEffect):
    type              : children(AttributeChangeType)
    min_change_rate   : optional(AttributeRate) = None
    max_change_rate   : optional(AttributeRate) = None
    change_rate       : AttributeRate
    ignore_protection : set(ProtectingAttribute)
```

Generalization object for effects that change the resistor's current attribute values at a flat per-second rate. The change value can optionally be limited to an interval with `min_change_rate` as the lower bound and `max_change_rate` as the upper bound.

Note that you cannot use this effect object directly and have to choose one of the specializations `FlatAttributeChangeDecrease` or `FlatAttributeChangeIncrease`.

**type**
The effect will be matched with a `resistance.continuous.flat_attribute_change.type` namespace object that stores the same `AttributeChangeType` object in its `type` member. Otherwise, the effect will not be applied.

**min_change_rate**
The applied change rate can never go lower than the specified rate.

**max_change_rate**
The applied change rate can never go higher than the specified rate.

**change_rate**
The gross per-second rate at which the attribute points of the resistor change. The net change rate (applied rate) is calculated by subtracting the resistor's `block_rate` from the effector's `change_rate`.

```math
applied\_rate = change\_rate - block\_rate
```

The applied rate is further bound by the interval defined by `min_change_rate` and `max_change_rate` if these members are set.

**ignore_protection**
Ignores the `ProtectingAttribute`s in the set when changing the attributes of the target.

## effect.continuous.flat_attribute_change.type.FlatAttributeChangeDecrease

```python
FlatAttributeChangeDecrease(FlatAttributeChange):
    pass
```

Specialization of the continuous `FlatAttributeChange` effect that *decreases* the resistor's current attribute value at a per-second rate.

## effect.continuous.flat_attribute_change.type.FlatAttributeChangeIncrease

```python
FlatAttributeChangeIncrease(FlatAttributeChange):
    pass
```

Specialization of the continuous `FlatAttributeChange` effect that *increases* the resistor's current attribute value at a per-second rate.

## effect.continuous.lure.type.Lure

```python
Lure(ContinuousEffect):
    type                        : children(LureType)
    destination                 : set(GameEntity)
    min_distance_to_destination : float
```

Makes the target move to another game entity.

**type**
The effect will be matched with a `resistance.continuous.lure.type` namespace object that stores the same `LureType` object in its `type` member. Otherwise, the effect will not be applied.

**destination**
Possible destinations the target can move to. Whichever game entity is closest will be chosen.

**min_range_to_destination**
Minimum distance the target has to have to any destination.

## effect.continuous.time_relative_attribute_change.TimeRelativeAttributeChange

```python
TimeRelativeAttributeChange(ContinuousEffect):
    type              : children(AttributeChangeType)
    total_change_time : float
    ignore_protection : set(ProtectingAttribute)
```

Generalization object for effects that change the resistor's current attribute values at a per-second rate relative to the `max_value` of their attribute settings. The change rate is scaled such that it would increase the attribute points of the resistor from `min_value` to `max_value` (or decrease them from `max_value` to `min_value`, respectively) in a fixed amount of time. The current attribute value of the target is not considered. Calculating the change rate can be done by using this formula:

```math
applied\_rate = resistor\_max\_value / total\_change\_time
```

*Example*: Consider a resistor with 50 max HP / 0 min HP and an effector with a `TimeRelativeAttributeDecrease` time of 5 seconds. The per-second change rate is calculated by dividing the maximum HP value by the time requirement of the effect. Hence, the change rate is 10HP/s. This rate is fix as long as the maximum HP value does not change. If the resistor currently has 30 HP, it would arrive at 0 HP in 3 seconds.

**type**
The effect will be matched with a `resistance.continuous.time_relative_attribute_change.type` namespace object that stores the same `AttributeChangeType` object in its `type` member. Otherwise, the effect will not be applied.

**total_change_time**
The total time needed to change the resistors attribute points from `max_value` to `min_value` (for `TimeRelativeAttributeDecrease`) or from `min_value` to `max_value` (for `TimeRelativeAttributeIncrease`).

**ignore_protection**
Ignores the `ProtectingAttribute`s in the set when changing the attributes of the target.

## effect.continuous.time_relative_attribute_change.type.TimeRelativeAttributeDecrease

```python
TimeRelativeAttributeDecrease(TimeRelativeAttributeChange):
    pass
```

Specialization of the continuous `TimeRelativeAttributeChange` effect that *decreases* the resistor's current attribute value in a fixed amount of time relative to their attribute's `max_value`.

## effect.continuous.time_relative_attribute_change.type.TimeRelativeAttributeIncrease

```python
TimeRelativeAttributeIncrease(TimeRelativeAttributeChange):
    pass
```

Specialization of the continuous `TimeRelativeAttributeChange` effect that *increases* the resistor's current attribute value in a fixed amount of time relative to their attribute's `max_value`.

## effect.continuous.time_relative_progress_change.TimeRelativeProgressChange

```python
TimeRelativeProgressChange(ContinuousEffect):
    type              : children(ProgressType)
    total_change_time : float
```

Generalization object for effects that changes a resistor's current progress amount at a per-second rate relative to 100%. The change rate is scaled such that it would increase the specified progress amount of the resistor from 0% to 100% (or decrease it from 100% to 0%, respectively) in a fixed amount of time. The current progress amount is not considered. Calculating the change rate can be done by using this formula:

```math
applied\_rate = 100 / total\_change\_time
```

*Example*: Consider a constructable resistor and an effector with a `TimeRelativeProgressIncrease` time of 10 seconds. The per-second change rate is calculated by dividing 100% by the time requirement of the effect. Hence, the change rate is 10%/s. This rate is fix. If the resistor currently has 30% construction progress, it would be fully constructed in 7 seconds.

**type**
The effect will be matched with a `resistance.continuous.time_relative_progress_change.type` namespace object that stores the same `ProgressType` object in its `type` member. Otherwise, the effect will not be applied.

**total_change_time**
The total time needed to change the resistors attribute points from 100% to 0% (for `TimeRelativeProgressDecrease`) or from 0% to 100% (for `TimeRelativeProgressIncrease`).

## effect.continuous.time_relative_progress.type.TimeRelativeProgressDecrease

```python
TimeRelativeProgressDecrease(TimeRelativeProgressChange):
    pass
```

Specialization of the continuous `TimeRelativeProgressChange` effect that *decreases* the resistor's progress amount in a fixed amount of time relative to 100%.

## effect.continuous.time_relative_progress.type.TimeRelativeProgressIncrease

```python
TimeRelativeProgressIncrease(TimeRelativeProgressChange):
    pass
```

Specialization of the continuous `TimeRelativeProgressChange` effect that *increases* the resistor's progress amount in a fixed amount of time relative to 100%.
´
## effect.discrete.DiscreteEffect

```python
DiscreteEffect(Effect):
    pass
```

Generalization object for effects that are applied immediately.

## effect.discrete.convert.Convert

```python
Convert(DiscreteEffect):
    type               : children(ConvertType)
    min_chance_success : optional(float) = None
    max_chance_success : optional(float) = None
    chance_success     : float
    cost_fail          : optional(Cost) = None
```

Change the owner of the target unit to the player who owns the effector game entity.

**type**
The effect will be matched with a `resistance.discrete.convert.type` namespace object that stores the same `ConvertType` object in its `type` member. Otherwise, the effect will not be applied.

**min_chance_success**
The applied chance can never go lower than the specified percentage.

**max_chance_success**
The applied chance can never go higher than the specified percentage.

**chance_success**
Gross chance for the conversion to succeed as a percentage chance. The percentage should be stored as a float value between *0.0* and *1.0*. The net chance (applied chance) of success is calculated by subtracting the resistor's `chance_resist` from the effector's `chance_success`.

```math
applied\_chance = chance\_success - chance\_resist
```

Any value below *0.0* is an automatic failure, while any value above *1.0* is an automatic success. The applied chance is further bound by the interval defined by `min_chance_success` and `max_chance_success`, if these members are set.

**cost_fail**
The amount of attribute points or resources removed from the effector if the conversion fails.

## effect.discrete.convert.type.AoE2Convert

```python
AoE2Convert(Convert):
    skip_guaranteed_rounds : int
    skip_protected_rounds  : int
```

Specialized conversion effect that is implemented in AoE2. The convert chance at the start is guaranteed to be *0.0* for `guaranteed_resist_rounds` rounds and guaranteed to be *1.0* after `protected_rounds` rounds (both defined by the `resistance.discrete.convert.type.AoE2Convert` object).

When the effector stops applying the effect, the resistor's protected rounds are increased until they reach their maximum value again. Running out of the range of the effector does not count as stopping the effect application.

**skip_guaranteed_rounds**
Lowers the number of rounds that the resistor is guaranteed to resist every time.

**skip_protected_rounds**
Lowers the number of rounds that are needed for the success chance to always be *1.0*.

## effect.discrete.flat_attribute_change.FlatAttributeChange

```python
FlatAttributeChange(DiscreteEffect):
    type              : children(AttributeChangeType)
    min_change_value  : optional(AttributeAmount) = None
    max_change_value  : optional(AttributeAmount) = None
    change_value      : AttributeAmount
    ignore_protection : set(ProtectingAttribute)
```

Generalization object for effects that change a resistor's current attribute value by a flat amount. The change value can optionally be limited to an interval with `min_change_value` as the lower bound and `max_change_value` as the upper bound.

Note that you cannot use this effect object directly and have to choose one of the specializations `FlatAttributeChangeDecrease` or `FlatAttributeChangeIncrease`.

**type**
The effect will be matched with a `resistance.discrete.flat_attribute_change.type` namespace object that stores the same `AttributeChangeType` object in its `type` member. Otherwise, the effect will not be applied.

**min_change_value**
The applied change value can never go lower than the specified amount.

**max_change_value**
The applied change value can never go higher than the specified amount.

**change_value**
The gross amount by that the attribute points of the resistor change. The net change value (applied value) is calculated by subtracting the resistor's `block_value` from the effector's `change_value`.

```math
applied\_value = change\_value - block\_value
```

The applied rate is further bound by the interval defined by `min_change_value` and `max_change_value`, if these members are set.

**ignore_protection**
Ignores the `ProtectingAttribute`s in the set when changing the attributes of the target.

## effect.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease

```python
FlatAttributeChangeDecrease(FlatAttributeChange):
    pass
```

Specialization of the discrete `FlatAttributeChange` effect that decreases the resistor's current attribute value by a flat amount.

## effect.discrete.flat_attribute_change.type.FlatAttributeChangeIncrease

```python
FlatAttributeChangeIncrease(FlatAttributeChange):
    pass
```

Specialization of the discrete `FlatAttributeChange` effect that increases the resistor's current attribute value by a flat amount.

## effect.discrete.make_harvestable.type.MakeHarvestable

```python
MakeHarvestable(DiscreteEffect):
    resource_spot : ResourceSpot
```

Makes a resource spot harvestable, if it is not already harbestable by default.

**resource_spot**
Resource spot that should be made harvestable. The effect will be matched with a `resistance.discrete.make_harvestable.type` namespace object that stores the same `ResourceSpot` object in its `resource_spot` member. Additionally, the target needs to have a `Harvestable` ability that contains the resource spot.

## effect.discrete.send_to_container.type.SendToContainer

```python
SendToContainer(DiscreteEffect):
    type     : children(SendToStorageType)
    storages : set(EntityContainer)
```

Makes the target move to and enter the nearest game entity where it can be stored. The resistor needs an `EnterContainer` ability for at least one of the containers for this to work.

**type**
The effect will be matched with a `resistance.discrete.send_to_container.type` namespace object that stores the same `SendToStorageType` object in its `type` member. Otherwise, the effect will not be applied.

**storages**
Containers the target can enter. The target will choose the nearest game entity which it references one of these containers and for which it has a `EnterContainer` ability.

# engine.effect

Reference documentation of the `engine.effect` module of the openage modding API.

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
applied\_rate = change\_rate - block\_rate
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

```math
applied\_rate = resistor\_max\_value / total\_change\_time
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

```math
applied\_rate = 100 / total\_change\_time
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
applied\_chance = chance\_success - chance\_resist
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
applied\_value = change\_value - block\_value
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

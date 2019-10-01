# engine.resistance

Reference documentation of the `engine.resistance` module of the openage modding API.

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
applied\_rate = change\_rate - block\_rate
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
applied\_chance = chance\_success - chance\_resist
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
applied\_value = change\_value - block\_value
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

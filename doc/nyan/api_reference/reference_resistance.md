# engine.resistance

Reference documentation of the `engine.resistance` module of the openage modding API.

## resistance.Resistance

```python
Resistance(Object):
    properties : dict(abstract(ResistanceProperty), ResistanceProperty) = {}
```

Generalization object for all resistances.

**properties**
Further specializes the resistance beyond the standard behaviour.

The engine expects objects from the namespace `engine.resistance.property.type` as keys. Values must always be an instance of the object used as key.

Standard behavior without properties:

* Effects are **only applied** if the resistor has a **matching** `Resistance` object. Matching can be further defined depending on the concrete effect.
* Resisting **costs nothing**.
* When multiple effectors are applying effects at the same time, results are accumulated without adjustments.

Properties:

* `Cost`: Makes resistances cost attribute points or resources.
* `Stacked`: Adjust effect calculation when multiple effectors are applying effects simultaneously.

## resistance.property.ResistanceProperty

```python
ResistanceProperty(Object):
    pass
```

Generalization object for all properties of resistances.

## resistance.property.type.Cost

```python
Cost(ResistanceProperty):
    cost : Cost
```

Make the resistance cost attribute points or resources.

**cost**
The amount of attribute points or resources removed from the resistor.

## resistance.property.type.Stacked

```python
Stacked(ResistanceProperty):
    stack_limit       : int
    calculation_type  : CalculationType
    distribution_type : DistributionType
```

Configure the calculation of the applied value when a resistor if affected by multiple effectors.

The final applied value is the product of

* The accumulated and averaged change values of all effects (`dist_value`)
* An influence factor based on the number of effectors (`stack_factor`).

```math
applied\_value = stack\_factor * dist\_value
```

The calculation method for both of these values is set in this property.

**stack_limit**
Maximum number of effectors that can apply their effects on the resistor. The effectors are chosen on a *first-come-first-serve* basis. Setting the stack limit to `inf` allows an unlimited number of effectors.

**calculation_type**
The calculation method used to determine the *stack_factor*.

**distribution_type**
The calculation method used to determine the *dist_value*.

## resistance.continuous.ContinuousResistance

```python
ContinuousResistance(Resistance):
    pass
```

Generalization object for resistances to continuous effects.

## resistance.continuous.flat_attribute_change.FlatAttributeChange

```python
FlatAttributeChange(ContinuousResistance):
    type       : children(AttributeChangeType)
    block_rate : set(AttributeRate)
```

Generalization object for resistances to continuous `FlatAttributeChange` effects.

Note that you cannot use this resistance object directly and have to choose one of the specializations `FlatAttributeChangeDecrease` or `FlatAttributeChangeIncrease`.

**type**
Effects of type `effect.continuous.flat_attribute_change.FlatAttributeChange` are matched to this resistance if they store the same `AttributeChangeType` object in their `type` member. Otherwise, the effect will not be applied.

**block_rate**
The per-second attribute rate that is blocked by the resistor. The net change rate (applied rate) is calculated by subtracting the resistor's `block_rate` from the effector's `change_rate`.

```math
applied\_rate = change\_rate - block\_rate
```

The applied rate is further bound by the interval defined by `min_change_rate` and `max_change_rate` in the effect, if these members are set.

## resistance.continuous.flat_attribute_change.type.FlatAttributeChangeDecrease

```python
FlatAttributeChangeDecrease(FlatAttributeChange):
    pass
```

Specialization of the continuous `FlatAttributeChange` resistance that blocks the effector's attribute decrease rate.


## resistance.continuous.flat_attribute_change.type.FlatAttributeChangeIncrease

```python
FlatAttributeChangeIncrease(FlatAttributeChange):
    pass
```

Specialization of the continuous `FlatAttributeChange` resistance that blocks the effector's attribute increase rate.

## resistance.continuous.lure.type.Lure

```python
Lure(ContinuousResistance):
    type : children(LureType)
```

Resistance to the `Lure` effect.

**type**
Effects of type `effect.continuous.lure.type.Lure` are matched to this resistance if they store the same `LureType` object in their `type` member. Otherwise, the effect will not be applied.

## resistance.continuous.time_relative_attribute_change.TimeRelativeAttributeChange

```python
TimeRelativeAttributeChange(ContinuousResistance):
    type : children(AttributeChangeType)
```

Generalization object for resistances to continuous `TimeRelativeAttributeChange` effects.

Note that you cannot use this resistance object directly and have to choose one of the specializations `TimeRelativeAttributeDecrease` or `TimeRelativeAttributeIncrease`.

**type**
Effects of type `effect.continuous.time_relative_attribute_change.TimeRelativeAttributeChange` are matched to this resistance if they store the same `AttributeChangeType` object in their `type` member. Otherwise, the effect will not be applied.

## resistance.continuous.time_relative_attribute_change.type.TimeRelativeAttributeDecrease

```python
TimeRelativeAttributeDecrease(TimeRelativeAttributeChange):
    pass
```

Specialization of the continuous `TimeRelativeAttributeChange` resistance that decreases the resistor's current attribute value in a fixed amount of time relative to their attribute's `max_value`.

## resistance.continuous.time_relative_attribute_change.type.TimeRelativeAttributeIncrease

```python
TimeRelativeAttributeIncrease(TimeRelativeAttributeChange):
    pass
```

Specialization of the continuous `TimeRelativeAttributeChange` resistance that increases the resistor's current attribute value in a fixed amount of time relative to their attribute's `max_value`.

## resistance.continuous.time_relative_progress.TimeRelativeProgressChange

```python
TimeRelativeProgressChange(ContinuousResistance):
    type : children(ProgressType)
```

Generalization object for resistances to continuous `TimeRelativeProgressChange` effects.

Note that you cannot use this resistance object directly and have to choose one of the specializations `TimeRelativeProgressDecrease` or `TimeRelativeProgressIncrease`.

**type**
Effects of type `effect.continuous.time_relative_progress.TimeRelativeProgressChange` are matched to this resistance if they store the same `ProgressType` object in their `type` member. Otherwise, the effect will not be applied.

## resistance.continuous.time_relative_progress.type.TimeRelativeProgressDecrease

```python
TimeRelativeProgressDecrease(TimeRelativeProgressChange):
    pass
```

Specialization of the continuous `TimeRelativeProgressChange` resistance that decreases the resistor's progress amount in a fixed amount of time relative to 100%.

## resistance.continuous.time_relative_progress.type.TimeRelativeProgressIncrease

```python
TimeRelativeProgressIncrease(TimeRelativeProgressChange):
    pass
```

Specialization of the continuous `TimeRelativeProgressChange` resistance that increases the resistor's progress amount in a fixed amount of time relative to 100%.

## resistance.discrete.DiscreteResistance

```python
DiscreteResistance(Resistance):
    pass
```

Generalization object for resistances to discrete effects.

## resistance.discrete.convert.Convert

```python
Convert(DiscreteResistance):
    type          : children(ConvertType)
    chance_resist : float
```

Resistance to the `Convert` effect.

**type**
Effects of type `effect.discrete.convert.Convert` are matched to this resistance if they store the same `ConvertType` object in their `type` member. Otherwise, the effect will not be applied.

**chance_resist**
Percentage amount subtracted from the effector's success chance. The percentage should be stored as a float value between *0.0* and *1.0*. The net chance (applied chance) of success is calculated by subtracting the resistor's `chance_resist` from the effector's `chance_success`.

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
    type        : children(AttributeChangeType)
    block_value : set(AttributeAmount)
```

Generalization object for resistances to discrete `FlatAttributeChange` effects.

Note that you cannot use this resistance object directly and have to choose one of the specializations `FlatAttributeChangeDecrease` or `FlatAttributeChangeIncrease`.

**type**
Effects of type `effect.discrete.flat_attribute_change.FlatAttributeChange` are matched to this resistance if they store the same `AttributeChangeType` object in their `type` member. Otherwise, the effect will not be applied.

**change_value**
The flat attribute amount that is blocked by the resistor. The net change value (applied value) is calculated by subtracting the resistor's `block_value` from the effector's `change_value`.

```math
applied\_value = change\_value - block\_value
```

The applied value is further bound by the interval defined by `min_change_value` and `max_change_value` in the effect, if these members are set.

## resistance.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease

```python
FlatAttributeChangeDecrease(FlatAttributeChange):
    pass
```

Specialization of the discrete `FlatAttributeChange` resistance that blocks the effector's attribute decrease value.


## resistance.discrete.flat_attribute_change.type.FlatAttributeChangeIncrease

```python
FlatAttributeChangeIncrease(FlatAttributeChange):
    pass
```

Specialization of the discrete `FlatAttributeChange` resistance that blocks the effector's attribute increase value.

## resistance.discrete.make_harvestable.type.MakeHarvestable

```python
MakeHarvestable(DiscreteResistance):
    resource_spot     : ResourceSpot
    resist_condition  : set(LogicElement)
```

Resistance to the `MakeHarvestable` effect.

**resource_spot**
Resource spot that should be made harvestable. Effects of type `effect.discrete.make_harvestable.type.MakeHarvestable` are matched to this resistance if they store the same `ResourceSpot` object in their `resource_spot` member. Additionally, the target needs to have a `Harvestable` ability that contains the resource spot.

**resist_condition**
Condition which must he fulfilled to make the resource spot harvestable.

## resistance.discrete.send_to_container.type.SendToContainer

```python
SendToContainer(DiscreteResistance):
    type              : children(SendToContainerType)
    search_range      : float
    ignore_containers : set(EntityContainer)
```

Resistance to the `SendToContainer` effect.

**type**
Effects of type `effect.discrete.send_to_container.type.SendToContainer` are matched to this resistance if they store the same `SendToContainerType` object in their `type` member. Otherwise, the effect will not be applied.

**search_range**
The range in which the resistor will search for a container.

**ignore_containers**
Excludes the containers from the set when searching for a target container.

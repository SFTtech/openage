# engine.aux

Reference documentation of the `engine.aux` module of the openage modding API.

## aux.accuracy.Accuracy

```python
Accuracy(Entity):
    accuracy             : float
    accuracy_dispersion  : float
    dispersion_dropoff   : DropOffType
    target_types         : set(GameEntityType)
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
* The overridden ability is deactivated
* The override animation would be the same as the standard animation

**ability**
The ability whose animations should be overridden. This member can reference a specific ability of the game entity or an API ability. If an API ability is referenced, all its instances in the ability set of the game entity will be overridden.

**animations**
The replacement animations of the override.

**priority**
Priority of the override. Overrides are only executed if their priority is *greater than or equal to* (>=) an already existing override. The default animation of an ability always has a priority of 0.

## aux.animation_override.AnimationOverride

```python
Reset(AnimationOverride):
    animations = {}
    priority   = 0
```

Resets the animation of the specified ability by removing the current animation override.

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

A special type for `FlatAttributeChange`. Effects with this type are only evaluated if the accumulated applied value or applied rate of all other effects is outside of a specified interval. The interval is defined by the `FlatAttributeChange` object that has its `type` member set to `Fallback`. Upper and lower bounds are `[min_change_rate,max_change_rate]` (continuous effects) and `[min_change_value,max_change_value]` (discrete effects). The fallback effect is also evaluated if no other `FlatAttributeChange` effect is present or matched to any resistances. However, fallback effects still needs to be matched against a resistance object with `type` set to `Fallback`.

For example, effects that utilize fallback can be used to model minimum or maximum damage of a game entity.

## aux.calculation_type.CalculationType

```python
CalculationType(Entity):
    pass
```

Generalization object for the calculation method for the influence factor of a resistance with the `Stacked` property.

## aux.calculation_type.type.Hyperbolic

```python
Hyperbolic(CalculationType):
    shift_x      : int
    shift_y      : int
    scale_factor : float
```

Calculates the influence factor using the hyperbolic equation. This will *decrease* the influence factor towards `shift_y` the more effectors are present.

```math
influence\_factor = scale\_factor \ (num\_effectors - shift\_x) + shift\_y
```

**shift_x**
Horizontal shift in the hyperbola equation.

**shift_y**
Vertical shift in the hyperbola equation.

**scale_factor**
Scale factor in the hyperbola equation.

## aux.calculation_type.type.Linear

```python
Linear(CalculationType):
    shift_x      : int
    shift_y      : int
    scale_factor : float
```

Calculates the influence factor using the linear equation. This will *increase* the influence factor the more effectors are present.

```math
influence\_factor = scale\_factor * (num\_effectors - shift\_x) + shift\_y
```

**shift_x**
Horizontal shift in the linear equation.

**shift_y**
Vertical shift in the linear equation.

**scale_factor**
Scale factor in the linear equation.

## aux.calculation_type.type.NoStack

```python
NoStack(CalculationType):
    pass
```

Sets the influence factor to 1 regardless of the number of effectors. Note that this is different to setting the stack limit to 1, since the distribution type can still be influenced by multiple effectors.

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

Civilization customize the base state of the game for a player. This includes availability of units, buildings and techs as well as changing their abilities and modifiers plus their individual members.

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
    game_entity     : GameEntity
    variants        : set(Variant)
    cost            : Cost
    creation_time   : float
    creation_sounds : set(Sound)
    condition       : set(LogicElement)
    placement_modes : set(PlacementMode)
```

Defines preconditions, placement and spawn configurations for a new instance of a game entity created by a `Create` ability.

**game_entity**
Reference to the `GameEntity` object that is spawned.

**variants**
Variants can alter the game entity before they are created. The requirement and extent of the changes depends on the `Variant` object.

**cost**
Resource amount spent to initiate creation. Cancelling the creation results in a refund of the spent resources.

**creation_time**
Time until the game entity is spawned from the creating game entity.

**creation_sounds**
Sounds that are played on creating an instance of the game entity.

**condition**
Conditions that unlock the creatable game entity for the creating game entity. Only one condition needs to be fulfilled to trigger the unlock. If the set is empty, the game entity is considered available from the start for the creating game entity.

**placement_modes**
Decides where and how the game entity instance is spawned as a `PlacementMode` object.

## aux.diplomatic_stance.DiplomaticStance

```python
DiplomaticStance(Entity):
```

Generalization object for diplomatic stances that can be used for diplomacy ingame. Diplomatic stances also define which player can use the abilities, modifiers and effects of a game entity.

## aux.diplomatic_stance.Any

```python
Any(DiplomaticStance):
    pass
```

Can be used to address any diplomatic stance.

## aux.diplomatic_stance.type.Self

```python
Self(DiplomaticStance):
```

The diplomatic stance of a player towards themselves.

## aux.distribution_type.DistributionType

```python
DistributionType(Entity):
    pass
```

Generalization object for the calculation method for the distribution value of a resistance with the `Stacked` property.

## aux.distribution_type.type.Mean

```python
Mean(DistributionType):
    pass
```

Calculates the distribution value by using the mean of all change values of effectors.

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

## aux.effect_batch.EffectBatch

```python
EffectBatch(Entity):
    effects    : set(DiscreteEffect)
    properties : dict(abstract(children(BatchProperty)), children(BatchProperty))
```

Generalization object for a collection of discrete effects. Batches combine the discrete effects to transactions. Batches - like effects - can have properties to configure the batch application.

## aux.effect_batch.property.BatchProperty

```python
BatchProperty(Entity):
    pass
```

Generalization object for all properties of effect batches.

## aux.effect_batch.property.type.Chance

```python
Chance(BatchProperty):
    chance : float
```

The batch has a random chance to be applied.

**chance**
Chance of the batch to be applied. Muste be a value between 0.0 and 1.0.

## aux.effect_batch.property.type.Priority

```python
Priority(BatchProperty):
    priority : int
```

Assigns a priority value to the batch that is used to determine the order of batch application in `ApplyDiscreteEffect`.

**priority**
The priority of the batch. When comparing with other batches, the batch with higher values are applied first.

## aux.effect_batch.type.ChainedBatch

```python
ChainedBatch(EffectBatch):
    pass
```

The effects in the batch are applied using the effect property `Priority` for ordering. If two or more effects have the same priority, it is assumed that the order for these effects does not matter. Effects are applied as long there are matching resistance objects stored by the resistor. If an effect cannot be applied because of a missing match, the application of the remaining effects is canceled.

## aux.effect_batch.type.OrderedBatch

```python
OrderedBatch(EffectBatch):
    pass
```

The effects in the batch are applied using the effect property `Priority` for ordering. If two or more effects have the same priority, it is assumed that the order for these effects does not matter.

## aux.effect_batch.type.UnorderedBatch

```python
UnorderedBatch(EffectBatch):
    pass
```

The effects in the batch are applied without any particular order.

## aux.exchange_mode.ExchangeMode

```python
ExchangeMode(Entity):
    fee_multiplier : float
```

Generalization object for exchange modes of the `ExchangeResource` ability.

**fee_multiplier**
Share of resource amounts that have to be payed as a fee for using the exchange mode. The fee is additional to the exchanged resources.

## aux.exchange_mode.type.Buy

```python
Buy(ExchangeMode):
    pass
```

Buy an amount of `resource_a` by paying with an amount of `resource_b`. `resource_a * fee_multiplier` is charged as the additional fee.

## aux.exchange_mode.type.Sell

```python
Sell(ExchangeMode):
    pass
```

Sell an amount of `resource_a` and receive an amount of `resource_b`. `resource_a * fee_multiplier` is charged as the additional fee.

## aux.exchange_rate.ExchangeRate

```python
ExchangeRate(Entity):
    base_price   : float
    price_adjust : optional(dict(ExchangeMode, PriceMode))
    price_pool   : optional(PricePool)
```

Defines an exchange rate for the resources in the `ExchangeResources` ability.

**base_price**
Price per unit at the start of the game.

**price_adjust**
Method to adjust the current price of the exchange rate at runtime. This method will be used every time the `ExchangeResource` ability is used. Different types of adjustment can be configured for every exchange mode.

**price_pool**
Can be used to sync the current price at runtime across game entities. All exchange rates that use the same price pool share the same price at runtime. If `ExchangeRate` objects reference the same `PricePool`, but have different `base_price` values defined, the price at the start of the game is set to the lowest `base_price` value.

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
    ordering_priority : int
```

Subdivision of a formation. It defines the structure and placement of game entities when the formation is formed.

**ordering_priority**
Ordering priority in relation to other subformations. Formations are ordered in with descending priority, i.e. the subformation with the highest priority is placed in front.

## aux.game_entity.GameEntity

```python
GameEntity(Entity):
    types     : set(GameEntityType)
    abilities : set(Ability)
    modifiers : set(Modifier)
```

For definition of all ingame objects, including units, buildings, items, projectiles and ambience. Their capabilities are handled through `Ability` and `Modifier` API objects stored in the members.

**types**
Classification of the game entity via `GameEntityType` objects.

**abilities**
Define what the game entity *does* and *is* through `Ability` objects.

**modifiers**
Change the stats of abilities belonging to the game entity. Mostly used to give boni or mali in certain situations (see `Modifier`).

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
    search_range       : float
    ability_preference : orderedset(Ability)
    type_preference    : orderedset(GameEntityType)
```

Generalization object for activity stances for the `GameEntityStance` ability.

**search_range**
Defines the range in which the game entity will lok for targets.

**ability_preference**
The abilities which the game entity will execute or search targets for. Their order in the set defines the priority of usage.

**type_preference**
Determines which game entity types are prioritized as targets. Their order in the set defines the priority of usage. Game entities with types that are not in the set will be ignored.

## aux.game_entity_stance.type.Aggressive

```python
Aggressive(GameEntityStance):
```

The game entity will use ranged abilities or move to the nearest target in its line of sight to use other abilities. If the target gets out of the line of sight, the game entity searches for a new target. When no new target can be found, the game entity stops moving and returns to an idle state.

## aux.game_entity_stance.type.Defensive

```python
Defensive(GameEntityStance):
```

The game entity will use ranged abilities or move to the nearest target in its line of sight to use other abilities. If the target gets out of range or the line of sight, the game entity searches for a new target. When no new target can be found, the game entity returns to its original position and returns to an idle state.

## aux.game_entity_stance.type.Passive

```python
Passive(GameEntityStance):
```

The game entity will stay at its current position and only reacts to manual commands given by players. Abilities in `ability_preference` will be ignored.

## aux.game_entity_stance.type.StandGround

```python
StandGround(GameEntityStance):
```

The game entity will stay at its current position.

## aux.game_entity_type.GameEntityType

```python
GameEntityType(Entity):
```

Classification for a game entity.

## aux.game_entity_type.Any

```python
Any(GameEntityType):
    pass
```

Can be used to address any game entity, even ones that have no `GameEntityType` assigned.

## aux.graphics.Animation

```python
Animation(Entity):
    sprite : file
```

Points to a openage sprite definition file in the `.sprite` format. The specified animation can be used by `AnimatedAbility` objects.

## aux.graphics.Palette

```python
Palette(Entity):
    palette : file
```

Points to a openage palette definition file in the `.opal` format.

## aux.graphics.Terrain

```python
Terrain(Entity):
    sprite : file
```

Points to a openage terrain definition file in the `.terrain` format. This object is used by the `terrain.Terrain` object that defines properties of ingame terrain.

## aux.herdable_mode.HerdableMode

```python
HerdableMode(Entity):
    pass
```

Used by the `Herdable` ability to determine who gets ownership of the herdable.

## aux.herdable_mode.type.ClosestHerding

```python
ClosestHerding(HerdableMode):
    pass
```

The player with the closest herding game entity gets ownership.

## aux.herdable_mode.type.LongestTimeInRange

```python
LongestTimeInRange(HerdableMode):
    pass
```

The player of the game entity which has been in range for the longest time gets ownership.

## aux.herdable_mode.type.MostHerding

```python
MostHerding(HerdableMode):
    pass
```

The player with the most herding game entities in range gets ownership.

## aux.hitbox.Hitbox

```python
Hitbox(Entity):
    radius_x : float
    radius_y : float
    radius_z : float
```

Defines the hitbox of a game entity.

**radius_x**
Width of the game entity.

**radius_y**
Length of the game entity.

**radius_z**
Height of the game entity.

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

## aux.lock.LockPool

```python
LockPool(Entity):
    slots : int
```

Used by abilities to block each other. The lock pool has a defined number of slots available which are occupied by abilities with the `Lock` property when they execute. If no slots are available at runtime, the execution of the ability is blocked.

**slots**
Maximum number of abilities using the pool that can be active at the same time.

## aux.logic.LogicElement

```python
LogicElement(LogicElement):
    only_once : bool
```

Generalization object for a logical elements (literals, gates or constants) in the API. Logical elements are either true or false at a given point in time.

**only_once**
If this is set, the logical element is always true if it has been true at least once during the game.

## aux.logic.const.False

```python
False(LogicElement):
    pass
```

A logic element that is always false.

## aux.logic.const.True

```python
True(LogicElement):
    pass
```

A logic element that is always true.

## aux.logic.gate.LogicGate

```python
LogicGate(LogicElement):
    inputs : set(LogicElement)
```

Gneralization object for a logic gate implementing a Boolean function. The Boolean value of the gate at runtime is evaluated using the defined inputs.

**inputs**
Inputs for the gate. Some types of gate may require a minimum or maximum number of inputs. Gates with 0 inputs are always evaluated to false.

## aux.logic.gate.type.AND

```python
AND(LogicGate):
    pass
```

Evaluates to true if *all* inputs are true.

## aux.logic.gate.type.NOT

```python
NOT(LogicGate):
    pass
```

Negates the input.

Only one input is allowed for this type of gate. If more than one input is defined, the gate evaluates to false.

## aux.logic.gate.type.MULTIXOR

```python
MULTIXOR(LogicGate):
    pass
```

Evaluates to true if *an uneven number of* inputs is true.

## aux.logic.gate.type.OR

```python
OR(LogicGate):
    pass
```

Evaluates to true if *at least one* input is true.

## aux.logic.gate.type.SUBSETMAX

```python
SUBSETMAX(LogicGate):
    size : int
```

Evaluates to true if *at most* `size` inputs are true.

**size**
Maximum number of inputs that should be true.

## aux.logic.gate.type.SUBSETMIN

```python
SUBSETMIN(LogicGate):
    size : int
```

Evaluates to true if *at least* `size` inputs are true.

**size**
Minimum number of inputs that must be true.

## aux.logic.gate.type.XOR

```python
XOR(LogicGate):
    pass
```

Evaluates to true if *exactly one* input is true.

## aux.logic.literal.Literal

```python
Literal(LogicElement):
    scope : LiteralScope
```

Generalization object for a logical statement about the game world that is either true or false at a goven point in time.

**scope**
Scope in which the statement is checked.

## aux.logic.literal.type.AttributeAbovePercentage

```python
AttributeAbovePercentage(Literal):
    attribute : Attribute
    threshold : float
```

Is true when the current attribute value's share of its maximum value of a game entity is higher than a specified threshold.

**attribute**
Attribute that is checked.

**threshold**
Value threshold that must be passed for the literal to be true.

## aux.logic.literal.type.AttributeAboveValue

```python
AttributeAboveValue(Literal):
    attribute : Attribute
    threshold : float
```

Is true when the current attribute value of a game entity is higher than a specified threshold.

**attribute**
Attribute that is checked.

**threshold**
Value threshold that must be passed for the literal to be true.

## aux.logic.literal.type.AttributeBelowPercentage

```python
AttributeBelowPercentage(Literal):
    attribute : Attribute
    threshold : float
```

Is true when the current attribute value's share of its maximum value of a game entity is lower than a specified threshold.

**attribute**
Attribute that is checked.

**threshold**
Value threshold that must be passed for the literal to be true.

## aux.logic.literal.type.AttributeBelowValue

```python
AttributeBelowValue(Literal):
    attribute : Attribute
    threshold : float
```

Is true when the current attribute value of a game entity is lower than a specified threshold.

**attribute**
Attribute that is checked.

**threshold**
Value threshold that must be passed for the literal to be true.

## aux.logic.literal.type.GameEntityProgress

```python
GameEntityProgress(Literal):
    game_entity : GameEntity
    status   ´  : ProgressStatus
```

Is true when an instance of a game entity has reached a certain progress.

**game_entity**
The game entity that should have progressed this far.

**status**
Status of a game entity as a `ProgressStatus` object.

## aux.logic.literal.type.OwnsGameEntity

```python
OwnsGameEntity(Literal):
    game_entity : GameEntity
```

Triggers when a game entity is owned by a player in the defined scope.

## aux.logic.literal.type.ProjectileHitTerrain

```python
ProjectileHitTerrain(Literal):
    pass
```

Triggers when a game entity's hitbox collides with the terrain of the map.

## aux.logic.literal.type.ProjectilePassThrough

```python
ProjectilePassThrough(Literal):
    pass_through_range : int
```

Triggers when the distance to the spawn location of the game entity becomes greater than a defined value.

**pass_through_range**
Distance to the spawn location. Only (x,y) coordinates are considered.

## aux.logic.literal.type.ResourceSpotsDepleted

```python
ResourceSpotsDepleted(Literal):
    only_enabled : bool
```

Triggers when all resource spots in `Harvestable` abilities have been depleted.

**only_enabled**
The condition only considers resource spots of enabled `Harvestable` abilities.

## aux.logic.literal.type.StateChangeActive

```python
StateChangeActive(Literal):
    state_change : StateChanger
```

Triggers when the defined state changer is active for the game entity.

## aux.logic.literal.type.TechResearched

```python
TechResearched(Literal):
    tech : Tech
```

Is true when the technology has been researched by the player.

**tech**
The technology that has to be researched.

## aux.logic.literal.type.Timer

```python
Timer(Literal):
    time : float
```

Triggers after a specified amount of time has passed after the `Despawn` ability has been activated. If the `Despawn` ability is disabled before the timer has finished, it will be reset.

**time**
Time that has to pass after the activation of the corresponding `Despawn` ability.

## aux.logic.literal_scope.LiteralScope

```python
LiteralScope(Entity):
    pass
```

Configures the scope in which the fulfillment of the literal is checked.

## aux.logic.literal_scope.type.Default

```python
Default(LiteralScope):
    pass
```

Use the default scope defined by the literal.

## aux.logic.literal_scope.type.Diplomatic

```python
Diplomatic(LiteralScope):
    stances : set(DiplomaticStance)
```

Include the state of players with the specified stances in the scope.

**stances**
Diplomatic stances included in the scope.

## aux.lure_type.LureType

```python
LureType(Entity):
```

Used by `Lure` effects and resistances for matching.

## aux.mod.Mod

```python
Mod(Entity):
    patches  : orderedset(Patch)
    priority : int
```

A set of patches that will be automatically applied when the modpack is loaded.

**patches**
Changes the game state through patches. Any members and objects can be patched. Every `Patch` will be applied to all players. `DiplomaticPatch` objects will also be applied to all players.

**priority**
Determines the application order of the mod in comparison to other `Mod` objects. `Mod` objects are applied starting with the object with the highest priority value.

## aux.modifier_scope.ModifierScope

```python
ModifierScope(Entity):
```

Generalization object for scopes of a `ScopeModifier` object.

## aux.modifier_scope.type.GameEntityScope

```python
GameEntityScope(ModifierScope):
    affected_types       : set(GameEntityType)
    blacklisted_entities : set(GameEntity)
```

Defines a scope of game entities the modifier should apply to.

**affected_types**
Whitelist of game entity types that the modifier should apply to.

**blacklisted_entities**
Blacklist for specific game entities that would be covered by `affected_types`, but should be excplicitly excluded.

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

## aux.move_mode.type.Guard

```python
Guard(MoveMode):
    range : float
```

Follow another game entity at a defined range and defend it from other game entities. The movement speed is adjusted to the guarded game entity, but cannot go higher than the `speed` value from `Move`. Stances from `GameEntityStance` ability are ignored during movement. Guarding is stopped when the guarded game entity gets out of the line of sight of the guarding game entity.

**range**
The range at which the other game entity is guarded.

## aux.move_mode.type.Normal

```python
AttackMove(MoveMode):
```

Move to a position on the map. Stances from `GameEntityStance` ability are ignored during movement.

## aux.move_mode.type.Patrol

```python
Patrol(MoveMode):
```

Lets player set two or more waypoints that the game entity will follow. Stances from `GameEntityStance` ability are considered during movement.

## aux.passable_mode.PassableMode

```python
PassableMode(Entity):
    allowed_types        : set(GameEntityType)
    blacklisted_entities : set(GameEntity)
```

Generalization object for all passable modes. Define passability options for the `Passable` ability.

**allowed_types**
Lists the game entities types which can pass the hitbox.

**blacklisted_entities**
Used to blacklist game entities that have one of the types listed in `allowed_types`, but should not be covered by this `PassableMode` object.

## aux.passable_mode.type.Gate

```python
Gate(PassableMode):
    stances : set(DiplomaticStance)
```

Lets all compatible game entities from players with the specified stances pass through the hitbox. Game entities of players with other stances can also pass through while any unit is passing through.

**stances**
Stances of players whose game entities are always allowed to pass though the hitbox.

## aux.passable_mode.type.Normal

```python
Normal(PassableMode):
    pass
```

Lets all compatible game entities pass through the hitbox.

## aux.patch.NyanPatch

```python
NyanPatch(Entity):
    pass
```

Parent object for nyan patches used in the openage API. All nyan patches must inherit from this object.

## aux.patch.Patch

```python
Patch(Entity):
    properties : dict(abstract(children(PatchProperty)), children(PatchProperty))
    patch      : NyanPatch
```

Wrapper for nyan patches with additional configuration options throuh properties.

**properties**
Further specializes the patch beyond the standard behaviour.

Properties:

* `Diplomatic`: The patch is applied to the database view of players with defined diplomatic stances.

**patch**
The nyan patch that gets applied by the wrapper.

## aux.patch.property.PatchProperty

```python
PatchProperty(Entity):
    pass
```

Generalization object for all properties of patches.

## aux.patch.property.type.Diplomatic

```python
Diplomatic(PatchProperty):
    stances : set(DiplomaticStance)
```

The patch that is applied to all players that have the specified diplomatic stances from the viewpoint of the patch's initiator.

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

## aux.payment_mode.type.Shadow

```python
Shadow(PaymentMode):
```

Requires the resources or attribute points to be available to the player, but does not require payment of the resources.

## aux.placement_mode.PlacementMode

```python
PlacementMode(Entity):
    pass
```

Generalization object for all placement modes that are configurable for a `CreatableGameEntity` object.

## aux.placement_mode.type.Eject

```python
Eject(PlacementMode):
    pass
```

The game entity is ejected from the creating game entity. Ejecting considers the game entity's `Hitbox` and `TerrainRequirement` abilities for the ejection location.

## aux.placement_mode.type.OwnStorage

```python
OwnStorage(PlacementMode):
    container : Container
```

The game entity is stored into a container of the creating game entity.

**container**
Container used for storing the game entity. If the creating game entity does not have a `Storage` ability wth this container or the container is already full, the placement mode cannot be used.

## aux.placement_mode.type.Place

```python
Place(PlacementMode):
    tile_snap_distance       : float
    clearance_size_x         : float
    clearance_size_y         : float
    allow_rotation           : bool
    max_elevation_difference : float
```

The game entity can be placed on the map after its creation. Placement considers the game entity's `Hitbox` and `TerrainRequirement` abilities for the placement location.

**tile_snap_distance**
Distance between the snap anchors when placing the game entity in the game world.

**clearance_size_x**
Necessary free space on the x-axis for placing the game entity.

**clearance_size_y**
Necessary free space on the y-axis for placing the game entity.

**allow_rotation**
The player can cycle through the `PerspectiveVariant` variants of the game entity before placement.

**max_elevation_difference**
Maximum elevation difference between the lowest and hightest point in the placement area.

## aux.placement_mode.type.Replace

```python
Replace(PlacementMode):
    game_entities : set(GameEntity)
```

Replaces one or more existing game entities. The replaced game entities are permanently removed from the game.

**game_entities**
Game entities that can be replaced.

## aux.price_mode.PriceMode

```python
PriceMode(Entity):
    pass
```

Generalization object for price modes used for adjusting prices of exchange rates.

## aux.price_mode.type.Dynamic

```python
Dynamic(PriceMode):
    change_value : float
    min_price    : float
    max_price    : float
```

The price is adjusted dynamically on every exchange using a fixed value.

**change_value**
The amount y which the price is adjusted.

**min_price**
Lower bound for the price. The price cannot be changed to a value lower than this.

**min_price**
Upper bound for the price. The price cannot be changed to a value higher than this.

## aux.price_mode.type.Fixed

```python
Fixed(PriceMode):
    pass
```

The price is not adjusted on an exchange.

## aux.price_pool.PricePool

```python
PricePool(Entity):
    pass
```

Allows syncing the price (at runtime) of an exchange rate across several game entities. The price for exchanging resources is the same for every exchange rate using the same price pool.

## aux.production_mode.ProductionMode

```python
ProductionMode(Entity):
    pass
```

Generalization object for all production modes used by a `ProductionQueue` ability.

## aux.production_mode.type.Creatables

```python
Creatables(ProductionMode):
    exclude : set(CreatableGameEntity)
```

The queue can store production requests for `CreatableGameEntity` instances.

**exclude**
`CreatableGameEntity` instances that are blacklisted from being appended to the queue.

## aux.production_mode.type.Researchables

```python
Researchables(ProductionMode):
    exclude : set(ResearchableTech)
```

The queue can store production requests for `ResearchableTech` instances.

**exclude**
`ResearchableTech` instances that are blacklisted from being appended to the queue.

## aux.progress.Progress

```python
Progress(Entity):
    properties     : dict(abstract(children(ProgressProperty)), children(ProgressProperty))
    left_boundary  : float
    right_boundary : float
```

Generalization object for progression types.

*Progress* can refer to

* The progress of ability actions **or**
* The percentage of a value at runtime for values that operate in a min-max range

It is defined as a float between 0.0 (0%) and 1.0 (100%).

`Progress` objects define what happens if the progress of an action or runtime value enters a specified interval. The direction at which the interval is entered does not matter. The boundaries of the interval are defined by `left_boundary` and `right_boundary`. The interval is *left-closed* and *right-open* which means the `Progress` object is associazed with a progress in this range:

```math
left\_boundary <= progress < right\_boundary
```

In the special case of `left_boundary = 0.0` the interval is considered as *left-open*. This is done to ensure that progress can be exclusively checked *during* an ability action (between 0% and 100%) without having to consider the *before* or *after*. If progress should additionally be checked for the case 0%, a `Progress` object with the exact values `left_boundary = 0.0` and `right_boundary = 0.0` must be defined. Similarly, If progress should additionally be checked for the case 100%, a `Progress` object with the exact values `left_boundary = 1.0` and `right_boundary = 1.0` must be defined.

**properties**
Defines what happens if the progress enters the defined interval.

Properties:

* `AnimatedProgress`: Overrides the animation of an ability.
* `TerrainProgress`: Changes the underlying terrain of the game entity.
* `TerrainOverlayProgress`: Changes terrain overlays of a game entity.
* `StateChangeProgress`: Alters the base abilities and modifiers of the game entity through `StateChanger` objects.

**left_boundary**
Defines the left boundary of the progression interval. Must be a float between 0.0 and 1.0 that represents a percentage of progression. Must be smaller than `right_boundary`.

**right_boundary**
Defines the left boundary of the progression interval. Must be a float between 0.0 and 1.0 that represents a percentage of progression. Must be larger than `left_boundary`.

## aux.property.ProgressProperty

```python
ProgressProperty(Entity):
    pass
```

Generalization object for all properties of `Progress` objects.

## aux.progress.property.type.AnimationOverlay

```python
AnimationOverlay(ProgressProperty):
    overlays : set(Animation)
```

Overlays the animation of abilities with the specified animations.

**overlays**
The overlay animations.

## aux.progress.property.type.Animated

```python
Animated(ProgressProperty):
    overrides : set(AnimationOverride)
```

Overrides the animation of abilities when the specified progress interval has been reached.

**overrides**
The overriding animations.

## aux.progress.property.type.StateChange

```python
StateChange(ProgressProperty):
    state_change : StateChanger
```

Alters the base abilities and modifiers of a game entity when the specified progress interval has been reached.

**state_change**
The state modifications as a `StateChanger` object.

## aux.progress.property.type.TerrainOverlay

```python
TerrainOverlay(ProgressProperty):
    terrain_overlay : Terrain
```

Changes overlayed terrain of a game entity when the specified progress interval has been reached. The game entity needs an enabled `OverlayTerrain` ability for this to work.

**terrain**
Overrides the overlayed terrain of the currently enabled `OverlayTerrain` ability of the game entity. The override stops when another terrain overlay override is initiated, the overridden `OverlayTerrain` ability is deactivated or the override terrain is the same as the default overlayed terrain of the ability.

## aux.progress.property.type.Terrain

```python
Terrain(ProgressProperty):
    terrain : Terrain
```

Changes the underlying terrain of a game entity when the specified progress interval has been reached.

**terrain**
The new terrain that will be permanently placed under the game entity.

## aux.progress.type.AttributeChangeProgress

```python
AttributeChangeProgress(Progress):
    pass
```

Compares the current attribute value in relation to the `max_value` of an attribute of a game entity. The `Progress` objects are stored in a `Damageable` ability which specifies the type of attribute that is monitored. When the attribute value is equal to `max_value` of the attribute defined by the game entity, the progress is 0\%. Once the attribute value reaches the `min_value`, the progress is 100\%.

## aux.progress.type.CarryProgress

```python
CarryProgress(Progress):
    pass
```

Monitors the occupied storage space of a `Storage` or `Gather` ability. An empty storage has a progress of 0\% and a full storage a progress of 100\%.

## aux.progress.type.ConstructionProgress

```python
ConstructionProgress(Progress):
    pass
```

Monitors the construction progress of a game entity with `Contructable` ability. An unconstructed game entity has a progress of 0\% and a fully constructed game entity a progress of 100\%.

## aux.progress.type.HarvestProgress

```python
HarvestProgress(Progress):
    pass
```

Monitors the harvesting progress of a resource spot stored by a `Harvestable` ability. A resource spot with full capacity has a progress of 0\% and a fully harvested resource spot a progress of 100\%.

## aux.progress.type.RestockProgress

```python
RestockProgress(Progress):
    pass
```

Monitors the restock progress of a restockable resource spot stored by a `Harvestable` ability. The restocking progress is initiated by the `Restock` ability of another game entity. At the start of the restocking process, the progress is 0\%. After the restocking has finished, the progress is 100\%.

Restocking progress is only tracked between the start and end of restock process. Therefore, state changes initiated by `RestockProgress` objects of type `StateChangerProgress` will be deactivated after the progress reaches 100\%.

## aux.progress.type.TransformProgress

```python
TransformProgress(Progress):
    pass
```

Monitors the progress of a transformation initiated by a `TransformTo` ability. At the start of the transformation, the progress is 0\%. After the transformation has finished, the progress is 100\%.

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
    tech            : Tech
    cost            : Cost
    research_time   : float
    research_sounds : set(Sound)
    condition       : set(LogicElement)
```

Defines preconditions for researching a technology with the `Research` ability.

**tech**
Reference to the `Tech` object that is researched.

**cost**
Resource amount spent to initiate the research. Cancelling the research results in a refund of the spent resources.

**research_time**
Time until the `Tech` object's patches are applied.

**research_sounds**
Sounds that are played when the research finishes.

**condition**
Condition that unlock the technology for the researching game entity. Only one condition needs to be fulfilled to trigger the unlock. If the set is empty, the technology is considered available from the start for the researching game entity.

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

* Static amounts can be acquired like normal resources
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
    resource        : Resource
    max_amount      : int
    starting_amount : int
    decay_rate      : float
```

Amount of resources that is gatherable through the `Harvestable` ability of a game entity.

**resource**
Type of resource that can be harvested.

**max_amount**
Maximum resource capacity of the resource spot.

**starting_amount**
Gatherable amount when the resource spot is created.

**decay_rate**
Determines how much resources are lost each second after the resource spot is activated (see `harvestable_by_default` for details).

## aux.selection_box.SelectionBox

```python
SelectionBox(Entity):
    pass
```

Defines the selection area for the `Selectable` ability.

## aux.selection_box.type.MatchToSprite

```python
MatchToSprite(SelectionBox):
    pass
```

Use animation sprites of the game entity as the selection area. Transparent pixels are excluded.

## aux.selection_box.type.Rectangle

```python
Rectangle(SelectionBox):
    radius_x : float
    radius_y : float
```

Use a rectangular box as the selection area.

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
    transform_pool    : optional(TransformPool)
    priority          : int
```

State changes alter the *base state* of a game entity which is defined by the abilities and modifiers stored in a `GameEntity` object. They are allowed to enable new and disable existing abilities as well as modifiers. Multiple state changes can be applied at once. Only abilities and modifiers with a priority *lower than or equal to* (<=) the one defined in the `StateChanger` object will be disabled.

**enable_abilities**
A set of abilities that are enabled when the state change is active.

**disable_abilities**
A set of abilities that are disabled when the state change is active and the abilities have a priority *lower than or equal to* (<=) the `priority` in the disabling `StateChanger` object.

**enable_modifiers**
A set of modifiers that are enabled when the state change is active.

**disable_modifiers**
A set of modifiers that are disabled when the state change is active and the modifiers have a priority *lower than or equal to* (<=) the `priority` in the disabling `StateChanger` object.

**transform_pool**
Transform pool the state change uses. Only one state change can be active per transform pool. If this is `None`, the base state is changed.

**priority**
Priority of a state change. The value can be negative. Abilities and modifiers belonging to the base state have an implicit priority of 0.

## aux.state_machine.Reset

```python
Reset(StateChanger):
    enable_abilities  = {}
    disable_abilities = {}
    enable_modifiers  = {}
    disable_modifiers = {}
    transform_pool    = None
    priority          = 0
```

Resets the game entity to the *base* state. This means that all state changes are cleared immediately on activation.

## aux.storage.Container

```python
Container(Entity):
    allowed_types        : set(GameEntityType)
    blacklisted_entities : set(GameEntity)
    storage_element_defs : set(StorageElementDefinition)
    slots                : int
    carry_progress       : set(CarryProgress)
```

Used by the `Storage` ability to set the allowed game entities and store definitions of how the stored game entities influence the storing game entity.

**allowed_types**
Whitelist of game entity types that can be stored in this container.

**blacklisted_entities**
Blacklist for specific game entities that would be covered by `allowed_types`, but should be explicitly excluded.

**storage_element_defs**
Contains further configuration settings for specific game entities.

**slots**
Defines how many slots for game entities the container has. Multiple game entities may be stacked in one slot depending on the `elements_per_slot` member in `StorageElementDefinition`.

**carry_progress**
A set of `CarryProgress` objects that can activate state changes and animation overrides when the container is filled.

## aux.storage.ResourceContainer

```python
ResourceContainer(Entity):
    resource       : Resource
    capacity       : int
    carry_progress : set(CarryProgress)
```

Used by the `ResourceStorage` ability to define storage space for resources that can be carried by a game entity.

**resource**
Resource stored in the container.

**capacity**
Maximum amount of resources that can be stored in the container.

**carry_progress**
A set of `CarryProgress` objects that can activate state changes and animation overrides when the container is filled.

## aux.storage.InternalDropSite

```python
InternalDropSite(ResourceContainer):
    update_time    : float
```

Deposits the resources directly into the game entity owner's global resource amount. Instead of dropping the resources at a drop site, the resource amount in the internal drop site is added to the player's resource amounts in defined intervals.

**update_time**
Update interval between the automatic deposition of the stored resources in seconds.

## aux.storage.StorageElementDefinition

```python
StorageElementDefinition(Entity):
    storage_element   : GameEntity
    elements_per_slot : int
    conflicts         : set(StorageElementDefinition)
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

Makes the projectile path end at the current position of the target when the projectile spawned.

## aux.target_mode.type.ExpectedPosition

```python
ExpectedPosition(TargetMode):
```

Makes the projectile path end at the position where the target is expected to be when the projectile is supposed to hit it.

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
    types            : set(TechType)
    name             : TranslatedString
    description      : TranslatedMarkupFile
    long_description : TranslatedMarkupFile
    updates          : orderedset(Patch)
```

An object that can apply changes through patching. It follows the standard implementation from most other strategy games. The `Tech` object only stores the patches that change the game state, while cost, research time, and requirements are decided by the `Research` ability of a game entity. By default, technologies can only be applied once and researched by one game entity at a time. Afterwards, the engine sets a flag that the `Tech` was applied and automatically forbids researching it again.

**types**
Classification of the tech.

**name**
The name of the technology as a translatable string.

**description**
A description of the technology as a translatable markup file.

**long_description**
A longer description of the technology as a translatable markup file.

**updates**
Changes the game state through patches. Any members and objects can be patched. Normal `Patch` objects will only be applied to the player. To apply patches to other player with specific diplomatic stances, use `DiplomaticPatch`.

## aux.tech_type.TechType

```python
TechType(Entity):
    pass
```

Classification for a tech.

## aux.tech_type.Any

```python
Any(TechType):
    pass
```

Can be used to address any tech, even ones that have no `TechType` assigned.

## aux.terrain.Terrain

```python
Terrain(Entity):
    types           : set(TerrainType)
    name            : TranslatedString
    terrain_graphic : Terrain
    sound           : Sound
    ambience        : set(TerrainAmbient)
```

Terrains define the properties of the ground which the game entities are placed on.

**types**
Classification of the terrain.

**name**
The name of the terrain as a translatable string.

**terrain_graphic**
Texture of the terrain (see `Terrain`). openage uses 3D terrain on which the texture is painted on.

**sound**
Ambient sound played when the camera of the player is looking onto the terrain.

**ambience**
Defines ambient objects placed on the terrain through a set of `TerrainAmbient` objects.

## aux.terrain.TerrainAmbient

```python
TerrainAmbient(Entity):
    object      : GameEntity
    max_density : int
```

An ambient game entity that is placed randomly on a chunk of terrain (10x10 tiles).

**object**
The game entity placed on the terrain.

**max_density**
Defines how many ambient objects are allowed to be placed on a chunk at maximum.

## aux.terrain_type.TerrainType

```python
TerrainType(Entity):
    pass
```

Classification for a terrain.

## aux.terrain_type.Any

```python
Any(TerrainType):
    pass
```

Can be used to address any terrain, even ones that have no `TerrainType` assigned.

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

```math
trade\_amount = 0.46 \cdot tiles\_distance \cdot ((tiles\_distance / map\_size) + 0.3)
```

## aux.transform_pool.TransformPool

```python
TransformPool(Entity):
    pass
```

Defines a pool for `StateChanger` objects. Only one state change can be active per transform pool.

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
All translations of the markup files as language-file pairs (see `LanguageMarkupPair`).

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

## aux.variant.type.AdjacentTilesVariant

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

Variant depending on the placement angle of the game entity. Currently only works with the `PlacementMode` of type `Place` with the `allow_ratation` member set to true.

**angle**
Angle of the game entity. An angle of *0* points to the south-west direction.

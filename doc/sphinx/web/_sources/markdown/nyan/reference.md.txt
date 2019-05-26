# Reference

(Fill meeeeeeeeeeeee)

## AnimationOverride

```python
AnimationOverride(Entity):
    ability    : Ability
    animations : set(Animation)
```

Internally overrides the animations used for an ability. The ability must be an `AnimatedAbility` when the override occurs. The override stops when another override of the same ability is initiated or depending on the type of override.

**ability**
The ability whos animations should be overriden. This member can reference a specific ability of the game entity or an API ability. If an API ability is referenced, all its instances in the ability set of the game entity will be overriden.

**animations**
The replacement animations of the override.

## AvailabilityPrerequisite

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

## availability_prerequisites.TechResearched

```python
TechResearched(AvailabilityPrerequisite):
    tech : Tech
```

Is true when the technology has been researched by the player.

**tech**
The technology that has to be researched.

## availability_prerequisites.GameEntityProgress

```python
GameEntityStatus(AvailabilityPrerequisite):
    game_entity : GameEntity
    progress ´   : PrerequisiteStatus
```

Is true when an instance of a game entity owned by the player has reached a certain progress.

**game_entity**
The game entity that should have progressed this far.

**progress**
The progress that must be reached or surpassed. This only works if an instance of the referenced game entity stores the `Progress` object in one of their abilities.

## AvailabilityRequirement

```python
AvailabilityRequirement(Entity):
    requirements : set(AvailabilityPrerequisite)
```

*This object is part of a mechanic for unlocking game entities (see `CreatableGameEntity`) through specifying a set of requirements.* (TODO: explain)

To evaluate to `true`, all of the `Prerequisites` in the `AvailabilityRequirement` must be true as well.

**requirements**
A number of prerequisites that can be true or false. If all of the prerequisites are true, the availability requirement is fulfilled.

## Civilization

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

## DropoffType

```python
DropoffType(Entity):
```

Used for calculating the effectiveness over distance of `AreaEffect`s and `Accuracy` of projectiles. The dropoff modifier is always relational to the maximum range.

## dropoff_types.InverseLinear

```python
InverseLinear(DropoffType):
```

The effectiveness starts at 0\% (for zero distance) and linearly increases to 100\% (for maximum distance).

## dropoff_types.Linear

```python
Linear(DropoffType):
```

The effectiveness starts at 100\% (for zero distance) and linearly decreases to 0\% (for maximum distance).

## dropoff_types.NoDropoff

```python
NoDropoff(DropoffType):
```

The effectiveness is constant and independent from the range to the target.

## Language

```python
Language(Entity):
    ietf_string : text
```

A language definition. Languages are used for translated strings, markup files and sounds. 

**ietf_string**
The IETF identification tag of the language. [See here](https://tools.ietf.org/html/rfc4646) for more information on how the tags are established.

## language.LanguageMarkupPair

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

## language.LanguageSoundPair

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

## language.LanguageTextPair

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

## Mod

```python
Mod(Entity):
    patches : orderedset(Patch)
```

A set of patches that will be automatically applied when the modpack is loaded. It is recommended to only use one `Mod` object per modpack since the correct order of patches cannot be guaranteed otherwise.

**patches**
Changes the game state through patches. Any members and objects can be patched. Every `Patch` will be applied to all players. `DiplomaticPatch` objects will also be applied to all players.

## Patch

```python
Patch(Entity):
```

Generalization object for all nyan patchesv. Let nyan patches inherit from this object to make them usable for the openage API.

## patch.DiplomaticPatch

```python
DiplomaticPatch(Patch):
    stances : set(DiplomaticStance)
```

A patch that is applied to all players that have the specified diplomatic stances from the viewpoint of the patch's initiator.

**stances**
The diplomatic stances of the players the patch should apply to.

## Resource

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

## resource.ResourceContingent

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

## ResourceAmount

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

## ResourceRate

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

## ResourceSpot

```python
ResourceSpot(Entity):
    resources              : ResourceAmount
    decay_rate             : float
    harvest_progress       : set(HarvestProgress)
    gatherer_limit         : int
    harvestable_by_default : bool
```

Amount of resources that is gatherable through the `Harvestable` ability of a game entity.

**resources**
Gatherable amount when the resource spot is created.

**decay_rate**
Determines how much resources are lost each second after the resource spot is activated (see `harvestable_by_default` for details).

**harvest_progress**
Can alter the game entity the resource spot belongs to after certain percentages of the  resource amount are harvested.

**gatherer_limit**
Limits the amount of gatherers that can access the resource spot simultaneously.

**harvestable_by_default**
Determines whether the resource spot is harvestable when it is created. If `true`, the resource spot will be accessible without any conditions as long as the corresponding `Harvestable` ability of the game entity is enabled. When set to false, the resource spot must be activated with a `MakeHarvestable` effect. The conditions under which the activation succeeds are decided by the `MakeHarvestable` resistance of the game entity the resource spot belongs to.

## resource_spots.RestockableResourceSpot

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

## Tech

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

## Terrain

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

## terrain.TerrainAmbient

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

## TranslatedObject

```python
TranslatedObject(Entity):
```

Generalization object for any objects that are or should be different depending on the language. Currently we support translations for strings, markup files and sounds.

## translated.TranslatedMarkupFile

```python
TranslatedMarkupFile(TranslatedObject):
    translations : set(LanguageMarkupPair)
```

The translated versions of a longer text stored in markup files.

**translations**
All translations of the makrup files as language-file pairs (see `LanguageMarkupPair`).

## translated.TranslatedSound

```python
TranslatedSound(TranslatedObject):
    translations : set(LanguageSoundPair)
```

The translated versions of a sound.

**translations**
All translations of the sound as language-sound pairs (see `LanguageSoundPair`).

## translated.TranslatedString

```python
TranslatedString(TranslatedObject):
    translations : set(LanguageTextPair)
```

The translated versions of a string.

**translations**
All translations of the string as language-text pairs (see `LanguageTextPair`).

## Variant

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

## variants.RandomVariant

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

## variants.PerspectiveVariant

```python
PerspectiveVariant(Variant):
    angle : int
```

Variant depending on the placement angle of the game entity. Cuurently only works with the `PlacementMode` of type `PlaceAndRotate`.

**angle**
Angle of the game entity. An angle of *0* points to the south-west direction.

## variants.AdjacencyVariant

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
# Patches

Patches are special objects that can change the member values of objects. Everything that is considered an *upgrade* in real-time strategy games is done through patching in nyan.

## Basic synatx

```
PatchName<TargetObject>():
    # This is a comment
    int_member -= 5
    text_member += "yes"
    set_member &= {Archer, Knight, Spearman}
    object_member = MoveAbility
    ...
```

The above example shows the declaration of a patch. Patches always have exactly one target denoted in angled brackets (`<>`). The target indicates the type of object that can be changed by the patch. Target objects may be abstract.

Patches themselves are only allowed to change existing members of an object, not to introduce new ones. However, they can add new parents to an object which leads to the parent members being added to the object. This is discussed in section [Adding inheritance](#adding-inheritance).

Apart from having to specify a target and the lacking ability to introduce new members directly, patches have the same characteristics as normal objects. This includes the ability to inherit from other patches. When a parent of a patch already specified a target, the derived patch is not allowed to change it. All children of a patch will implicitly affect the same patch target.

## Applying a patch

Patches are applied at runtime during a game. When, where and why a patch is applied is decided by the openage engine functions and is only limited by the type of the target object. The number of applications of a patch is not limited and independent from its declaration. Depending on the game, it could be applied once, infinite times or never.

It is also important to mention that a patch **does not** change the declaration of an object and has no influence on the actual nyan files. Patches operate on *instances* of objects in the nyan database of the engine. Only these instances are altered. 

Patches can be applied to all instances, a set of instances or one specific instance of an object. This is an important difference to the system used in Age of Empires II where upgrades usually affect all instances of an object. In nyan, every object that has the type of the target object has the potential of being patched, but does not have to be patched necessarily.

The example below shows how an instance is altered by a patch.

### Example

```
# This is a normal object
Archer(RangedUnit):
    hp = 30
    attack = 4
    speed = 0.96

# This is a patch
Fletching<RangedUnit>(Tech):
    attack += 1
```

Here we see two objects: `Archer` and `Fletching`. `Fletching` is also a patch because it has specified `RangedUnit` as a target. It increases the value of `attack` by `1` each time it is applied. Because `Archer` inherits from `RangedUnit`, it is of the type `RangedUnit` and the patch can potentially be applied to its instances.

Let's have a look at an instance of archer ***before*** the application of the patch.

```
(Archer instance)
    hp = 30
    attack = 4
    speed = 0.96
```

This instance still has the default values of the object declaration. We now apply `Fletching`, e.g. by an engine function for researching technologies.

```
(Archer instance)
    hp = 30
    attack = 5
    speed = 0.96
```

`attack` has increased by 1, as we expected. We now apply the patch a second time.

```
(Archer instance)
    hp = 30
    attack = 6
    speed = 0.96
```

Again, the `attack` value increased by 1.

## Patching a Patch

Because patches are just a special kind of object, they can be patched too. This is especially useful for mods, which want to change the effects of existing technologies or unit upgrades.

```
Fletching<RangedUnit>(Tech):
    attack += 1

BetterFletching<Fletching>():
    attack +=2
```

In the above example we see two patches: `Fletching` and `BetterFletching`. `BetterFletching` patches `Fletching` by increasing the value of the member `attack` by 2.

There are peculiarities to consider as we will see in the next example.

```
DebuffAttack<Unit>():
    attack -= 1

DebuffPatch<DebuffAttack>(Tech):
    attack += 2
```

In a naive approach, some might expect the member of `DebuffAttack` to be `attack += 1` after applying `DebuffPatch`. However, this would be wrong. The patch only changes the value, it does **not** touch the operator. Therefore, the member will be patched to `attack -= 3` by `DebuffPatch`. Changing the operator requires an additional `@` symbol. Using `@` overwrites both the operator and the value.

```
DebuffAttack<Unit>():
    attack -= 1

OverwriteOperator<DebuffAttack>():
    # overwrite operator and value of 'attack'
    attack @+= 2
```

Applying `OverwriteOperator` will alter the member attack to `attack += 2`. The `@` modifier can also be stacked to overwrite the overwrite operators.

```
DebuffAttack<Unit>():
    attack -= 1

OverwriteOperator<DebuffAttack>():
    # overwrite operator and value of 'attack'
    attack @+= 2

FixOperator<OverwriteOperator>():
    # this overwrites the operator of 'OverwriteOperator'
    # with '*='
    attack @*= 1

FixOperatorFix<OverwriteOperator>():
    # this overwrites the operator of 'OverwriteOperator'
    # with '@*='
    attack @@*= 1
```

To show how `@` can be stacked, we have introduced two patches for `OverwriteOperator`: `FixOperator` and `FixOperatorFix`. They both overwrite the operator of `OverwriteOperator`, but have a slightly different outcome.

`FixOperator` overwrites the operator of `OverwriteOperator` with `*=`. That means, after applying `FixOperator` the instance of OverwriteOperator will look like this:

```
(OverwriteOperator instance)
    # no more '@' because it was overwritten
    attack *= 1
```

The application of `FixOperator` also overwrote the `@` modifier. Therefore, the application of `OverwriteOperator` will no longer overwrite the operator of `DebuffAttack`.

On the other hand, `FixOperatorFix` overwrites the operator of `OverwriteOperator` with `@*=`, or more specifically with everything after the first occurence of `@`.

```
(OverwriteOperator instance)
    # '@' is still present
    attack @*= 1
```

When `OverwriteOperator` is applied now, it will still overwrite the operator of `DebuffAttack`, but with the operator it got from the application of `FixOperatorFix`.

## Adding inheritance

A special feature of patches is their ability to change the inheritance parents of a nyan object. This is introduced to allow the injection of a parent in between an existing parent-child relationship. This is denoted by the `[NewParent+, +OtherNewParent, ...]` syntax. The position of the `+` specifies if the object is added at the front (Name+) or the end of the current parent list (+Name).

```
RootObject():
    magic_value : int = 9001

ChildObject(RootObject):
    magic_value -= 1

InjectedObject(RootObject):
    magic_value -= 7661

InjectIt<ChildObject>[InjectedObject+]():
    magic_value += 2
```

When the `InjectIt` patch is applied, `InjectedObject` will be the new direct parent of `ChildObject`.
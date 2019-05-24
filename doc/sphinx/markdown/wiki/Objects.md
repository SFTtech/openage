# Objects

Objects represent every entity in the game world. Everything is described as an object, from units and buildings over ambient entities such as cliffs up to technologies and boni.

## Basic Syntax

```
ObjectName():
    # This is a comment
    member_name : type = value
    other_member : type = value
    member_without_value : type # Comments can also be here 
    ...
```
The example above shows the initial declaration of a nyan object. A nyan object is a named group of key-value pairs. The name of the group is the nyan object name (in this example: `ObjectName`). Key-value pairs assigned in this object are called *members*. The number of members per object is unlimited. Each initial definition of a member must be annotated with a name, which is used as an identifier, and a type. Members can have no value (see [Abstract objects](#abstract-objects)).

There must not be two members with the same name in an object. The order of the members does not matter. A member's type determines which vales can be assigned to it. The chapter [Data Types](Data-Types) explains the types offered by nyan.

Comments are denoted by a `#` and can be placed after the declaration of a member in the same line. The nyan interpreter will skip everything in the line after reading the `#` character. 

## Inheritance

Instead of redefinig an object from scratch, one can use inheritance to specialize an already defined object. When an object *inherits* from another one it automatically aquires all members of the object it inherits from, including their defined types and assigned values. The inheriting object can also add its own members or choose to reassign values to members aquired. One can look at inheritance as a form of **specialization**.

The inheriting object is called *child object* or *child* for short. Objects that are inherited from are called *parent objects*. Thus the phrases "Object B inherits from Object A" and "B is a child of A" or "A is a parent of B" are equal.

In nyan, inheritance is *transitive*, so a child object is of type of all its parent objects, and the parents of those, and so on. Inheritance is declared by writing the name of the parent object in the paranthesis next the the name of the child object.

```
# Empty paranthesis means the object does not inherit
BaseObject():
    parent_member : int = 2
    other_member : text = "Hello"

# Let ChildObject inherit from BaseObject
ChildObject(BaseObject):
    child_member : float = 12.65
```

By inheriting `ChildObject` automatically aquires the members `parent_member` and `other_member`. It also defines its own member `child_member` which is independent of `BaseObject`. By definition, `ChildObject` is also of type `BaseObject`. Without inheritance, the objects' declaration would look like this:

```
BaseObject():
    parent_member : int = 2
    other_member : text = "Hello"

ChildObject():
    parent_member : int = 2
    other_member : text = "Hello"
    child_member : float = 12.65
```

Clearly this is inefficient because we define the same members twice. We would also have to make every change to `parent_member` in both `BaseObject` and `ChildObject`.

### Changing members while inheriting

Children are not stuck with the values assigned to a parents member. They have the option to reassign or change a new value in their object declaration. On the other hand, the type of the member must not be changed.

```
BaseObject():
    name : text = "Gustav"
    parent_member : int = 2

ChildObject(BaseObject):
    # Reassigns a new value to the member "name"
    name = "Lukas"

    child_member : float = 12.65
```

In the above example the value of the member `name` is changed to `Lukas` by the child object. Because type changes are not allowed for inheritance the reassignment of `name` does not need to specify it. Doing so will result in a error. `parent_member` is also a member of `ChildObject` and keeps the value `2` from the declaration in the parent. `child_member` is first declared in `ChildObject` and is therefore required to specify the type.

```
BaseObject():
    name : text = "Gustav"
    parent_member : int = 2

ChildObject(BaseObject):
    # Adds the value to the member "parent_member"
    parent_member += 5

    child_member : float = 12.65
```

In this example `parent_member` is not reassigned, but changed by using the `+=` operator. The value of `parent_member` in `ChildObject` is now `7`, while it is still `2` in `BaseObject`. The available operations for each type can be found in the chapter [Data Types](Data-Types).

### Multiple Inheritance

nyan supports a mechanism called *multiple inheritance* which means that an object can inherit from an unlimited number of other objects.

```
BaseObject():
    parent_member : int = 2
    other_member : text = "Hello"

DifferentObject():
    different_member : bool = true

# Let ChildObject inherit from BaseObject and DifferentObject
ChildObject(BaseObject, DifferentObject):
    child_member : float = 12.65
```

The names of the parents are separated by a comma (`,`) in the declaration of `ChildObject`. `ChildObject` has four members. The members `parent_member` and `other_member` are gained from the parent `BaseObject`. `different_member` is aquired from `DifferentObject`. There is also `child_member` which is first declared in `ChildObject` and not inherited. Because of the rules of transitivity `ChildObject` is also of type `BaseObject` and `DifferentObject`.

Multiple inheritance can create the problem of naming ambiguities. They can occur when multiple parents define a member with the same name. An example is shown below.

```
Base():
    setting : int = 0

Intermediate0(Base):
    setting += 1
    property : int = 10

Intermediate1(Base):
    setting *= 2
    property : int = 20

Independent():
    setting : int = 100
    property : int = 1234

Mixed(Intermediate0, Intermediate1, Independent):
    setting += 3   # which origin is meant?
    property += 5
```

Here the intended origin of both `setting` and `property` in `MixedObject` is unclear. For example, the result for `setting` could be `4` (origin: `Intermediate0`), `3` (origin: `Intermediate1`) or `103` (origin: `Independent`). To resolve this conflict, one has to specify the intended parent in front of the member name, for example `Independent.setting`.

```

Base():
    setting : int = 0

Intermediate0(Base):
    setting += 1
    property : int = 10

Intermediate1(Base):
    setting *= 2
    property : int = 20

Independent():
    setting : int = 100
    property : int = 1234

Mixed(Intermediate0, Intermediate1, Independent):
    # origin is clear now
    Independent.setting += 3
    Intermediate0.property += 5
```

The above example is only one of the possible solutions. `Intermediate0.setting` or `Intermediate1.setting` would have also been viable for clarifying the origin of `setting`.

## Patches

A special variation of a normal nyan object is the *patch*. Patches are used to change member values of an object or add additional members. They are discussed in detail in the chapter [Patches](Patches).

## Abstract Objects

An object is called *abstract* when one of its members was not assigned a value. Abstract objects cannot be assigned as values to a member (but can be set as types). The values have to be specified by a child object.

From a non-technical viewpoint, abstract objects are templates for the game entities players eventually interact with. An example is the abstract object `Unit` used in the openage engine. For example, `Unit` contains the members `hp` and `name`, but does not define values for them. That way one can guarantee that an ingame object derived from `Unit` always provides the members `hp` and `name` and has also assigned values for them.
 
## Nested Objects

Objects can also be defined within another object. The inner one is a nested object, and can contain nested objects itself. The purpose of nested objects is to allow further grouping. In openage they are useful for specifying abiltiies and boni of units. Nested objects are allowed to have their own members and inherit from other objects. The name of a nested object is a combination of the name of its parent and the nested object name.

```
Rainbow(): # top-level object
    Goldpot(): # nested object
        amount : int = 9001
```

In this example `Goldpot` is a nested object of `Rainbow`. The member `amount` belongs to the nested object.
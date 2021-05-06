# Copyright 2019-2021 the openage authors. See copying.md for legal info.
#
# pylint: disable=line-too-long,too-many-lines,too-many-statements
"""
Loads the API into the converter.

TODO: Implement a parser instead of hardcoded
object creation.
"""

from ....nyan.nyan_structs import NyanMemberType
from ....nyan.nyan_structs import NyanObject, NyanMember, MemberType, MemberSpecialValue,\
    MemberOperator

# Common primitive types
# We can use these so we don't have to create them every single time
N_INT = NyanMemberType("int")
N_FLOAT = NyanMemberType("float")
N_TEXT = NyanMemberType("text")
N_FILE = NyanMemberType("file")
N_BOOL = NyanMemberType("bool")


def load_api():
    """
    Returns a dict with the API object's fqon as keys
    and the API objects as values.
    """
    api_objects = {}

    api_objects = _create_objects(api_objects)
    _insert_members(api_objects)

    return api_objects


def _create_objects(api_objects):
    """
    Creates the API objects.
    """
    # engine.root
    # engine.root.Object
    nyan_object = NyanObject("Object")
    fqon = "engine.root.Object"
    nyan_object.set_fqon(fqon)

    api_objects.update({fqon: nyan_object})

    # engine.ability
    # engine.ability.Ability
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Ability", parents)
    fqon = "engine.ability.Ability"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.property.AbilityProperty
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("AbilityProperty", parents)
    fqon = "engine.ability.property.AbilityProperty"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.property.type.Animated
    parents = [api_objects["engine.ability.property.AbilityProperty"]]
    nyan_object = NyanObject("Animated", parents)
    fqon = "engine.ability.property.type.Animated"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.property.type.AnimationOverride
    parents = [api_objects["engine.ability.property.AbilityProperty"]]
    nyan_object = NyanObject("AnimationOverride", parents)
    fqon = "engine.ability.property.type.AnimationOverride"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.property.type.CommandSound
    parents = [api_objects["engine.ability.property.AbilityProperty"]]
    nyan_object = NyanObject("CommandSound", parents)
    fqon = "engine.ability.property.type.CommandSound"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.property.type.Diplomatic
    parents = [api_objects["engine.ability.property.AbilityProperty"]]
    nyan_object = NyanObject("Diplomatic", parents)
    fqon = "engine.ability.property.type.Diplomatic"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.property.type.ExecutionSound
    parents = [api_objects["engine.ability.property.AbilityProperty"]]
    nyan_object = NyanObject("ExecutionSound", parents)
    fqon = "engine.ability.property.type.ExecutionSound"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.property.type.Lock
    parents = [api_objects["engine.ability.property.AbilityProperty"]]
    nyan_object = NyanObject("Lock", parents)
    fqon = "engine.ability.property.type.Lock"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.ActiveTransformTo
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("ActiveTransformTo", parents)
    fqon = "engine.ability.type.ActiveTransformTo"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.ApplyContinuousEffect
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("ApplyContinuousEffect", parents)
    fqon = "engine.ability.type.ApplyContinuousEffect"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.ApplyDiscreteEffect
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("ApplyDiscreteEffect", parents)
    fqon = "engine.ability.type.ApplyDiscreteEffect"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.AttributeChangeTracker
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("AttributeChangeTracker", parents)
    fqon = "engine.ability.type.AttributeChangeTracker"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Cloak
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Cloak", parents)
    fqon = "engine.ability.type.Cloak"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.CollectStorage
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("CollectStorage", parents)
    fqon = "engine.ability.type.CollectStorage"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Constructable
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Constructable", parents)
    fqon = "engine.ability.type.Constructable"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Create
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Create", parents)
    fqon = "engine.ability.type.Create"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Despawn
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Despawn", parents)
    fqon = "engine.ability.type.Despawn"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.DetectCloak
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("DetectCloak", parents)
    fqon = "engine.ability.type.DetectCloak"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.DropResources
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("DropResources", parents)
    fqon = "engine.ability.type.DropResources"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.DropSite
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("DropSite", parents)
    fqon = "engine.ability.type.DropSite"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.EnterContainer
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("EnterContainer", parents)
    fqon = "engine.ability.type.EnterContainer"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.ExchangeResources
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("ExchangeResources", parents)
    fqon = "engine.ability.type.ExchangeResources"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.ExitContainer
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("ExitContainer", parents)
    fqon = "engine.ability.type.ExitContainer"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Fly
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Fly", parents)
    fqon = "engine.ability.type.Fly"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Formation
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Formation", parents)
    fqon = "engine.ability.type.Formation"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Foundation
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Foundation", parents)
    fqon = "engine.ability.type.Foundation"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.GameEntityStance
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("GameEntityStance", parents)
    fqon = "engine.ability.type.GameEntityStance"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Gather
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Gather", parents)
    fqon = "engine.ability.type.Gather"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Harvestable
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Harvestable", parents)
    fqon = "engine.ability.type.Harvestable"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Herd
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Herd", parents)
    fqon = "engine.ability.type.Herd"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Herdable
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Herdable", parents)
    fqon = "engine.ability.type.Herdable"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Hitbox
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Hitbox", parents)
    fqon = "engine.ability.type.Hitbox"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Idle
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Idle", parents)
    fqon = "engine.ability.type.Idle"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.LineOfSight
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("LineOfSight", parents)
    fqon = "engine.ability.type.LineOfSight"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Live
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Live", parents)
    fqon = "engine.ability.type.Live"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Lock
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Lock", parents)
    fqon = "engine.ability.type.Lock"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Move
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Move", parents)
    fqon = "engine.ability.type.Move"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Named
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Named", parents)
    fqon = "engine.ability.type.Named"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.OverlayTerrain
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("OverlayTerrain", parents)
    fqon = "engine.ability.type.OverlayTerrain"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Passable
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Passable", parents)
    fqon = "engine.ability.type.Passable"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.PassiveTransformTo
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("PassiveTransformTo", parents)
    fqon = "engine.ability.type.PassiveTransformTo"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.ProductionQueue
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("ProductionQueue", parents)
    fqon = "engine.ability.type.ProductionQueue"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Projectile
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Projectile", parents)
    fqon = "engine.ability.type.Projectile"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.ProvideContingent
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("ProvideContingent", parents)
    fqon = "engine.ability.type.ProvideContingent"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.RallyPoint
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("RallyPoint", parents)
    fqon = "engine.ability.type.RallyPoint"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.RangedContinuousEffect
    parents = [api_objects["engine.ability.type.ApplyContinuousEffect"]]
    nyan_object = NyanObject("RangedContinuousEffect", parents)
    fqon = "engine.ability.type.RangedContinuousEffect"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.RangedDiscreteEffect
    parents = [api_objects["engine.ability.type.ApplyDiscreteEffect"]]
    nyan_object = NyanObject("RangedDiscreteEffect", parents)
    fqon = "engine.ability.type.RangedDiscreteEffect"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.RegenerateAttribute
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("RegenerateAttribute", parents)
    fqon = "engine.ability.type.RegenerateAttribute"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.RegenerateResourceSpot
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("RegenerateResourceSpot", parents)
    fqon = "engine.ability.type.RegenerateResourceSpot"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.RemoveStorage
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("RemoveStorage", parents)
    fqon = "engine.ability.type.RemoveStorage"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Research
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Research", parents)
    fqon = "engine.ability.type.Research"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Resistance
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Resistance", parents)
    fqon = "engine.ability.type.Resistance"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.ResourceStorage
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("ResourceStorage", parents)
    fqon = "engine.ability.type.ResourceStorage"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Restock
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Restock", parents)
    fqon = "engine.ability.type.Restock"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Selectable
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Selectable", parents)
    fqon = "engine.ability.type.Selectable"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.SendBackToTask
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("SendBackToTask", parents)
    fqon = "engine.ability.type.SendBackToTask"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.ShootProjectile
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("ShootProjectile", parents)
    fqon = "engine.ability.type.ShootProjectile"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Stop
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Stop", parents)
    fqon = "engine.ability.type.Stop"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Storage
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Storage", parents)
    fqon = "engine.ability.type.Storage"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.TerrainRequirement
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("TerrainRequirement", parents)
    fqon = "engine.ability.type.TerrainRequirement"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Trade
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Trade", parents)
    fqon = "engine.ability.type.Trade"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.TradePost
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("TradePost", parents)
    fqon = "engine.ability.type.TradePost"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.TransferStorage
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("TransferStorage", parents)
    fqon = "engine.ability.type.TransferStorage"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Turn
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Turn", parents)
    fqon = "engine.ability.type.Turn"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.UseContingent
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("UseContingent", parents)
    fqon = "engine.ability.type.UseContingent"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.Visibility
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Visibility", parents)
    fqon = "engine.ability.type.Visibility"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util
    # engine.util.accuracy.Accuracy
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Accuracy", parents)
    fqon = "engine.util.accuracy.Accuracy"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.animation_override.AnimationOverride
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("AnimationOverride", parents)
    fqon = "engine.util.animation_override.AnimationOverride"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.animation_override.type.Reset
    parents = [api_objects["engine.util.animation_override.AnimationOverride"]]
    nyan_object = NyanObject("Reset", parents)
    fqon = "engine.util.animation_override.type.Reset"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.attribute.Attribute
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Attribute", parents)
    fqon = "engine.util.attribute.Attribute"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.attribute.AttributeAmount
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("AttributeAmount", parents)
    fqon = "engine.util.attribute.AttributeAmount"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.attribute.AttributeRate
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("AttributeRate", parents)
    fqon = "engine.util.attribute.AttributeRate"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.attribute.AttributeSetting
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("AttributeSetting", parents)
    fqon = "engine.util.attribute.AttributeSetting"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.attribute.ProtectingAttribute
    parents = [api_objects["engine.util.attribute.Attribute"]]
    nyan_object = NyanObject("ProtectingAttribute", parents)
    fqon = "engine.util.attribute.ProtectingAttribute"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.attribute_change_type.AttributeChangeType
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("AttributeChangeType", parents)
    fqon = "engine.util.attribute_change_type.AttributeChangeType"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.attribute_change_type.type.Fallback
    parents = [api_objects["engine.util.attribute_change_type.AttributeChangeType"]]
    nyan_object = NyanObject("Fallback", parents)
    fqon = "engine.util.attribute_change_type.type.Fallback"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.calculation_type.CalculationType
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("CalculationType", parents)
    fqon = "engine.util.calculation_type.CalculationType"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.calculation_type.type.Hyperbolic
    parents = [api_objects["engine.util.calculation_type.CalculationType"]]
    nyan_object = NyanObject("Hyperbolic", parents)
    fqon = "engine.util.calculation_type.type.Hyperbolic"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.calculation_type.type.Linear
    parents = [api_objects["engine.util.calculation_type.CalculationType"]]
    nyan_object = NyanObject("Linear", parents)
    fqon = "engine.util.calculation_type.type.Linear"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.calculation_type.type.NoStack
    parents = [api_objects["engine.util.calculation_type.CalculationType"]]
    nyan_object = NyanObject("NoStack", parents)
    fqon = "engine.util.calculation_type.type.NoStack"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.cheat.Cheat
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Cheat", parents)
    fqon = "engine.util.cheat.Cheat"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.container_type.SendToContainerType
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("SendToContainerType", parents)
    fqon = "engine.util.container_type.SendToContainerType"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.convert_type.ConvertType
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("ConvertType", parents)
    fqon = "engine.util.convert_type.ConvertType"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.cost.Cost
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Cost", parents)
    fqon = "engine.util.cost.Cost"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.cost.type.AttributeCost
    parents = [api_objects["engine.util.cost.Cost"]]
    nyan_object = NyanObject("AttributeCost", parents)
    fqon = "engine.util.cost.type.AttributeCost"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.cost.type.ResourceCost
    parents = [api_objects["engine.util.cost.Cost"]]
    nyan_object = NyanObject("ResourceCost", parents)
    fqon = "engine.util.cost.type.ResourceCost"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.create.CreatableGameEntity
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("CreatableGameEntity", parents)
    fqon = "engine.util.create.CreatableGameEntity"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.diplomatic_stance.DiplomaticStance
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("DiplomaticStance", parents)
    fqon = "engine.util.diplomatic_stance.DiplomaticStance"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.diplomatic_stance.type.Any
    parents = [api_objects["engine.util.diplomatic_stance.DiplomaticStance"]]
    nyan_object = NyanObject("Any", parents)
    fqon = "engine.util.diplomatic_stance.type.Any"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.diplomatic_stance.type.Self
    parents = [api_objects["engine.util.diplomatic_stance.DiplomaticStance"]]
    nyan_object = NyanObject("Self", parents)
    fqon = "engine.util.diplomatic_stance.type.Self"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.distribution_type.DistributionType
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("DistributionType", parents)
    fqon = "engine.util.distribution_type.DistributionType"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.distribution_type.type.Mean
    parents = [api_objects["engine.util.distribution_type.DistributionType"]]
    nyan_object = NyanObject("Mean", parents)
    fqon = "engine.util.distribution_type.type.Mean"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.dropoff_type.DropoffType
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("DropoffType", parents)
    fqon = "engine.util.dropoff_type.DropoffType"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.dropoff_type.type.InverseLinear
    parents = [api_objects["engine.util.dropoff_type.DropoffType"]]
    nyan_object = NyanObject("InverseLinear", parents)
    fqon = "engine.util.dropoff_type.type.InverseLinear"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.dropoff_type.type.Linear
    parents = [api_objects["engine.util.dropoff_type.DropoffType"]]
    nyan_object = NyanObject("Linear", parents)
    fqon = "engine.util.dropoff_type.type.Linear"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.dropoff_type.type.NoDropoff
    parents = [api_objects["engine.util.dropoff_type.DropoffType"]]
    nyan_object = NyanObject("NoDropoff", parents)
    fqon = "engine.util.dropoff_type.type.NoDropoff"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.effect_batch.EffectBatch
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("EffectBatch", parents)
    fqon = "engine.util.effect_batch.EffectBatch"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.effect_batch.property.BatchProperty
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("BatchProperty", parents)
    fqon = "engine.util.effect_batch.property.BatchProperty"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.effect_batch.property.type.Chance
    parents = [api_objects["engine.util.effect_batch.property.BatchProperty"]]
    nyan_object = NyanObject("Chance", parents)
    fqon = "engine.util.effect_batch.property.type.Chance"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.effect_batch.property.type.Priority
    parents = [api_objects["engine.util.effect_batch.property.BatchProperty"]]
    nyan_object = NyanObject("Priority", parents)
    fqon = "engine.util.effect_batch.property.type.Priority"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.effect_batch.type.ChainedBatch
    parents = [api_objects["engine.util.effect_batch.EffectBatch"]]
    nyan_object = NyanObject("ChainedBatch", parents)
    fqon = "engine.util.effect_batch.type.ChainedBatch"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.effect_batch.type.OrderedBatch
    parents = [api_objects["engine.util.effect_batch.EffectBatch"]]
    nyan_object = NyanObject("OrderedBatch", parents)
    fqon = "engine.util.effect_batch.type.OrderedBatch"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.effect_batch.type.UnorderedBatch
    parents = [api_objects["engine.util.effect_batch.EffectBatch"]]
    nyan_object = NyanObject("UnorderedBatch", parents)
    fqon = "engine.util.effect_batch.type.UnorderedBatch"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.exchange_mode.ExchangeMode
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("ExchangeMode", parents)
    fqon = "engine.util.exchange_mode.ExchangeMode"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.exchange_mode.type.Buy
    parents = [api_objects["engine.util.exchange_mode.ExchangeMode"]]
    nyan_object = NyanObject("Buy", parents)
    fqon = "engine.util.exchange_mode.type.Buy"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.exchange_mode.type.Sell
    parents = [api_objects["engine.util.exchange_mode.ExchangeMode"]]
    nyan_object = NyanObject("Sell", parents)
    fqon = "engine.util.exchange_mode.type.Sell"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.exchange_rate.ExchangeRate
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("ExchangeRate", parents)
    fqon = "engine.util.exchange_rate.ExchangeRate"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.formation.Formation
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Formation", parents)
    fqon = "engine.util.formation.Formation"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.formation.Subformation
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Subformation", parents)
    fqon = "engine.util.formation.Subformation"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.game_entity.GameEntity
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("GameEntity", parents)
    fqon = "engine.util.game_entity.GameEntity"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.game_entity_formation.GameEntityFormation
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("GameEntityFormation", parents)
    fqon = "engine.util.game_entity_formation.GameEntityFormation"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.game_entity_stance.GameEntityStance
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("GameEntityStance", parents)
    fqon = "engine.util.game_entity_stance.GameEntityStance"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.game_entity_stance.type.Aggressive
    parents = [api_objects["engine.util.game_entity_stance.GameEntityStance"]]
    nyan_object = NyanObject("Aggressive", parents)
    fqon = "engine.util.game_entity_stance.type.Aggressive"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.game_entity_stance.type.Defensive
    parents = [api_objects["engine.util.game_entity_stance.GameEntityStance"]]
    nyan_object = NyanObject("Defensive", parents)
    fqon = "engine.util.game_entity_stance.type.Defensive"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.game_entity_stance.type.Passive
    parents = [api_objects["engine.util.game_entity_stance.GameEntityStance"]]
    nyan_object = NyanObject("Passive", parents)
    fqon = "engine.util.game_entity_stance.type.Passive"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.game_entity_stance.type.StandGround
    parents = [api_objects["engine.util.game_entity_stance.GameEntityStance"]]
    nyan_object = NyanObject("StandGround", parents)
    fqon = "engine.util.game_entity_stance.type.StandGround"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.game_entity_type.GameEntityType
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("GameEntityType", parents)
    fqon = "engine.util.game_entity_type.GameEntityType"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.game_entity_type.type.Any
    parents = [api_objects["engine.util.game_entity_type.GameEntityType"]]
    nyan_object = NyanObject("Any", parents)
    fqon = "engine.util.game_entity_type.type.Any"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.graphics.Animation
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Animation", parents)
    fqon = "engine.util.graphics.Animation"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.graphics.Palette
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Palette", parents)
    fqon = "engine.util.graphics.Palette"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.graphics.Terrain
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Terrain", parents)
    fqon = "engine.util.graphics.Terrain"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.herdable_mode.HerdableMode
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("HerdableMode", parents)
    fqon = "engine.util.herdable_mode.HerdableMode"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.herdable_mode.type.ClosestHerding
    parents = [api_objects["engine.util.herdable_mode.HerdableMode"]]
    nyan_object = NyanObject("ClosestHerding", parents)
    fqon = "engine.util.herdable_mode.type.ClosestHerding"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.herdable_mode.type.LongestTimeInRange
    parents = [api_objects["engine.util.herdable_mode.HerdableMode"]]
    nyan_object = NyanObject("LongestTimeInRange", parents)
    fqon = "engine.util.herdable_mode.type.LongestTimeInRange"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.herdable_mode.type.MostHerding
    parents = [api_objects["engine.util.herdable_mode.HerdableMode"]]
    nyan_object = NyanObject("MostHerding", parents)
    fqon = "engine.util.herdable_mode.type.MostHerding"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.hitbox.Hitbox
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Hitbox", parents)
    fqon = "engine.util.hitbox.Hitbox"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.language.Language
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Language", parents)
    fqon = "engine.util.language.Language"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.language.LanguageMarkupPair
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("LanguageMarkupPair", parents)
    fqon = "engine.util.language.LanguageMarkupPair"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.language.LanguageSoundPair
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("LanguageSoundPair", parents)
    fqon = "engine.util.language.LanguageSoundPair"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.language.LanguageTextPair
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("LanguageTextPair", parents)
    fqon = "engine.util.language.LanguageTextPair"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.language.translated.TranslatedObject
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("TranslatedObject", parents)
    fqon = "engine.util.language.translated.TranslatedObject"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.language.translated.type.TranslatedMarkupFile
    parents = [api_objects["engine.util.language.translated.TranslatedObject"]]
    nyan_object = NyanObject("TranslatedMarkupFile", parents)
    fqon = "engine.util.language.translated.type.TranslatedMarkupFile"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.language.translated.type.TranslatedSound
    parents = [api_objects["engine.util.language.translated.TranslatedObject"]]
    nyan_object = NyanObject("TranslatedSound", parents)
    fqon = "engine.util.language.translated.type.TranslatedSound"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.language.translated.type.TranslatedString
    parents = [api_objects["engine.util.language.translated.TranslatedObject"]]
    nyan_object = NyanObject("TranslatedString", parents)
    fqon = "engine.util.language.translated.type.TranslatedString"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.lock.LockPool
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("LockPool", parents)
    fqon = "engine.util.lock.LockPool"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.LogicElement
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("LogicElement", parents)
    fqon = "engine.util.logic.LogicElement"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.const.False
    parents = [api_objects["engine.util.logic.LogicElement"]]
    nyan_object = NyanObject("False", parents)
    fqon = "engine.util.logic.const.False"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.const.True
    parents = [api_objects["engine.util.logic.LogicElement"]]
    nyan_object = NyanObject("True", parents)
    fqon = "engine.util.logic.const.True"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.gate.LogicGate
    parents = [api_objects["engine.util.logic.LogicElement"]]
    nyan_object = NyanObject("LogicGate", parents)
    fqon = "engine.util.logic.gate.LogicGate"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.gate.type.AND
    parents = [api_objects["engine.util.logic.gate.LogicGate"]]
    nyan_object = NyanObject("AND", parents)
    fqon = "engine.util.logic.gate.type.AND"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.gate.type.MULTIXOR
    parents = [api_objects["engine.util.logic.gate.LogicGate"]]
    nyan_object = NyanObject("MULTIXOR", parents)
    fqon = "engine.util.logic.gate.type.MULTIXOR"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.gate.type.NOT
    parents = [api_objects["engine.util.logic.gate.LogicGate"]]
    nyan_object = NyanObject("NOT", parents)
    fqon = "engine.util.logic.gate.type.NOT"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.gate.type.OR
    parents = [api_objects["engine.util.logic.gate.LogicGate"]]
    nyan_object = NyanObject("OR", parents)
    fqon = "engine.util.logic.gate.type.OR"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.gate.type.SUBSETMAX
    parents = [api_objects["engine.util.logic.gate.LogicGate"]]
    nyan_object = NyanObject("SUBSETMAX", parents)
    fqon = "engine.util.logic.gate.type.SUBSETMAX"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.gate.type.SUBSETMIN
    parents = [api_objects["engine.util.logic.gate.LogicGate"]]
    nyan_object = NyanObject("SUBSETMIN", parents)
    fqon = "engine.util.logic.gate.type.SUBSETMIN"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.gate.type.XOR
    parents = [api_objects["engine.util.logic.gate.LogicGate"]]
    nyan_object = NyanObject("XOR", parents)
    fqon = "engine.util.logic.gate.type.XOR"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.literal.Literal
    parents = [api_objects["engine.util.logic.LogicElement"]]
    nyan_object = NyanObject("Literal", parents)
    fqon = "engine.util.logic.literal.Literal"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.literal.type.AttributeAbovePercentage
    parents = [api_objects["engine.util.logic.literal.Literal"]]
    nyan_object = NyanObject("AttributeAbovePercentage", parents)
    fqon = "engine.util.logic.literal.type.AttributeAbovePercentage"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.literal.type.AttributeAboveValue
    parents = [api_objects["engine.util.logic.literal.Literal"]]
    nyan_object = NyanObject("AttributeAboveValue", parents)
    fqon = "engine.util.logic.literal.type.AttributeAboveValue"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.literal.type.AttributeBelowPercentage
    parents = [api_objects["engine.util.logic.literal.Literal"]]
    nyan_object = NyanObject("AttributeBelowPercentage", parents)
    fqon = "engine.util.logic.literal.type.AttributeBelowPercentage"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.literal.type.AttributeBelowValue
    parents = [api_objects["engine.util.logic.literal.Literal"]]
    nyan_object = NyanObject("AttributeBelowValue", parents)
    fqon = "engine.util.logic.literal.type.AttributeBelowValue"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.literal.type.GameEntityProgress
    parents = [api_objects["engine.util.logic.literal.Literal"]]
    nyan_object = NyanObject("GameEntityProgress", parents)
    fqon = "engine.util.logic.literal.type.GameEntityProgress"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.literal.type.OwnsGameEntity
    parents = [api_objects["engine.util.logic.literal.Literal"]]
    nyan_object = NyanObject("OwnsGameEntity", parents)
    fqon = "engine.util.logic.literal.type.OwnsGameEntity"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.literal.type.ProjectileHitTerrain
    parents = [api_objects["engine.util.logic.literal.Literal"]]
    nyan_object = NyanObject("ProjectileHitTerrain", parents)
    fqon = "engine.util.logic.literal.type.ProjectileHitTerrain"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.literal.type.ProjectilePassThrough
    parents = [api_objects["engine.util.logic.literal.Literal"]]
    nyan_object = NyanObject("ProjectilePassThrough", parents)
    fqon = "engine.util.logic.literal.type.ProjectilePassThrough"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.literal.type.ResourceSpotsDepleted
    parents = [api_objects["engine.util.logic.literal.Literal"]]
    nyan_object = NyanObject("ResourceSpotsDepleted", parents)
    fqon = "engine.util.logic.literal.type.ResourceSpotsDepleted"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.literal.type.StateChangeActive
    parents = [api_objects["engine.util.logic.literal.Literal"]]
    nyan_object = NyanObject("StateChangeActive", parents)
    fqon = "engine.util.logic.literal.type.StateChangeActive"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.literal.type.TechResearched
    parents = [api_objects["engine.util.logic.literal.Literal"]]
    nyan_object = NyanObject("TechResearched", parents)
    fqon = "engine.util.logic.literal.type.TechResearched"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.literal.type.Timer
    parents = [api_objects["engine.util.logic.literal.Literal"]]
    nyan_object = NyanObject("Timer", parents)
    fqon = "engine.util.logic.literal.type.Timer"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.literal_scope.LiteralScope
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("LiteralScope", parents)
    fqon = "engine.util.logic.literal_scope.LiteralScope"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.literal_scope.type.Any
    parents = [api_objects["engine.util.logic.literal_scope.LiteralScope"]]
    nyan_object = NyanObject("Any", parents)
    fqon = "engine.util.logic.literal_scope.type.Any"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.logic.literal_scope.type.Self
    parents = [api_objects["engine.util.logic.literal_scope.LiteralScope"]]
    nyan_object = NyanObject("Self", parents)
    fqon = "engine.util.logic.literal_scope.type.Self"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.lure_type.LureType
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("LureType", parents)
    fqon = "engine.util.lure_type.LureType"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.mod.Mod
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Mod", parents)
    fqon = "engine.util.mod.Mod"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.modifier_scope.ModifierScope
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("ModifierScope", parents)
    fqon = "engine.util.modifier_scope.ModifierScope"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.modifier_scope.type.GameEntityScope
    parents = [api_objects["engine.util.modifier_scope.ModifierScope"]]
    nyan_object = NyanObject("GameEntityScope", parents)
    fqon = "engine.util.modifier_scope.type.GameEntityScope"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.modifier_scope.type.Standard
    parents = [api_objects["engine.util.modifier_scope.ModifierScope"]]
    nyan_object = NyanObject("Standard", parents)
    fqon = "engine.util.modifier_scope.type.Standard"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.move_mode.MoveMode
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("MoveMode", parents)
    fqon = "engine.util.move_mode.MoveMode"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.move_mode.type.AttackMove
    parents = [api_objects["engine.util.move_mode.MoveMode"]]
    nyan_object = NyanObject("AttackMove", parents)
    fqon = "engine.util.move_mode.type.AttackMove"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.move_mode.type.Follow
    parents = [api_objects["engine.util.move_mode.MoveMode"]]
    nyan_object = NyanObject("Follow", parents)
    fqon = "engine.util.move_mode.type.Follow"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.move_mode.type.Guard
    parents = [api_objects["engine.util.move_mode.MoveMode"]]
    nyan_object = NyanObject("Guard", parents)
    fqon = "engine.util.move_mode.type.Guard"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.move_mode.type.Normal
    parents = [api_objects["engine.util.move_mode.MoveMode"]]
    nyan_object = NyanObject("Normal", parents)
    fqon = "engine.util.move_mode.type.Normal"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.move_mode.type.Patrol
    parents = [api_objects["engine.util.move_mode.MoveMode"]]
    nyan_object = NyanObject("Patrol", parents)
    fqon = "engine.util.move_mode.type.Patrol"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.passable_mode.PassableMode
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("PassableMode", parents)
    fqon = "engine.util.passable_mode.PassableMode"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.passable_mode.type.Gate
    parents = [api_objects["engine.util.passable_mode.PassableMode"]]
    nyan_object = NyanObject("Gate", parents)
    fqon = "engine.util.passable_mode.type.Gate"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.passable_mode.type.Normal
    parents = [api_objects["engine.util.passable_mode.PassableMode"]]
    nyan_object = NyanObject("Normal", parents)
    fqon = "engine.util.passable_mode.type.Normal"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.patch.NyanPatch
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("NyanPatch", parents)
    fqon = "engine.util.patch.NyanPatch"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.patch.Patch
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Patch", parents)
    fqon = "engine.util.patch.Patch"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.patch.property.PatchProperty
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("PatchProperty", parents)
    fqon = "engine.util.patch.property.PatchProperty"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.patch.property.type.Diplomatic
    parents = [api_objects["engine.util.patch.property.PatchProperty"]]
    nyan_object = NyanObject("Diplomatic", parents)
    fqon = "engine.util.patch.property.type.Diplomatic"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.payment_mode.PaymentMode
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("PaymentMode", parents)
    fqon = "engine.util.payment_mode.PaymentMode"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.payment_mode.type.Adaptive
    parents = [api_objects["engine.util.payment_mode.PaymentMode"]]
    nyan_object = NyanObject("Adaptive", parents)
    fqon = "engine.util.payment_mode.type.Adaptive"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.payment_mode.type.Advance
    parents = [api_objects["engine.util.payment_mode.PaymentMode"]]
    nyan_object = NyanObject("Advance", parents)
    fqon = "engine.util.payment_mode.type.Advance"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.payment_mode.type.Arrear
    parents = [api_objects["engine.util.payment_mode.PaymentMode"]]
    nyan_object = NyanObject("Arrear", parents)
    fqon = "engine.util.payment_mode.type.Arrear"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.payment_mode.type.Shadow
    parents = [api_objects["engine.util.payment_mode.PaymentMode"]]
    nyan_object = NyanObject("Shadow", parents)
    fqon = "engine.util.payment_mode.type.Shadow"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.placement_mode.PlacementMode
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("PlacementMode", parents)
    fqon = "engine.util.placement_mode.PlacementMode"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.placement_mode.type.Eject
    parents = [api_objects["engine.util.placement_mode.PlacementMode"]]
    nyan_object = NyanObject("Eject", parents)
    fqon = "engine.util.placement_mode.type.Eject"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.placement_mode.type.OwnStorage
    parents = [api_objects["engine.util.placement_mode.PlacementMode"]]
    nyan_object = NyanObject("OwnStorage", parents)
    fqon = "engine.util.placement_mode.type.OwnStorage"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.placement_mode.type.Place
    parents = [api_objects["engine.util.placement_mode.PlacementMode"]]
    nyan_object = NyanObject("Place", parents)
    fqon = "engine.util.placement_mode.type.Place"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.placement_mode.type.Replace
    parents = [api_objects["engine.util.placement_mode.PlacementMode"]]
    nyan_object = NyanObject("Replace", parents)
    fqon = "engine.util.placement_mode.type.Replace"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.price_mode.PriceMode
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("PriceMode", parents)
    fqon = "engine.util.price_mode.PriceMode"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.price_mode.type.Dynamic
    parents = [api_objects["engine.util.price_mode.PriceMode"]]
    nyan_object = NyanObject("Dynamic", parents)
    fqon = "engine.util.price_mode.type.Dynamic"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.price_mode.type.Fixed
    parents = [api_objects["engine.util.price_mode.PriceMode"]]
    nyan_object = NyanObject("Fixed", parents)
    fqon = "engine.util.price_mode.type.Fixed"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.price_pool.PricePool
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("PricePool", parents)
    fqon = "engine.util.price_pool.PricePool"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.production_mode.ProductionMode
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("ProductionMode", parents)
    fqon = "engine.util.production_mode.ProductionMode"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.production_mode.type.Creatables
    parents = [api_objects["engine.util.production_mode.ProductionMode"]]
    nyan_object = NyanObject("Creatables", parents)
    fqon = "engine.util.production_mode.type.Creatables"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.production_mode.type.Researchables
    parents = [api_objects["engine.util.production_mode.ProductionMode"]]
    nyan_object = NyanObject("Researchables", parents)
    fqon = "engine.util.production_mode.type.Researchables"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.progress.Progress
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Progress", parents)
    fqon = "engine.util.progress.Progress"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.progress.property.ProgressProperty
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("ProgressProperty", parents)
    fqon = "engine.util.progress.property.ProgressProperty"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.progress.property.type.Animated
    parents = [api_objects["engine.util.progress.property.ProgressProperty"]]
    nyan_object = NyanObject("Animated", parents)
    fqon = "engine.util.progress.property.type.Animated"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.progress.property.type.AnimationOverlay
    parents = [api_objects["engine.util.progress.property.ProgressProperty"]]
    nyan_object = NyanObject("AnimationOverlay", parents)
    fqon = "engine.util.progress.property.type.AnimationOverlay"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.progress.property.type.StateChange
    parents = [api_objects["engine.util.progress.property.ProgressProperty"]]
    nyan_object = NyanObject("StateChange", parents)
    fqon = "engine.util.progress.property.type.StateChange"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.progress.property.type.TerrainOverlay
    parents = [api_objects["engine.util.progress.property.ProgressProperty"]]
    nyan_object = NyanObject("TerrainOverlay", parents)
    fqon = "engine.util.progress.property.type.TerrainOverlay"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.progress.property.type.Terrain
    parents = [api_objects["engine.util.progress.property.ProgressProperty"]]
    nyan_object = NyanObject("Terrain", parents)
    fqon = "engine.util.progress.property.type.Terrain"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.progress_status.ProgressStatus
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("ProgressStatus", parents)
    fqon = "engine.util.progress_status.ProgressStatus"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.progress_type.ProgressType
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("ProgressType", parents)
    fqon = "engine.util.progress_type.ProgressType"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.progress_type.type.AttributeChange
    parents = [api_objects["engine.util.progress_type.ProgressType"]]
    nyan_object = NyanObject("AttributeChange", parents)
    fqon = "engine.util.progress_type.type.AttributeChange"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.progress_type.type.Carry
    parents = [api_objects["engine.util.progress_type.ProgressType"]]
    nyan_object = NyanObject("Carry", parents)
    fqon = "engine.util.progress_type.type.Carry"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.progress_type.type.Construct
    parents = [api_objects["engine.util.progress_type.ProgressType"]]
    nyan_object = NyanObject("Construct", parents)
    fqon = "engine.util.progress_type.type.Construct"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.progress_type.type.Harvest
    parents = [api_objects["engine.util.progress_type.ProgressType"]]
    nyan_object = NyanObject("Harvest", parents)
    fqon = "engine.util.progress_type.type.Harvest"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.progress_type.type.Restock
    parents = [api_objects["engine.util.progress_type.ProgressType"]]
    nyan_object = NyanObject("Restock", parents)
    fqon = "engine.util.progress_type.type.Restock"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.progress_type.type.Transform
    parents = [api_objects["engine.util.progress_type.ProgressType"]]
    nyan_object = NyanObject("Transform", parents)
    fqon = "engine.util.progress_type.type.Transform"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.research.ResearchableTech
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("ResearchableTech", parents)
    fqon = "engine.util.research.ResearchableTech"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.resource.Resource
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Resource", parents)
    fqon = "engine.util.resource.Resource"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.resource.ResourceContingent
    parents = [api_objects["engine.util.resource.Resource"]]
    nyan_object = NyanObject("ResourceContingent", parents)
    fqon = "engine.util.resource.ResourceContingent"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.resource.ResourceAmount
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("ResourceAmount", parents)
    fqon = "engine.util.resource.ResourceAmount"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.resource.ResourceRate
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("ResourceRate", parents)
    fqon = "engine.util.resource.ResourceRate"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.resource_spot.ResourceSpot
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("ResourceSpot", parents)
    fqon = "engine.util.resource_spot.ResourceSpot"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.selection_box.SelectionBox
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("SelectionBox", parents)
    fqon = "engine.util.selection_box.SelectionBox"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.selection_box.type.MatchToSprite
    parents = [api_objects["engine.util.selection_box.SelectionBox"]]
    nyan_object = NyanObject("MatchToSprite", parents)
    fqon = "engine.util.selection_box.type.MatchToSprite"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.selection_box.type.Rectangle
    parents = [api_objects["engine.util.selection_box.SelectionBox"]]
    nyan_object = NyanObject("Rectangle", parents)
    fqon = "engine.util.selection_box.type.Rectangle"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.setup.PlayerSetup
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("PlayerSetup", parents)
    fqon = "engine.util.setup.PlayerSetup"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.sound.Sound
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Sound", parents)
    fqon = "engine.util.sound.Sound"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.state_machine.StateChanger
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("StateChanger", parents)
    fqon = "engine.util.state_machine.StateChanger"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.state_machine.Reset
    parents = [api_objects["engine.util.state_machine.StateChanger"]]
    nyan_object = NyanObject("Reset", parents)
    fqon = "engine.util.state_machine.Reset"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.storage.EntityContainer
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("EntityContainer", parents)
    fqon = "engine.util.storage.EntityContainer"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.storage.ResourceContainer
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("ResourceContainer", parents)
    fqon = "engine.util.storage.ResourceContainer"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.storage.resource_container.type.InternalDropSite
    parents = [api_objects["engine.util.storage.ResourceContainer"]]
    nyan_object = NyanObject("InternalDropSite", parents)
    fqon = "engine.util.storage.resource_container.type.InternalDropSite"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.storage.StorageElementDefinition
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("StorageElementDefinition", parents)
    fqon = "engine.util.storage.StorageElementDefinition"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.target_mode.TargetMode
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("TargetMode", parents)
    fqon = "engine.util.target_mode.TargetMode"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.target_mode.type.CurrentPosition
    parents = [api_objects["engine.util.target_mode.TargetMode"]]
    nyan_object = NyanObject("CurrentPosition", parents)
    fqon = "engine.util.target_mode.type.CurrentPosition"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.target_mode.type.ExpectedPosition
    parents = [api_objects["engine.util.target_mode.TargetMode"]]
    nyan_object = NyanObject("ExpectedPosition", parents)
    fqon = "engine.util.target_mode.type.ExpectedPosition"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.taunt.Taunt
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Taunt", parents)
    fqon = "engine.util.taunt.Taunt"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.tech.Tech
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Tech", parents)
    fqon = "engine.util.tech.Tech"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.tech_type.TechType
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("TechType", parents)
    fqon = "engine.util.tech_type.TechType"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.tech_type.type.Any
    parents = [api_objects["engine.util.tech_type.TechType"]]
    nyan_object = NyanObject("Any", parents)
    fqon = "engine.util.tech_type.type.Any"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.terrain.Terrain
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Terrain", parents)
    fqon = "engine.util.terrain.Terrain"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.terrain.TerrainAmbient
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("TerrainAmbient", parents)
    fqon = "engine.util.terrain.TerrainAmbient"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.terrain_type.TerrainType
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("TerrainType", parents)
    fqon = "engine.util.terrain_type.TerrainType"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.terrain_type.type.Any
    parents = [api_objects["engine.util.terrain_type.TerrainType"]]
    nyan_object = NyanObject("Any", parents)
    fqon = "engine.util.terrain_type.type.Any"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.trade_route.TradeRoute
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("TradeRoute", parents)
    fqon = "engine.util.trade_route.TradeRoute"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.trade_route.type.AoE1TradeRoute
    parents = [api_objects["engine.util.trade_route.TradeRoute"]]
    nyan_object = NyanObject("AoE1TradeRoute", parents)
    fqon = "engine.util.trade_route.type.AoE1TradeRoute"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.trade_route.type.AoE2TradeRoute
    parents = [api_objects["engine.util.trade_route.TradeRoute"]]
    nyan_object = NyanObject("AoE2TradeRoute", parents)
    fqon = "engine.util.trade_route.type.AoE2TradeRoute"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.transform_pool.TransformPool
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("TransformPool", parents)
    fqon = "engine.util.transform_pool.TransformPool"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.variant.Variant
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Variant", parents)
    fqon = "engine.util.variant.Variant"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.variant.type.AdjacentTilesVariant
    parents = [api_objects["engine.util.variant.Variant"]]
    nyan_object = NyanObject("AdjacentTilesVariant", parents)
    fqon = "engine.util.variant.type.AdjacentTilesVariant"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.variant.type.MiscVariant
    parents = [api_objects["engine.util.variant.Variant"]]
    nyan_object = NyanObject("MiscVariant", parents)
    fqon = "engine.util.variant.type.MiscVariant"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.variant.type.RandomVariant
    parents = [api_objects["engine.util.variant.Variant"]]
    nyan_object = NyanObject("RandomVariant", parents)
    fqon = "engine.util.variant.type.RandomVariant"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.util.variant.type.PerspectiveVariant
    parents = [api_objects["engine.util.variant.Variant"]]
    nyan_object = NyanObject("PerspectiveVariant", parents)
    fqon = "engine.util.variant.type.PerspectiveVariant"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect
    # engine.effect.Effect
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Effect", parents)
    fqon = "engine.effect.Effect"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.property.EffectProperty
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("EffectProperty", parents)
    fqon = "engine.effect.property.EffectProperty"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.property.type.Area
    parents = [api_objects["engine.effect.property.EffectProperty"]]
    nyan_object = NyanObject("Area", parents)
    fqon = "engine.effect.property.type.Area"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.property.type.Cost
    parents = [api_objects["engine.effect.property.EffectProperty"]]
    nyan_object = NyanObject("Cost", parents)
    fqon = "engine.effect.property.type.Cost"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.property.type.Diplomatic
    parents = [api_objects["engine.effect.property.EffectProperty"]]
    nyan_object = NyanObject("Diplomatic", parents)
    fqon = "engine.effect.property.type.Diplomatic"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.property.type.Priority
    parents = [api_objects["engine.effect.property.EffectProperty"]]
    nyan_object = NyanObject("Priority", parents)
    fqon = "engine.effect.property.type.Priority"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.continuous.ContinuousEffect
    parents = [api_objects["engine.effect.Effect"]]
    nyan_object = NyanObject("ContinuousEffect", parents)
    fqon = "engine.effect.continuous.ContinuousEffect"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.continuous.flat_attribute_change.FlatAttributeChange
    parents = [api_objects["engine.effect.continuous.ContinuousEffect"]]
    nyan_object = NyanObject("FlatAttributeChange", parents)
    fqon = "engine.effect.continuous.flat_attribute_change.FlatAttributeChange"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.continuous.flat_attribute_change.type.FlatAttributeChangeDecrease
    parents = [api_objects["engine.effect.continuous.flat_attribute_change.FlatAttributeChange"]]
    nyan_object = NyanObject("FlatAttributeChangeDecrease", parents)
    fqon = "engine.effect.continuous.flat_attribute_change.type.FlatAttributeChangeDecrease"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.continuous.flat_attribute_change.type.FlatAttributeChangeIncrease
    parents = [api_objects["engine.effect.continuous.flat_attribute_change.FlatAttributeChange"]]
    nyan_object = NyanObject("FlatAttributeChangeIncrease", parents)
    fqon = "engine.effect.continuous.flat_attribute_change.type.FlatAttributeChangeIncrease"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.continuous.type.Lure
    parents = [api_objects["engine.effect.continuous.ContinuousEffect"]]
    nyan_object = NyanObject("Lure", parents)
    fqon = "engine.effect.continuous.type.Lure"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.continuous.time_relative_attribute.TimeRelativeAttributeChange
    parents = [api_objects["engine.effect.continuous.ContinuousEffect"]]
    nyan_object = NyanObject("TimeRelativeAttributeChange", parents)
    fqon = "engine.effect.continuous.time_relative_attribute.TimeRelativeAttributeChange"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.continuous.time_relative_attribute.type.TimeRelativeAttributeDecrease
    parents = [api_objects["engine.effect.continuous.time_relative_attribute.TimeRelativeAttributeChange"]]
    nyan_object = NyanObject("TimeRelativeAttributeDecrease", parents)
    fqon = "engine.effect.continuous.time_relative_attribute.type.TimeRelativeAttributeDecrease"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.continuous.time_relative_attribute.type.TimeRelativeAttributeIncrease
    parents = [api_objects["engine.effect.continuous.time_relative_attribute.TimeRelativeAttributeChange"]]
    nyan_object = NyanObject("TimeRelativeAttributeIncrease", parents)
    fqon = "engine.effect.continuous.time_relative_attribute.type.TimeRelativeAttributeIncrease"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.continuous.time_relative_progress.TimeRelativeProgressChange
    parents = [api_objects["engine.effect.continuous.ContinuousEffect"]]
    nyan_object = NyanObject("TimeRelativeProgressChange", parents)
    fqon = "engine.effect.continuous.time_relative_progress.TimeRelativeProgressChange"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.continuous.time_relative_progress.type.TimeRelativeProgressDecrease
    parents = [api_objects["engine.effect.continuous.time_relative_progress.TimeRelativeProgressChange"]]
    nyan_object = NyanObject("TimeRelativeProgressDecrease", parents)
    fqon = "engine.effect.continuous.time_relative_progress.type.TimeRelativeProgressDecrease"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.continuous.time_relative_progress.type.TimeRelativeProgressIncrease
    parents = [api_objects["engine.effect.continuous.time_relative_progress.TimeRelativeProgressChange"]]
    nyan_object = NyanObject("TimeRelativeProgressIncrease", parents)
    fqon = "engine.effect.continuous.time_relative_progress.type.TimeRelativeProgressIncrease"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.discrete.DiscreteEffect
    parents = [api_objects["engine.effect.Effect"]]
    nyan_object = NyanObject("DiscreteEffect", parents)
    fqon = "engine.effect.discrete.DiscreteEffect"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.discrete.convert.Convert
    parents = [api_objects["engine.effect.discrete.DiscreteEffect"]]
    nyan_object = NyanObject("Convert", parents)
    fqon = "engine.effect.discrete.convert.Convert"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.discrete.convert.type.AoE2Convert
    parents = [api_objects["engine.effect.discrete.convert.Convert"]]
    nyan_object = NyanObject("AoE2Convert", parents)
    fqon = "engine.effect.discrete.convert.type.AoE2Convert"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.discrete.flat_attribute_change.FlatAttributeChange
    parents = [api_objects["engine.effect.discrete.DiscreteEffect"]]
    nyan_object = NyanObject("FlatAttributeChange", parents)
    fqon = "engine.effect.discrete.flat_attribute_change.FlatAttributeChange"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease
    parents = [api_objects["engine.effect.discrete.flat_attribute_change.FlatAttributeChange"]]
    nyan_object = NyanObject("FlatAttributeChangeDecrease", parents)
    fqon = "engine.effect.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.discrete.flat_attribute_change.type.FlatAttributeChangeIncrease
    parents = [api_objects["engine.effect.discrete.flat_attribute_change.FlatAttributeChange"]]
    nyan_object = NyanObject("FlatAttributeChangeIncrease", parents)
    fqon = "engine.effect.discrete.flat_attribute_change.type.FlatAttributeChangeIncrease"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.discrete.type.MakeHarvestable
    parents = [api_objects["engine.effect.discrete.DiscreteEffect"]]
    nyan_object = NyanObject("MakeHarvestable", parents)
    fqon = "engine.effect.discrete.type.MakeHarvestable"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.discrete.type.SendToContainer
    parents = [api_objects["engine.effect.discrete.DiscreteEffect"]]
    nyan_object = NyanObject("SendToContainer", parents)
    fqon = "engine.effect.discrete.type.SendToContainer"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance
    # engine.resistance.Resistance
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Resistance", parents)
    fqon = "engine.resistance.Resistance"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.property.ResistanceProperty
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("ResistanceProperty", parents)
    fqon = "engine.resistance.property.ResistanceProperty"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.property.type.Cost
    parents = [api_objects["engine.resistance.property.ResistanceProperty"]]
    nyan_object = NyanObject("Cost", parents)
    fqon = "engine.resistance.property.type.Cost"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.property.type.Stacked
    parents = [api_objects["engine.resistance.property.ResistanceProperty"]]
    nyan_object = NyanObject("Stacked", parents)
    fqon = "engine.resistance.property.type.Stacked"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.continuous.ContinuousResistance
    parents = [api_objects["engine.resistance.Resistance"]]
    nyan_object = NyanObject("Resistance", parents)
    fqon = "engine.resistance.continuous.ContinuousResistance"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.continuous.flat_attribute_change.FlatAttributeChange
    parents = [api_objects["engine.resistance.continuous.ContinuousResistance"]]
    nyan_object = NyanObject("FlatAttributeChange", parents)
    fqon = "engine.resistance.continuous.flat_attribute_change.FlatAttributeChange"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.continuous.flat_attribute_change.type.FlatAttributeChangeDecrease
    parents = [api_objects["engine.resistance.continuous.flat_attribute_change.FlatAttributeChange"]]
    nyan_object = NyanObject("FlatAttributeChangeDecrease", parents)
    fqon = "engine.resistance.continuous.flat_attribute_change.type.FlatAttributeChangeDecrease"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.continuous.flat_attribute_change.type.FlatAttributeChangeIncrease
    parents = [api_objects["engine.resistance.continuous.flat_attribute_change.FlatAttributeChange"]]
    nyan_object = NyanObject("FlatAttributeChangeIncrease", parents)
    fqon = "engine.resistance.continuous.flat_attribute_change.type.FlatAttributeChangeIncrease"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.continuous.type.Lure
    parents = [api_objects["engine.resistance.continuous.ContinuousResistance"]]
    nyan_object = NyanObject("Lure", parents)
    fqon = "engine.resistance.continuous.type.Lure"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.continuous.time_relative_attribute.TimeRelativeAttributeChange
    parents = [api_objects["engine.resistance.continuous.ContinuousResistance"]]
    nyan_object = NyanObject("TimeRelativeAttributeChange", parents)
    fqon = "engine.resistance.continuous.time_relative_attribute.TimeRelativeAttributeChange"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.continuous.time_relative_attribute.type.TimeRelativeAttributeDecrease
    parents = [api_objects["engine.resistance.continuous.time_relative_attribute.TimeRelativeAttributeChange"]]
    nyan_object = NyanObject("TimeRelativeAttributeDecrease", parents)
    fqon = "engine.resistance.continuous.time_relative_attribute.type.TimeRelativeAttributeDecrease"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.continuous.time_relative_attribute.type.TimeRelativeAttributeIncrease
    parents = [api_objects["engine.resistance.continuous.time_relative_attribute.TimeRelativeAttributeChange"]]
    nyan_object = NyanObject("TimeRelativeAttributeIncrease", parents)
    fqon = "engine.resistance.continuous.time_relative_attribute.type.TimeRelativeAttributeIncrease"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.continuous.time_relative_progress.TimeRelativeProgressChange
    parents = [api_objects["engine.resistance.continuous.ContinuousResistance"]]
    nyan_object = NyanObject("TimeRelativeProgressChange", parents)
    fqon = "engine.resistance.continuous.time_relative_progress.TimeRelativeProgressChange"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.continuous.time_relative_progress.type.TimeRelativeProgressDecrease
    parents = [api_objects["engine.resistance.continuous.time_relative_progress.TimeRelativeProgressChange"]]
    nyan_object = NyanObject("TimeRelativeProgressDecrease", parents)
    fqon = "engine.resistance.continuous.time_relative_progress.type.TimeRelativeProgressDecrease"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.continuous.time_relative_progress.type.TimeRelativeProgressIncrease
    parents = [api_objects["engine.resistance.continuous.time_relative_progress.TimeRelativeProgressChange"]]
    nyan_object = NyanObject("TimeRelativeProgressIncrease", parents)
    fqon = "engine.resistance.continuous.time_relative_progress.type.TimeRelativeProgressIncrease"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.discrete.DiscreteResistance
    parents = [api_objects["engine.resistance.Resistance"]]
    nyan_object = NyanObject("DiscreteResistance", parents)
    fqon = "engine.resistance.discrete.DiscreteResistance"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.discrete.convert.Convert
    parents = [api_objects["engine.resistance.discrete.DiscreteResistance"]]
    nyan_object = NyanObject("Convert", parents)
    fqon = "engine.resistance.discrete.convert.Convert"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.discrete.convert.type.AoE2Convert
    parents = [api_objects["engine.resistance.discrete.convert.Convert"]]
    nyan_object = NyanObject("AoE2Convert", parents)
    fqon = "engine.resistance.discrete.convert.type.AoE2Convert"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.discrete.flat_attribute_change.FlatAttributeChange
    parents = [api_objects["engine.resistance.discrete.DiscreteResistance"]]
    nyan_object = NyanObject("FlatAttributeChange", parents)
    fqon = "engine.resistance.discrete.flat_attribute_change.FlatAttributeChange"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease
    parents = [api_objects["engine.resistance.discrete.flat_attribute_change.FlatAttributeChange"]]
    nyan_object = NyanObject("FlatAttributeChangeDecrease", parents)
    fqon = "engine.resistance.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.discrete.flat_attribute_change.type.FlatAttributeChangeIncrease
    parents = [api_objects["engine.resistance.discrete.flat_attribute_change.FlatAttributeChange"]]
    nyan_object = NyanObject("FlatAttributeChangeIncrease", parents)
    fqon = "engine.resistance.discrete.flat_attribute_change.type.FlatAttributeChangeIncrease"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.discrete.type.MakeHarvestable
    parents = [api_objects["engine.resistance.discrete.DiscreteResistance"]]
    nyan_object = NyanObject("MakeHarvestable", parents)
    fqon = "engine.resistance.discrete.type.MakeHarvestable"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.discrete.type.SendToContainer
    parents = [api_objects["engine.resistance.discrete.DiscreteResistance"]]
    nyan_object = NyanObject("SendToContainer", parents)
    fqon = "engine.resistance.discrete.type.SendToContainer"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier
    # engine.modifier.Modifier
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("Modifier", parents)
    fqon = "engine.modifier.Modifier"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.property.ModifierProperty
    parents = [api_objects["engine.root.Object"]]
    nyan_object = NyanObject("ModifierProperty", parents)
    fqon = "engine.modifier.property.ModifierProperty"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.property.type.Multiplier
    parents = [api_objects["engine.modifier.property.ModifierProperty"]]
    nyan_object = NyanObject("Multiplier", parents)
    fqon = "engine.modifier.property.type.Multiplier"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.property.type.Scoped
    parents = [api_objects["engine.modifier.property.ModifierProperty"]]
    nyan_object = NyanObject("Scoped", parents)
    fqon = "engine.modifier.property.type.Scoped"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.property.type.Stacked
    parents = [api_objects["engine.modifier.property.ModifierProperty"]]
    nyan_object = NyanObject("Stacked", parents)
    fqon = "engine.modifier.property.type.Stacked"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.effect.flat_attribute_change.type.ElevationDifferenceHigh
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("ElevationDifferenceHigh", parents)
    fqon = "engine.modifier.effect.flat_attribute_change.type.ElevationDifferenceHigh"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.effect.flat_attribute_change.type.ElevationDifferenceLow
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("ElevationDifferenceLow", parents)
    fqon = "engine.modifier.effect.flat_attribute_change.type.ElevationDifferenceLow"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.effect.flat_attribute_change.type.Flyover
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("Flyover", parents)
    fqon = "engine.modifier.effect.flat_attribute_change.type.Flyover"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.effect.flat_attribute_change.type.Terrain
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("Terrain", parents)
    fqon = "engine.modifier.effect.flat_attribute_change.type.Terrain"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.effect.flat_attribute_change.type.Unconditional
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("Unconditional", parents)
    fqon = "engine.modifier.effect.flat_attribute_change.type.Unconditional"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.effect.type.TimeRelativeAttributeChange
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("TimeRelativeAttributeChange", parents)
    fqon = "engine.modifier.effect.type.TimeRelativeAttributeChangeTime"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.effect.type.TimeRelativeProgressChange
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("TimeRelativeProgress", parents)
    fqon = "engine.modifier.multiplier.effect.type.TimeRelativeProgressChange"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.resistance.flat_attribute_change.type.ElevationDifferenceHigh
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("ElevationDifferenceHigh", parents)
    fqon = "engine.modifier.resistance.flat_attribute_change.type.ElevationDifferenceHigh"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.resistance.flat_attribute_change.type.ElevationDifferenceLow
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("ElevationDifferenceLow", parents)
    fqon = "engine.modifier.resistance.flat_attribute_change.type.ElevationDifferenceLow"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.resistance.flat_attribute_change.type.Stray
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("Stray", parents)
    fqon = "engine.modifier.resistance.flat_attribute_change.type.Stray"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.resistance.flat_attribute_change.type.Terrain
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("Terrain", parents)
    fqon = "engine.modifier.resistance.flat_attribute_change.type.Terrain"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.resistance.flat_attribute_change.type.Unconditional
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("Unconditional", parents)
    fqon = "engine.modifier.resistance.flat_attribute_change.type.Unconditional"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.AbsoluteProjectileAmount
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("AbsoluteProjectileAmount", parents)
    fqon = "engine.modifier.type.AbsoluteProjectileAmount"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.AoE2ProjectileAmount
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("AoE2ProjectileAmount", parents)
    fqon = "engine.modifier.type.AoE2ProjectileAmount"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.AttributeSettingsValue
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("AttributeSettingsValue", parents)
    fqon = "engine.modifier.type.AttributeSettingsValue"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.EntityContainerCapacity
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("EntityContainerCapacity", parents)
    fqon = "engine.modifier.type.EntityContainerCapacity"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.ContinuousResource
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("ContinuousResource", parents)
    fqon = "engine.modifier.type.ContinuousResource"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.CreationAttributeCost
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("CreationAttributeCost", parents)
    fqon = "engine.modifier.type.CreationAttributeCost"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.CreationResourceCost
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("CreationResourceCost", parents)
    fqon = "engine.modifier.type.CreationResourceCost"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.CreationTime
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("CreationTime", parents)
    fqon = "engine.modifier.type.CreationTime"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.DepositResourcesOnProgress
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("DepositResourcesOnProgress", parents)
    fqon = "engine.modifier.type.DepositResourcesOnProgress"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.DiplomaticLineOfSight
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("DiplomaticLineOfSight", parents)
    fqon = "engine.modifier.type.DiplomaticLineOfSight"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.GatheringEfficiency
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("GatheringEfficiency", parents)
    fqon = "engine.modifier.type.GatheringEfficiency"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.GatheringRate
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("GatheringRate", parents)
    fqon = "engine.modifier.type.GatheringRate"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.InContainerContinuousEffect
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("InContainerContinuousEffect", parents)
    fqon = "engine.modifier.type.InContainerContinuousEffect"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.InContainerDiscreteEffect
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("InContainerDiscreteEffect", parents)
    fqon = "engine.modifier.type.InContainerDiscreteEffect"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.InstantTechResearch
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("InstantTechResearch", parents)
    fqon = "engine.modifier.type.InstantTechResearch"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.MoveSpeed
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("MoveSpeed", parents)
    fqon = "engine.modifier.type.MoveSpeed"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.RefundOnCondition
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("RefundOnCondition", parents)
    fqon = "engine.modifier.type.RefundOnCondition"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.ReloadTime
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("ReloadTime", parents)
    fqon = "engine.modifier.type.ReloadTime"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.ResearchAttributeCost
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("ResearchAttributeCost", parents)
    fqon = "engine.modifier.type.ResearchAttributeCost"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.ResearchResourceCost
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("ResearchResourceCost", parents)
    fqon = "engine.modifier.type.ResearchResourceCost"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.ResearchTime
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("ResearchTime", parents)
    fqon = "engine.modifier.type.ResearchTime"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.Reveal
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("Reveal", parents)
    fqon = "engine.modifier.type.Reveal"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.StorageElementCapacity
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("StorageElementCapacity", parents)
    fqon = "engine.modifier.type.StorageElementCapacity"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    return api_objects


def _insert_members(api_objects):
    """
    Creates members for API objects.
    """
    # engine.ability
    # engine.ability.Ability
    api_object = api_objects["engine.ability.Ability"]

    subtype = NyanMemberType(api_objects["engine.ability.property.AbilityProperty"])
    key_type = NyanMemberType(MemberType.ABSTRACT, (subtype,))
    member_type = NyanMemberType(MemberType.DICT, (key_type, subtype))
    member = NyanMember("properties", member_type, {}, MemberOperator.ASSIGN, 0)
    api_object.add_member(member)

    # engine.ability.property.type.Animated
    api_object = api_objects["engine.ability.property.type.Animated"]

    elem_type = NyanMemberType(api_objects["engine.util.graphics.Animation"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("animations", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.property.type.AnimationOverride
    api_object = api_objects["engine.ability.property.type.AnimationOverride"]

    elem_type = NyanMemberType(api_objects["engine.util.animation_override.AnimationOverride"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("overrides", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.property.type.CommandSound
    api_object = api_objects["engine.ability.property.type.CommandSound"]

    elem_type = NyanMemberType(api_objects["engine.util.sound.Sound"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("sounds", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.property.type.Diplomatic
    api_object = api_objects["engine.ability.property.type.Diplomatic"]

    subtype = NyanMemberType(api_objects["engine.util.diplomatic_stance.DiplomaticStance"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("stances", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.property.type.ExecutionSound
    api_object = api_objects["engine.ability.property.type.ExecutionSound"]

    elem_type = NyanMemberType(api_objects["engine.util.sound.Sound"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("sounds", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.property.type.Lock
    api_object = api_objects["engine.ability.property.type.Lock"]

    member_type = NyanMemberType(api_objects["engine.util.lock.LockPool"])
    member = NyanMember("lock_pool", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.ActiveTransformTo
    api_object = api_objects["engine.ability.type.ActiveTransformTo"]

    member_type = NyanMemberType(api_objects["engine.util.state_machine.StateChanger"])
    member = NyanMember("target_state", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("transform_time", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.progress.Progress"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("transform_progress", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.ApplyContinuousEffect
    api_object = api_objects["engine.ability.type.ApplyContinuousEffect"]

    elem_type = NyanMemberType(api_objects["engine.effect.continuous.ContinuousEffect"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("effects", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("application_delay", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    subtype = NyanMemberType(api_objects["engine.util.game_entity_type.GameEntityType"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("allowed_types", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("blacklisted_entities", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.ApplyDiscreteEffect
    api_object = api_objects["engine.ability.type.ApplyDiscreteEffect"]

    elem_type = NyanMemberType(api_objects["engine.util.effect_batch.EffectBatch"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("batches", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("reload_time", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("application_delay", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    subtype = NyanMemberType(api_objects["engine.util.game_entity_type.GameEntityType"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("allowed_types", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("blacklisted_entities", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.AttributeChangeTracker
    api_object = api_objects["engine.ability.type.AttributeChangeTracker"]

    member_type = NyanMemberType(api_objects["engine.util.attribute.Attribute"])
    member = NyanMember("attribute", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.progress.Progress"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("change_progress", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Cloak
    api_object = api_objects["engine.ability.type.Cloak"]

    elem_type = NyanMemberType(api_objects["engine.ability.Ability"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("interrupted_by", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("interrupt_cooldown", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.CollectStorage
    api_object = api_objects["engine.ability.type.CollectStorage"]

    member_type = NyanMemberType(api_objects["engine.util.storage.EntityContainer"])
    member = NyanMember("container", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("storage_elements", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Constructable
    api_object = api_objects["engine.ability.type.Constructable"]

    member = NyanMember("starting_progress", N_INT, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.progress.Progress"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("construction_progress", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Create
    api_object = api_objects["engine.ability.type.Create"]

    elem_type = NyanMemberType(api_objects["engine.util.create.CreatableGameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("creatables", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Despawn
    api_object = api_objects["engine.ability.type.Despawn"]

    elem_type = NyanMemberType(api_objects["engine.util.logic.LogicElement"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("activation_condition", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.logic.LogicElement"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("despawn_condition", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("despawn_time", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.state_machine.StateChanger"])
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("state_change", member_type, MemberSpecialValue.NYAN_NONE,
                        MemberOperator.ASSIGN, 0)
    api_object.add_member(member)

    # engine.ability.type.DetectCloak
    api_object = api_objects["engine.ability.type.DetectCloak"]

    member = NyanMember("range", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    subtype = NyanMemberType(api_objects["engine.util.game_entity_type.GameEntityType"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("allowed_types", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("blacklisted_entities", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.DropResources
    api_object = api_objects["engine.ability.type.DropResources"]

    elem_type = NyanMemberType(api_objects["engine.util.storage.ResourceContainer"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("containers", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("search_range", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    subtype = NyanMemberType(api_objects["engine.util.game_entity_type.GameEntityType"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("allowed_types", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("blacklisted_entities", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.DropSite
    api_object = api_objects["engine.ability.type.DropSite"]

    elem_type = NyanMemberType(api_objects["engine.util.storage.ResourceContainer"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("accepts_from", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.EnterContainer
    api_object = api_objects["engine.ability.type.EnterContainer"]

    elem_type = NyanMemberType(api_objects["engine.util.storage.EntityContainer"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("allowed_containers", member_type, None, None, 0)
    api_object.add_member(member)
    subtype = NyanMemberType(api_objects["engine.util.game_entity_type.GameEntityType"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("allowed_types", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("blacklisted_entities", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.ExchangeResources
    api_object = api_objects["engine.ability.type.ExchangeResources"]

    member_type = NyanMemberType(api_objects["engine.util.resource.Resource"])
    member = NyanMember("resource_a", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.resource.Resource"])
    member = NyanMember("resource_b", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.exchange_rate.ExchangeRate"])
    member = NyanMember("exchange_rate", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.exchange_mode.ExchangeMode"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("exchange_modes", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.ExitContainer
    api_object = api_objects["engine.ability.type.ExitContainer"]

    elem_type = NyanMemberType(api_objects["engine.util.storage.EntityContainer"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("allowed_containers", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Fly
    api_object = api_objects["engine.ability.type.Fly"]

    member = NyanMember("height", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Formation
    api_object = api_objects["engine.ability.type.Formation"]

    elem_type = NyanMemberType(api_objects["engine.util.game_entity_formation.GameEntityFormation"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("formations", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Foundation
    api_object = api_objects["engine.ability.type.Foundation"]

    member_type = NyanMemberType(api_objects["engine.util.terrain.Terrain"])
    member = NyanMember("foundation_terrain", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.GameEntityStance
    api_object = api_objects["engine.ability.type.GameEntityStance"]

    elem_type = NyanMemberType(api_objects["engine.util.game_entity_stance.GameEntityStance"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("stances", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Gather
    api_object = api_objects["engine.ability.type.Gather"]

    member = NyanMember("auto_resume", N_BOOL, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("resume_search_range", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.resource_spot.ResourceSpot"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("targets", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.resource.ResourceRate"])
    member = NyanMember("gather_rate", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.storage.ResourceContainer"])
    member = NyanMember("container", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Harvestable
    api_object = api_objects["engine.ability.type.Harvestable"]

    member_type = NyanMemberType(api_objects["engine.util.resource_spot.ResourceSpot"])
    member = NyanMember("resources", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.progress.Progress"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("harvest_progress", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.progress.Progress"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("restock_progress", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("gatherer_limit", N_INT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("harvestable_by_default", N_BOOL, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Herd
    api_object = api_objects["engine.ability.type.Herd"]

    member = NyanMember("range", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("strength", N_INT, None, None, 0)
    api_object.add_member(member)
    subtype = NyanMemberType(api_objects["engine.util.game_entity_type.GameEntityType"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("allowed_types", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("blacklisted_entities", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Herdable
    api_object = api_objects["engine.ability.type.Herdable"]

    member = NyanMember("adjacent_discover_range", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.herdable_mode.HerdableMode"])
    member = NyanMember("mode", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Hitbox
    api_object = api_objects["engine.ability.type.Hitbox"]

    member_type = NyanMemberType(api_objects["engine.util.hitbox.Hitbox"])
    member = NyanMember("hitbox", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.LineOfSight
    api_object = api_objects["engine.ability.type.LineOfSight"]

    member = NyanMember("range", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Live
    api_object = api_objects["engine.ability.type.Live"]

    elem_type = NyanMemberType(api_objects["engine.util.attribute.AttributeSetting"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("attributes", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Lock
    api_object = api_objects["engine.ability.type.Lock"]

    elem_type = NyanMemberType(api_objects["engine.util.lock.LockPool"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("lock_pools", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Move
    api_object = api_objects["engine.ability.type.Move"]

    member = NyanMember("speed", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.move_mode.MoveMode"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("modes", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Named
    api_object = api_objects["engine.ability.type.Named"]

    member_type = NyanMemberType(api_objects["engine.util.language.translated.type.TranslatedString"])
    member = NyanMember("name", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.language.translated.type.TranslatedMarkupFile"])
    member = NyanMember("description", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.language.translated.type.TranslatedMarkupFile"])
    member = NyanMember("long_description", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.OverlayTerrain
    api_object = api_objects["engine.ability.type.OverlayTerrain"]

    member_type = NyanMemberType(api_objects["engine.util.terrain.Terrain"])
    member = NyanMember("terrain_overlay", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Passable
    api_object = api_objects["engine.ability.type.Passable"]

    member_type = NyanMemberType(api_objects["engine.util.hitbox.Hitbox"])
    member = NyanMember("hitbox", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.passable_mode.PassableMode"])
    member = NyanMember("mode", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.PassiveTransformTo
    api_object = api_objects["engine.ability.type.PassiveTransformTo"]

    elem_type = NyanMemberType(api_objects["engine.util.logic.LogicElement"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("condition", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("transform_time", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.state_machine.StateChanger"])
    member = NyanMember("target_state", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.progress.Progress"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("transform_progress", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.ProductionQueue
    api_object = api_objects["engine.ability.type.ProductionQueue"]

    member = NyanMember("size", N_INT, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.production_mode.ProductionMode"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("production_modes", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Projectile
    api_object = api_objects["engine.ability.type.Projectile"]

    member = NyanMember("arc", N_INT, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.accuracy.Accuracy"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("accuracy", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.target_mode.TargetMode"])
    member = NyanMember("target_mode", member_type, None, None, 0)
    api_object.add_member(member)
    subtype = NyanMemberType(api_objects["engine.util.game_entity_type.GameEntityType"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("ignored_types", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("unignored_entities", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.ProvideContingent
    api_object = api_objects["engine.ability.type.ProvideContingent"]

    elem_type = NyanMemberType(api_objects["engine.util.resource.ResourceAmount"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("amount", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.RangedContinuousEffect
    api_object = api_objects["engine.ability.type.RangedContinuousEffect"]

    member = NyanMember("min_range", N_INT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("max_range", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.RangedDiscreteEffect
    api_object = api_objects["engine.ability.type.RangedDiscreteEffect"]

    member = NyanMember("min_range", N_INT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("max_range", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.RegenerateAttribute
    api_object = api_objects["engine.ability.type.RegenerateAttribute"]

    member_type = NyanMemberType(api_objects["engine.util.attribute.AttributeRate"])
    member = NyanMember("rate", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.RegenerateResourceSpot
    api_object = api_objects["engine.ability.type.RegenerateResourceSpot"]

    member_type = NyanMemberType(api_objects["engine.util.resource.ResourceRate"])
    member = NyanMember("rate", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.resource_spot.ResourceSpot"])
    member = NyanMember("resource_spot", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.RemoveStorage
    api_object = api_objects["engine.ability.type.RemoveStorage"]

    member_type = NyanMemberType(api_objects["engine.util.storage.EntityContainer"])
    member = NyanMember("container", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("storage_elements", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Research
    api_object = api_objects["engine.ability.type.Research"]

    elem_type = NyanMemberType(api_objects["engine.util.research.ResearchableTech"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("researchables", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Resistance
    api_object = api_objects["engine.ability.type.Resistance"]

    elem_type = NyanMemberType(api_objects["engine.resistance.Resistance"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("resistances", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.ResourceStorage
    api_object = api_objects["engine.ability.type.ResourceStorage"]

    elem_type = NyanMemberType(api_objects["engine.util.storage.ResourceContainer"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("containers", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Restock
    api_object = api_objects["engine.ability.type.Restock"]

    member = NyanMember("auto_restock", N_BOOL, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.resource_spot.ResourceSpot"])
    member = NyanMember("target", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("restock_time", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.cost.Cost"])
    member = NyanMember("manual_cost", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.cost.Cost"])
    member = NyanMember("auto_cost", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("amount", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Selectable
    api_object = api_objects["engine.ability.type.Selectable"]

    member_type = NyanMemberType(api_objects["engine.util.selection_box.SelectionBox"])
    member = NyanMember("selection_box", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.SendBackToTask
    api_object = api_objects["engine.ability.type.SendBackToTask"]

    subtype = NyanMemberType(api_objects["engine.util.game_entity_type.GameEntityType"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("allowed_types", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("blacklisted_entities", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.ShootProjectile
    api_object = api_objects["engine.ability.type.ShootProjectile"]

    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.ORDEREDSET, (elem_type,))
    member = NyanMember("projectiles", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("min_projectiles", N_INT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("max_projectiles", N_INT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("min_range", N_INT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("max_range", N_INT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("reload_time", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("spawn_delay", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("projectile_delay", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("require_turning", N_BOOL, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("manual_aiming_allowed", N_BOOL, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("spawning_area_offset_x", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("spawning_area_offset_y", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("spawning_area_offset_z", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("spawning_area_width", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("spawning_area_height", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("spawning_area_randomness", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    subtype = NyanMemberType(api_objects["engine.util.game_entity_type.GameEntityType"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("allowed_types", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("blacklisted_entities", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Storage
    api_object = api_objects["engine.ability.type.Storage"]

    member_type = NyanMemberType(api_objects["engine.util.storage.EntityContainer"])
    member = NyanMember("container", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.logic.LogicElement"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("empty_condition", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.TerrainRequirement
    api_object = api_objects["engine.ability.type.TerrainRequirement"]

    subtype = NyanMemberType(api_objects["engine.util.terrain_type.TerrainType"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("allowed_types", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.terrain.Terrain"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("blacklisted_terrains", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Trade
    api_object = api_objects["engine.ability.type.Trade"]

    elem_type = NyanMemberType(api_objects["engine.util.trade_route.TradeRoute"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("trade_routes", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.storage.ResourceContainer"])
    member = NyanMember("container", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.TradePost
    api_object = api_objects["engine.ability.type.TradePost"]

    elem_type = NyanMemberType(api_objects["engine.util.trade_route.TradeRoute"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("trade_routes", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.TransferStorage
    api_object = api_objects["engine.ability.type.TransferStorage"]

    member_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member = NyanMember("storage_element", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.storage.EntityContainer"])
    member = NyanMember("source_container", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.storage.EntityContainer"])
    member = NyanMember("target_container", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Turn
    api_object = api_objects["engine.ability.type.Turn"]

    member = NyanMember("turn_speed", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.UseContingent
    api_object = api_objects["engine.ability.type.UseContingent"]

    elem_type = NyanMemberType(api_objects["engine.util.resource.ResourceAmount"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("amount", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.ability.type.Visibility
    api_object = api_objects["engine.ability.type.Visibility"]

    member = NyanMember("visible_in_fog", N_BOOL, None, None, 0)
    api_object.add_member(member)

    # engine.util
    # engine.util.accuracy.Accuracy
    api_object = api_objects["engine.util.accuracy.Accuracy"]

    member = NyanMember("accuracy", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("accuracy_dispersion", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.dropoff_type.DropoffType"])
    member = NyanMember("dispersion_dropoff", member_type, None, None, 0)
    api_object.add_member(member)
    subtype = NyanMemberType(api_objects["engine.util.game_entity_type.GameEntityType"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("target_types", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("blacklisted_entities", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.animation_override.AnimationOverride
    api_object = api_objects["engine.util.animation_override.AnimationOverride"]

    subtype = NyanMemberType(api_objects["engine.ability.Ability"])
    member_type = NyanMemberType(MemberType.ABSTRACT, (subtype,))
    member = NyanMember("ability", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.graphics.Animation"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("animations", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("priority", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.util.animation_override.type.Reset
    api_object = api_objects["engine.util.animation_override.type.Reset"]
    member_origin = api_objects["engine.util.animation_override.AnimationOverride"]

    member = api_object.get_member_by_name("animations", member_origin)
    member.set_value([], MemberOperator.ASSIGN)
    member = api_object.get_member_by_name("priority", member_origin)
    member.set_value(0, MemberOperator.ASSIGN)

    # engine.util.attribute.Attribute
    api_object = api_objects["engine.util.attribute.Attribute"]

    member_type = NyanMemberType(api_objects["engine.util.language.translated.type.TranslatedString"])
    member = NyanMember("name", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.language.translated.type.TranslatedString"])
    member = NyanMember("abbreviation", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.attribute.AttributeAmount
    api_object = api_objects["engine.util.attribute.AttributeAmount"]

    member_type = NyanMemberType(api_objects["engine.util.attribute.Attribute"])
    member = NyanMember("type", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("amount", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.util.attribute.AttributeRate
    api_object = api_objects["engine.util.attribute.AttributeRate"]

    member_type = NyanMemberType(api_objects["engine.util.attribute.Attribute"])
    member = NyanMember("type", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("rate", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.util.attribute.AttributeSetting
    api_object = api_objects["engine.util.attribute.AttributeSetting"]

    member_type = NyanMemberType(api_objects["engine.util.attribute.Attribute"])
    member = NyanMember("attribute", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("min_value", N_INT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("max_value", N_INT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("starting_value", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.util.attribute.ProtectingAttribute
    api_object = api_objects["engine.util.attribute.ProtectingAttribute"]

    member_type = NyanMemberType(api_objects["engine.util.attribute.Attribute"])
    member = NyanMember("protects", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.calculation_type.type.Hyperbolic
    api_object = api_objects["engine.util.calculation_type.type.Hyperbolic"]

    member = NyanMember("shift_x", N_INT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("shift_y", N_INT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("scale_factor", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.util.calculation_type.type.Linear
    api_object = api_objects["engine.util.calculation_type.type.Linear"]

    member = NyanMember("shift_x", N_INT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("shift_y", N_INT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("scale_factor", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.util.cheat.Cheat
    api_object = api_objects["engine.util.cheat.Cheat"]

    member = NyanMember("activation_message", N_TEXT, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.patch.Patch"])
    member_type = NyanMemberType(MemberType.ORDEREDSET, (elem_type,))
    member = NyanMember("changes", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.cost.Cost
    api_object = api_objects["engine.util.cost.Cost"]

    member_type = NyanMemberType(api_objects["engine.util.payment_mode.PaymentMode"])
    member = NyanMember("payment_mode", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.cost.type.AttributeCost
    api_object = api_objects["engine.util.cost.type.AttributeCost"]

    elem_type = NyanMemberType(api_objects["engine.util.attribute.AttributeAmount"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("amount", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.cost.type.ResourceCost
    api_object = api_objects["engine.util.cost.type.ResourceCost"]

    elem_type = NyanMemberType(api_objects["engine.util.resource.ResourceAmount"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("amount", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.create.CreatableGameEntity
    api_object = api_objects["engine.util.create.CreatableGameEntity"]

    member_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member = NyanMember("game_entity", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.variant.Variant"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("variants", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.cost.Cost"])
    member = NyanMember("cost", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("creation_time", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.sound.Sound"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("creation_sounds", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.logic.LogicElement"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("condition", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.placement_mode.PlacementMode"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("placement_modes", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.effect_batch.EffectBatch
    api_object = api_objects["engine.util.effect_batch.EffectBatch"]

    elem_type = NyanMemberType(api_objects["engine.effect.discrete.DiscreteEffect"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("effects", member_type, None, None, 0)
    api_object.add_member(member)
    subtype = NyanMemberType(api_objects["engine.util.effect_batch.property.BatchProperty"])
    key_type = NyanMemberType(MemberType.ABSTRACT, (subtype,))
    member_type = NyanMemberType(MemberType.DICT, (key_type, subtype))
    member = NyanMember("properties", member_type, {}, MemberOperator.ASSIGN, 0)
    api_object.add_member(member)

    # engine.util.effect_batch.property.type.Chance
    api_object = api_objects["engine.util.effect_batch.property.type.Chance"]

    member = NyanMember("chance", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.util.effect_batch.property.type.Priority
    api_object = api_objects["engine.util.effect_batch.property.type.Priority"]

    member = NyanMember("priority", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.util.exchange_mode.ExchangeMode
    api_object = api_objects["engine.util.exchange_mode.ExchangeMode"]

    member = NyanMember("fee_multiplier", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.util.exchange_rate.ExchangeRate
    api_object = api_objects["engine.util.exchange_rate.ExchangeRate"]

    member = NyanMember("base_price", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    key_type = NyanMemberType(api_objects["engine.util.exchange_mode.ExchangeMode"])
    value_type = NyanMemberType(api_objects["engine.util.price_mode.PriceMode"])
    elem_type = NyanMemberType(MemberType.DICT, (key_type, value_type))
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("price_adjust", member_type, MemberSpecialValue.NYAN_NONE,
                        MemberOperator.ASSIGN, 0)
    api_object.add_member(member)
    subtype = NyanMemberType(api_objects["engine.util.price_pool.PricePool"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("price_pool", member_type, MemberSpecialValue.NYAN_NONE,
                        MemberOperator.ASSIGN, 0)
    api_object.add_member(member)

    # engine.util.formation.Formation
    api_object = api_objects["engine.util.formation.Formation"]

    elem_type = NyanMemberType(api_objects["engine.util.formation.Subformation"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("subformations", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.formation.Subformation
    api_object = api_objects["engine.util.formation.Subformation"]

    member = NyanMember("ordering_priority", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.util.game_entity.GameEntity
    api_object = api_objects["engine.util.game_entity.GameEntity"]

    subtype = NyanMemberType(api_objects["engine.util.game_entity_type.GameEntityType"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("types", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.ability.Ability"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("abilities", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.modifier.Modifier"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("modifiers", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.variant.Variant"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("variants", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.game_entity_formation.GameEntityFormation
    api_object = api_objects["engine.util.game_entity_formation.GameEntityFormation"]

    member_type = NyanMemberType(api_objects["engine.util.formation.Formation"])
    member = NyanMember("formation", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.formation.Subformation"])
    member = NyanMember("subformation", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.game_entity_stance.GameEntityStance
    api_object = api_objects["engine.util.game_entity_stance.GameEntityStance"]

    member = NyanMember("search_range", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.ability.Ability"])
    member_type = NyanMemberType(MemberType.ORDEREDSET, (elem_type,))
    member = NyanMember("ability_preference", member_type, None, None, 0)
    api_object.add_member(member)
    subtype = NyanMemberType(api_objects["engine.util.game_entity_type.GameEntityType"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.ORDEREDSET, (elem_type,))
    member = NyanMember("type_preference", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.graphics.Animation
    api_object = api_objects["engine.util.graphics.Animation"]

    member = NyanMember("sprite", N_FILE, None, None, 0)
    api_object.add_member(member)

    # engine.util.graphics.Palette
    api_object = api_objects["engine.util.graphics.Palette"]

    member = NyanMember("palette", N_FILE, None, None, 0)
    api_object.add_member(member)

    # engine.util.graphics.Terrain
    api_object = api_objects["engine.util.graphics.Terrain"]

    member = NyanMember("sprite", N_FILE, None, None, 0)
    api_object.add_member(member)

    # engine.util.hitbox.hitbox
    api_object = api_objects["engine.util.hitbox.Hitbox"]

    member = NyanMember("radius_x", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("radius_y", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("radius_z", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.util.language.Language
    api_object = api_objects["engine.util.language.Language"]

    member = NyanMember("ietf_string", N_TEXT, None, None, 0)
    api_object.add_member(member)

    # engine.util.language.LanguageMarkupPair
    api_object = api_objects["engine.util.language.LanguageMarkupPair"]

    member_type = NyanMemberType(api_objects["engine.util.language.Language"])
    member = NyanMember("language", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("markup_file", N_FILE, None, None, 0)
    api_object.add_member(member)

    # engine.util.language.LanguageSoundPair
    api_object = api_objects["engine.util.language.LanguageSoundPair"]

    member_type = NyanMemberType(api_objects["engine.util.language.Language"])
    member = NyanMember("language", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.sound.Sound"])
    member = NyanMember("sound", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.language.LanguageTextPair
    api_object = api_objects["engine.util.language.LanguageTextPair"]

    member_type = NyanMemberType(api_objects["engine.util.language.Language"])
    member = NyanMember("language", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("string", N_TEXT, None, None, 0)
    api_object.add_member(member)

    # engine.util.language.translated.type.TranslatedMarkupFile
    api_object = api_objects["engine.util.language.translated.type.TranslatedMarkupFile"]

    elem_type = NyanMemberType(api_objects["engine.util.language.LanguageMarkupPair"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("translations", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.language.translated.type.TranslatedSound
    api_object = api_objects["engine.util.language.translated.type.TranslatedSound"]

    elem_type = NyanMemberType(api_objects["engine.util.language.LanguageSoundPair"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("translations", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.language.translated.type.TranslatedString
    api_object = api_objects["engine.util.language.translated.type.TranslatedString"]

    elem_type = NyanMemberType(api_objects["engine.util.language.LanguageTextPair"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("translations", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.logic.LogicElement
    api_object = api_objects["engine.util.lock.LockPool"]

    member = NyanMember("slots", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.util.logic.LogicElement
    api_object = api_objects["engine.util.logic.LogicElement"]

    member = NyanMember("only_once", N_BOOL, None, None, 0)
    api_object.add_member(member)

    # engine.util.logic.gate.LogicGate
    api_object = api_objects["engine.util.logic.gate.LogicGate"]

    elem_type = NyanMemberType(api_objects["engine.util.logic.LogicElement"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("inputs", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.logic.gate.type.SUBSETMAX
    api_object = api_objects["engine.util.logic.gate.type.SUBSETMAX"]

    member = NyanMember("size", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.util.logic.gate.type.SUBSETMIN
    api_object = api_objects["engine.util.logic.gate.type.SUBSETMIN"]

    member = NyanMember("size", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.util.logic.literal.Literal
    api_object = api_objects["engine.util.logic.literal.Literal"]

    member_type = NyanMemberType(api_objects["engine.util.logic.literal_scope.LiteralScope"])
    member = NyanMember("scope", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.logic.literal_scope.LiteralScope
    api_object = api_objects["engine.util.logic.literal_scope.LiteralScope"]

    elem_type = NyanMemberType(api_objects["engine.util.diplomatic_stance.DiplomaticStance"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("stances", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.logic.literal.type.AttributeAbovePercentage
    api_object = api_objects["engine.util.logic.literal.type.AttributeAbovePercentage"]

    member_type = NyanMemberType(api_objects["engine.util.attribute.Attribute"])
    member = NyanMember("attribute", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("threshold", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.util.logic.literal.type.AttributeAboveValue
    api_object = api_objects["engine.util.logic.literal.type.AttributeAboveValue"]

    member_type = NyanMemberType(api_objects["engine.util.attribute.Attribute"])
    member = NyanMember("attribute", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("threshold", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.util.logic.literal.type.AttributeBelowPercentage
    api_object = api_objects["engine.util.logic.literal.type.AttributeBelowPercentage"]

    member_type = NyanMemberType(api_objects["engine.util.attribute.Attribute"])
    member = NyanMember("attribute", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("threshold", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.util.logic.literal.type.AttributeBelowValue
    api_object = api_objects["engine.util.logic.literal.type.AttributeBelowValue"]

    member_type = NyanMemberType(api_objects["engine.util.attribute.Attribute"])
    member = NyanMember("attribute", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("threshold", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.util.logic.literal.type.GameEntityProgress
    api_object = api_objects["engine.util.logic.literal.type.GameEntityProgress"]

    member_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member = NyanMember("game_entity", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.progress_status.ProgressStatus"])
    member = NyanMember("progress_status", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.logic.literal.type.OwnsGameEntity
    api_object = api_objects["engine.util.logic.literal.type.OwnsGameEntity"]

    member_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member = NyanMember("game_entity", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.logic.literal.type.ProjectilePassThrough
    api_object = api_objects["engine.util.logic.literal.type.ProjectilePassThrough"]

    member = NyanMember("pass_through_range", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.util.logic.literal.type.ResourceSpotsDepleted
    api_object = api_objects["engine.util.logic.literal.type.ResourceSpotsDepleted"]

    member = NyanMember("only_enabled", N_BOOL, None, None, 0)
    api_object.add_member(member)

    # engine.util.logic.literal.type.StateChangeActive
    api_object = api_objects["engine.util.logic.literal.type.StateChangeActive"]

    member_type = NyanMemberType(api_objects["engine.util.state_machine.StateChanger"])
    member = NyanMember("state_change", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.logic.literal.type.TechResearched
    api_object = api_objects["engine.util.logic.literal.type.TechResearched"]

    member_type = NyanMemberType(api_objects["engine.util.tech.Tech"])
    member = NyanMember("tech", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.logic.literal.type.Timer
    api_object = api_objects["engine.util.logic.literal.type.Timer"]

    member = NyanMember("time", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.util.mod.Mod
    api_object = api_objects["engine.util.mod.Mod"]

    elem_type = NyanMemberType(api_objects["engine.util.patch.Patch"])
    member_type = NyanMemberType(MemberType.ORDEREDSET, (elem_type,))
    member = NyanMember("patches", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("priority", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.util.modifier_scope.type.GameEntityScope
    api_object = api_objects["engine.util.modifier_scope.type.GameEntityScope"]

    subtype = NyanMemberType(api_objects["engine.util.game_entity_type.GameEntityType"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("affected_types", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("blacklisted_entities", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.move_mode.type.Follow
    api_object = api_objects["engine.util.move_mode.type.Follow"]

    member = NyanMember("range", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.util.move_mode.type.Guard
    api_object = api_objects["engine.util.move_mode.type.Guard"]

    member = NyanMember("range", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.util.passable_mode.PassableMode
    api_object = api_objects["engine.util.passable_mode.PassableMode"]

    subtype = NyanMemberType(api_objects["engine.util.game_entity_type.GameEntityType"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("allowed_types", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("blacklisted_entities", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.passable_mode.type.Gate
    api_object = api_objects["engine.util.passable_mode.type.Gate"]

    subtype = NyanMemberType(api_objects["engine.util.diplomatic_stance.DiplomaticStance"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("stances", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.patch.Patch
    api_object = api_objects["engine.util.patch.Patch"]

    subtype = NyanMemberType(api_objects["engine.util.patch.property.PatchProperty"])
    key_type = NyanMemberType(MemberType.ABSTRACT, (subtype,))
    member_type = NyanMemberType(MemberType.DICT, (key_type, subtype))
    member = NyanMember("properties", member_type, {}, MemberOperator.ASSIGN, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.patch.NyanPatch"])
    member_type = NyanMemberType(MemberType.CHILDREN, (elem_type,))
    member = NyanMember("patch", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.patch.property.type.Diplomatic
    api_object = api_objects["engine.util.patch.property.type.Diplomatic"]

    elem_type = NyanMemberType(api_objects["engine.util.diplomatic_stance.DiplomaticStance"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("stances", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.placement_mode.type.OwnStorage
    api_object = api_objects["engine.util.placement_mode.type.OwnStorage"]

    member_type = NyanMemberType(api_objects["engine.util.storage.EntityContainer"])
    member = NyanMember("container", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.placement_mode.type.Place
    api_object = api_objects["engine.util.placement_mode.type.Place"]

    member = NyanMember("tile_snap_distance", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("clearance_size_x", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("clearance_size_y", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("allow_rotation", N_BOOL, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("max_elevation_difference", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.util.placement_mode.type.Replace
    api_object = api_objects["engine.util.placement_mode.type.Replace"]

    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("game_entities", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.price_mode.type.Dynamic
    api_object = api_objects["engine.util.price_mode.type.Dynamic"]

    member = NyanMember("change_value", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("min_price", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("max_price", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.util.production_mode.type.Creatables
    api_object = api_objects["engine.util.production_mode.type.Creatables"]

    elem_type = NyanMemberType(api_objects["engine.util.create.CreatableGameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("exclude", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.production_mode.type.Researchables
    api_object = api_objects["engine.util.production_mode.type.Researchables"]

    elem_type = NyanMemberType(api_objects["engine.util.research.ResearchableTech"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("exclude", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.progress.Progress
    api_object = api_objects["engine.util.progress.Progress"]

    subtype = NyanMemberType(api_objects["engine.util.progress.property.ProgressProperty"])
    key_type = NyanMemberType(MemberType.ABSTRACT, (subtype,))
    member_type = NyanMemberType(MemberType.DICT, (key_type, subtype))
    member = NyanMember("properties", member_type, {}, MemberOperator.ASSIGN, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.progress_type.ProgressType"])
    member_type = NyanMemberType(MemberType.CHILDREN, (elem_type,))
    member = NyanMember("type", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("left_boundary", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("right_boundary", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.util.progress.property.type.Animated
    api_object = api_objects["engine.util.progress.property.type.Animated"]

    elem_type = NyanMemberType(api_objects["engine.util.animation_override.AnimationOverride"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("overrides", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.progress.property.type.AnimationOverlay
    api_object = api_objects["engine.util.progress.property.type.AnimationOverlay"]

    elem_type = NyanMemberType(api_objects["engine.util.graphics.Animation"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("overlays", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.progress.property.type.StateChange
    api_object = api_objects["engine.util.progress.property.type.StateChange"]

    member_type = NyanMemberType(api_objects["engine.util.state_machine.StateChanger"])
    member = NyanMember("state_change", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.progress.property.type.TerrainOverlay
    api_object = api_objects["engine.util.progress.property.type.TerrainOverlay"]

    member_type = NyanMemberType(api_objects["engine.util.terrain.Terrain"])
    member = NyanMember("terrain_overlay", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.progress.property.type.Terrain
    api_object = api_objects["engine.util.progress.property.type.Terrain"]

    member_type = NyanMemberType(api_objects["engine.util.terrain.Terrain"])
    member = NyanMember("terrain", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.progress_status.ProgressStatus
    api_object = api_objects["engine.util.progress_status.ProgressStatus"]

    elem_type = NyanMemberType(api_objects["engine.util.progress_type.ProgressType"])
    member_type = NyanMemberType(MemberType.CHILDREN, (elem_type,))
    member = NyanMember("progress_type", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("progress", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.util.research.ResearchableTech
    api_object = api_objects["engine.util.research.ResearchableTech"]

    member_type = NyanMemberType(api_objects["engine.util.tech.Tech"])
    member = NyanMember("tech", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.cost.Cost"])
    member = NyanMember("cost", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("research_time", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.sound.Sound"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("research_sounds", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.logic.LogicElement"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("condition", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.resource.Resource
    api_object = api_objects["engine.util.resource.Resource"]

    member_type = NyanMemberType(api_objects["engine.util.language.translated.type.TranslatedString"])
    member = NyanMember("name", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("max_storage", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.util.resource.ResourceContingent
    api_object = api_objects["engine.util.resource.ResourceContingent"]

    member = NyanMember("min_amount", N_INT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("max_amount", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.util.resource.ResourceAmount
    api_object = api_objects["engine.util.resource.ResourceAmount"]

    member_type = NyanMemberType(api_objects["engine.util.resource.Resource"])
    member = NyanMember("type", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("amount", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.util.resource.ResourceRate
    api_object = api_objects["engine.util.resource.ResourceRate"]

    member_type = NyanMemberType(api_objects["engine.util.resource.Resource"])
    member = NyanMember("type", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("rate", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.util.resource_spot.ResourceSpot
    api_object = api_objects["engine.util.resource_spot.ResourceSpot"]

    member_type = NyanMemberType(api_objects["engine.util.resource.Resource"])
    member = NyanMember("resource", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("max_amount", N_INT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("starting_amount", N_INT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("decay_rate", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.util.selection_box.type.Rectangle
    api_object = api_objects["engine.util.selection_box.type.Rectangle"]

    member = NyanMember("width", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("height", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.util.setup.PlayerSetup
    api_object = api_objects["engine.util.setup.PlayerSetup"]

    member_type = NyanMemberType(api_objects["engine.util.language.translated.type.TranslatedString"])
    member = NyanMember("name", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.language.translated.type.TranslatedMarkupFile"])
    member = NyanMember("description", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.language.translated.type.TranslatedMarkupFile"])
    member = NyanMember("long_description", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.language.translated.type.TranslatedString"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("leader_names", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.modifier.Modifier"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("modifiers", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.resource.ResourceAmount"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("starting_resources", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.patch.Patch"])
    member_type = NyanMemberType(MemberType.ORDEREDSET, (elem_type,))
    member = NyanMember("game_setup", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.sound.Sound
    api_object = api_objects["engine.util.sound.Sound"]

    member = NyanMember("play_delay", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    elem_type = N_FILE
    member_type = NyanMemberType(MemberType.ORDEREDSET, (elem_type,))
    member = NyanMember("sounds", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.state_machine.StateChanger
    api_object = api_objects["engine.util.state_machine.StateChanger"]

    subtype = NyanMemberType(api_objects["engine.ability.Ability"])
    elem_type = NyanMemberType(MemberType.ABSTRACT, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("enable_abilities", member_type, None, None, 0)
    api_object.add_member(member)
    subtype = NyanMemberType(api_objects["engine.ability.Ability"])
    elem_type = NyanMemberType(MemberType.ABSTRACT, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("disable_abilities", member_type, None, None, 0)
    api_object.add_member(member)
    subtype = NyanMemberType(api_objects["engine.modifier.Modifier"])
    elem_type = NyanMemberType(MemberType.ABSTRACT, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("enable_modifiers", member_type, None, None, 0)
    api_object.add_member(member)
    subtype = NyanMemberType(api_objects["engine.modifier.Modifier"])
    elem_type = NyanMemberType(MemberType.ABSTRACT, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("disable_modifiers", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.transform_pool.TransformPool"])
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("transform_pool", member_type, MemberSpecialValue.NYAN_NONE,
                        MemberOperator.ASSIGN, 0)
    api_object.add_member(member)
    member = NyanMember("priority", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.util.state_machine.Reset
    api_object = api_objects["engine.util.state_machine.Reset"]
    member_origin = api_objects["engine.util.state_machine.StateChanger"]

    member = api_object.get_member_by_name("enable_abilities", member_origin)
    member.set_value([], MemberOperator.ASSIGN)
    member = api_object.get_member_by_name("disable_abilities", member_origin)
    member.set_value([], MemberOperator.ASSIGN)
    member = api_object.get_member_by_name("enable_modifiers", member_origin)
    member.set_value([], MemberOperator.ASSIGN)
    member = api_object.get_member_by_name("disable_modifiers", member_origin)
    member.set_value([], MemberOperator.ASSIGN)
    member = api_object.get_member_by_name("priority", member_origin)
    member.set_value(0, MemberOperator.ASSIGN)

    # engine.util.storage.EntityContainer
    api_object = api_objects["engine.util.storage.EntityContainer"]

    subtype = NyanMemberType(api_objects["engine.util.game_entity_type.GameEntityType"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("allowed_types", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("blacklisted_entities", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.storage.StorageElementDefinition"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("storage_element_defs", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("slots", N_INT, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.progress.Progress"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("carry_progress", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.storage.ResourceContainer
    api_object = api_objects["engine.util.storage.ResourceContainer"]

    member_type = NyanMemberType(api_objects["engine.util.resource.Resource"])
    member = NyanMember("resource", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("max_amount", N_INT, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.progress.Progress"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("carry_progress", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.storage.resource_container.type.InternalDropSite
    api_object = api_objects["engine.util.storage.resource_container.type.InternalDropSite"]

    member = NyanMember("update_time", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.util.storage.StorageElementDefinition
    api_object = api_objects["engine.util.storage.StorageElementDefinition"]

    member_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member = NyanMember("storage_element", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("elements_per_slot", N_INT, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.storage.StorageElementDefinition"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("conflicts", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.state_machine.StateChanger"])
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("state_change", member_type, MemberSpecialValue.NYAN_NONE,
                        MemberOperator.ASSIGN, 0)
    api_object.add_member(member)

    # engine.util.taunt.Taunt
    api_object = api_objects["engine.util.taunt.Taunt"]

    member = NyanMember("activation_message", N_TEXT, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.language.translated.type.TranslatedString"])
    member = NyanMember("display_message", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.sound.Sound"])
    member = NyanMember("sound", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.tech.Tech
    api_object = api_objects["engine.util.tech.Tech"]

    subtype = NyanMemberType(api_objects["engine.util.tech_type.TechType"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("types", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.language.translated.type.TranslatedString"])
    member = NyanMember("name", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.language.translated.type.TranslatedMarkupFile"])
    member = NyanMember("description", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.language.translated.type.TranslatedMarkupFile"])
    member = NyanMember("long_description", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.patch.Patch"])
    member_type = NyanMemberType(MemberType.ORDEREDSET, (elem_type,))
    member = NyanMember("updates", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.terrain.Terrain
    api_object = api_objects["engine.util.terrain.Terrain"]

    subtype = NyanMemberType(api_objects["engine.util.terrain_type.TerrainType"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("types", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.language.translated.type.TranslatedString"])
    member = NyanMember("name", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.graphics.Terrain"])
    member = NyanMember("terrain_graphic", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.sound.Sound"])
    member = NyanMember("sound", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.terrain.TerrainAmbient"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("ambience", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.terrain.TerrainAmbient
    api_object = api_objects["engine.util.terrain.TerrainAmbient"]

    member_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member = NyanMember("object", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("max_density", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.util.trade_route.TradeRoute
    api_object = api_objects["engine.util.trade_route.TradeRoute"]

    member_type = NyanMemberType(api_objects["engine.util.resource.Resource"])
    member = NyanMember("trade_resource", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member = NyanMember("start_trade_post", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member = NyanMember("end_trade_post", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.trade_route.type.AoE1TradeRoute
    api_object = api_objects["engine.util.trade_route.type.AoE1TradeRoute"]

    elem_type = NyanMemberType(api_objects["engine.util.resource.Resource"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("exchange_resources", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("trade_amount", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.util.variant.Variant
    api_object = api_objects["engine.util.variant.Variant"]

    elem_type = NyanMemberType(api_objects["engine.util.patch.Patch"])
    member_type = NyanMemberType(MemberType.ORDEREDSET, (elem_type,))
    member = NyanMember("changes", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("priority", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.util.variant.type.AdjacentTilesVariant
    api_object = api_objects["engine.util.variant.type.AdjacentTilesVariant"]

    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("north", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("north_east", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("east", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("south_east", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("south", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("south_west", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("west", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("north_west", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.util.variant.type.RandomVariant
    api_object = api_objects["engine.util.variant.type.RandomVariant"]

    member = NyanMember("chance_share", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.util.variant.type.PerspectiveVariant
    api_object = api_objects["engine.util.variant.type.PerspectiveVariant"]

    member = NyanMember("angle", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.effect
    # engine.effect.Effect
    api_object = api_objects["engine.effect.Effect"]

    subtype = NyanMemberType(api_objects["engine.effect.property.EffectProperty"])
    key_type = NyanMemberType(MemberType.ABSTRACT, (subtype,))
    member_type = NyanMemberType(MemberType.DICT, (key_type, subtype))
    member = NyanMember("properties", member_type, {}, MemberOperator.ASSIGN, 0)
    api_object.add_member(member)

    # engine.effect.property.type.Area
    api_object = api_objects["engine.effect.property.type.Area"]

    member = NyanMember("range", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.dropoff_type.DropoffType"])
    member = NyanMember("dropoff", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.effect.property.type.Cost
    api_object = api_objects["engine.effect.property.type.Cost"]

    member_type = NyanMemberType(api_objects["engine.util.cost.Cost"])
    member = NyanMember("cost", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.effect.property.type.Diplomatic
    api_object = api_objects["engine.effect.property.type.Diplomatic"]

    subtype = NyanMemberType(api_objects["engine.util.diplomatic_stance.DiplomaticStance"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("stances", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.effect.property.type.Priority
    api_object = api_objects["engine.effect.property.type.Priority"]

    member = NyanMember("priority", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.effect.continuous.flat_attribute_change.FlatAttributeChange
    api_object = api_objects["engine.effect.continuous.flat_attribute_change.FlatAttributeChange"]

    elem_type = NyanMemberType(api_objects["engine.util.attribute_change_type.AttributeChangeType"])
    member_type = NyanMemberType(MemberType.CHILDREN, (elem_type,))
    member = NyanMember("type", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.attribute.AttributeRate"])
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("min_change_rate", member_type, MemberSpecialValue.NYAN_NONE,
                        MemberOperator.ASSIGN, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.attribute.AttributeRate"])
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("max_change_rate", member_type, MemberSpecialValue.NYAN_NONE,
                        MemberOperator.ASSIGN, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.attribute.AttributeRate"])
    member = NyanMember("change_rate", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.attribute.ProtectingAttribute"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("ignore_protection", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.effect.continuous.type.Lure
    api_object = api_objects["engine.effect.continuous.type.Lure"]

    elem_type = NyanMemberType(api_objects["engine.util.lure_type.LureType"])
    member_type = NyanMemberType(MemberType.CHILDREN, (elem_type,))
    member = NyanMember("type", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("destination", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("min_distance_to_destination", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.effect.continuous.time_relative_attribute.TimeRelativeAttributeChange
    api_object = api_objects["engine.effect.continuous.time_relative_attribute.TimeRelativeAttributeChange"]

    elem_type = NyanMemberType(api_objects["engine.util.attribute_change_type.AttributeChangeType"])
    member_type = NyanMemberType(MemberType.CHILDREN, (elem_type,))
    member = NyanMember("type", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("total_change_time", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.attribute.ProtectingAttribute"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("ignore_protection", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.effect.continuous.time_relative_progress.TimeRelativeProgressChange
    api_object = api_objects["engine.effect.continuous.time_relative_progress.TimeRelativeProgressChange"]

    elem_type = NyanMemberType(api_objects["engine.util.progress_type.ProgressType"])
    member_type = NyanMemberType(MemberType.CHILDREN, (elem_type,))
    member = NyanMember("type", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("total_change_time", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.effect.discrete.convert.Convert
    api_object = api_objects["engine.effect.discrete.convert.Convert"]

    elem_type = NyanMemberType(api_objects["engine.util.convert_type.ConvertType"])
    member_type = NyanMemberType(MemberType.CHILDREN, (elem_type,))
    member = NyanMember("type", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = N_FLOAT
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("min_chance_success", member_type, MemberSpecialValue.NYAN_NONE,
                        MemberOperator.ASSIGN, 0)
    api_object.add_member(member)
    elem_type = N_FLOAT
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("max_chance_success", member_type, MemberSpecialValue.NYAN_NONE,
                        MemberOperator.ASSIGN, 0)
    api_object.add_member(member)
    member = NyanMember("chance_success", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.cost.Cost"])
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("cost_fail", member_type, MemberSpecialValue.NYAN_NONE,
                        MemberOperator.ASSIGN, 0)
    api_object.add_member(member)

    # engine.effect.discrete.convert.type.AoE2Convert
    api_object = api_objects["engine.effect.discrete.convert.type.AoE2Convert"]

    member = NyanMember("skip_guaranteed_rounds", N_INT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("skip_protected_rounds", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.effect.discrete.flat_attribute_change.FlatAttributeChange
    api_object = api_objects["engine.effect.discrete.flat_attribute_change.FlatAttributeChange"]

    elem_type = NyanMemberType(api_objects["engine.util.attribute_change_type.AttributeChangeType"])
    member_type = NyanMemberType(MemberType.CHILDREN, (elem_type,))
    member = NyanMember("type", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.attribute.AttributeAmount"])
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("min_change_value", member_type, MemberSpecialValue.NYAN_NONE,
                        MemberOperator.ASSIGN, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.attribute.AttributeAmount"])
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("max_change_value", member_type, MemberSpecialValue.NYAN_NONE,
                        MemberOperator.ASSIGN, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.attribute.AttributeAmount"])
    member = NyanMember("change_value", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.attribute.ProtectingAttribute"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("ignore_protection", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.effect.discrete.type.MakeHarvestable
    api_object = api_objects["engine.effect.discrete.type.MakeHarvestable"]

    member_type = NyanMemberType(api_objects["engine.util.resource_spot.ResourceSpot"])
    member = NyanMember("resource_spot", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.effect.discrete.type.SendToContainer
    api_object = api_objects["engine.effect.discrete.type.SendToContainer"]

    elem_type = NyanMemberType(api_objects["engine.util.container_type.SendToContainerType"])
    member_type = NyanMemberType(MemberType.CHILDREN, (elem_type,))
    member = NyanMember("type", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.storage.EntityContainer"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("storages", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.resistance
    # engine.resistance.Resistance
    api_object = api_objects["engine.resistance.Resistance"]

    subtype = NyanMemberType(api_objects["engine.resistance.property.ResistanceProperty"])
    key_type = NyanMemberType(MemberType.ABSTRACT, (subtype,))
    member_type = NyanMemberType(MemberType.DICT, (key_type, subtype))
    member = NyanMember("properties", member_type, {}, MemberOperator.ASSIGN, 0)
    api_object.add_member(member)

    # engine.resistance.property.type.Cost
    api_object = api_objects["engine.resistance.property.type.Cost"]

    member_type = NyanMemberType(api_objects["engine.util.cost.Cost"])
    member = NyanMember("cost", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.resistance.property.type.Stacked
    api_object = api_objects["engine.resistance.property.type.Stacked"]

    member = NyanMember("stack_limit", N_INT, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.calculation_type.CalculationType"])
    member = NyanMember("calculation_type", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.distribution_type.DistributionType"])
    member = NyanMember("distribution_type", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.resistance.continuous.flat_attribute_change.FlatAttributeChange
    api_object = api_objects["engine.resistance.continuous.flat_attribute_change.FlatAttributeChange"]

    elem_type = NyanMemberType(api_objects["engine.util.attribute_change_type.AttributeChangeType"])
    member_type = NyanMemberType(MemberType.CHILDREN, (elem_type,))
    member = NyanMember("type", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.attribute.AttributeRate"])
    member = NyanMember("block_rate", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.resistance.continuous.type.Lure
    api_object = api_objects["engine.resistance.continuous.type.Lure"]

    elem_type = NyanMemberType(api_objects["engine.util.lure_type.LureType"])
    member_type = NyanMemberType(MemberType.CHILDREN, (elem_type,))
    member = NyanMember("type", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.resistance.continuous.time_relative_attribute.TimeRelativeAttributeChange
    api_object = api_objects["engine.resistance.continuous.time_relative_attribute.TimeRelativeAttributeChange"]

    elem_type = NyanMemberType(api_objects["engine.util.attribute_change_type.AttributeChangeType"])
    member_type = NyanMemberType(MemberType.CHILDREN, (elem_type,))
    member = NyanMember("type", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.resistance.continuous.time_relative_progress.TimeRelativeProgress
    api_object = api_objects["engine.resistance.continuous.time_relative_progress.TimeRelativeProgressChange"]

    elem_type = NyanMemberType(api_objects["engine.util.progress_type.ProgressType"])
    member_type = NyanMemberType(MemberType.CHILDREN, (elem_type,))
    member = NyanMember("type", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.resistance.discrete.convert.Convert
    api_object = api_objects["engine.resistance.discrete.convert.Convert"]

    elem_type = NyanMemberType(api_objects["engine.util.convert_type.ConvertType"])
    member_type = NyanMemberType(MemberType.CHILDREN, (elem_type,))
    member = NyanMember("type", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("chance_resist", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.resistance.discrete.convert.type.AoE2Convert
    api_object = api_objects["engine.resistance.discrete.convert.type.AoE2Convert"]

    member = NyanMember("guaranteed_resist_rounds", N_INT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("protected_rounds", N_INT, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("protection_round_recharge_time", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.resistance.discrete.flat_attribute_change.FlatAttributeChange
    api_object = api_objects["engine.resistance.discrete.flat_attribute_change.FlatAttributeChange"]

    elem_type = NyanMemberType(api_objects["engine.util.attribute_change_type.AttributeChangeType"])
    member_type = NyanMemberType(MemberType.CHILDREN, (elem_type,))
    member = NyanMember("type", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.util.attribute.AttributeAmount"])
    member = NyanMember("block_value", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.resistance.discrete.type.MakeHarvestable
    api_object = api_objects["engine.resistance.discrete.type.MakeHarvestable"]

    member_type = NyanMemberType(api_objects["engine.util.resource_spot.ResourceSpot"])
    member = NyanMember("resource_spot", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.logic.LogicElement"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("harvest_conditions", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.resistance.discrete.type.SendToContainer
    api_object = api_objects["engine.resistance.discrete.type.SendToContainer"]

    elem_type = NyanMemberType(api_objects["engine.util.container_type.SendToContainerType"])
    member_type = NyanMemberType(MemberType.CHILDREN, (elem_type,))
    member = NyanMember("type", member_type, None, None, 0)
    api_object.add_member(member)
    member = NyanMember("search_range", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.storage.EntityContainer"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("ignore_containers", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier
    # engine.modifier.Modifier
    api_object = api_objects["engine.modifier.Modifier"]

    subtype = NyanMemberType(api_objects["engine.modifier.property.ModifierProperty"])
    key_type = NyanMemberType(MemberType.ABSTRACT, (subtype,))
    member_type = NyanMemberType(MemberType.DICT, (key_type, subtype))
    member = NyanMember("properties", member_type, {}, MemberOperator.ASSIGN, 0)
    api_object.add_member(member)

    # engine.modifier.property.type.Multiplier
    api_object = api_objects["engine.modifier.property.type.Multiplier"]

    member = NyanMember("multiplier", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.property.type.Scope
    api_object = api_objects["engine.modifier.property.type.Scoped"]

    elem_type = NyanMemberType(api_objects["engine.util.diplomatic_stance.DiplomaticStance"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("stances", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.property.type.Stacked
    api_object = api_objects["engine.modifier.property.type.Stacked"]

    member = NyanMember("stack_limit", N_INT, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.effect.flat_attribute_change.type.ElevationDifferenceHigh
    api_object = api_objects["engine.modifier.effect.flat_attribute_change.type.ElevationDifferenceHigh"]

    elem_type = N_FLOAT
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("min_elevation_difference", member_type, MemberSpecialValue.NYAN_NONE,
                        MemberOperator.ASSIGN, 0)
    api_object.add_member(member)

    # engine.modifier.effect.flat_attribute_change.type.ElevationDifferenceLow
    api_object = api_objects["engine.modifier.effect.flat_attribute_change.type.ElevationDifferenceLow"]

    elem_type = N_FLOAT
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("min_elevation_difference", member_type, MemberSpecialValue.NYAN_NONE,
                        MemberOperator.ASSIGN, 0)
    api_object.add_member(member)

    # engine.modifier.effect.flat_attribute_change.type.Flyover
    api_object = api_objects["engine.modifier.effect.flat_attribute_change.type.Flyover"]

    member = NyanMember("relative_angle", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    subtype = NyanMemberType(api_objects["engine.util.game_entity_type.GameEntityType"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("flyover_types", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("blacklisted_entities", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.effect.flat_attribute_change.type.Terrain
    api_object = api_objects["engine.modifier.effect.flat_attribute_change.type.Terrain"]

    member_type = NyanMemberType(api_objects["engine.util.terrain.Terrain"])
    member = NyanMember("terrain", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.resistance.flat_attribute_change.type.ElevationDifferenceHigh
    api_object = api_objects["engine.modifier.resistance.flat_attribute_change.type.ElevationDifferenceHigh"]

    elem_type = N_FLOAT
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("min_elevation_difference", member_type, MemberSpecialValue.NYAN_NONE,
                        MemberOperator.ASSIGN, 0)
    api_object.add_member(member)

    # engine.modifier.resistance.flat_attribute_change.type.ElevationDifferenceLow
    api_object = api_objects["engine.modifier.resistance.flat_attribute_change.type.ElevationDifferenceLow"]

    elem_type = N_FLOAT
    member_type = NyanMemberType(MemberType.OPTIONAL, (elem_type,))
    member = NyanMember("min_elevation_difference", member_type, MemberSpecialValue.NYAN_NONE,
                        MemberOperator.ASSIGN, 0)
    api_object.add_member(member)

    # engine.modifier.resistance.flat_attribute_change.type.Terrain
    api_object = api_objects["engine.modifier.resistance.flat_attribute_change.type.Terrain"]

    member_type = NyanMemberType(api_objects["engine.util.terrain.Terrain"])
    member = NyanMember("terrain", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.type.AbsoluteProjectileAmount
    api_object = api_objects["engine.modifier.type.AbsoluteProjectileAmount"]

    member = NyanMember("amount", N_FLOAT, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.type.AoE2ProjectileAmount
    api_object = api_objects["engine.modifier.type.AoE2ProjectileAmount"]

    elem_type = NyanMemberType(api_objects["engine.ability.type.ApplyDiscreteEffect"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("provider_abilities", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.ability.type.ApplyDiscreteEffect"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("receiver_abilities", member_type, None, None, 0)
    api_object.add_member(member)
    subtype = NyanMemberType(api_objects["engine.util.attribute_change_type.AttributeChangeType"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("change_types", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.type.AttributeSettingsValue
    api_object = api_objects["engine.modifier.type.AttributeSettingsValue"]

    member_type = NyanMemberType(api_objects["engine.util.attribute.Attribute"])
    member = NyanMember("attribute", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.type.EntityContainerCapacity
    api_object = api_objects["engine.modifier.type.EntityContainerCapacity"]

    member_type = NyanMemberType(api_objects["engine.util.storage.EntityContainer"])
    member = NyanMember("container", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.type.ContinuousResource
    api_object = api_objects["engine.modifier.type.ContinuousResource"]

    elem_type = NyanMemberType(api_objects["engine.util.resource.ResourceRate"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("rates", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.type.CreationAttributeCost
    api_object = api_objects["engine.modifier.type.CreationAttributeCost"]

    elem_type = NyanMemberType(api_objects["engine.util.attribute.Attribute"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("attributes", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.create.CreatableGameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("creatables", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.type.CreationResourceCost
    api_object = api_objects["engine.modifier.type.CreationResourceCost"]

    elem_type = NyanMemberType(api_objects["engine.util.resource.Resource"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("resources", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.create.CreatableGameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("creatables", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.type.CreationTime
    api_object = api_objects["engine.modifier.type.CreationTime"]

    elem_type = NyanMemberType(api_objects["engine.util.create.CreatableGameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("creatables", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.type.DepositResourcesOnProgress
    api_object = api_objects["engine.modifier.type.DepositResourcesOnProgress"]

    member_type = NyanMemberType(api_objects["engine.util.progress_status.ProgressStatus"])
    member = NyanMember("progress_status", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.resource.Resource"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("resources", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity_type.GameEntityType"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("affected_types", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("blacklisted_entities", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.type.DiplomaticLineOfSight
    api_object = api_objects["engine.modifier.type.DiplomaticLineOfSight"]

    member_type = NyanMemberType(api_objects["engine.util.diplomatic_stance.DiplomaticStance"])
    member = NyanMember("diplomatic_stance", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.type.GatheringEfficiency
    api_object = api_objects["engine.modifier.type.GatheringEfficiency"]

    member_type = NyanMemberType(api_objects["engine.util.resource_spot.ResourceSpot"])
    member = NyanMember("resource_spot", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.type.GatheringRate
    api_object = api_objects["engine.modifier.type.GatheringRate"]

    member_type = NyanMemberType(api_objects["engine.util.resource_spot.ResourceSpot"])
    member = NyanMember("resource_spot", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.type.InContainerContinuousEffect
    api_object = api_objects["engine.modifier.type.InContainerContinuousEffect"]

    elem_type = NyanMemberType(api_objects["engine.util.storage.EntityContainer"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("containers", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.ability.type.ApplyContinuousEffect"])
    member = NyanMember("ability", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.type.InContainerDiscreteEffect
    api_object = api_objects["engine.modifier.type.InContainerDiscreteEffect"]

    elem_type = NyanMemberType(api_objects["engine.util.storage.EntityContainer"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("containers", member_type, None, None, 0)
    api_object.add_member(member)
    member_type = NyanMemberType(api_objects["engine.ability.type.ApplyDiscreteEffect"])
    member = NyanMember("ability", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.type.InstantTechResearch
    api_object = api_objects["engine.modifier.type.InstantTechResearch"]

    member_type = NyanMemberType(api_objects["engine.util.tech.Tech"])
    member = NyanMember("tech", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.logic.LogicElement"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("condition", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.type.RefundOnCondition
    api_object = api_objects["engine.modifier.type.RefundOnCondition"]

    elem_type = NyanMemberType(api_objects["engine.util.resource.ResourceAmount"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("refund_amount", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.type.ResearchAttributeCost
    api_object = api_objects["engine.modifier.type.ResearchAttributeCost"]

    elem_type = NyanMemberType(api_objects["engine.util.attribute.Attribute"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("attributes", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.research.ResearchableTech"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("researchables", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.type.ResearchResourceCost
    api_object = api_objects["engine.modifier.type.ResearchResourceCost"]

    elem_type = NyanMemberType(api_objects["engine.util.resource.Resource"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("resources", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.research.ResearchableTech"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("researchables", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.type.ResearchTime
    api_object = api_objects["engine.modifier.type.ResearchTime"]

    elem_type = NyanMemberType(api_objects["engine.util.research.ResearchableTech"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("researchables", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.type.Reveal
    api_object = api_objects["engine.modifier.type.Reveal"]

    member = NyanMember("line_of_sight", N_FLOAT, None, None, 0)
    api_object.add_member(member)
    subtype = NyanMemberType(api_objects["engine.util.game_entity_type.GameEntityType"])
    elem_type = NyanMemberType(MemberType.CHILDREN, (subtype,))
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("affected_types", member_type, None, None, 0)
    api_object.add_member(member)
    elem_type = NyanMemberType(api_objects["engine.util.game_entity.GameEntity"])
    member_type = NyanMemberType(MemberType.SET, (elem_type,))
    member = NyanMember("blacklisted_entities", member_type, None, None, 0)
    api_object.add_member(member)

    # engine.modifier.type.StorageElementCapacity
    api_object = api_objects["engine.modifier.type.StorageElementCapacity"]

    member_type = NyanMemberType(api_objects["engine.util.storage.StorageElementDefinition"])
    member = NyanMember("storage_element", member_type, None, None, 0)
    api_object.add_member(member)

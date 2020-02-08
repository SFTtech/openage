# Copyright 2019-2020 the openage authors. See copying.md for legal info.

"""
Loads the API into the converter.

TODO: Implement a parser instead of hardcoded
object creation.
"""

from ...nyan.nyan_structs import NyanObject, NyanMember
from openage.nyan.nyan_structs import MemberType


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
    # engine.root.Entity
    nyan_object = NyanObject("Entity")
    fqon = "engine.root.Entity"
    nyan_object.set_fqon(fqon)

    api_objects.update({fqon: nyan_object})

    # engine.ability
    # engine.ability.Ability
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Ability", parents)
    fqon = "engine.ability.Ability"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.specialization.AnimatedAbility
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("AnimatedAbility", parents)
    fqon = "engine.ability.specialization.AnimatedAbility"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.specialization.AnimationOverrideAbility
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("AnimationOverrideAbility", parents)
    fqon = "engine.ability.specialization.AnimationOverrideAbility"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.specialization.CommandSoundAbility
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("CommandSoundAbility", parents)
    fqon = "engine.ability.specialization.CommandSoundAbility"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.specialization.DiplomaticAbility
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("DiplomaticAbility", parents)
    fqon = "engine.ability.specialization.DiplomaticAbility"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.specialization.ExecutionSoundAbility
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("ExecutionSoundAbility", parents)
    fqon = "engine.ability.specialization.ExecutionSoundAbility"
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

    # engine.ability.type.Deletable
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Deletable", parents)
    fqon = "engine.ability.type.Deletable"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.ability.type.DepositResources
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("DepositResources", parents)
    fqon = "engine.ability.type.DepositResources"
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

    # engine.ability.type.FormFormation
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("FormFormation", parents)
    fqon = "engine.ability.type.FormFormation"
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

    # engine.ability.type.Transform
    parents = [api_objects["engine.ability.Ability"]]
    nyan_object = NyanObject("Transform", parents)
    fqon = "engine.ability.type.Transform"
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

    # engine.aux
    # engine.aux.accuracy.Accuracy
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Accuracy", parents)
    fqon = "engine.aux.accuracy.Accuracy"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.animation_override.AnimationOverride
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("AnimationOverride", parents)
    fqon = "engine.aux.animation_override.AnimationOverride"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.attribute.Attribute
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Attribute", parents)
    fqon = "engine.aux.attribute.Attribute"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.attribute.AttributeAmount
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("AttributeAmount", parents)
    fqon = "engine.aux.attribute.AttributeAmount"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.attribute.AttributeRate
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("AttributeRate", parents)
    fqon = "engine.aux.attribute.AttributeRate"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.attribute.AttributeSetting
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("AttributeSetting", parents)
    fqon = "engine.aux.attribute.AttributeSetting"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.attribute.ProtectingAttribute
    parents = [api_objects["engine.aux.attribute.Attribute"]]
    nyan_object = NyanObject("ProtectingAttribute", parents)
    fqon = "engine.aux.attribute.ProtectingAttribute"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.attribute_change_type.AttributeChangeType
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("AttributeChangeType", parents)
    fqon = "engine.aux.attribute_change_type.AttributeChangeType"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.attribute_change_type.type.Fallback
    parents = [api_objects["engine.aux.attribute_change_type.AttributeChangeType"]]
    nyan_object = NyanObject("Fallback", parents)
    fqon = "engine.aux.attribute_change_type.type.Fallback"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.boolean.Clause
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Clause", parents)
    fqon = "engine.aux.boolean.Clause"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.boolean.Literal
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Literal", parents)
    fqon = "engine.aux.boolean.Literal"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.boolean.literal.type.AttributeInterval
    parents = [api_objects["engine.aux.boolean.Literal"]]
    nyan_object = NyanObject("AttributeInterval", parents)
    fqon = "engine.aux.boolean.literal.type.AttributeInterval"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.boolean.literal.type.GameEntityProgress
    parents = [api_objects["engine.aux.boolean.Literal"]]
    nyan_object = NyanObject("GameEntityProgress", parents)
    fqon = "engine.aux.boolean.literal.type.GameEntityProgress"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.boolean.literal.type.ProjectileHit
    parents = [api_objects["engine.aux.boolean.Literal"]]
    nyan_object = NyanObject("ProjectileHit", parents)
    fqon = "engine.aux.boolean.literal.type.ProjectileHit"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.boolean.literal.type.ProjectileHitTerrain
    parents = [api_objects["engine.aux.boolean.Literal"]]
    nyan_object = NyanObject("ProjectileHitTerrain", parents)
    fqon = "engine.aux.boolean.literal.type.ProjectileHitTerrain"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.boolean.literal.type.ProjectilePassThrough
    parents = [api_objects["engine.aux.boolean.Literal"]]
    nyan_object = NyanObject("ProjectilePassThrough", parents)
    fqon = "engine.aux.boolean.literal.type.ProjectilePassThrough"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.boolean.literal.type.ResourceSpotsDepleted
    parents = [api_objects["engine.aux.boolean.Literal"]]
    nyan_object = NyanObject("ResourceSpotsDepleted", parents)
    fqon = "engine.aux.boolean.literal.type.ResourceSpotsDepleted"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.boolean.literal.type.TechResearched
    parents = [api_objects["engine.aux.boolean.Literal"]]
    nyan_object = NyanObject("TechResearched", parents)
    fqon = "engine.aux.boolean.literal.type.TechResearched"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.boolean.literal.type.Timer
    parents = [api_objects["engine.aux.boolean.Literal"]]
    nyan_object = NyanObject("Timer", parents)
    fqon = "engine.aux.boolean.literal.type.Timer"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.boolean.literal_scope.LiteralScope
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("LiteralScope", parents)
    fqon = "engine.aux.boolean.literal_scope.LiteralScope"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.boolean.literal_scope.type.Any
    parents = [api_objects["engine.aux.boolean.literal_scope.LiteralScope"]]
    nyan_object = NyanObject("Any", parents)
    fqon = "engine.aux.boolean.literal_scope.type.Any"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.boolean.literal_scope.type.Self
    parents = [api_objects["engine.aux.boolean.literal_scope.LiteralScope"]]
    nyan_object = NyanObject("Self", parents)
    fqon = "engine.aux.boolean.literal_scope.type.Self"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.boolean.requirement_mode.RequirementMode
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("RequirementMode", parents)
    fqon = "engine.aux.boolean.requirement_mode.RequirementMode"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.boolean.requirement_mode.type.All
    parents = [api_objects["engine.aux.boolean.requirement_mode.RequirementMode"]]
    nyan_object = NyanObject("All", parents)
    fqon = "engine.aux.boolean.requirement_mode.type.All"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.boolean.requirement_mode.type.Any
    parents = [api_objects["engine.aux.boolean.requirement_mode.RequirementMode"]]
    nyan_object = NyanObject("Any", parents)
    fqon = "engine.aux.boolean.requirement_mode.type.Any"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.boolean.requirement_mode.type.Subset
    parents = [api_objects["engine.aux.boolean.requirement_mode.RequirementMode"]]
    nyan_object = NyanObject("Subset", parents)
    fqon = "engine.aux.boolean.requirement_mode.type.Subset"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.calculation_type.CalculationType
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("CalculationType", parents)
    fqon = "engine.aux.calculation_type.CalculationType"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.calculation_type.type.Hyperbolic
    parents = [api_objects["engine.aux.calculation_type.CalculationType"]]
    nyan_object = NyanObject("Hyperbolic", parents)
    fqon = "engine.aux.calculation_type.type.Hyperbolic"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.calculation_type.type.Linear
    parents = [api_objects["engine.aux.calculation_type.CalculationType"]]
    nyan_object = NyanObject("Linear", parents)
    fqon = "engine.aux.calculation_type.type.Linear"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.calculation_type.type.NoStack
    parents = [api_objects["engine.aux.calculation_type.CalculationType"]]
    nyan_object = NyanObject("NoStack", parents)
    fqon = "engine.aux.calculation_type.type.NoStack"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.cheat.Cheat
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Cheat", parents)
    fqon = "engine.aux.cheat.Cheat"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.civilization.Civilization
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Civilization", parents)
    fqon = "engine.aux.civilization.Civilization"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.container_type.SendToContainerType
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("SendToContainerType", parents)
    fqon = "engine.aux.container_type.SendToContainerType"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.convert_type.ConvertType
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("ConvertType", parents)
    fqon = "engine.aux.convert_type.ConvertType"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.cost.Cost
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Cost", parents)
    fqon = "engine.aux.cost.Cost"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.cost.type.AttributeCost
    parents = [api_objects["engine.aux.cost.Cost"]]
    nyan_object = NyanObject("AttributeCost", parents)
    fqon = "engine.aux.cost.type.AttributeCost"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.cost.type.ResourceCost
    parents = [api_objects["engine.aux.cost.Cost"]]
    nyan_object = NyanObject("ResourceCost", parents)
    fqon = "engine.aux.cost.type.ResourceCost"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.create.CreatableGameEntity
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("CreatableGameEntity", parents)
    fqon = "engine.aux.create.CreatableGameEntity"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.diplomatic_stance.DiplomaticStance
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("DiplomaticStance", parents)
    fqon = "engine.aux.diplomatic_stance.DiplomaticStance"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.diplomatic_stance.type.Self
    parents = [api_objects["engine.aux.diplomatic_stance.DiplomaticStance"]]
    nyan_object = NyanObject("Self", parents)
    fqon = "engine.aux.diplomatic_stance.type.Self"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.distribution_type.DistributionType
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("DistributionType", parents)
    fqon = "engine.aux.distribution_type.DistributionType"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.distribution_type.type.Mean
    parents = [api_objects["engine.aux.distribution_type.DistributionType"]]
    nyan_object = NyanObject("Mean", parents)
    fqon = "engine.aux.distribution_type.type.Mean"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.dropoff_type.DropoffType
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("DropoffType", parents)
    fqon = "engine.aux.dropoff_type.DropoffType"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.dropoff_type.type.InverseLinear
    parents = [api_objects["engine.aux.dropoff_type.DropoffType"]]
    nyan_object = NyanObject("InverseLinear", parents)
    fqon = "engine.aux.dropoff_type.type.InverseLinear"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.dropoff_type.type.Linear
    parents = [api_objects["engine.aux.dropoff_type.DropoffType"]]
    nyan_object = NyanObject("Linear", parents)
    fqon = "engine.aux.dropoff_type.type.Linear"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.dropoff_type.type.NoDropoff
    parents = [api_objects["engine.aux.dropoff_type.DropoffType"]]
    nyan_object = NyanObject("NoDropoff", parents)
    fqon = "engine.aux.dropoff_type.type.NoDropoff"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.exchange_mode.ExchangeMode
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("ExchangeMode", parents)
    fqon = "engine.aux.exchange_mode.ExchangeMode"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.exchange_mode.type.Fixed
    parents = [api_objects["engine.aux.exchange_mode.ExchangeMode"]]
    nyan_object = NyanObject("Fixed", parents)
    fqon = "engine.aux.exchange_mode.type.Fixed"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.exchange_mode.volatile.Volatile
    parents = [api_objects["engine.aux.exchange_mode.ExchangeMode"]]
    nyan_object = NyanObject("Volatile", parents)
    fqon = "engine.aux.exchange_mode.volatile.Volatile"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.exchange_mode.volatile.VolatileFlat
    parents = [api_objects["engine.aux.exchange_mode.volatile.Volatile"]]
    nyan_object = NyanObject("VolatileFlat", parents)
    fqon = "engine.aux.exchange_mode.volatile.VolatileFlat"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.exchange_scope.ExchangeScope
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("ExchangeScope", parents)
    fqon = "engine.aux.exchange_scope.ExchangeScope"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.formation.Formation
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Formation", parents)
    fqon = "engine.aux.formation.Formation"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.formation.Subformation
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Subformation", parents)
    fqon = "engine.aux.formation.Subformation"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.formation.PrecedingSubformation
    parents = [api_objects["engine.aux.formation.Subformation"]]
    nyan_object = NyanObject("PrecedingSubformation", parents)
    fqon = "engine.aux.formation.PrecedingSubformation"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.game_entity.GameEntity
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("GameEntity", parents)
    fqon = "engine.aux.game_entity.GameEntity"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.game_entity_formation.GameEntityFormation
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("GameEntityFormation", parents)
    fqon = "engine.aux.game_entity_formation.GameEntityFormation"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.game_entity_stance.GameEntityStance
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("GameEntityStance", parents)
    fqon = "engine.aux.game_entity_stance.GameEntityStance"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.game_entity_stance.type.Aggressive
    parents = [api_objects["engine.aux.game_entity_stance.GameEntityStance"]]
    nyan_object = NyanObject("Aggressive", parents)
    fqon = "engine.aux.game_entity_stance.type.Aggressive"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.game_entity_stance.type.Defensive
    parents = [api_objects["engine.aux.game_entity_stance.GameEntityStance"]]
    nyan_object = NyanObject("Defensive", parents)
    fqon = "engine.aux.game_entity_stance.type.Defensive"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.game_entity_stance.type.Passive
    parents = [api_objects["engine.aux.game_entity_stance.GameEntityStance"]]
    nyan_object = NyanObject("Passive", parents)
    fqon = "engine.aux.game_entity_stance.type.Passive"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.game_entity_stance.type.StandGround
    parents = [api_objects["engine.aux.game_entity_stance.GameEntityStance"]]
    nyan_object = NyanObject("StandGround", parents)
    fqon = "engine.aux.game_entity_stance.type.StandGround"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.game_entity_type.GameEntityType
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("GameEntityType", parents)
    fqon = "engine.aux.game_entity_type.GameEntityType"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.graphics.Animation
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Animation", parents)
    fqon = "engine.aux.graphics.Animation"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.graphics.Terrain
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Terrain", parents)
    fqon = "engine.aux.graphics.Terrain"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.herdable_mode.HerdableMode
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("HerdableMode", parents)
    fqon = "engine.aux.herdable_mode.HerdableMode"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.herdable_mode.type.ClosestHerding
    parents = [api_objects["engine.aux.herdable_mode.HerdableMode"]]
    nyan_object = NyanObject("ClosestHerding", parents)
    fqon = "engine.aux.herdable_mode.type.ClosestHerding"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.herdable_mode.type.LongestTimeInRange
    parents = [api_objects["engine.aux.herdable_mode.HerdableMode"]]
    nyan_object = NyanObject("LongestTimeInRange", parents)
    fqon = "engine.aux.herdable_mode.type.LongestTimeInRange"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.herdable_mode.type.MostHerding
    parents = [api_objects["engine.aux.herdable_mode.HerdableMode"]]
    nyan_object = NyanObject("MostHerding", parents)
    fqon = "engine.aux.herdable_mode.type.MostHerding"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.hitbox.Hitbox
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Hitbox", parents)
    fqon = "engine.aux.hitbox.Hitbox"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.language.Language
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Language", parents)
    fqon = "engine.aux.language.Language"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.language.LanguageMarkupPair
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("LanguageMarkupPair", parents)
    fqon = "engine.aux.language.LanguageMarkupPair"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.language.LanguageSoundPair
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("LanguageSoundPair", parents)
    fqon = "engine.aux.language.LanguageSoundPair"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.language.LanguageTextPair
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("LanguageTextPair", parents)
    fqon = "engine.aux.language.LanguageTextPair"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.lure_type.LureType
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("LureType", parents)
    fqon = "engine.aux.lure_type.LureType"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.mod.Mod
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Mod", parents)
    fqon = "engine.aux.mod.Mod"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.modifier_scope.ModifierScope
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("ModifierScope", parents)
    fqon = "engine.aux.modifier_scope.ModifierScope"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.modifier_scope.type.GameEntityScope
    parents = [api_objects["engine.aux.modifier_scope.ModifierScope"]]
    nyan_object = NyanObject("GameEntityScope", parents)
    fqon = "engine.aux.modifier_scope.type.GameEntityScope"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.modifier_scope.type.Standard
    parents = [api_objects["engine.aux.modifier_scope.ModifierScope"]]
    nyan_object = NyanObject("Standard", parents)
    fqon = "engine.aux.modifier_scope.type.Standard"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.move_mode.MoveMode
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("MoveMode", parents)
    fqon = "engine.aux.move_mode.MoveMode"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.move_mode.type.AttackMove
    parents = [api_objects["engine.aux.move_mode.MoveMode"]]
    nyan_object = NyanObject("AttackMove", parents)
    fqon = "engine.aux.move_mode.type.AttackMove"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.move_mode.type.Follow
    parents = [api_objects["engine.aux.move_mode.MoveMode"]]
    nyan_object = NyanObject("Follow", parents)
    fqon = "engine.aux.move_mode.type.Follow"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.move_mode.type.Normal
    parents = [api_objects["engine.aux.move_mode.MoveMode"]]
    nyan_object = NyanObject("Normal", parents)
    fqon = "engine.aux.move_mode.type.Normal"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.move_mode.type.Patrol
    parents = [api_objects["engine.aux.move_mode.MoveMode"]]
    nyan_object = NyanObject("Patrol", parents)
    fqon = "engine.aux.move_mode.type.Patrol"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.passable_mode.PassableMode
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("PassableMode", parents)
    fqon = "engine.aux.passable_mode.PassableMode"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.passable_mode.type.Gate
    parents = [api_objects["engine.aux.passable_mode.PassableMode"]]
    nyan_object = NyanObject("Gate", parents)
    fqon = "engine.aux.passable_mode.type.Gate"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.passable_mode.type.Normal
    parents = [api_objects["engine.aux.passable_mode.PassableMode"]]
    nyan_object = NyanObject("Normal", parents)
    fqon = "engine.aux.passable_mode.type.Normal"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.patch.Patch
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Patch", parents)
    fqon = "engine.aux.patch.Patch"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.patch.type.DiplomaticPatch
    parents = [api_objects["engine.aux.patch.Patch"]]
    nyan_object = NyanObject("DiplomaticPatch", parents)
    fqon = "engine.aux.patch.type.DiplomaticPatch"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.payment_mode.PaymentMode
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("PaymentMode", parents)
    fqon = "engine.aux.payment_mode.PaymentMode"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.payment_mode.type.Adaptive
    parents = [api_objects["engine.aux.payment_mode.PaymentMode"]]
    nyan_object = NyanObject("Adaptive", parents)
    fqon = "engine.aux.payment_mode.type.Adaptive"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.payment_mode.type.Advance
    parents = [api_objects["engine.aux.payment_mode.PaymentMode"]]
    nyan_object = NyanObject("Advance", parents)
    fqon = "engine.aux.payment_mode.type.Advance"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.payment_mode.type.Arrear
    parents = [api_objects["engine.aux.payment_mode.PaymentMode"]]
    nyan_object = NyanObject("Arrear", parents)
    fqon = "engine.aux.payment_mode.type.Arrear"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.payment_mode.type.Shadow
    parents = [api_objects["engine.aux.payment_mode.PaymentMode"]]
    nyan_object = NyanObject("Shadow", parents)
    fqon = "engine.aux.payment_mode.type.Shadow"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.placement_mode.PlacementMode
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("PlacementMode", parents)
    fqon = "engine.aux.placement_mode.PlacementMode"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.placement_mode.type.Eject
    parents = [api_objects["engine.aux.placement_mode.PlacementMode"]]
    nyan_object = NyanObject("Eject", parents)
    fqon = "engine.aux.placement_mode.type.Eject"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.placement_mode.type.OwnStorage
    parents = [api_objects["engine.aux.placement_mode.PlacementMode"]]
    nyan_object = NyanObject("OwnStorage", parents)
    fqon = "engine.aux.placement_mode.type.OwnStorage"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.placement_mode.type.Place
    parents = [api_objects["engine.aux.placement_mode.PlacementMode"]]
    nyan_object = NyanObject("Place", parents)
    fqon = "engine.aux.placement_mode.type.Place"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.production_mode.ProductionMode
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("ProductionMode", parents)
    fqon = "engine.aux.production_mode.ProductionMode"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.production_mode.type.Creatables
    parents = [api_objects["engine.aux.production_mode.ProductionMode"]]
    nyan_object = NyanObject("Creatables", parents)
    fqon = "engine.aux.production_mode.type.Creatables"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.production_mode.type.Researchables
    parents = [api_objects["engine.aux.production_mode.ProductionMode"]]
    nyan_object = NyanObject("Researchables", parents)
    fqon = "engine.aux.production_mode.type.Researchables"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.progress.Progress
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Progress", parents)
    fqon = "engine.aux.progress.Progress"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.progress.specialization.AnimatedProgress
    parents = [api_objects["engine.aux.progress.Progress"]]
    nyan_object = NyanObject("AnimatedProgress", parents)
    fqon = "engine.aux.progress.specialization.AnimatedProgress"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.progress.specialization.StateChangeProgress
    parents = [api_objects["engine.aux.progress.Progress"]]
    nyan_object = NyanObject("StateChangeProgress", parents)
    fqon = "engine.aux.progress.specialization.StateChangeProgress"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.progress.specialization.TerrainOverlayProgress
    parents = [api_objects["engine.aux.progress.Progress"]]
    nyan_object = NyanObject("TerrainOverlayProgress", parents)
    fqon = "engine.aux.progress.specialization.TerrainOverlayProgress"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.progress.specialization.TerrainProgress
    parents = [api_objects["engine.aux.progress.Progress"]]
    nyan_object = NyanObject("TerrainProgress", parents)
    fqon = "engine.aux.progress.specialization.TerrainProgress"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.progress.type.AttributeChangeProgress
    parents = [api_objects["engine.aux.progress.Progress"]]
    nyan_object = NyanObject("AttributeChangeProgress", parents)
    fqon = "engine.aux.progress.type.AttributeChangeProgress"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.progress.type.CarryProgress
    parents = [api_objects["engine.aux.progress.Progress"]]
    nyan_object = NyanObject("CarryProgress", parents)
    fqon = "engine.aux.progress.type.CarryProgress"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.progress.type.ConstructionProgress
    parents = [api_objects["engine.aux.progress.Progress"]]
    nyan_object = NyanObject("ConstructionProgress", parents)
    fqon = "engine.aux.progress.type.ConstructionProgress"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.progress.type.HarvestProgress
    parents = [api_objects["engine.aux.progress.Progress"]]
    nyan_object = NyanObject("HarvestProgress", parents)
    fqon = "engine.aux.progress.type.HarvestProgress"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.progress.type.RestockProgress
    parents = [api_objects["engine.aux.progress.Progress"]]
    nyan_object = NyanObject("RestockProgress", parents)
    fqon = "engine.aux.progress.type.RestockProgress"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.progress.type.TransformProgress
    parents = [api_objects["engine.aux.progress.Progress"]]
    nyan_object = NyanObject("TransformProgress", parents)
    fqon = "engine.aux.progress.type.TransformProgress"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.progress_status.ProgressStatus
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("ProgressStatus", parents)
    fqon = "engine.aux.progress_status.ProgressStatus"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.progress_type.ProgressType
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("ProgressType", parents)
    fqon = "engine.aux.progress_type.ProgressType"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.progress_type.type.Construct
    parents = [api_objects["engine.aux.progress_type.ProgressType"]]
    nyan_object = NyanObject("Construct", parents)
    fqon = "engine.aux.progress_type.type.Construct"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.research.ResearchableTech
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("ResearchableTech", parents)
    fqon = "engine.aux.research.ResearchableTech"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.resource.Resource
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Resource", parents)
    fqon = "engine.aux.resource.Resource"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.resource.ResourceContingent
    parents = [api_objects["engine.aux.resource.Resource"]]
    nyan_object = NyanObject("ResourceContingent", parents)
    fqon = "engine.aux.resource.ResourceContingent"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.resource.ResourceAmount
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("ResourceAmount", parents)
    fqon = "engine.aux.resource.ResourceAmount"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.resource.ResourceRate
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("ResourceRate", parents)
    fqon = "engine.aux.resource.ResourceRate"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.resource_spot.ResourceSpot
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("ResourceSpot", parents)
    fqon = "engine.aux.resource_spot.ResourceSpot"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.resource_spot.RestockableResourceSpot
    parents = [api_objects["engine.aux.resource_spot.ResourceSpot"]]
    nyan_object = NyanObject("RestockableResourceSpot", parents)
    fqon = "engine.aux.resource_spot.RestockableResourceSpot"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.selection_box.SelectionBox
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("SelectionBox", parents)
    fqon = "engine.aux.selection_box.SelectionBox"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.selection_box.type.MatchToSprite
    parents = [api_objects["engine.aux.selection_box.SelectionBox"]]
    nyan_object = NyanObject("MatchToSprite", parents)
    fqon = "engine.aux.selection_box.type.MatchToSprite"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.selection_box.type.Rectangle
    parents = [api_objects["engine.aux.selection_box.SelectionBox"]]
    nyan_object = NyanObject("Rectangle", parents)
    fqon = "engine.aux.selection_box.type.Rectangle"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.sound.Sound
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Sound", parents)
    fqon = "engine.aux.sound.Sound"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.state_machine.StateChanger
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("StateChanger", parents)
    fqon = "engine.aux.state_machine.StateChanger"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.storage.Container
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Container", parents)
    fqon = "engine.aux.storage.Container"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.storage.StorageElement
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("StorageElement", parents)
    fqon = "engine.aux.storage.StorageElement"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.target_mode.TargetMode
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("TargetMode", parents)
    fqon = "engine.aux.target_mode.TargetMode"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.target_mode.type.CurrentPosition
    parents = [api_objects["engine.aux.target_mode.TargetMode"]]
    nyan_object = NyanObject("CurrentPosition", parents)
    fqon = "engine.aux.target_mode.type.CurrentPosition"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.target_mode.type.ExpectedPosition
    parents = [api_objects["engine.aux.target_mode.TargetMode"]]
    nyan_object = NyanObject("ExpectedPosition", parents)
    fqon = "engine.aux.target_mode.type.ExpectedPosition"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.taunt.Taunt
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Taunt", parents)
    fqon = "engine.aux.taunt.Taunt"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.tech.Tech
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Tech", parents)
    fqon = "engine.aux.tech.Tech"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.terrain.Terrain
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Terrain", parents)
    fqon = "engine.aux.terrain.Terrain"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.terrain.TerrainAmbient
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("TerrainAmbient", parents)
    fqon = "engine.aux.terrain.TerrainAmbient"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.terrain_type.TerrainType
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("TerrainType", parents)
    fqon = "engine.aux.terrain_type.TerrainType"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.trade_route.TradeRoute
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("TradeRoute", parents)
    fqon = "engine.aux.trade_route.TradeRoute"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.trade_route.type.AoE1TradeRoute
    parents = [api_objects["engine.aux.trade_route.TradeRoute"]]
    nyan_object = NyanObject("AoE1TradeRoute", parents)
    fqon = "engine.aux.trade_route.type.AoE1TradeRoute"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.trade_route.type.AoE2TradeRoute
    parents = [api_objects["engine.aux.trade_route.TradeRoute"]]
    nyan_object = NyanObject("AoE2TradeRoute", parents)
    fqon = "engine.aux.trade_route.type.AoE2TradeRoute"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.transform_pool.TransformPool
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("TransformPool", parents)
    fqon = "engine.aux.transform_pool.TransformPool"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.translated.TranslatedObject
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("TranslatedObject", parents)
    fqon = "engine.aux.translated.TranslatedObject"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.translated.type.TranslatedMarkupFile
    parents = [api_objects["engine.aux.translated.TranslatedObject"]]
    nyan_object = NyanObject("TranslatedMarkupFile", parents)
    fqon = "engine.aux.translated.type.TranslatedMarkupFile"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.translated.type.TranslatedSound
    parents = [api_objects["engine.aux.translated.TranslatedObject"]]
    nyan_object = NyanObject("TranslatedSound", parents)
    fqon = "engine.aux.translated.type.TranslatedSound"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.translated.type.TranslatedString
    parents = [api_objects["engine.aux.translated.TranslatedObject"]]
    nyan_object = NyanObject("TranslatedString", parents)
    fqon = "engine.aux.translated.type.TranslatedString"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.variant.Variant
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Variant", parents)
    fqon = "engine.aux.variant.Variant"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.variant.type.AdjacentTilesVariant
    parents = [api_objects["engine.aux.variant.Variant"]]
    nyan_object = NyanObject("AdjacentTilesVariant", parents)
    fqon = "engine.aux.variant.type.AdjacentTilesVariant"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.variant.type.RandomVariant
    parents = [api_objects["engine.aux.variant.Variant"]]
    nyan_object = NyanObject("RandomVariant", parents)
    fqon = "engine.aux.variant.type.RandomVariant"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.aux.variant.type.PerspectiveVariant
    parents = [api_objects["engine.aux.variant.Variant"]]
    nyan_object = NyanObject("PerspectiveVariant", parents)
    fqon = "engine.aux.variant.type.PerspectiveVariant"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect
    # engine.effect.Effect
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Effect", parents)
    fqon = "engine.effect.Effect"
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

    # engine.effect.continuous.time_relative_attribute_change.TimeRelativeAttributeChange
    parents = [api_objects["engine.effect.continuous.ContinuousEffect"]]
    nyan_object = NyanObject("TimeRelativeAttributeChange", parents)
    fqon = "engine.effect.continuous.time_relative_attribute_change.TimeRelativeAttributeChange"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.continuous.time_relative_attribute_change.type.TimeRelativeAttributeDecrease
    parents = [api_objects["engine.effect.continuous.time_relative_attribute_change.TimeRelativeAttributeChange"]]
    nyan_object = NyanObject("TimeRelativeAttributeDecrease", parents)
    fqon = "engine.effect.continuous.time_relative_attribute_change.type.TimeRelativeAttributeDecrease"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.continuous.time_relative_attribute_change.type.TimeRelativeAttributeIncrease
    parents = [api_objects["engine.effect.continuous.time_relative_attribute_change.TimeRelativeAttributeChange"]]
    nyan_object = NyanObject("TimeRelativeAttributeIncrease", parents)
    fqon = "engine.effect.continuous.time_relative_attribute_change.type.TimeRelativeAttributeIncrease"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.continuous.time_relative_progress_change.TimeRelativeProgressChange
    parents = [api_objects["engine.effect.continuous.ContinuousEffect"]]
    nyan_object = NyanObject("TimeRelativeProgressChange", parents)
    fqon = "engine.effect.continuous.time_relative_progress_change.TimeRelativeProgressChange"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.continuous.time_relative_progress.type.TimeRelativeProgressDecrease
    parents = [api_objects["engine.effect.continuous.time_relative_progress_change.TimeRelativeProgressChange"]]
    nyan_object = NyanObject("TimeRelativeProgressDecrease", parents)
    fqon = "engine.effect.continuous.time_relative_progress.type.TimeRelativeProgressDecrease"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.continuous.time_relative_progress.type.TimeRelativeProgressIncrease
    parents = [api_objects["engine.effect.continuous.time_relative_progress_change.TimeRelativeProgressChange"]]
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

    # engine.effect.specialization.AreaEffect
    parents = [api_objects["engine.effect.Effect"]]
    nyan_object = NyanObject("AreaEffect", parents)
    fqon = "engine.effect.specialization.AreaEffect"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.specialization.CostEffect
    parents = [api_objects["engine.effect.Effect"]]
    nyan_object = NyanObject("CostEffect", parents)
    fqon = "engine.effect.specialization.CostEffect"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.effect.specialization.DiplomaticEffect
    parents = [api_objects["engine.effect.Effect"]]
    nyan_object = NyanObject("DiplomaticEffect", parents)
    fqon = "engine.effect.specialization.DiplomaticEffect"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance
    # engine.resistance.Resistance
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Resistance", parents)
    fqon = "engine.resistance.Resistance"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.specialization.StackedResistance
    parents = [api_objects["engine.resistance.Resistance"]]
    nyan_object = NyanObject("StackedResistance", parents)
    fqon = "engine.resistance.specialization.StackedResistance"
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

    # engine.resistance.continuous.time_relative_attribute_change.TimeRelativeAttributeChange
    parents = [api_objects["engine.resistance.continuous.ContinuousResistance"]]
    nyan_object = NyanObject("TimeRelativeAttributeChange", parents)
    fqon = "engine.resistance.continuous.time_relative_attribute_change.TimeRelativeAttributeChange"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.continuous.time_relative_attribute_change.type.TimeRelativeAttributeDecrease
    parents = [api_objects["engine.resistance.continuous.time_relative_attribute_change.TimeRelativeAttributeChange"]]
    nyan_object = NyanObject("TimeRelativeAttributeDecrease", parents)
    fqon = "engine.resistance.continuous.time_relative_attribute_change.type.TimeRelativeAttributeDecrease"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.continuous.time_relative_attribute_change.type.TimeRelativeAttributeIncrease
    parents = [api_objects["engine.resistance.continuous.time_relative_attribute_change.TimeRelativeAttributeChange"]]
    nyan_object = NyanObject("TimeRelativeAttributeIncrease", parents)
    fqon = "engine.resistance.continuous.time_relative_attribute_change.type.TimeRelativeAttributeIncrease"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.continuous.time_relative_progress_change.TimeRelativeProgressChange
    parents = [api_objects["engine.resistance.continuous.ContinuousResistance"]]
    nyan_object = NyanObject("TimeRelativeProgressChange", parents)
    fqon = "engine.resistance.continuous.time_relative_progress_change.TimeRelativeProgressChange"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.continuous.time_relative_progress.type.TimeRelativeProgressDecrease
    parents = [api_objects["engine.resistance.continuous.time_relative_progress_change.TimeRelativeProgressChange"]]
    nyan_object = NyanObject("TimeRelativeProgressDecrease", parents)
    fqon = "engine.resistance.continuous.time_relative_progress.type.TimeRelativeProgressDecrease"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.resistance.continuous.time_relative_progress.type.TimeRelativeProgressIncrease
    parents = [api_objects["engine.resistance.continuous.time_relative_progress_change.TimeRelativeProgressChange"]]
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

    # engine.resistance.specialization.CostResistance
    parents = [api_objects["engine.resistance.Resistance"]]
    nyan_object = NyanObject("CostResistance", parents)
    fqon = "engine.resistance.specialization.CostResistance"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier
    # engine.modifier.Modifier
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Modifier", parents)
    fqon = "engine.modifier.Modifier"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.specialization.ScopeModifier
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("ScopeModifier", parents)
    fqon = "engine.modifier.specialization.ScopeModifier"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.specialization.StackedModifier
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("StackedModifier", parents)
    fqon = "engine.modifier.specialization.StackedModifier"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.MultiplierModifier
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("MultiplierModifier", parents)
    fqon = "engine.modifier.multiplier.MultiplierModifier"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.effect.EffectMultiplierModifier
    parents = [api_objects["engine.modifier.multiplier.MultiplierModifier"]]
    nyan_object = NyanObject("EffectMultiplierModifier", parents)
    fqon = "engine.modifier.multiplier.effect.EffectMultiplierModifier"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.effect.flat_attribute_change.FlatAttributeChangeModifier
    parents = [api_objects["engine.modifier.multiplier.effect.EffectMultiplierModifier"]]
    nyan_object = NyanObject("FlatAttributeChangeModifier", parents)
    fqon = "engine.modifier.multiplier.effect.flat_attribute_change.FlatAttributeChangeModifier"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.effect.flat_attribute_change.type.ElevationDifferenceHigh
    parents = [api_objects["engine.modifier.multiplier.effect.flat_attribute_change.FlatAttributeChangeModifier"]]
    nyan_object = NyanObject("ElevationDifferenceHigh", parents)
    fqon = "engine.modifier.multiplier.effect.flat_attribute_change.type.ElevationDifferenceHigh"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.effect.flat_attribute_change.type.Flyover
    parents = [api_objects["engine.modifier.multiplier.effect.flat_attribute_change.FlatAttributeChangeModifier"]]
    nyan_object = NyanObject("Flyover", parents)
    fqon = "engine.modifier.multiplier.effect.flat_attribute_change.type.Flyover"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.effect.flat_attribute_change.type.Terrain
    parents = [api_objects["engine.modifier.multiplier.effect.flat_attribute_change.FlatAttributeChangeModifier"]]
    nyan_object = NyanObject("Terrain", parents)
    fqon = "engine.modifier.multiplier.effect.flat_attribute_change.type.Terrain"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.effect.flat_attribute_change.type.Unconditional
    parents = [api_objects["engine.modifier.multiplier.effect.flat_attribute_change.FlatAttributeChangeModifier"]]
    nyan_object = NyanObject("Unconditional", parents)
    fqon = "engine.modifier.multiplier.effect.flat_attribute_change.type.Unconditional"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.effect.type.TimeRelativeAttributeChangeTime
    parents = [api_objects["engine.modifier.multiplier.effect.EffectMultiplierModifier"]]
    nyan_object = NyanObject("TimeRelativeAttributeChangeTime", parents)
    fqon = "engine.modifier.multiplier.effect.type.TimeRelativeAttributeChangeTime"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.effect.type.TimeRelativeProgressTime
    parents = [api_objects["engine.modifier.multiplier.effect.EffectMultiplierModifier"]]
    nyan_object = NyanObject("TimeRelativeProgressTime", parents)
    fqon = "engine.modifier.multiplier.effect.type.TimeRelativeProgressTime"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.resistance.ResistanceMultiplierModifier
    parents = [api_objects["engine.modifier.multiplier.MultiplierModifier"]]
    nyan_object = NyanObject("ResistanceMultiplierModifier", parents)
    fqon = "engine.modifier.multiplier.resistance.ResistanceMultiplierModifier"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.resistance.flat_attribute_change.FlatAttributeChangeModifier
    parents = [api_objects["engine.modifier.multiplier.resistance.ResistanceMultiplierModifier"]]
    nyan_object = NyanObject("FlatAttributeChangeModifier", parents)
    fqon = "engine.modifier.multiplier.resistance.flat_attribute_change.FlatAttributeChangeModifier"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.resistance.flat_attribute_change.type.ElevationDifferenceLow
    parents = [api_objects["engine.modifier.multiplier.resistance.flat_attribute_change.FlatAttributeChangeModifier"]]
    nyan_object = NyanObject("ElevationDifferenceLow", parents)
    fqon = "engine.modifier.multiplier.resistance.flat_attribute_change.type.ElevationDifferenceLow"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.resistance.flat_attribute_change.type.Stray
    parents = [api_objects["engine.modifier.multiplier.resistance.flat_attribute_change.FlatAttributeChangeModifier"]]
    nyan_object = NyanObject("Stray", parents)
    fqon = "engine.modifier.multiplier.resistance.flat_attribute_change.type.Stray"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.resistance.flat_attribute_change.type.Terrain
    parents = [api_objects["engine.modifier.multiplier.resistance.flat_attribute_change.FlatAttributeChangeModifier"]]
    nyan_object = NyanObject("Terrain", parents)
    fqon = "engine.modifier.multiplier.resistance.flat_attribute_change.type.Terrain"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.resistance.flat_attribute_change.type.Unconditional
    parents = [api_objects["engine.modifier.multiplier.resistance.flat_attribute_change.FlatAttributeChangeModifier"]]
    nyan_object = NyanObject("Unconditional", parents)
    fqon = "engine.modifier.multiplier.resistance.flat_attribute_change.type.Unconditional"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.type.AttributeSettingsValue
    parents = [api_objects["engine.modifier.multiplier.MultiplierModifier"]]
    nyan_object = NyanObject("AttributeSettingsValue", parents)
    fqon = "engine.modifier.multiplier.type.AttributeSettingsValue"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.type.ContainerCapacity
    parents = [api_objects["engine.modifier.multiplier.MultiplierModifier"]]
    nyan_object = NyanObject("ContainerCapacity", parents)
    fqon = "engine.modifier.multiplier.type.ContainerCapacity"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.type.CreationAttributeCost
    parents = [api_objects["engine.modifier.multiplier.MultiplierModifier"]]
    nyan_object = NyanObject("CreationAttributeCost", parents)
    fqon = "engine.modifier.multiplier.type.CreationAttributeCost"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.type.CreationResourceCost
    parents = [api_objects["engine.modifier.multiplier.MultiplierModifier"]]
    nyan_object = NyanObject("CreationResourceCost", parents)
    fqon = "engine.modifier.multiplier.type.CreationResourceCost"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.type.CreationTime
    parents = [api_objects["engine.modifier.multiplier.MultiplierModifier"]]
    nyan_object = NyanObject("CreationTime", parents)
    fqon = "engine.modifier.multiplier.type.CreationTime"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.type.GatheringEfficiency
    parents = [api_objects["engine.modifier.multiplier.MultiplierModifier"]]
    nyan_object = NyanObject("GatheringEfficiency", parents)
    fqon = "engine.modifier.multiplier.type.GatheringEfficiency"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.type.GatheringRate
    parents = [api_objects["engine.modifier.multiplier.MultiplierModifier"]]
    nyan_object = NyanObject("GatheringRate", parents)
    fqon = "engine.modifier.multiplier.type.GatheringRate"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.type.MoveSpeed
    parents = [api_objects["engine.modifier.multiplier.MultiplierModifier"]]
    nyan_object = NyanObject("MoveSpeed", parents)
    fqon = "engine.modifier.multiplier.type.MoveSpeed"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.type.ReloadTime
    parents = [api_objects["engine.modifier.multiplier.MultiplierModifier"]]
    nyan_object = NyanObject("ReloadTime", parents)
    fqon = "engine.modifier.multiplier.type.ReloadTime"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.type.ResearchAttributeCost
    parents = [api_objects["engine.modifier.multiplier.MultiplierModifier"]]
    nyan_object = NyanObject("ResearchAttributeCost", parents)
    fqon = "engine.modifier.multiplier.type.ResearchAttributeCost"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.type.ResearchResourceCost
    parents = [api_objects["engine.modifier.multiplier.MultiplierModifier"]]
    nyan_object = NyanObject("ResearchResourceCost", parents)
    fqon = "engine.modifier.multiplier.type.ResearchResourceCost"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.type.ResearchTime
    parents = [api_objects["engine.modifier.multiplier.MultiplierModifier"]]
    nyan_object = NyanObject("ResearchTime", parents)
    fqon = "engine.modifier.multiplier.type.ResearchTime"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.multiplier.type.StorageElementCapacity
    parents = [api_objects["engine.modifier.multiplier.MultiplierModifier"]]
    nyan_object = NyanObject("StorageElementCapacity", parents)
    fqon = "engine.modifier.multiplier.type.StorageElementCapacity"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.relative_projectile_amount.AoE2ProjectileAmount
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("AoE2ProjectileAmount", parents)
    fqon = "engine.modifier.relative_projectile_amount.AoE2ProjectileAmount"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.relative_projectile_amount.type.RelativeProjectileAmountModifier
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("RelativeProjectileAmountModifier", parents)
    fqon = "engine.modifier.relative_projectile_amount.type.RelativeProjectileAmountModifier"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.AbsoluteProjectileAmount
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("AbsoluteProjectileAmount", parents)
    fqon = "engine.modifier.type.AbsoluteProjectileAmount"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.AlwaysHerd
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("AlwaysHerd", parents)
    fqon = "engine.modifier.type.AlwaysHerd"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.ContinuousResource
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("ContinuousResource", parents)
    fqon = "engine.modifier.type.ContinuousResource"
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

    # engine.modifier.type.RefundOnDeath
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("RefundOnDeath", parents)
    fqon = "engine.modifier.type.RefundOnDeath"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    # engine.modifier.type.Reveal
    parents = [api_objects["engine.modifier.Modifier"]]
    nyan_object = NyanObject("Reveal", parents)
    fqon = "engine.modifier.type.Reveal"
    nyan_object.set_fqon(fqon)
    api_objects.update({fqon: nyan_object})

    return api_objects


def _insert_members(api_objects):
    """
    Creates members for API objects.
    """

    # engine.ability
    # engine.ability.specialization.AnimatedAbility
    api_object = api_objects["engine.ability.specialization.AnimatedAbility"]

    set_type = api_objects["engine.aux.graphics.Animation"]
    member = NyanMember("animations", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.specialization.AnimationOverrideAbility
    api_object = api_objects["engine.ability.specialization.AnimationOverrideAbility"]

    set_type = api_objects["engine.aux.animation_override.AnimationOverride"]
    member = NyanMember("overrides", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.specialization.CommandSoundAbility
    api_object = api_objects["engine.ability.specialization.CommandSoundAbility"]

    set_type = api_objects["engine.aux.sound.Sound"]
    member = NyanMember("sounds", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.specialization.DiplomaticAbility
    api_object = api_objects["engine.ability.specialization.DiplomaticAbility"]

    set_type = api_objects["engine.aux.diplomatic_stance.DiplomaticStance"]
    member = NyanMember("stances", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.specialization.ExecutionSoundAbility
    api_object = api_objects["engine.ability.specialization.ExecutionSoundAbility"]

    set_type = api_objects["engine.aux.sound.Sound"]
    member = NyanMember("sounds", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.ActiveTransformTo
    api_object = api_objects["engine.ability.type.ActiveTransformTo"]

    ref_object = api_objects["engine.aux.state_machine.StateChanger"]
    member = NyanMember("target_state", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("transform_time", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.progress.type.TransformProgress"]
    member = NyanMember("transform_progress", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.ApplyContinuousEffect
    api_object = api_objects["engine.ability.type.ApplyContinuousEffect"]

    set_type = api_objects["engine.effect.continuous.ContinuousEffect"]
    member = NyanMember("effects", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    member = NyanMember("application_delay", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity_type.GameEntityType"]
    member = NyanMember("allowed_types", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("blacklisted_game_entities", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.ApplyDiscreteEffect
    api_object = api_objects["engine.ability.type.ApplyDiscreteEffect"]

    set_type = api_objects["engine.effect.discrete.DiscreteEffect"]
    member = NyanMember("effects", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    member = NyanMember("reload_time", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("application_delay", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity_type.GameEntityType"]
    member = NyanMember("allowed_types", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("blacklisted_game_entities", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.AttributeChangeTracker
    api_object = api_objects["engine.ability.type.AttributeChangeTracker"]

    ref_object = api_objects["engine.aux.attribute.Attribute"]
    member = NyanMember("attribute", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.progress.type.AttributeChangeProgress"]
    member = NyanMember("change_progress", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.Cloak
    api_object = api_objects["engine.ability.type.Cloak"]

    set_type = api_objects["engine.ability.Ability"]
    member = NyanMember("interrupted_by", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    member = NyanMember("interrupt_cooldown", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.ability.type.CollectStorage
    api_object = api_objects["engine.ability.type.CollectStorage"]

    ref_object = api_objects["engine.aux.storage.Container"]
    member = NyanMember("container", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("storage_elements", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.Constructable
    api_object = api_objects["engine.ability.type.Constructable"]

    member = NyanMember("starting_progress", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.progress.type.ConstructionProgress"]
    member = NyanMember("construction_progress", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.Create
    api_object = api_objects["engine.ability.type.Create"]

    set_type = api_objects["engine.aux.create.CreatableGameEntity"]
    member = NyanMember("creatables", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.DepositResources
    api_object = api_objects["engine.ability.type.DepositResources"]

    member = NyanMember("search_range", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity_type.GameEntityType"]
    member = NyanMember("allowed_types", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("blacklisted_game_entities", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.Despawn
    api_object = api_objects["engine.ability.type.Despawn"]

    set_type = api_objects["engine.aux.boolean.Clause"]
    member = NyanMember("activation_condition", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.boolean.Clause"]
    member = NyanMember("despawn_condition", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    member = NyanMember("despawn_time", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.state_machine.StateChanger"]
    member = NyanMember("state_change", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.ability.type.DropSite
    api_object = api_objects["engine.ability.type.DropSite"]

    set_type = api_objects["engine.aux.resource.Resource"]
    member = NyanMember("accepts", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.EnterContainer
    api_object = api_objects["engine.ability.type.EnterContainer"]

    set_type = api_objects["engine.aux.storage.Container"]
    member = NyanMember("allowed_containers", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity_type.GameEntityType"]
    member = NyanMember("allowed_types", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("blacklisted_game_entities", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.ExchangeResources
    api_object = api_objects["engine.ability.type.ExchangeResources"]

    set_type = api_objects["engine.aux.resource.ResourceAmount"]
    member = NyanMember("source_resources", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.resource.ResourceAmount"]
    member = NyanMember("target_resources", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    member = NyanMember("source_fee", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("target_fee", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.exchange_mode.ExchangeMode"]
    member = NyanMember("exchange_mode", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.ExitContainer
    api_object = api_objects["engine.ability.type.ExitContainer"]

    set_type = api_objects["engine.aux.storage.Container"]
    member = NyanMember("allowed_containers", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.Fly
    api_object = api_objects["engine.ability.type.Fly"]

    member = NyanMember("height", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.ability.type.FormFormation
    api_object = api_objects["engine.ability.type.FormFormation"]

    set_type = api_objects["engine.aux.game_entity_formation.GameEntityFormation"]
    member = NyanMember("formations", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.Foundation
    api_object = api_objects["engine.ability.type.Foundation"]

    ref_object = api_objects["engine.aux.terrain.Terrain"]
    member = NyanMember("foundation_terrain", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("flatten_ground", MemberType.BOOLEAN, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.ability.type.GameEntityStance
    api_object = api_objects["engine.ability.type.GameEntityStance"]

    set_type = api_objects["engine.aux.game_entity_stance.GameEntityStance"]
    member = NyanMember("stances", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.Gather
    api_object = api_objects["engine.ability.type.Gather"]

    member = NyanMember("auto_resume", MemberType.BOOLEAN, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("resume_search_range", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.resource_spot.ResourceSpot"]
    member = NyanMember("targets", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    member = NyanMember("carry_capacity", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.resource.ResourceRate"]
    member = NyanMember("gather_rate", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.progress.type.CarryProgress"]
    member = NyanMember("carry_progress", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.Harvestable
    api_object = api_objects["engine.ability.type.Harvestable"]

    ref_object = api_objects["engine.aux.resource_spot.ResourceSpot"]
    member = NyanMember("resources", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.progress.type.HarvestProgress"]
    member = NyanMember("harvest_progress", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.progress.type.RestockProgress"]
    member = NyanMember("restock_progress", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    member = NyanMember("gatherer_limit", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("harvestable_by_default", MemberType.BOOLEAN, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.ability.type.Herd
    api_object = api_objects["engine.ability.type.Herd"]

    member = NyanMember("range", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity_type.GameEntityType"]
    member = NyanMember("allowed_types", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("blacklisted_game_entities", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.Herdable
    api_object = api_objects["engine.ability.type.Herdable"]

    member = NyanMember("adjacent_discover_range", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.herdable_mode.HerdableMode"]
    member = NyanMember("herdable_mode", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.ability.type.LineOfSight
    api_object = api_objects["engine.ability.type.LineOfSight"]

    member = NyanMember("range", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.ability.type.Live
    api_object = api_objects["engine.ability.type.Live"]

    set_type = api_objects["engine.aux.attribute.AttributeSetting"]
    member = NyanMember("attributes", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.Move
    api_object = api_objects["engine.ability.type.Move"]

    member = NyanMember("speed", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.move_mode.MoveMode"]
    member = NyanMember("modes", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.Named
    api_object = api_objects["engine.ability.type.Named"]

    ref_object = api_objects["engine.aux.translated.type.TranslatedString"]
    member = NyanMember("name", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.translated.type.TranslatedMarkupFile"]
    member = NyanMember("description", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.translated.type.TranslatedMarkupFile"]
    member = NyanMember("long_description", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.ability.type.Passable
    api_object = api_objects["engine.ability.type.Passable"]

    ref_object = api_objects["engine.aux.hitbox.Hitbox"]
    member = NyanMember("hitbox", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.passable_mode.PassableMode"]
    member = NyanMember("mode", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.ability.type.PassiveTransformTo
    api_object = api_objects["engine.ability.type.PassiveTransformTo"]

    set_type = api_objects["engine.aux.boolean.Clause"]
    member = NyanMember("death_condition", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    member = NyanMember("death_time", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.state_machine.StateChanger"]
    member = NyanMember("state_change", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.ability.type.Projectile
    api_object = api_objects["engine.ability.type.Projectile"]

    member = NyanMember("arc", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.accuracy.Accuracy"]
    member = NyanMember("accuracy", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.target_mode.TargetMode"]
    member = NyanMember("target_mode", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity_type.GameEntityType"]
    member = NyanMember("ignored_types", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("unignore_entities", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.ProvideContingent
    api_object = api_objects["engine.ability.type.ProvideContingent"]

    set_type = api_objects["engine.aux.resource.ResourceAmount"]
    member = NyanMember("amount", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.RallyPoint
    api_object = api_objects["engine.ability.type.RallyPoint"]

    ref_object = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("indicator", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.ability.type.RangedContinuousEffect
    api_object = api_objects["engine.ability.type.RangedContinuousEffect"]

    member = NyanMember("min_range", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("max_range", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.ability.type.RangedDiscreteEffect
    api_object = api_objects["engine.ability.type.RangedDiscreteEffect"]

    member = NyanMember("min_range", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("max_range", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.ability.type.RegenerateAttribute
    api_object = api_objects["engine.ability.type.RegenerateAttribute"]

    ref_object = api_objects["engine.aux.attribute.AttributeRate"]
    member = NyanMember("rate", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.ability.type.RegenerateResourceSpot
    api_object = api_objects["engine.ability.type.RegenerateResourceSpot"]

    ref_object = api_objects["engine.aux.resource.ResourceRate"]
    member = NyanMember("rate", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.resource_spot.ResourceSpot"]
    member = NyanMember("resource_spot", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.ability.type.RemoveStorage
    api_object = api_objects["engine.ability.type.RemoveStorage"]

    ref_object = api_objects["engine.aux.storage.Container"]
    member = NyanMember("container", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("storage_elements", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.Research
    api_object = api_objects["engine.ability.type.Research"]

    set_type = api_objects["engine.aux.research.ResearchableTech"]
    member = NyanMember("researchables", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.Resistance
    api_object = api_objects["engine.ability.type.Resistance"]

    set_type = api_objects["engine.resistance.Resistance"]
    member = NyanMember("resistances", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.Restock
    api_object = api_objects["engine.ability.type.Restock"]

    member = NyanMember("auto_restock", MemberType.BOOLEAN, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.resource_spot.ResourceSpot"]
    member = NyanMember("auto_restock", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("restock_time", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.cost.Cost"]
    member = NyanMember("manual_cost", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.cost.Cost"]
    member = NyanMember("auto_cost", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("amount", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.ability.type.SendBackToTask
    api_object = api_objects["engine.ability.type.SendBackToTask"]

    set_type = api_objects["engine.aux.game_entity_type.GameEntityType"]
    member = NyanMember("allowed_types", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("blacklisted_game_entities", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.Selectable
    api_object = api_objects["engine.ability.type.Selectable"]

    ref_object = api_objects["engine.aux.selection_box.SelectionBox"]
    member = NyanMember("selection_box", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.ability.type.ShootProjectile
    api_object = api_objects["engine.ability.type.ShootProjectile"]

    set_type = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("projectiles", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    member = NyanMember("min_projectiles", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("max_projectiles", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("min_range", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("max_range", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("reload_time", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("spawn_delay", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("projectile_delay", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("require_turning", MemberType.BOOLEAN, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("manual_aiming_allowed", MemberType.BOOLEAN, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("spawning_area_offset_x", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("spawning_area_offset_y", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("spawning_area_offset_z", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("spawning_area_width", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("spawning_area_height", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("spawning_area_randomness", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity_type.GameEntityType"]
    member = NyanMember("allowed_types", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("blacklisted_game_entities", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.Storage
    api_object = api_objects["engine.ability.type.Storage"]

    ref_object = api_objects["engine.aux.storage.Container"]
    member = NyanMember("container", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.attribute.AttributeAmount"]
    member = NyanMember("empty_threshold", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.ability.type.TerrainRequirement
    api_object = api_objects["engine.ability.type.TerrainRequirement"]

    set_type = api_objects["engine.aux.terrain_type.TerrainType"]
    member = NyanMember("allowed_types", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.terrain.Terrain"]
    member = NyanMember("blacklisted_terrains", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.Trade
    api_object = api_objects["engine.ability.type.Trade"]

    set_type = api_objects["engine.aux.trade_route.TradeRoute"]
    member = NyanMember("trade_routes", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.TradePost
    api_object = api_objects["engine.ability.type.TradePost"]

    set_type = api_objects["engine.aux.trade_route.TradeRoute"]
    member = NyanMember("trade_routes", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.Transform
    api_object = api_objects["engine.ability.type.Transform"]

    ref_object = api_objects["engine.aux.transform_pool.TransformPool"]
    member = NyanMember("transform_pool", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.state_machine.StateChanger"]
    member = NyanMember("states", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.state_machine.StateChanger"]
    member = NyanMember("initial_state", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.ability.type.Turn
    api_object = api_objects["engine.ability.type.Turn"]

    member = NyanMember("turn_speed", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.ability.type.UseContingent
    api_object = api_objects["engine.ability.type.UseContingent"]

    set_type = api_objects["engine.aux.resource.ResourceAmount"]
    member = NyanMember("amount", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.ability.type.Visibility
    api_object = api_objects["engine.ability.type.Visibility"]

    member = NyanMember("visible_in_fog", MemberType.BOOLEAN, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux
    # engine.aux.accuracy.Accuracy
    api_object = api_objects["engine.aux.accuracy.Accuracy"]

    member = NyanMember("accuracy", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("accuracy_dispersion", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.dropoff_type.DropoffType"]
    member = NyanMember("dispersion_dropoff", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity_type.GameEntityType"]
    member = NyanMember("target_types", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("blacklisted_entities", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.aux.animation_override.AnimationOverride
    api_object = api_objects["engine.aux.animation_override.AnimationOverride"]

    ref_object = api_objects["engine.ability.Ability"]
    member = NyanMember("ability", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.graphics.Animation"]
    member = NyanMember("animations", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    member = NyanMember("priority", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.attribute.Attribute
    api_object = api_objects["engine.aux.attribute.Attribute"]

    ref_object = api_objects["engine.aux.translated.type.TranslatedString"]
    member = NyanMember("name", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.translated.type.TranslatedString"]
    member = NyanMember("abbreviation", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.attribute.AttributeAmount
    api_object = api_objects["engine.aux.attribute.AttributeAmount"]

    ref_object = api_objects["engine.aux.attribute.Attribute"]
    member = NyanMember("type", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("amount", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.attribute.AttributeRate
    api_object = api_objects["engine.aux.attribute.AttributeRate"]

    ref_object = api_objects["engine.aux.attribute.Attribute"]
    member = NyanMember("type", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("rate", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.attribute.AttributeSetting
    api_object = api_objects["engine.aux.attribute.AttributeSetting"]

    ref_object = api_objects["engine.aux.attribute.Attribute"]
    member = NyanMember("attribute", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("min_value", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("max_value", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("starting_value", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.attribute.ProtectingAttribute
    api_object = api_objects["engine.aux.attribute.ProtectingAttribute"]

    ref_object = api_objects["engine.aux.attribute.Attribute"]
    member = NyanMember("protects", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.boolean.Clause
    api_object = api_objects["engine.aux.boolean.Clause"]

    ref_object = api_objects["engine.aux.boolean.requirement_mode.RequirementMode"]
    member = NyanMember("clause_requirement", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.boolean.Literal"]
    member = NyanMember("literals", MemberType.ORDEREDSET, None, None, 0, set_type, False)
    api_object.add_member(member)
    member = NyanMember("only_once", MemberType.BOOLEAN, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.boolean.Literal
    api_object = api_objects["engine.aux.boolean.Literal"]

    ref_object = api_objects["engine.aux.boolean.literal_scope.LiteralScope"]
    member = NyanMember("scope", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.cheat.Cheat
    api_object = api_objects["engine.aux.cheat.Cheat"]

    member = NyanMember("activation_message", MemberType.TEXT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("display_message", MemberType.TEXT, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.patch.Patch"]
    member = NyanMember("changes", MemberType.ORDEREDSET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.aux.civilization.Civilization
    api_object = api_objects["engine.aux.civilization.Civilization"]

    ref_object = api_objects["engine.aux.translated.type.TranslatedString"]
    member = NyanMember("name", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.translated.type.TranslatedMarkupFile"]
    member = NyanMember("description", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.translated.type.TranslatedMarkupFile"]
    member = NyanMember("long_description", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.translated.type.TranslatedString"]
    member = NyanMember("leader_names", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.modifier.Modifier"]
    member = NyanMember("modifiers", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.resource.ResourceAmount"]
    member = NyanMember("starting_resources", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.patch.Patch"]
    member = NyanMember("civ_setup", MemberType.ORDEREDSET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.aux.cost.Cost
    api_object = api_objects["engine.aux.cost.Cost"]

    ref_object = api_objects["engine.aux.payment_mode.PaymentMode"]
    member = NyanMember("payment_mode", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.cost.type.AttributeCost
    api_object = api_objects["engine.aux.cost.type.AttributeCost"]

    set_type = api_objects["engine.aux.attribute.AttributeAmount"]
    member = NyanMember("amount", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.aux.cost.type.ResourceCost
    api_object = api_objects["engine.aux.cost.type.ResourceCost"]

    set_type = api_objects["engine.aux.resource.ResourceAmount"]
    member = NyanMember("amount", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.aux.create.CreatableGameEntity
    api_object = api_objects["engine.aux.create.CreatableGameEntity"]

    ref_object = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("game_entity", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.cost.Cost"]
    member = NyanMember("cost", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("creation_time", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.sound.Sound"]
    member = NyanMember("creation_sound", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.boolean.Clause"]
    member = NyanMember("condition", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.placement_mode.PlacementMode"]
    member = NyanMember("placement_modes", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.aux.exchange_mode.volatile.Volatile
    api_object = api_objects["engine.aux.exchange_mode.volatile.Volatile"]

    member = NyanMember("source_min_amount", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("source_max_amount", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("target_min_amount", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("target_max_amount", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.exchange_scope.ExchangeScope"]
    member = NyanMember("scope", ref_object, None, None, 0, None, True)
    api_object.add_member(member)

    # engine.aux.exchange_mode.volatile.VolatileFlat
    api_object = api_objects["engine.aux.exchange_mode.volatile.Volatile"]

    member = NyanMember("change_source_amount", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("change_target_amount", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.formation.Formation
    api_object = api_objects["engine.aux.formation.Formation"]

    set_type = api_objects["engine.aux.formation.Subformation"]
    member = NyanMember("subformations", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.aux.formation.PrecedingSubformation
    api_object = api_objects["engine.aux.formation.PrecedingSubformation"]

    ref_object = api_objects["engine.aux.formation.Subformation"]
    member = NyanMember("precedes", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.game_entity.GameEntity
    api_object = api_objects["engine.aux.game_entity.GameEntity"]

    set_type = api_objects["engine.aux.game_entity_type.GameEntityType"]
    member = NyanMember("types", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.variant.Variant"]
    member = NyanMember("variants", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.ability.Ability"]
    member = NyanMember("abilities", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.modifier.Modifier"]
    member = NyanMember("modifiers", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.aux.game_entity_formation.GameEntityFormation
    api_object = api_objects["engine.aux.game_entity_formation.GameEntityFormation"]

    ref_object = api_objects["engine.aux.formation.Formation"]
    member = NyanMember("formation", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.formation.Subformation"]
    member = NyanMember("subformation", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.game_entity_stance.GameEntityStance
    api_object = api_objects["engine.aux.game_entity_stance.GameEntityStance"]

    set_type = api_objects["engine.ability.Ability"]
    member = NyanMember("ability_preference", MemberType.ORDEREDSET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity_type.GameEntityType"]
    member = NyanMember("type_preference", MemberType.ORDEREDSET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.aux.graphics.Animation
    api_object = api_objects["engine.aux.graphics.Animation"]

    member = NyanMember("sprite", MemberType.FILE, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.graphics.Terrain
    api_object = api_objects["engine.aux.graphics.Terrain"]

    member = NyanMember("sprite", MemberType.FILE, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.language.Language
    api_object = api_objects["engine.aux.language.Language"]

    member = NyanMember("ietf_string", MemberType.TEXT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.language.LanguageMarkupPair
    api_object = api_objects["engine.aux.language.LanguageMarkupPair"]

    ref_object = api_objects["engine.aux.language.Language"]
    member = NyanMember("language", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("markup_file", MemberType.FILE, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.language.LanguageSoundPair
    api_object = api_objects["engine.aux.language.LanguageSoundPair"]

    ref_object = api_objects["engine.aux.language.Language"]
    member = NyanMember("language", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.sound.Sound"]
    member = NyanMember("sound", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.language.LanguageTextPair
    api_object = api_objects["engine.aux.language.LanguageTextPair"]

    ref_object = api_objects["engine.aux.language.Language"]
    member = NyanMember("language", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("string", MemberType.TEXT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.mod.Mod
    api_object = api_objects["engine.aux.mod.Mod"]

    member = NyanMember("priority", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.patch.Patch"]
    member = NyanMember("patches", MemberType.ORDEREDSET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.aux.modifier_scope.type.GameEntityScope
    api_object = api_objects["engine.aux.modifier_scope.type.GameEntityScope"]

    set_type = api_objects["engine.aux.game_entity_type.GameEntityType"]
    member = NyanMember("affected_types", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("blacklisted_game_entities", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.aux.move_mode.type.Follow
    api_object = api_objects["engine.aux.move_mode.type.Follow"]

    member = NyanMember("range", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.patch.type.DiplomaticPatch
    api_object = api_objects["engine.aux.patch.type.DiplomaticPatch"]

    set_type = api_objects["engine.aux.diplomatic_stance.DiplomaticStance"]
    member = NyanMember("stances", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.aux.placement_mode.type.Place
    api_object = api_objects["engine.aux.placement_mode.type.Place"]

    member = NyanMember("snap_to_tiles", MemberType.BOOLEAN, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.progress.Progress
    api_object = api_objects["engine.aux.progress.Progress"]

    member = NyanMember("progress", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.progress.specialization.AnimatedProgress
    api_object = api_objects["engine.aux.progress.specialization.AnimatedProgress"]

    ref_object = api_objects["engine.aux.animation_override.AnimationOverride"]
    member = NyanMember("progress_sprite", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.progress.specialization.StateChangeProgress
    api_object = api_objects["engine.aux.progress.specialization.StateChangeProgress"]

    ref_object = api_objects["engine.aux.state_machine.StateChanger"]
    member = NyanMember("state_change", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.progress.specialization.TerrainOverlayProgress
    api_object = api_objects["engine.aux.progress.specialization.TerrainOverlayProgress"]

    ref_object = api_objects["engine.aux.terrain.Terrain"]
    member = NyanMember("terrain_overlay", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.progress.specialization.TerrainProgress
    api_object = api_objects["engine.aux.progress.specialization.TerrainProgress"]

    ref_object = api_objects["engine.aux.terrain.Terrain"]
    member = NyanMember("terrain", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.progress_status.ProgressStatus
    api_object = api_objects["engine.aux.progress_status.ProgressStatus"]

    ref_object = api_objects["engine.aux.progress_type.ProgressType"]
    member = NyanMember("progress_type", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("progress", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.research.ResearchableTech
    api_object = api_objects["engine.aux.research.ResearchableTech"]

    ref_object = api_objects["engine.aux.tech.Tech"]
    member = NyanMember("tech", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.cost.Cost"]
    member = NyanMember("cost", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("research_time", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.sound.Sound"]
    member = NyanMember("research_sound", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.boolean.Clause"]
    member = NyanMember("condition", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.aux.resource.Resource
    api_object = api_objects["engine.aux.resource.Resource"]

    ref_object = api_objects["engine.aux.translated.type.TranslatedString"]
    member = NyanMember("name", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("max_storage", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.resource.ResourceContingent
    api_object = api_objects["engine.aux.resource.ResourceContingent"]

    member = NyanMember("min_amount", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("max_amount", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.resource.ResourceAmount
    api_object = api_objects["engine.aux.resource.ResourceAmount"]

    ref_object = api_objects["engine.aux.resource.Resource"]
    member = NyanMember("type", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("amount", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.resource.ResourceRate
    api_object = api_objects["engine.aux.resource.ResourceRate"]

    ref_object = api_objects["engine.aux.resource.Resource"]
    member = NyanMember("type", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("rate", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.resource_spot.ResourceSpot
    api_object = api_objects["engine.aux.resource_spot.ResourceSpot"]

    ref_object = api_objects["engine.aux.resource.Resource"]
    member = NyanMember("resource", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("max_amount", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("starting_amount", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("decay_rate", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.sound.Sound
    api_object = api_objects["engine.aux.sound.Sound"]

    member = NyanMember("play_delay", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = MemberType.FILE
    member = NyanMember("sounds", MemberType.ORDEREDSET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.aux.state_machine.StateChanger
    api_object = api_objects["engine.aux.state_machine.StateChanger"]

    set_type = api_objects["engine.ability.Ability"]
    member = NyanMember("enable_abilities", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.ability.Ability"]
    member = NyanMember("disable_abilities", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.modifier.Modifier"]
    member = NyanMember("enable_modifiers", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.modifier.Modifier"]
    member = NyanMember("disable_modifiers", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    member = NyanMember("priority", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.storage.Container
    api_object = api_objects["engine.aux.storage.Container"]

    set_type = api_objects["engine.aux.storage.StorageElement"]
    member = NyanMember("storage_elements", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    member = NyanMember("slots", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.progress.type.CarryProgress"]
    member = NyanMember("carry_progress", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.aux.storage.StorageElement
    api_object = api_objects["engine.aux.storage.StorageElement"]

    ref_object = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("storage_element", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("elements_per_slot", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.storage.StorageElement"]
    member = NyanMember("conflicts", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.state_machine.StateChanger"]
    member = NyanMember("state_change", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.taunt.Taunt
    api_object = api_objects["engine.aux.taunt.Taunt"]

    member = NyanMember("activation_message", MemberType.TEXT, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.translated.type.TranslatedString"]
    member = NyanMember("display_message", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.sound.Sound"]
    member = NyanMember("sound", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.tech.Tech
    api_object = api_objects["engine.aux.tech.Tech"]

    ref_object = api_objects["engine.aux.translated.type.TranslatedString"]
    member = NyanMember("name", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.translated.type.TranslatedMarkupFile"]
    member = NyanMember("description", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.translated.type.TranslatedMarkupFile"]
    member = NyanMember("long_description", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.patch.Patch"]
    member = NyanMember("updates", MemberType.ORDEREDSET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.aux.terrain.Terrain
    api_object = api_objects["engine.aux.terrain.Terrain"]

    set_type = api_objects["engine.aux.terrain_type.TerrainType"]
    member = NyanMember("types", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.translated.type.TranslatedString"]
    member = NyanMember("name", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.graphics.Terrain"]
    member = NyanMember("terrain_graphic", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.sound.Sound"]
    member = NyanMember("sound", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.terrain.TerrainAmbient"]
    member = NyanMember("ambience", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.aux.terrain.TerrainAmbient
    api_object = api_objects["engine.aux.terrain.TerrainAmbient"]

    ref_object = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("object", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("max_density", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.trade_route.TradeRoute
    api_object = api_objects["engine.aux.trade_route.TradeRoute"]

    ref_object = api_objects["engine.aux.resource.Resource"]
    member = NyanMember("trade_resource", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("start_trade_post", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("end_trade_post", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.trade_route.type.AoE1TradeRoute
    api_object = api_objects["engine.aux.trade_route.type.AoE1TradeRoute"]

    set_type = api_objects["engine.aux.resource.Resource"]
    member = NyanMember("exchange_resources", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    member = NyanMember("trade_amount", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.translated.type.TranslatedMarkupFile
    api_object = api_objects["engine.aux.translated.type.TranslatedMarkupFile"]

    set_type = api_objects["engine.aux.language.LanguageMarkupPair"]
    member = NyanMember("translations", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.aux.translated.type.TranslatedSound
    api_object = api_objects["engine.aux.translated.type.TranslatedSound"]

    set_type = api_objects["engine.aux.language.LanguageSoundPair"]
    member = NyanMember("translations", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.aux.translated.type.TranslatedString
    api_object = api_objects["engine.aux.translated.type.TranslatedString"]

    set_type = api_objects["engine.aux.language.LanguageTextPair"]
    member = NyanMember("translations", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.aux.variant.Variant
    api_object = api_objects["engine.aux.variant.Variant"]

    set_type = api_objects["engine.aux.patch.Patch"]
    member = NyanMember("changes", MemberType.ORDEREDSET, None, None, 0, set_type, False)
    api_object.add_member(member)
    member = NyanMember("priority", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.variant.type.AdjacentTilesVariant
    api_object = api_objects["engine.aux.variant.type.AdjacentTilesVariant"]

    ref_object = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("north", ref_object, None, None, 0, None, True)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("north_east", ref_object, None, None, 0, None, True)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("east", ref_object, None, None, 0, None, True)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("south_east", ref_object, None, None, 0, None, True)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("south", ref_object, None, None, 0, None, True)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("south_west", ref_object, None, None, 0, None, True)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("west", ref_object, None, None, 0, None, True)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("north_west", ref_object, None, None, 0, None, True)
    api_object.add_member(member)

    # engine.aux.variant.type.RandomVariant
    api_object = api_objects["engine.aux.variant.type.RandomVariant"]

    member = NyanMember("chance_share", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.aux.variant.type.PerspectiveVariant
    api_object = api_objects["engine.aux.variant.type.PerspectiveVariant"]

    member = NyanMember("angle", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.effect
    # engine.effect.continuous.flat_attribute_change.FlatAttributeChange
    api_object = api_objects["engine.effect.continuous.flat_attribute_change.FlatAttributeChange"]

    ref_object = api_objects["engine.aux.attribute_change_type.AttributeChangeType"]
    member = NyanMember("type", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.attribute.AttributeRate"]
    member = NyanMember("min_change_rate", ref_object, None, None, 0, None, True)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.attribute.AttributeRate"]
    member = NyanMember("max_change_rate", ref_object, None, None, 0, None, True)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.attribute.AttributeRate"]
    member = NyanMember("change_rate", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.attribute.ProtectingAttribute"]
    member = NyanMember("ignore_protection", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.effect.continuous.type.Lure
    api_object = api_objects["engine.effect.continuous.type.Lure"]

    ref_object = api_objects["engine.aux.lure_type.LureType"]
    member = NyanMember("type", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("destination", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    member = NyanMember("min_distance_to_destination", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.effect.continuous.time_relative_attribute_change.TimeRelativeAttributeChange
    api_object = api_objects["engine.effect.continuous.time_relative_attribute_change.TimeRelativeAttributeChange"]

    ref_object = api_objects["engine.aux.attribute_change_type.AttributeChangeType"]
    member = NyanMember("type", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("total_change_time", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.attribute.ProtectingAttribute"]
    member = NyanMember("ignore_protection", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.effect.continuous.time_relative_progress_change.TimeRelativeProgressChange
    api_object = api_objects["engine.effect.continuous.time_relative_progress_change.TimeRelativeProgressChange"]

    ref_object = api_objects["engine.aux.progress_type.ProgressType"]
    member = NyanMember("type", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("total_change_time", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.effect.discrete.convert.Convert
    api_object = api_objects["engine.effect.discrete.convert.Convert"]

    ref_object = api_objects["engine.aux.convert_type.ConvertType"]
    member = NyanMember("type", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("min_chance_success", MemberType.FLOAT, None, None, 0, None, True)
    api_object.add_member(member)
    member = NyanMember("max_chance_success", MemberType.FLOAT, None, None, 0, None, True)
    api_object.add_member(member)
    member = NyanMember("chance_success", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.cost.Cost"]
    member = NyanMember("cost_fail", ref_object, None, None, 0, None, True)
    api_object.add_member(member)

    # engine.effect.discrete.convert.type.AoE2Convert
    api_object = api_objects["engine.effect.discrete.convert.type.AoE2Convert"]

    member = NyanMember("skip_guaranteed_rounds", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("skip_protected_rounds", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.effect.discrete.flat_attribute_change.FlatAttributeChange
    api_object = api_objects["engine.effect.discrete.flat_attribute_change.FlatAttributeChange"]

    ref_object = api_objects["engine.aux.attribute_change_type.AttributeChangeType"]
    member = NyanMember("type", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.attribute.AttributeAmount"]
    member = NyanMember("min_change_value", ref_object, None, None, 0, None, True)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.attribute.AttributeAmount"]
    member = NyanMember("max_change_value", ref_object, None, None, 0, None, True)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.attribute.AttributeAmount"]
    member = NyanMember("change_value", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.attribute.ProtectingAttribute"]
    member = NyanMember("ignore_protection", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.effect.discrete.type.MakeHarvestable
    api_object = api_objects["engine.effect.discrete.type.MakeHarvestable"]

    ref_object = api_objects["engine.aux.resource_spot.ResourceSpot"]
    member = NyanMember("resource_spot", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.effect.discrete.type.SendToContainer
    api_object = api_objects["engine.effect.discrete.type.SendToContainer"]

    ref_object = api_objects["engine.aux.container_type.SendToContainerType"]
    member = NyanMember("type", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.storage.Container"]
    member = NyanMember("storages", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.effect.specialization.AreaEffect
    api_object = api_objects["engine.effect.specialization.AreaEffect"]

    member = NyanMember("range", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.dropoff_type.DropoffType"]
    member = NyanMember("dropoff", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.effect.specialization.CostEffect
    api_object = api_objects["engine.effect.specialization.CostEffect"]

    ref_object = api_objects["engine.aux.cost.Cost"]
    member = NyanMember("cost", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.effect.specialization.DiplomaticEffect
    api_object = api_objects["engine.effect.specialization.DiplomaticEffect"]

    set_type = api_objects["engine.aux.diplomatic_stance.DiplomaticStance"]
    member = NyanMember("stances", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.resistance
    # engine.resistance.continuous.flat_attribute_change.FlatAttributeChange
    api_object = api_objects["engine.resistance.continuous.flat_attribute_change.FlatAttributeChange"]

    ref_object = api_objects["engine.aux.attribute_change_type.AttributeChangeType"]
    member = NyanMember("type", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.attribute.AttributeRate"]
    member = NyanMember("block_rate", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.resistance.continuous.type.Lure
    api_object = api_objects["engine.resistance.continuous.type.Lure"]

    ref_object = api_objects["engine.aux.lure_type.LureType"]
    member = NyanMember("type", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.resistance.continuous.time_relative_attribute_change.TimeRelativeAttributeChange
    api_object = api_objects["engine.resistance.continuous.time_relative_attribute_change.TimeRelativeAttributeChange"]

    ref_object = api_objects["engine.aux.attribute_change_type.AttributeChangeType"]
    member = NyanMember("type", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.resistance.continuous.time_relative_progress.TimeRelativeProgress
    api_object = api_objects["engine.resistance.continuous.time_relative_progress_change.TimeRelativeProgressChange"]

    ref_object = api_objects["engine.aux.progress_type.ProgressType"]
    member = NyanMember("type", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.resistance.discrete.convert.Convert
    api_object = api_objects["engine.resistance.discrete.convert.Convert"]

    ref_object = api_objects["engine.aux.convert_type.ConvertType"]
    member = NyanMember("type", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("chance_resist", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.resistance.discrete.convert.type.AoE2Convert
    api_object = api_objects["engine.resistance.discrete.convert.type.AoE2Convert"]

    member = NyanMember("guaranteed_resist_rounds", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("protected_rounds", MemberType.INT, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("protection_round_recharge_time", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.resistance.discrete.flat_attribute_change.FlatAttributeChange
    api_object = api_objects["engine.resistance.discrete.flat_attribute_change.FlatAttributeChange"]

    ref_object = api_objects["engine.aux.attribute_change_type.AttributeChangeType"]
    member = NyanMember("type", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.aux.attribute.AttributeAmount"]
    member = NyanMember("block_value", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.resistance.discrete.type.MakeHarvestable
    api_object = api_objects["engine.resistance.discrete.type.MakeHarvestable"]

    ref_object = api_objects["engine.aux.resource_spot.ResourceSpot"]
    member = NyanMember("resource_spot", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.boolean.Clause"]
    member = NyanMember("harvest_conditions", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.resistance.discrete.type.SendToContainer
    api_object = api_objects["engine.resistance.discrete.type.SendToContainer"]

    ref_object = api_objects["engine.aux.container_type.SendToContainerType"]
    member = NyanMember("type", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    member = NyanMember("search_range", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.storage.Container"]
    member = NyanMember("ignore_containers", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.resistance.specialization.CostResistance
    api_object = api_objects["engine.resistance.specialization.CostResistance"]

    ref_object = api_objects["engine.aux.cost.Cost"]
    member = NyanMember("cost", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.modifier
    # engine.modifier.specialization.ScopeModifier
    api_object = api_objects["engine.modifier.specialization.ScopeModifier"]

    set_type = api_objects["engine.aux.diplomatic_stance.DiplomaticStance"]
    member = NyanMember("diplomatic_stances", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.modifier.multiplier.MultiplierModifier
    api_object = api_objects["engine.modifier.multiplier.MultiplierModifier"]

    member = NyanMember("multiplier", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.modifier.multiplier.effect.flat_attribute_change.type.ElevationDifferenceHigh
    api_object = api_objects["engine.modifier.multiplier.effect.flat_attribute_change.type.ElevationDifferenceHigh"]

    member = NyanMember("elevation_difference", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.modifier.multiplier.effect.flat_attribute_change.type.Flyover
    api_object = api_objects["engine.modifier.multiplier.effect.flat_attribute_change.type.Flyover"]

    set_type = api_objects["engine.aux.game_entity_type.GameEntityType"]
    member = NyanMember("flyover_types", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("blacklisted_game_entities", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.modifier.multiplier.effect.flat_attribute_change.type.Terrain
    api_object = api_objects["engine.modifier.multiplier.effect.flat_attribute_change.type.Terrain"]

    ref_object = api_objects["engine.aux.terrain.Terrain"]
    member = NyanMember("terrain", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.modifier.multiplier.resistance.flat_attribute_change.type.ElevationDifferenceLow
    api_object = api_objects["engine.modifier.multiplier.resistance.flat_attribute_change.type.ElevationDifferenceLow"]

    member = NyanMember("elevation_difference", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.modifier.multiplier.resistance.flat_attribute_change.type.Terrain
    api_object = api_objects["engine.modifier.multiplier.resistance.flat_attribute_change.type.Terrain"]

    ref_object = api_objects["engine.aux.terrain.Terrain"]
    member = NyanMember("terrain", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.modifier.multiplier.type.AttributeSettingsValue
    api_object = api_objects["engine.modifier.multiplier.type.AttributeSettingsValue"]

    ref_object = api_objects["engine.aux.attribute.Attribute"]
    member = NyanMember("attribute", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.modifier.multiplier.type.ContainerCapacity
    api_object = api_objects["engine.modifier.multiplier.type.ContainerCapacity"]

    ref_object = api_objects["engine.aux.storage.Container"]
    member = NyanMember("container", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.modifier.multiplier.type.CreationAttributeCost
    api_object = api_objects["engine.modifier.multiplier.type.CreationAttributeCost"]

    set_type = api_objects["engine.aux.attribute.Attribute"]
    member = NyanMember("attributes", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.create.CreatableGameEntity"]
    member = NyanMember("creatables", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.modifier.multiplier.type.CreationResourceCost
    api_object = api_objects["engine.modifier.multiplier.type.CreationResourceCost"]

    set_type = api_objects["engine.aux.resource.Resource"]
    member = NyanMember("resources", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.create.CreatableGameEntity"]
    member = NyanMember("creatables", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.modifier.multiplier.type.CreationTime
    api_object = api_objects["engine.modifier.multiplier.type.CreationTime"]

    set_type = api_objects["engine.aux.create.CreatableGameEntity"]
    member = NyanMember("creatables", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.modifier.multiplier.type.GatheringEfficiency
    api_object = api_objects["engine.modifier.multiplier.type.GatheringEfficiency"]

    ref_object = api_objects["engine.aux.resource_spot.ResourceSpot"]
    member = NyanMember("resource_spot", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.modifier.multiplier.type.GatheringRate
    api_object = api_objects["engine.modifier.multiplier.type.GatheringRate"]

    ref_object = api_objects["engine.aux.resource_spot.ResourceSpot"]
    member = NyanMember("resource_spot", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.modifier.multiplier.type.ResearchAttributeCost
    api_object = api_objects["engine.modifier.multiplier.type.ResearchAttributeCost"]

    set_type = api_objects["engine.aux.attribute.Attribute"]
    member = NyanMember("attributes", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.research.ResearchableTech"]
    member = NyanMember("researchables", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.modifier.multiplier.type.ResearchResourceCost
    api_object = api_objects["engine.modifier.multiplier.type.ResearchResourceCost"]

    set_type = api_objects["engine.aux.resource.Resource"]
    member = NyanMember("resources", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.research.ResearchableTech"]
    member = NyanMember("researchables", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.modifier.multiplier.type.ResearchTime
    api_object = api_objects["engine.modifier.multiplier.type.ResearchTime"]

    set_type = api_objects["engine.aux.research.ResearchableTech"]
    member = NyanMember("researchables", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.modifier.multiplier.type.StorageElementCapacity
    api_object = api_objects["engine.modifier.multiplier.type.StorageElementCapacity"]

    ref_object = api_objects["engine.aux.storage.StorageElement"]
    member = NyanMember("storage_element", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.modifier.relative_projectile_amount.AoE2ProjectileAmount
    api_object = api_objects["engine.modifier.relative_projectile_amount.AoE2ProjectileAmount"]

    set_type = api_objects["engine.ability.type.ApplyDiscreteEffect"]
    member = NyanMember("provider_abilities", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.ability.type.ApplyDiscreteEffect"]
    member = NyanMember("receiver_abilities", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.attribute_change_type.AttributeChangeType"]
    member = NyanMember("change_types", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.modifier.type.AbsoluteProjectileAmount
    api_object = api_objects["engine.modifier.relative_projectile_amount.AoE2ProjectileAmount"]

    member = NyanMember("amount", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.modifier.type.ContinuousResource
    api_object = api_objects["engine.modifier.type.ContinuousResource"]

    set_type = api_objects["engine.aux.resource.ResourceRate"]
    member = NyanMember("rates", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.modifier.type.DepositResourcesOnProgress
    api_object = api_objects["engine.modifier.type.DepositResourcesOnProgress"]

    ref_object = api_objects["engine.aux.progress_status.ProgressStatus"]
    member = NyanMember("progress_status", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.resource.Resource"]
    member = NyanMember("resources", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity_type.GameEntityType"]
    member = NyanMember("affected_types", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("blacklisted_entities", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.modifier.type.DiplomaticLineOfSight
    api_object = api_objects["engine.modifier.type.DiplomaticLineOfSight"]

    ref_object = api_objects["engine.aux.diplomatic_stance.DiplomaticStance"]
    member = NyanMember("diplomatic_stance", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.modifier.type.InContainerContinuousEffect
    api_object = api_objects["engine.modifier.type.InContainerContinuousEffect"]

    set_type = api_objects["engine.aux.storage.Container"]
    member = NyanMember("containers", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.ability.type.ApplyContinuousEffect"]
    member = NyanMember("ability", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.modifier.type.InContainerDiscreteEffect
    api_object = api_objects["engine.modifier.type.InContainerDiscreteEffect"]

    set_type = api_objects["engine.aux.storage.Container"]
    member = NyanMember("containers", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    ref_object = api_objects["engine.ability.type.ApplyDiscreteEffect"]
    member = NyanMember("ability", ref_object, None, None, 0, None, False)
    api_object.add_member(member)

    # engine.modifier.type.InstantTechResearch
    api_object = api_objects["engine.modifier.type.InstantTechResearch"]

    ref_object = api_objects["engine.aux.tech.Tech"]
    member = NyanMember("tech", ref_object, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.boolean.Clause"]
    member = NyanMember("condition", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.modifier.type.RefundOnDeath
    api_object = api_objects["engine.modifier.type.RefundOnDeath"]

    set_type = api_objects["engine.aux.resource.ResourceAmount"]
    member = NyanMember("refund_amount", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

    # engine.modifier.type.Reveal
    api_object = api_objects["engine.modifier.type.Reveal"]

    member = NyanMember("line_of_sight", MemberType.FLOAT, None, None, 0, None, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity_type.GameEntityType"]
    member = NyanMember("affected_types", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)
    set_type = api_objects["engine.aux.game_entity.GameEntity"]
    member = NyanMember("blacklisted_entities", MemberType.SET, None, None, 0, set_type, False)
    api_object.add_member(member)

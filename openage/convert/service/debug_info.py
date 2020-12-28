# Copyright 2020-2020 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R
"""
Creates debug output from data in a conversion run.
"""
from openage.convert.entity_object.conversion.aoc.genie_tech import AgeUpgrade,\
    UnitLineUpgrade, BuildingLineUpgrade, UnitUnlock, BuildingUnlock
from openage.convert.entity_object.conversion.aoc.genie_unit import GenieUnitLineGroup,\
    GenieBuildingLineGroup, GenieStackBuildingGroup, GenieUnitTransformGroup,\
    GenieMonkGroup
from openage.convert.value_object.read.media.datfile.empiresdat import EmpiresDatWrapper
from openage.convert.value_object.read.read_members import IncludeMembers, MultisubtypeMember
from openage.util.fslike.filecollection import FileCollectionPath
from openage.util.fslike.path import Path


def debug_init(debugdir, args, loglevel):
    """
    Log the converter settings.
    """
    logfile = debugdir["args"]
    logtext = ""

    # Get CLI args
    arg_dict = {}
    for name, arg in vars(args).items():
        if name == "entrypoint":
            # args after entrypoint are not from CLI
            break

        arg_dict.update({name: arg})

    # Sort by name
    arg_dict = dict(sorted(arg_dict.items(), key=lambda item: item[0]))

    for name, arg in arg_dict.items():
        logtext += f"{name}: {arg}\n"

    with logfile.open("w") as log:
        log.write(logtext)

    # Log game version
    logfile = debugdir.joinpath("init/")["game_version"]
    logtext = ""

    logtext += (
        f"game edition:\n"
        f"    - {args.game_version[0]}\n"
    )

    if len(args.game_version[1]) > 0:
        logtext += "game expansions:\n"
        for expansion in args.game_version[1]:
            logtext += f"    - {expansion}\n"

    else:
        logtext += "game expansions: none detected"

    with logfile.open("w") as log:
        log.write(logtext)

    # Log mounts
    logfile = debugdir.joinpath("init/")["mounts"]
    logtext = ""

    mounts = args.srcdir.fsobj.obj.fsobj.mounts

    # Sort by mounted directory name
    mount_dict = {}
    for mount in mounts:
        if mount[0] in mount_dict.keys():
            mount_dict[mount[0]].append(mount[1])

        else:
            mount_dict[mount[0]] = [mount[1]]

    mount_dict = dict(sorted(mount_dict.items(), key=lambda item: item[0]))

    # Format mounts
    for mountpoint, resources in mount_dict.items():
        if len(mountpoint) == 0:
            logtext += f"mountpoint: ${{srcdir}}/\n"

        else:
            logtext += f"mountpoint: ${{srcdir}}/{mountpoint[0].decode()}/\n"

        for resource in resources:
            resource_type = None
            abs_path = ""
            file_count = 0

            if type(resource) is Path:
                resource_type = "dir"
                abs_path = resource.fsobj.path.decode()

            elif type(resource) is FileCollectionPath:
                resource_type = "file collection"
                abs_path = resource.fsobj.fileobj.name.decode()
                file_count = len(resource.fsobj.rootentries[0])

            logtext += f"    resource type: {resource_type}\n"
            logtext += f"    source path: {abs_path}\n"

            if resource_type == "file collection":
                logtext += f"    file count: {file_count}\n"

            logtext += f"    ----\n"

    with logfile.open("w") as log:
        log.write(logtext)


def debug_gamedata_format(debugdir, game_version, loglevel):
    """
    Logs the data format of a .dat file of a specific a game version.
    """
    logfile = debugdir.joinpath("read/")["data_format"]
    logtext = ""

    discovered_structs = {EmpiresDatWrapper}
    handled_structs = set()

    while discovered_structs:
        struct = discovered_structs.pop()

        if struct in handled_structs:
            continue

        members = struct.get_data_format_members(game_version)
        logtext += f"struct name: {struct.name_struct}\n"
        logtext += f"total member count: {len(members)}\n"

        max_name_width = 1
        max_vmemb_width = 1
        for member in members:
            # Find out width of columns for table formatting
            if len(str(member[1])) > max_name_width:
                max_name_width = len(str(member[1]))

            if len(str(member[2])) > max_vmemb_width:
                max_vmemb_width = len(str(member[2]))

            # Search for sub-structs
            if isinstance(member[3], IncludeMembers):
                discovered_structs.add(member[3].cls)

            elif isinstance(member[3], MultisubtypeMember):
                discovered_structs.update(member[3].class_lookup.values())

        for member in members:
            logtext += (
                f"{str(member[0].value):8}  "
                f"{str(member[1]):{max_name_width}}  "
                f"{str(member[2]):{max_vmemb_width}}  "
                f"{str(member[3])}\n"
            )

        handled_structs.add(struct)
        logtext += "\n"

    with logfile.open("w") as log:
        log.write(logtext)


def debug_string_resources(debugdir, string_resources, loglevel):
    """
    Create debug output for found string resources.
    """
    logfile = debugdir.joinpath("read/")["string_resources"]
    logtext = ""

    logtext += "found languages: "
    logtext += ", ".join(string_resources.get_tables().keys())
    logtext += "\n\n"

    for language, strings in string_resources.get_tables().items():
        logtext += f"{language}: {len(strings)} IDs\n"

    with logfile.open("w") as log:
        log.write(logtext)


def debug_registered_graphics(debugdir, existing_graphics, loglevel):
    """
    Create debug output for found graphics files.
    """
    logfile = debugdir.joinpath("read/")["existing_graphics"]
    logtext = ""

    logtext += f"file count: {len(existing_graphics)}\n\n"

    sorted_graphics = list(sorted(existing_graphics))
    logtext += "\n".join(sorted_graphics)

    with logfile.open("w") as log:
        log.write(logtext)


def debug_converter_objects(debugdir, dataset, loglevel):
    """
    Create debug output for ConverterObject instances from the
    conversion preprocessor.
    """
    logfile = debugdir.joinpath("conversion/")["preprocessor_objects"]
    logtext = ""

    logtext += (
        f"unit objects count: {len(dataset.genie_units)}\n"
        f"tech objects count: {len(dataset.genie_techs)}\n"
        f"civ objects count: {len(dataset.genie_civs)}\n"
        f"effect bundles count: {len(dataset.genie_effect_bundles)}\n"
        f"age connections count: {len(dataset.age_connections)}\n"
        f"building connections count: {len(dataset.building_connections)}\n"
        f"unit connections count: {len(dataset.unit_connections)}\n"
        f"tech connections count: {len(dataset.tech_connections)}\n"
        f"graphics objects count: {len(dataset.genie_graphics)}\n"
        f"sound objects count: {len(dataset.genie_sounds)}\n"
        f"terrain objects count: {len(dataset.genie_terrains)}\n"
    )

    with logfile.open("w") as log:
        log.write(logtext)


def debug_converter_object_groups(debugdir, dataset, loglevel):
    """
    Create debug output for ConverterObjectGroup instances from the
    conversion preprocessor.
    """
    enitity_groups = {}
    enitity_groups.update(dataset.unit_lines)
    enitity_groups.update(dataset.building_lines)
    enitity_groups.update(dataset.ambient_groups)
    enitity_groups.update(dataset.variant_groups)

    for key, line in enitity_groups.items():
        logfile = debugdir.joinpath("conversion/entity_groups/")[str(key)]
        logtext = ""

        logtext += f"repr: {line}\n"

        logtext += f"is_creatable: {line.is_creatable()}\n"
        logtext += f"is_harvestable: {line.is_harvestable()}\n"
        logtext += f"is_garrison: {line.is_garrison()}\n"
        logtext += f"is_gatherer: {line.is_gatherer()}\n"
        logtext += f"is_passable: {line.is_passable()}\n"
        logtext += f"is_projectile_shooter: {line.is_projectile_shooter()}\n"
        logtext += f"is_ranged: {line.is_ranged()}\n"
        logtext += f"is_melee: {line.is_melee()}\n"
        logtext += f"is_repairable: {line.is_repairable()}\n"
        logtext += f"is_unique: {line.is_unique()}\n"

        logtext += f"class id: {line.get_class_id()}\n"
        logtext += f"garrison mode: {line.get_garrison_mode()}\n"
        logtext += f"head unit: {line.get_head_unit()}\n"
        logtext += f"train location id: {line.get_train_location_id()}\n"

        logtext += "line:\n"
        for unit in line.line:
            logtext += f"    - {unit}\n"

        if len(line.creates) > 0:
            logtext += "creates:\n"
            for unit in line.creates:
                logtext += f"    - {unit}\n"

        else:
            logtext += "creates: nothing\n"

        if len(line.researches) > 0:
            logtext += "researches:\n"
            for tech in line.researches:
                logtext += f"    - {tech}\n"

        else:
            logtext += "researches: nothing\n"

        if len(line.garrison_entities) > 0:
            logtext += "garrisons units:\n"
            for unit in line.garrison_entities:
                logtext += f"    - {unit}\n"

        else:
            logtext += "garrisons units: nothing\n"

        if len(line.garrison_locations) > 0:
            logtext += "garrisons in:\n"
            for unit in line.garrison_locations:
                logtext += f"    - {unit}\n"

        else:
            logtext += "garrisons in: nothing\n"

        if isinstance(line, GenieUnitLineGroup):
            logtext += "\n"
            logtext += f"civ id: {line.get_civ_id()}\n"
            logtext += f"enabling research id: {line.get_enabling_research_id()}\n"

        if isinstance(line, GenieBuildingLineGroup):
            logtext += "\n"
            logtext += f"has_foundation: {line.has_foundation()}\n"
            logtext += f"is_dropsite: {line.is_dropsite()}\n"
            logtext += f"is_trade_post {line.is_trade_post()}\n"
            logtext += f"enabling research id: {line.get_enabling_research_id()}\n"
            logtext += f"dropoff gatherer ids: {line.get_gatherer_ids()}\n"

        if isinstance(line, GenieStackBuildingGroup):
            logtext += "\n"
            logtext += f"is_gate: {line.is_gate()}\n"
            logtext += f"stack unit: {line.get_stack_unit()}\n"

        if isinstance(line, GenieUnitTransformGroup):
            logtext += "\n"
            logtext += f"transform unit: {line.get_transform_unit()}\n"

        if isinstance(line, GenieMonkGroup):
            logtext += "\n"
            logtext += f"switch unit: {line.get_switch_unit()}\n"

        with logfile.open("w") as log:
            log.write(logtext)

    for key, civ in dataset.civ_groups.items():
        logfile = debugdir.joinpath("conversion/civ_groups/")[str(key)]
        logtext = ""

        logtext += f"repr: {civ}\n"
        logtext += f"team bonus: {civ.team_bonus}\n"
        logtext += f"tech tree: {civ.tech_tree}\n"

        logtext += "civ bonus ids:\n"
        for bonus in civ.civ_boni:
            logtext += f"    - {bonus}\n"

        logtext += "unique unit ids:\n"
        for unit in civ.unique_entities:
            logtext += f"    - {unit}\n"

        logtext += "unique tech ids:\n"
        for tech in civ.unique_techs:
            logtext += f"    - {tech}\n"

        with logfile.open("w") as log:
            log.write(logtext)

    for key, tech in dataset.tech_groups.items():
        logfile = debugdir.joinpath("conversion/tech_groups/")[str(key)]
        logtext = ""

        logtext += f"repr: {tech}\n"
        logtext += f"is_researchable: {tech.is_researchable()}\n"
        logtext += f"is_unique: {tech.is_unique()}\n"
        logtext += f"research location id: {tech.get_research_location_id()}\n"

        logtext += f"required tech count: {tech.get_required_tech_count()}\n"
        logtext += "required techs:\n"
        for req_tech in tech.get_required_techs():
            logtext += f"    - {req_tech}\n"

        if isinstance(tech, AgeUpgrade):
            logtext += "\n"
            logtext += f"researched age id: {tech.age_id}\n"

        if isinstance(tech, UnitLineUpgrade):
            logtext += "\n"
            logtext += f"upgraded line id: {tech.get_line_id()}\n"
            logtext += f"upgraded line: {tech.get_upgraded_line()}\n"
            logtext += f"upgrade target id: {tech.get_upgrade_target_id()}\n"

        if isinstance(tech, BuildingLineUpgrade):
            logtext += "\n"
            logtext += f"upgraded line id: {tech.get_line_id()}\n"
            logtext += f"upgrade target id: {tech.get_upgrade_target_id()}\n"

        if isinstance(tech, UnitUnlock):
            logtext += "\n"
            logtext += f"unlocked line: {tech.get_unlocked_line()}\n"

        if isinstance(tech, BuildingUnlock):
            logtext += "\n"
            logtext += f"unlocked line: {tech.get_unlocked_line()}\n"

        with logfile.open("w") as log:
            log.write(logtext)

    for key, terrain in dataset.terrain_groups.items():
        logfile = debugdir.joinpath("conversion/terrain_groups/")[str(key)]
        logtext = ""

        logtext += f"repr: {terrain}\n"
        logtext += f"has_subterrain: {terrain.has_subterrain()}\n"

        with logfile.open("w") as log:
            log.write(logtext)

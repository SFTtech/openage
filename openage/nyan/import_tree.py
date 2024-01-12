# Copyright 2020-2023 the openage authors. See copying.md for legal info.

"""
Tree structure for resolving imports.
"""
from __future__ import annotations
from enum import Enum
import typing

from openage.log import warn

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.export.formats.nyan_file import NyanFile
    from openage.nyan.nyan_structs import NyanObject


class NodeType(Enum):
    """
    Types for nodes.
    """
    ROOT      = "r"     # tree root
    FILESYS   = "f"     # directory or file
    OBJECT    = "o"     # object in file (top level)
    NESTED    = "no"    # nested object


class Node:
    """
    Node in the import tree. This can be a directory, a file
    or an object.
    """

    __slots__ = ('name', 'node_type', 'parent', 'depth', 'children', 'alias')

    def __init__(self, name: str, node_type: NodeType, parent):
        """
        Create a node for an import tree.

        :param name: Name of the node.
        :type name: str
        :param node_type: Type of the node.
        :type node_type: NodeType
        :param parent: Parent node of this node.
        :type parent: Node
        """

        self.name = name
        self.node_type = node_type
        self.parent: Node = parent

        if not self.parent and self.node_type is not NodeType.ROOT:
            raise TypeError("Only node with type ROOT are allowed to have no parent")

        self.depth = 0
        if self.node_type is NodeType.ROOT:
            self.depth = 0

        else:
            self.depth = self.parent.depth + 1

        self.children = {}

        self.alias = ""

    def add_child(self, child_node) -> None:
        """
        Adds a child node to this node.
        """
        self.children.update({child_node.name: child_node})

    def has_ancestor(self, ancestor_node, max_distance: int = 128) -> bool:
        """
        Checks is the node has a given node as ancestor.

        :param ancestor_node: Ancestor candidate node.
        :type ancestor_node: Node
        """
        current_node = self
        distance = 0
        while distance < max_distance:
            if current_node.parent is ancestor_node:
                return True

            if not current_node.parent:
                return False

            current_node = current_node.parent
            distance += 1

        return False

    def has_child(self, name: str) -> bool:
        """
        Checks if a child with the given name exists.

        :param name: Name of the child node.
        :type name: str
        """
        return name in self.children

    def get_child(self, name: str):
        """
        Returns the child noe with the given name.

        :param name: Name of the child node.
        :type name: str
        """
        return self.children[name]

    def get_fqon(self) -> tuple[str]:
        """
        Get the fqon that is associated with this node by traversing the tree upwards.
        """
        current_node = self
        fqon = []
        while current_node.node_type is not NodeType.ROOT:
            fqon.insert(0, current_node.name)
            current_node = current_node.parent

        return tuple(fqon)

    def set_alias(self, alias: str) -> None:
        """
        Give this node an alias name.

        :param alias: Alias for the node.
        :type alias: str
        """
        if self.node_type is not NodeType.FILESYS:
            raise TypeError("Only nodes of type FILESYS can have aliases")

        self.alias = alias


class ImportTree:
    """
    Tree for storing nyan object references.
    """

    __slots__ = ('root', 'alias_nodes', 'import_nodes')

    def __init__(self):
        self.root = Node("", NodeType.ROOT, None)

        # Saves nodes for the import dict that have an alias
        self.alias_nodes: set[Node] = set()

        # Saves nodes for the import dict that don't have an alias
        self.import_nodes: set[Node] = set()

    def add_alias(self, fqon: tuple[str], alias: str) -> None:
        """
        Adds an alias to the node with the specified fqon.

        :param fqon: Identifier of the node.
        :type fqon: tuple[str]
        :param alias: Alias for the node.
        :type alias: str
        """
        current_node = self.root
        for node_str in fqon:
            try:
                current_node = current_node.get_child(node_str)

            except KeyError:  # as err:
                # TODO: Fail when the fqon is not found in the tree
                warn(f"fqon '{'.'.join(fqon)}' "
                     "could not be found in import tree")
                return
                # raise KeyError(f"fqon '{'.'.join(fqon)}' "
                #               "could not be found in import tree") from err

        current_node.set_alias(alias)

    def clear_marks(self) -> None:
        """
        Remove all alias marks from the tree.
        """
        self.alias_nodes.clear()
        self.import_nodes.clear()

    def expand_from_file(self, nyan_file: NyanFile) -> None:
        """
        Expands the tree from a nyan file.

        :param nyan_file: File with nyan objects.
        :type nyan_file: NyanFile
        """
        current_node = self.root
        fqon = nyan_file.get_fqon()
        node_type = NodeType.FILESYS

        for node_str in fqon:
            if current_node.has_child(node_str):
                # Choose the already created node
                current_node = current_node.get_child(node_str)

            else:
                # Add a new node
                new_node = Node(node_str, node_type, current_node)
                current_node.add_child(new_node)
                current_node = new_node

        # Process fqons of the contained objects
        for nyan_object in nyan_file.nyan_objects:
            self.expand_from_object(nyan_object)

    def expand_from_object(self, nyan_object: NyanObject) -> None:
        """
        Expands the tree from a nyan object.

        :param nyan_object: A nyan object.
        :type nyan_object: NyanObject
        """
        # Process the object
        fqon = nyan_object.get_fqon()

        if fqon[0] != "engine":
            current_node = self.root
            node_type = NodeType.OBJECT

            for node_str in fqon:
                if current_node.has_child(node_str):
                    # Choose the already created node
                    current_node = current_node.get_child(node_str)

                else:
                    # Add a new node
                    new_node = Node(node_str, node_type, current_node)
                    current_node.add_child(new_node)
                    current_node = new_node

        else:
            # Workaround for API objects because they are not loaded
            # from files currently.
            # TODO: Remove workaround when API is loaded from files.
            current_node = self.root
            index = 0
            while index < len(fqon):
                node_str = fqon[index]
                if current_node.has_child(node_str):
                    # Choose the already created node
                    current_node = current_node.get_child(node_str)

                else:
                    # Add a new node
                    if node_str[0].islower():
                        # By convention, directory and file names are lower case
                        # We can check for that to determine the node type.
                        node_type = NodeType.FILESYS

                    else:
                        node_type = NodeType.OBJECT

                    new_node = Node(node_str, node_type, current_node)
                    current_node.add_child(new_node)
                    current_node = new_node

                index += 1

        self._expand_nested_objects(nyan_object)

    def _expand_nested_objects(self, nyan_object: NyanObject):
        """
        Recursively search the nyan objects for nested objects
        """
        unsearched_objects = []
        unsearched_objects.extend(nyan_object.get_nested_objects())
        found_nested_objects = []

        while len(unsearched_objects) > 0:
            current_nested_object = unsearched_objects[0]
            unsearched_objects.extend(current_nested_object.get_nested_objects())
            found_nested_objects.append(current_nested_object)

            unsearched_objects.remove(current_nested_object)

        # Process fqons of the nested objects
        for nested_object in found_nested_objects:
            current_node = self.root
            node_type = NodeType.NESTED
            fqon = nested_object.get_fqon()

            for node_str in fqon:
                if current_node.has_child(node_str):
                    # Choose the already created node
                    current_node = current_node.get_child(node_str)

                else:
                    # Add a new node
                    new_node = Node(node_str, node_type, current_node)
                    current_node.add_child(new_node)
                    current_node = new_node

    def get_alias_dict(self) -> dict[str, tuple[str]]:
        """
        Get the fqons of the nodes that are used for aliases, i.e. fqons of all
        nodes in self.alias_nodes. The dict can be used for creating imports
        of a nyan file.

        Call this function after all object references in a file have been
        searched for aliases with get_alias_fqon().
        """
        aliases = {}
        for current_node in self.alias_nodes:
            if current_node.alias in aliases:
                raise ValueError(f"duplicate alias: {current_node.alias}")

            aliases.update({current_node.alias: current_node.get_fqon()})

        # Sort by imported name because it looks NICE!
        aliases = dict(sorted(aliases.items(), key=lambda item: item[1]))

        return aliases

    def get_import_list(self) -> list[tuple[str]]:
        """
        Get the fqons of the nodes that are plain imports, i.e. fqons of all
        nodes in self.import_nodes. The dict can be used for creating imports
        of a nyan file.

        Call this function after all object references in a file have been
        searched for aliases with get_alias_fqon().
        """
        imports = []
        for current_node in self.import_nodes:
            imports.append(current_node.get_fqon())

        # Sort by imported name because it looks NICE!
        imports.sort()

        return imports

    def get_alias_fqon(self, fqon: tuple[str], namespace: tuple[str] = None) -> tuple[str]:
        """
        Find the (shortened) fqon by traversing the tree to the fqon node and
        then going upwards until an alias is found.

        :param fqon: Object reference for which an alias should be found.
        :type fqon: tuple[str]
        :param namespace: Identifier of a namespace. If this is a (nested) object,
                          we check if the fqon is in the namespace before
                          searching for an alias.
        :type namespace: tuple[str]
        """
        if namespace:
            current_node = self.root

            if len(namespace) <= len(fqon):
                # Check if the fqon is in the namespace by comparing their identifiers
                for index, namespace_part in enumerate(namespace):
                    current_node = current_node.get_child(namespace_part)

                    if namespace_part != fqon[index]:
                        break

                else:
                    # Check if the namespace node is an object
                    if current_node.node_type in (NodeType.OBJECT, NodeType.NESTED):
                        # The object with the fqon is nested and we don't have to look
                        # up an alias
                        return (fqon[-1],)

        # Traverse the tree downwards
        current_node = self.root
        for part in fqon:
            current_node = current_node.get_child(part)

        # Traverse the tree upwards
        sfqon = []
        file_node = None
        while current_node.depth > 0:
            if file_node is None and current_node.node_type == NodeType.FILESYS:
                file_node = current_node

            if current_node.alias:
                sfqon.insert(0, current_node.alias)
                self.alias_nodes.add(current_node)
                break

            sfqon.insert(0, current_node.name)

            current_node = current_node.parent

        else:
            # There is no alias so we have to import the correct file
            if file_node:
                self.import_nodes.add(file_node)

        return tuple(sfqon)

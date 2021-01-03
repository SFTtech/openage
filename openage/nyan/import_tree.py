# Copyright 2020-2021 the openage authors. See copying.md for legal info.

"""
Tree structure for resolving imports.
"""
from enum import Enum

from ..util.ordered_set import OrderedSet
from .nyan_structs import NyanObject, NyanPatch


class ImportTree:
    """
    Tree for storing nyan object references.
    """

    __slots__ = ('root')

    def __init__(self):
        self.root = Node("", NodeType.ROOT, None)

    def add_alias(self, fqon, alias):
        """
        Adds an alias to the node with the specified fqon.
        """
        current_node = self.root
        for node_str in fqon:
            try:
                current_node = current_node.get_child(node_str)

            except KeyError as err:
                # TODO: Do not silently fail
                return
                # raise KeyError(f"fqon '{'.'.join(fqon)}'"
                #                "could not be found in import tree") from err

        current_node.set_alias(alias)

    def clear_marks(self):
        """
        Remove all alias marks from the tree.
        """
        self.root.clear()

    def expand_from_file(self, nyan_file):
        """
        Expands the tree from a nyan file.

        :param nyan_file: File with nyan objects.
        :type nyan_file: .convert.export.formats.nyan_file.NyanFile
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

    def expand_from_object(self, nyan_object):
        """
        Expands the tree from a nyan object.

        :param nyan_object: A nyan object.
        :type nyan_object: .nyan_structs.NyanObject
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

        # Recursively search the nyan objects for nested objects
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

    def get_import_dict(self):
        """
        Get the fqons of the nodes that are used for alias, i.e. fqons of all
        marked nodes. The dict can be used for creating imports of a nyan file.
        Call this function after all object references in a file have been
        searched for aliases with get_alias_fqon().
        """
        aliases = {}
        unhandled_nodes = [self.root]

        while len(unhandled_nodes) > 0:
            current_node = unhandled_nodes.pop(0)
            unhandled_nodes.extend(current_node.children.values())

            if current_node.marked:
                if current_node.alias in aliases.keys():
                    raise Exception(f"duplicate alias: {current_node.alias}")

                aliases.update({current_node.alias: current_node.get_fqon()})

        # Sort by imported name because it looks NICE!
        aliases = dict(sorted(aliases.items(), key=lambda item: item[1]))

        return aliases

    def get_alias_fqon(self, fqon, namespace=None):
        """
        Find the (shortened) fqon by traversing the tree to the fqon node and
        then going upwards until an alias is found.

        :param fqon: Object reference for which an alias should be found.
        :type fqon: tuple
        :param namespace: Identifier of a namespace. If this is a (nested) object,
                          we check if the fqon is in the namespace before
                          searching for an alias.
        :type namespace: tuple
        """
        if namespace:
            current_node = self.root

            if len(namespace) <= len(fqon):
                # Check if the fqon is in the namespace by comparing their identifiers
                for index in range(len(namespace)):
                    current_node = current_node.get_child(namespace[index])

                    if namespace[index] != fqon[index]:
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
        while current_node.depth > 0:
            if current_node.alias:
                sfqon.insert(0, current_node.alias)
                current_node.mark()
                break

            sfqon.insert(0, current_node.name)

            current_node = current_node.parent

        if not current_node.alias:
            print(fqon)

        return tuple(sfqon)


class Node:
    """
    Node in the import tree. This can be a directory, a file
    or an object.
    """

    __slots__ = ('name', 'node_type', 'parent', 'depth', 'children', 'marked', 'alias')

    def __init__(self, name, node_type, parent):
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
        self.parent = parent

        if not self.parent and self.node_type is not NodeType.ROOT:
            raise Exception("Only node with type ROOT are allowed to have no parent")

        self.depth = 0
        if self.node_type is NodeType.ROOT:
            self.depth = 0

        else:
            self.depth = self.parent.depth + 1

        self.children = {}

        self.marked = False
        self.alias = ""

    def add_child(self, child_node):
        """
        Adds a child node to this node.
        """
        self.children.update({child_node.name: child_node})

    def clear(self):
        """
        Unmark node and all children.
        """
        self.unmark()
        for child in self.children.values():
            child.clear()

    def has_ancestor(self, ancestor_node, max_distance=128):
        """
        Checks is the node has a given node as ancestor.
        """
        current_node = self
        distance = 0
        while distance < max_distance:
            if current_node.parent is ancestor_node:
                return True

            elif not current_node.parent:
                return False

            current_node = current_node.parent
            distance += 1

        return False

    def has_child(self, name):
        """
        Checks if a child with the given name exists.
        """
        return name in self.children.keys()

    def get_child(self, name):
        """
        Returns the child noe with the given name.
        """
        return self.children[name]

    def get_fqon(self):
        """
        Get the fqon that is associated with this node by traversing the tree upwards.
        """
        current_node = self
        fqon = []
        while current_node.node_type is not NodeType.ROOT:
            fqon.insert(0, current_node.name)
            current_node = current_node.parent

        return tuple(fqon)

    def mark(self):
        """
        Mark this node as an alias node.
        """
        self.marked = True

    def unmark(self):
        """
        Unmark this node as an alias node.
        """
        self.marked = False

    def set_alias(self, alias):
        """
        Give this node an alias name.
        """
        self.alias = alias


class NodeType(Enum):
    """
    Types for nodes.
    """

    ROOT      = "r"     # tree root
    FILESYS   = "f"     # directory or file
    OBJECT    = "o"     # object in file (top level)
    NESTED    = "no"    # nested object

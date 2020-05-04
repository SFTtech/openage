# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Tree structure for resolving imports.
"""
from enum import Enum

from openage.nyan.nyan_structs import NyanObject, NyanPatch
from openage.util.ordered_set import OrderedSet


class ImportTree:
    """
    Tree for storing nyan object references.
    """

    __slots__ = ('root')

    def __init__(self):
        self.root = Node("", NodeType.ROOT, None)

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
        # Process fqon of the file
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

    def establish_import_dict(self, nyan_file, min_node_depth=2, max_size=15, ignore_names=[]):
        """
        Generate an import dict for a nyan file.
        """
        # Find all imports
        objects_in_file = []
        objects_in_file.extend(nyan_file.nyan_objects)

        for nyan_object in nyan_file.nyan_objects:
            unsearched_objects = []
            unsearched_objects.extend(nyan_object.get_nested_objects())

            while len(unsearched_objects) > 0:
                current_nested_object = unsearched_objects[0]
                unsearched_objects.extend(current_nested_object.get_nested_objects())
                objects_in_file.append(current_nested_object)

                unsearched_objects.remove(current_nested_object)

        referenced_objects = OrderedSet()
        for nyan_object in objects_in_file:
            referenced_objects.update(nyan_object.get_parents())

            if isinstance(nyan_object, NyanPatch):
                referenced_objects.add(nyan_object.get_target())

            for member in nyan_object.get_members():
                if isinstance(member.get_member_type(), NyanObject) and not member.is_optional():
                    referenced_objects.add(member.get_value())

                elif isinstance(member.get_set_type(), NyanObject):
                    for value in member.get_value():
                        referenced_objects.add(value)

        # Separate external imports (= imports from other files)
        # from internal imports (= same file)
        external_objects = []
        internal_objects = []
        file_fqon = nyan_file.get_fqon()
        for referenced_object in referenced_objects:
            obj_fqon = referenced_object.get_fqon()

            index = 0
            external = False
            while index < len(file_fqon):
                if file_fqon[index] != obj_fqon[index]:
                    external = True
                    break

                index += 1

            if external:
                external_objects.append(referenced_object)

            else:
                internal_objects.append(referenced_object)

        # Search the tree for the corresponding object nodes
        nodes = OrderedSet()
        for external_object in external_objects:
            obj_fqon = external_object.get_fqon()
            current_node = self.root

            for part in obj_fqon:
                current_node = current_node.children[part]

            nodes.add(current_node)

        # Mark the internal nodes
        for internal_object in internal_objects:
            obj_fqon = internal_object.get_fqon()
            current_node = self.root

            for part in obj_fqon:
                current_node = current_node.children[part]

            current_node.mark()

        # Generate aliases, check for conflicts, go upwards to resolve conflicts
        # Repeat until there are no more conflicts
        aliases = {}
        unhandled_nodes = []
        unhandled_nodes.extend(nodes)
        while len(unhandled_nodes) > 0:
            current_node = unhandled_nodes[0]
            alias_candidate = current_node.name

            if alias_candidate in aliases.keys() or alias_candidate in ignore_names:
                if current_node.parent:
                    unhandled_nodes.append(current_node.parent)

                unhandled_nodes.remove(current_node)

            else:
                aliases[alias_candidate] = current_node
                unhandled_nodes.remove(current_node)

        # Try to make the result smaller by finding common ancestors of nodes
        while len(aliases) > max_size:
            new_aliases = {}
            new_aliases.update(aliases)

            # key: node; value: number of alias children nodes
            common_parents = {}
            for node in new_aliases.values():
                parent = node.parent

                if parent in common_parents.keys():
                    common_parents[parent] += 1

                else:
                    common_parents[parent] = 1

            # Find the most common parent
            common_parents = sorted(common_parents.items(), key=lambda x: x[1], reverse=True)
            for common_parent in common_parents:
                most_common_parent = common_parent[0]
                most_common_count = common_parent[1]

                if not most_common_parent:
                    continue

                # If the parent's name is an ignored name, choose its parent instead
                if most_common_parent.name in ignore_names:
                    most_common_parent = most_common_parent.parent

                # Check if the parent's name is already an alias
                if most_common_parent.name in new_aliases.keys():
                    continue

                break

            else:
                break

            if most_common_parent.depth < min_node_depth or most_common_count == 1:
                break

            # Remove the parent's children from the aliases
            for node in aliases.values():
                if node.has_ancestor(most_common_parent):
                    new_aliases.pop(node.name)

            new_aliases.update({most_common_parent.name: most_common_parent})

            aliases = new_aliases

        # Mark the nodes as aliases in the tree
        for node in aliases.values():
            node.mark()

        fqon_aliases = {}
        for alias, node in aliases.items():
            fqon_aliases.update({alias: node.get_fqon()})

        return fqon_aliases

    def get_alias_fqon(self, fqon):
        """
        Find the (shortened) fqon by traversing the tree to the fqon node and
        then going upwards until a marked node is found.
        """
        # Traverse the tree downwards
        current_node = self.root
        for part in fqon:
            current_node = current_node.get_child(part)

        # Traverse the tree upwards
        sfqon = []
        while current_node.depth > 0:
            sfqon.insert(0, current_node.name)

            if current_node.alias:
                break

            current_node = current_node.parent

        return tuple(sfqon)


class Node:
    """
    Node in the import tree. This can be a directory, a file
    or an object.
    """

    __slots__ = ('name', 'node_type', 'parent', 'depth', 'children', 'alias')

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

        if not self.parent and not self.node_type is NodeType.ROOT:
            raise Exception("Only node with type ROOT are allowed to have no parent")

        self.depth = 0
        if self.node_type is NodeType.ROOT:
            self.depth = 0

        else:
            self.depth = self.parent.depth + 1

        self.children = {}

        self.alias = False

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
        self.alias = True

    def unmark(self):
        """
        Unmark this node as an alias node.
        """
        self.alias = False


class NodeType(Enum):
    """
    Types for nodes.
    """

    ROOT      = "r"     # tree root
    FILESYS   = "f"     # directory or file
    OBJECT    = "o"     # object in file (top level)
    NESTED    = "no"    # nested object

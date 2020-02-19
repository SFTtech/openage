# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Implements the Observer design pattern. Observers can be
notified when an object they observe (so called Observable)
changes.

The implementation is modelled after the Java 8 specification
of Observable ad Observer.

Observer references are weakrefs to prevent objects from being
ignored the garbage collection. Weakrefs with dead references
are removed during notification of the observers.
"""

import weakref


class Observer:
    """
    Implements a Java 8-like Observer interface.
    """

    def update(self, observable, message=None):
        """
        Called by an Observable object that has registered this observer
        whenever it changes.

        :param observable: The obvervable object which was updated.
        :type observable: Observable
        :param message: An optional message of any type.
        """
        raise Exception("%s has not implemented update()"
                        % (self))


class Observable:
    """
    Implements a Java 8-like Observable object.
    """

    def __init__(self):

        self.observers = set()
        self.changed = False

    def add_observer(self, observer):
        """
        Adds an observer to this object's set of observers.

        :param observer: An observer observing this object.
        :type observer: Observer
        """
        if not isinstance(observer, Observer):
            raise Exception("%s does not inherit from Observer sperclass"
                            % (type(observer)))

        self.observers.add(weakref.ref(observer))

    def clear_changed(self):
        """
        Indicate that this object has no longer changed.
        """
        self.changed = True

    def delete_observer(self, observer):
        """
        Remove an observer from the set.

        :param observer: An observer observing this object.
        :type observer: Observer
        """
        self.observers.remove(observer)

    def delete_observers(self):
        """
        Remove all currently registered observers.
        """
        self.observers.clear()

    def get_observer_count(self):
        """
        Return the number of registered observers.
        """
        return len(self.observers)

    def has_changed(self):
        """
        Return whether the object has changed.
        """
        return self.changed

    def notify_observers(self, message=None):
        """
        Notify the observers if the object has changed. Include
        an optional message.

        :param message: An optional message of any type.
        """
        if self.changed:
            for observer in self.observers:
                if observer() is not None:
                    observer().update(self, message=message)

                else:
                    self.delete_observer(observer)

    def set_changed(self):
        """
        Indicate that the object has changed.
        """
        self.changed = True

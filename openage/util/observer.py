# Copyright 2020-2023 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-few-public-methods

"""
Implements the Observer design pattern. Observers can be
notified when an object they observe (so-called Observable)
changes.

The implementation is modelled after the Java 8 specification
of Observable and Observer.

Observer references are weakrefs to prevent objects from being
ignored the garbage collection. Weakrefs with dead references
are removed during notification of the observers.
"""
from __future__ import annotations

from typing import Any, Optional
import weakref


class Observer:
    """
    Implements a Java 8-like Observer interface.
    """

    def update(self, observable: Observable, message: Optional[Any] = None):
        """
        Called by an Observable object that has registered this observer
        whenever it changes.

        :param observable: The obvervable object which was updated.
        :type observable: Observable
        :param message: An optional message of any type.
        """
        raise NotImplementedError(f"{self} has not implemented update()")


class Observable:
    """
    Implements a Java 8-like Observable object.
    """

    def __init__(self):

        self.observers: weakref.WeakSet[Observer] = weakref.WeakSet()
        self.changed = False

    def add_observer(self, observer: Observer) -> None:
        """
        Adds an observer to this object's set of observers.

        :param observer: An observer observing this object.
        :type observer: Observer
        """
        self.observers.add(observer)

    def clear_changed(self) -> None:
        """
        Indicate that this object has no longer changed.
        """
        self.changed = False

    def delete_observer(self, observer: Observer) -> None:
        """
        Remove an observer from the set.

        :param observer: An observer observing this object.
        :type observer: Observer
        """
        self.observers.remove(observer)

    def delete_observers(self) -> None:
        """
        Remove all currently registered observers.
        """
        self.observers.clear()

    def get_observer_count(self) -> int:
        """
        Return the number of registered observers.
        """
        return len(self.observers)

    def has_changed(self) -> bool:
        """
        Return whether the object has changed.
        """
        return self.changed

    def notify_observers(self, message: Optional[Any] = None) -> None:
        """
        Notify the observers if the object has changed. Include
        an optional message.

        :param message: An optional message of any type.
        """
        if self.changed:
            for observer in self.observers:
                observer.update(self, message=message)

    def set_changed(self) -> None:
        """
        Indicate that the object has changed.
        """
        self.changed = True

# Copyright 2015-2021 the openage authors. See copying.md for legal info.

"""
Provides some utility context guards.
"""

import types


class DummyGuard:
    """Context guard that does nothing."""

    def __enter__(self) -> None:
        pass

    def __exit__(
        self,
        exc_type: type[BaseException] | None,
        exc_value: BaseException | None,
        traceback: types.TracebackType | None,
    ) -> None:
        pass

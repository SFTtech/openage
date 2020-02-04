# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Backend logic of the openage launcher
"""

from pathlib import Path    # should this use openage's Path?
import toml

from .mod import ModInfo
from .. import VERSION
from ..game.main import main as openage_main

LAUNCHER_VERSION = "0.1.0"


class LauncherCore:
    """
    Holds the launcher status and provides methods for its functionalities
    exposes an API for the launcher UI
    """
    preset_path = Path('another/path')
    def __init__(self):
        if self.is_outdated():
            print('hey, update!')

        self.mods = {}

        # uids of installed mods
        self.installed = []
        # store uid of enabled mods, ordered by priority
        self.enabled = []

    @staticmethod
    def get_version():
        """
        returns current launcher version
        """
        return LAUNCHER_VERSION

    @staticmethod
    def get_latest_version():
        """
        returns the latest launcher version
        """
        return "0.0.0"

    @staticmethod
    def get_engine_version():
        """
        returns current launcher version
        """
        return VERSION

    @staticmethod
    def get_latest_engine_version():
        """
        returns the latest launcher version
        """
        return "0.0.0"

    def is_outdated(self):
        if self.get_engine_version() < self.get_latest_engine_version():    # TODO: pseudocode
            return True
        return False

    def update(self):
        pass

    def import_mod_info(self, info_string, path=None):
        # retrieval of info_string should be handled by something else
        # which also gives a path if the mod is installed locally
        mod = ModInfo(info_string, path)
        self.mods[mod.uid] = mod
        if path is not None:
            self.installed.append(mod.uid)

    def install_mod(self, mod_uid, path):
        mod = self.mods[mod_uid]
        # download and install the mod
        mod.set_path(path)
        self.installed.append(mod_uid)

    @staticmethod
    def _mods_conflict(mod1, mod2):
        if mod1 in mod2.get_conflicts() or mod2 in mod1.get_conflicts():
            return True
        return False

    def causes_conflict(self, mod_uid):
        # can't check conflicts if mod_info is not available
        if not mod_uid in self.mods:
            # complain
            return
        mod = self.mods[mod_uid]
        for other_uid in self.enabled:
            other = self.mods[other_uid]
            if self._mods_conflict(mod, other):
                return True
        return False

    def get_missing_requirements(self, mod_uid):
        # can't check requirements if mod_info is not available
        if not mod_uid in self.mods:
            # complain
            return
        mod = self.mods[mod_uid]
        missing = []
        for req in mod.get_requirements():
            if req not in self.enabled:
                missing.append(req)
        return missing

    def enable_mod(self, mod_uid):
        if mod_uid in self.enabled:
            return
        if mod_uid not in self.installed:
            # complain
            return
        if self.causes_conflict(mod_uid):
            # complain
            return
        missing_reqs = self.get_missing_requirements(mod_uid)
        if missing_reqs:
            # complain
            return
        self.enabled.append(mod_uid)

    def disable_mod(self, mod_uid):
        if mod_uid not in self.enabled:
            return
        self.enabled.remove(mod_uid)

    def set_mod_priority(self, mod_uid, priority):
        if mod_uid not in self.enabled:
            return
        idx = self.enabled.index(mod_uid)
        self.enabled.insert(priority, self.enabled.pop(idx))

    def get_mods(self):
        return list(self.mods.values())

    def get_installed_mods(self):
        return [mod for mod in self.mods if mod in self.installed]

    def get_enabled_mods(self):
        """
        returns ModInfo objects for all the enabled mods
        """
        return [mod for mod in self.mods if mod in self.enabled]

    def save_preset(self, preset_name):
        preset_path = self.preset_path/preset_name
        enabled = {'enabled': self.enabled}
        with open(preset_path, 'w+') as f:
            toml.dump(enabled, f)

    def load_preset(self, preset_name):
        preset_path = self.preset_path/preset_name
        if not preset_path.is_file():
            return
        with open(preset_path, 'r') as f:
            enabled = toml.load(f)['enabled']
        self.enabled = enabled

    def log_in(self, username):
        pass
        # ask for password?

    @staticmethod
    def get_news_feed():
        return 'Openage Feed'

    @staticmethod
    def run(args=None):
        openage_main(args, error=None)

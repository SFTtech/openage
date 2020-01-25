# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Backend logic of the openage launcher
"""

from .mod import ModInfo

LAUNCHER_VERSION = "0.1.0"


class LauncherCore:
    """
    Holds the launcher status and provides methods for its functionalities
    exposes an API for the launcher UI
    """
    default_path = 'a/path'
    default_repo = 'a.repo.com'
    def __init__(self):
        self.launcher_version = LAUNCHER_VERSION
        self.version = self.get_version()
        if self.is_outdated():
            print('hey, update!')

        self.local_mods = set()
        self.remote_mods = set()

        self.import_local_mods(self.default_path)
        self.import_remote_mods(self.default_repo)

        # store uid of enabled mods, ordered by priority
        self.enabled = []

    @staticmethod
    def get_version(self):
        """
        returns current openage version
        """
        return "0.0.0"

    @staticmethod
    def get_latest_version(self):
        """
        returns the latest available openage version
        """
        return "0.0.0"

    def is_outdated(self):
        if self.version < self.get_latest_version():    # TODO: pseudocode
            return True
        return False

    def import_local_mod(self, file_path):
        mod = ModInfo(file_path=file_path)
        self.local_mods.add(mod)

    def import_remote_mod(self, url):
        mod = ModInfo(url=url)
        self.remote_mods.add(mod)

    def import_local_mods(self, mods_dir):
        for mod_path in mods_dir:    # TODO: pseudocode
            self.import_local_mod(mod_path)

    def import_remote_mods(self, repo):
        for mod_url in repo:    # TODO: pseudocode
            self.import_remote_mod(mod_url)

    def download_remote_mod(self, mod_uid):
        pass

    def enable_mod(self, mod_uid):
        if mod_uid in self.enabled:
            return
        for other_mod in self.local_mods:
            if other_mod in self.enabled and other_mod.conflicts_with(mod_uid):
                # complain
                return
        self.enabled.append(mod_uid)

    def disable_mod(self, mod_uid):
        try:
            self.enabled.remove(mod_uid)
        except ValueError:
            # mod is not active
            return

    def change_mod_priority(self, mod_uid, shift):
        try:
            idx = self.enabled.index(mod_uid)
        except ValueError:
            # mod is not active
            return
        # this wraps around, feature or bug?
        self.enabled[idx], self.enabled[idx + shift] = self.enabled[idx + shift], self.enabled[idx]

    def mod_priority_up(self, mod_uid):
        self.change_mod_priority(mod_uid, -1)

    def mod_priority_down(self, mod_uid):
        self.change_mod_priority(mod_uid, 1)

    def log_in(self, username):
        pass
        # ask for password?

    @staticmethod
    def get_news_feed(self):
        return 'Openage Feed'

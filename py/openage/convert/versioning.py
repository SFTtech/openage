# Copyright 2015-2015 the openage authors. See copying.md for legal info.

# Provides versioning information for converted files.


from .gamedata import empiresdat

print(empiresdat.EmpiresDat.format_hash().hexdigest())

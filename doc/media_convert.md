How to use the original game assets?
------------------------------------

Openage currently depends on the original game assets, so you need a copy of the *original AoE II or AoE II: HD*.
The original AoE:TC game asset formats are, lets say, "special", so they need to be converted in order to be usable from openage.
That conversion is performed by the openage.convert python module when the game is run for the first time.

The game will ask for your AoE II installation folder; examples include:

    ~/.wine-age/drive_c/programs/ms-games/aoe2

    /var/run/media/windisk/Program Files (x86)/Steam/SteamApps/common/Age2HD

You will find the converted files in `assets/converted`.

The version of openage that the media files were converted with is written to `assets/converted/asset_version`.
Delete that file to trigger a full reconversion.

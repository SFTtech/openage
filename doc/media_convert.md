How to use the original game assets?
------------------------------------

Openage currently depends on the original game assets, so you need a copy of the *original AoE II or AoE II: HD*. You have a few options.

* You may use [Wine](https://www.winehq.org/) to run Steam for Windows.
* You can trick Steam for Linux/Mac into downloading the Windows game assets [with a game manifest file](https://gist.github.com/paulirish/758f262379092ff2910a). 
  1. Save the below code to **`appmanifest_221380.acf`** 
  2. Place the file in your steamapps folder, typically found in `~/.steam/steam/SteamApps` or `~/Library/Application Support/Steam/steamapps`

```json
    "AppState"
    {
      "AppID"  "221380"
      "Universe" "1"
      "installdir" "Age2HD"
      "StateFlags" "1026"
    }
```
* Alternatively, you may download the game assets using [SteamCMD](https://developer.valvesoftware.com/wiki/SteamCMD).
  1. Install SteamCMD by following the instructions on [Valve's developer wiki](https://developer.valvesoftware.com/wiki/SteamCMD).
  2. Download the assets with the following command (replacing `USERNAME` and `ASSET_DIR` as appropriate):

`./steamcmd.sh +@sSteamCmdForcePlatformType windows +login USERNAME +force_install_dir ASSET_DIR +app_update 221380 validate +quit`

In the future, using installation disks may be supported.

## Conversion

The original AoE:TC game asset formats are, lets say, "special", so they need to be converted in order to be usable from openage.
That conversion is performed by the openage.convert python module when the game is run for the first time.

The game will ask for your AoE II installation folder; examples include:

    ~/.wine-age/drive_c/programs/ms-games/aoe2
    /var/run/media/windisk/Program Files (x86)/Steam/SteamApps/common/Age2HD
    ~/Library/Application Support/Steam/steamapps/common

You will find the converted files in `assets/converted`.

The version of openage that the media files were converted with is written to `assets/converted/asset_version`.
Delete that file to trigger a full reconversion.

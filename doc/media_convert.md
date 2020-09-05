How to use the original game assets?
------------------------------------

Openage currently depends on the original game assets, so you need a valid copy of the *original game files*.

Currently we support conversion for these games:

* **RoR**: Age of Empires 1 (1997) + Rise of Rome
* **AoC**: Age of Empires 2 (1999) + The Conqueror's
* **SWGB**: Star Wars: Galactic Battlegrounds + Clone Campaigns
* **DE2**: Age of Empires 2: Definitve Edition

On Linux, you may have to use additional measures to download DE2 versions from Steam:

* Use [Proton](https://github.com/ValveSoftware/Proton) which is integrated into [Steam Play](https://store.steampowered.com/linux) to download the DE2 assets (and even play it).
* You may use [Wine](https://www.winehq.org/) to run Steam for Windows.
* You can trick Steam for Linux/Mac into downloading the Windows game assets [with a game manifest file](https://gist.github.com/paulirish/758f262379092ff2910a).
  1. Save the below code to **`appmanifest_813780.acf`**
  2. Place the file in your steamapps folder, typically found in `~/.steam/steam/SteamApps` or `~/Library/Application Support/Steam/steamapps`
  3. Restart Steam after placing the file

```json
    "AppState"
    {
      "AppID"  "813780"
      "Universe" "1"
      "installdir" "AOE2DE"
      "StateFlags" "1026"
    }
```
* Alternatively, you may download the game assets using [SteamCMD](https://developer.valvesoftware.com/wiki/SteamCMD).
  1. Install SteamCMD by following the instructions on [Valve's developer wiki](https://developer.valvesoftware.com/wiki/SteamCMD).
  2. Download the assets with the following command (replacing `USERNAME` and `ASSET_DIR` as appropriate):

`./steamcmd.sh +@sSteamCmdForcePlatformType windows +login USERNAME +force_install_dir ASSET_DIR +app_update 221380 validate +quit`

In the future, using installation disks may be supported.

## Conversion

The original AoC game asset formats are, lets say, "special", so they need to be converted in order to be usable from openage.
That conversion is performed by the `openage.convert` python module when the game is run for the first time.

The game will ask for your AoE II installation folder; examples include:

    ~/.wine-age/drive_c/programs/ms-games/aoe2
    /var/run/media/windisk/Program Files (x86)/Steam/SteamApps/common/Age2HD
    ~/.steam/steam/SteamApps/common/Age2HD
    ~/Library/Application Support/Steam/steamapps/common/Age2HD

You will find the converted files in `assets/converted`.

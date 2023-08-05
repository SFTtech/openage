How to use the original game assets?
------------------------------------

Openage currently depends on the original game assets, so you need a valid copy of the *original game files*.

Currently we *support* conversion for these games:

* **RoR**: Age of Empires 1 (1997) + Rise of Rome
* **AoC**: Age of Empires 2 (1999) + The Conqueror's
* **SWGB**: Star Wars: Galactic Battlegrounds + Clone Campaigns
* **HD**: Age of Empires 2 (2013) (formerly: Age of Empires 2: HD Edition)
* **DE1**: Age of Empires 1: Definitive Edition
* **DE2**: Age of Empires 2: Definitve Edition

On Linux, you may have to use additional measures to download DE2 versions from Steam:

* Use [Proton](https://github.com/ValveSoftware/Proton) which is integrated into [Steam Play](https://store.steampowered.com/linux) to download the DE2 assets (and even play it).
* Use [Wine](https://www.winehq.org/) to run Steam for Windows.
* Download the game assets using [SteamCMD](https://developer.valvesoftware.com/wiki/SteamCMD).
  1. Install SteamCMD by following the instructions on [Valve's developer wiki](https://developer.valvesoftware.com/wiki/SteamCMD).
  2. Download the assets with the following command (replacing `USERNAME` `GAME_APP_ID` and `ASSET_DIR` as appropriate):

`./steamcmd.sh +@sSteamCmdForcePlatformType windows +login USERNAME +force_install_dir ASSET_DIR +app_update GAME_APP_ID validate +quit`

The game version vs Game_ID table such as below:
 Age of Empires (2013): 221380
 Age of Empires: Definitive Edition: 1017900
 Age of Empires II (Rise of the Rajas): 488060
 Age of Empires II: Definitive Edition: 813780


## Conversion

The original AoC game asset formats are, lets say, "special", so they need to be converted in order to be usable from openage.
That conversion is performed by the `openage.convert` python module when the game is run for the first time.

The game will ask for your AoE II installation folder; examples include:

    ~/.wine-age/drive_c/programs/ms-games/aoe2
    /var/run/media/windisk/Program Files (x86)/Steam/SteamApps/common/Age2HD
    ~/.steam/steam/SteamApps/common/Age2HD
    ~/Library/Application Support/Steam/steamapps/common/Age2HD

Alternatively, if your game installation is not found, you can trigger the conversion manually:

```
python3 -m openage convert --force --source-dir /path/to/game/install
```

You will find the converted files in `assets/converted`.

# Troubleshooting

## Windows Installer

### More than one python installation

If you have two (or more) different python installations on your computer edit the `openage.bat` in the install directory:
Replace the line `python.exe -m openage` with `call "%INST_DIR%\python\python.exe" -m openage` to start `python.exe` explicitly.

## Asset Conversion

### Conversion fails when converting from *AoE II: HD Edition*

The newer versions of the HD Edition use another asset format. You need to have **Patch level 4.3** to successfully convert the assets. You can change the patch level by doing this:

1. Right-Click on Age of Empires 2: HD Edition in your games section
2. Select "Properties"
3. Select "Betas"
4. The beta you want to opt into is "patch43 - Patch 4.3"

After you've selected the beta program, wait for Steam to download the files and try converting again.

### Conversion fails for *The Conquerors* 1.0c

Make sure you don't have *UserPatch*, compatibility patches or modifications installed that make changes to the original asset files.

### Conversion fails for *age2_x1.xml* files / Wololo Kingdoms

If you have Wololo Kingdoms and various mods installed that change the base assets the converter will not work. A workaround would be to make a backup of your AGE2 directory and let the AGE2DIR environment variable point to that backup. In that backup at subfolder `AGE2/resources` delete all files ***except*** folders. Another workaround would be to backup your AGE2 folder and redownload it to have a clean install. After conversion you can replace it with the backup.

## Ingame

### UI elements are visible but the background stays black

openage does not start a game by default on startup. Press the "generate game" button on the left or, if the game has not finished loading the assets, tick the checkbox marked with "create_when_ready".

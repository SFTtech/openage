# Instructions for the Windows Setupscript


## 1. What does this script do?
This script helps you installing and compiling all the dependencies you will later need to compile, run and package ****openage**** on a windows system. It should help you avoiding the snares while setting up your system while also saving you a lot of work and time invested into that.

#### Different stages
The **first stage** of this script is to download and install/extract the dependencies into a folder structure it will set up for you in a place you chose.

The **second stage** is to precompile some C++ libraries needed for later. This step will *take over an hour*, depending on your systems performance even up to *three hours*.

The **third stage** is to configure and compile openage. This step will take probably around *half an hour* for the first time.

The **fourth stage** sets all the environment variables needed to run *openage*.

The **fifth stage** will package openage into an executable file for installing.

## 2. Setting up the variables

Clone the *openage*-Repository **or** download the the script in a compressed file from [here](https://LinkToFile.file) and then extract it.

If you cloned the repository you'll find the script in `&lt;openage-repository-dir&gt;/doc/build_instructions/windows-setupscript`.

1. Navigate to the `./includes`-Subdirectory and open `autobuild-vars.ps1` in a texteditor of your choice.

2. Set a value for `$vmhd_dir` on a harddrive with around **20 GiB** of free-space.

3. If you use this script for the first time, make sure `$progress` is set to `dependencies`

4. The standard method of this script to install and collect most of the dependencies is to use the windows package manager `chocolatey`. That makes it also easier to hold the dependencies up-to-date, if needed. You can set `$install_method` to `portable` though, if you prefer most of the dependencies to be existing in the subfolder structure, that will be created. Nevertheless, some of the dependencies (Visual Studio build tools, Python, Fonts) still need to be installed. Preferred method is `chocolatey`.

5. Leave the `$choco_dep_file` and `$choco_bin_file` variables untouched, if you didn't change anything in the subfolder structure of the script.

6. Set `$aoe_std_path` to the *Age of Empires II* directory on your computer. Standard path here is `%ProgramFiles(x86)%\Steam\steamapps\common\Age2HD`, for an AoE-Version installed by Steam.

	__NOTE__: In Steam right-click on `Age of Empires II -&gt; Properties -&gt; Beta` and make sure you chose `Patch43` level as this is the current version our converter supports.

7. Non-chocolatey: `$version_file` If you chose the portable install method, this should be set to a **version file** suiting your needs. Take a look in the `includes`-subdirectory of the script and check the `*.json` files.

8. `$dependency_dl_path` if you want to change the directory where the downloads will be saved, adjust this value. Normally everything gets downloaded and extracted into a subfolder structure of `$vmhd_dir/openage-dl/`

9. `$build_dir` ist the folder where *openage* will be compiled later. In this folder there will be some folders depending on which architecture you chose to compile for, e.g. `Win64` for a 64-bit windows.

10. `$vcpkg_path` is the path where *vcpkg* and all its *precompiled C++-Libraries* will be found later

11. `$build_config` is set to `RelWithDebInfo` as a default setting and will fit most consumers compiling *openage* themselves pretty well. If you want to help developing *openage* though, it will be better to chose another configuration setting for debugging.

12. If you are using a different version of Visual Studio you probably should change `$build_vs_ver` to the one you want to use to compile *openage*. Also you probably wand to adjust some other settings in the script to not install the *Visual Studio build tools*.

13. `$is64bit` is set to `$true` on default. If you want to download and compile all the dependencies and *openage* for 32-bit you should set this to `$false`. You can also set a environment variable on system-level `OPENAGEx64` to either `TRUE`or `FALSE`.

14. __NOTE:__ `Prebuilt Qt`: If you are not using the preferred install methods here from the script for Qt, you might want to take a look at `$set_env_path` at the bottom of the `autobuild-vars.ps1`-file to remove the comment mark for the line `[PSCustomObject]@{Path="$($preinstalled_qt_dir)\bin\";}` and set `$preinstalled_qt_dir` to the folder you installed Qt into.

## 3. Using the script

1. Open `PowerShell` as an administrator.
	__HINT:__ Press the `Win`-Key, search for **Powershell** and right-click on `Windows PowerShell -&gt; Run as administrator`

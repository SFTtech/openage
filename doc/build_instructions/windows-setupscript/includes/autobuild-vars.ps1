# Copyright 2019 the openage authors. See LICENSE for legal info.
# Licensed under GNU Affero General Public License v3.0

# PLEASE ADJUST THE FOLLOWING VALUES TO YOUR LIKING!

# Path to vmhd/harddrive you want to compile on
$vmhd_dir="D:\" 

# Set one of: 'dependencies', 'vcpkg', 'compile', 'env', 'package' or 'cleanup'
# to start of at a certain point
$progress = "env"

# Prefered install method: 'chocolatey' or 'portable'
# Standard: chocolatey
$install_method="chocolatey"

# Choco deps file
$choco_dep_file="$PSScriptRoot\includes\choco-deps.json"

# Choco bin file
$choco_bin_file="$PSScriptRoot\includes\choco-binaries.json"



# IMPORTANT STANDARD PATHS
 
## Path to the file with all the dependencies and versions
## You could also set the $OPENAGE_VF environment variable to the version file you like

# AGE2DIR
$aoe_std_path="$($vmhd_dir)openage-assets\Age2HD43"

#$version_file=$env:OPENAGE_VF
$version_file="$PSScriptRoot\includes\recent-deps.json"

## Change to preferred download path
$dependency_dl_path="$($vmhd_dir)openage-dl\";

## Change Build-Directory
$build_dir="$($vmhd_dir)openage-build\";

## vcpkg should be saved in
$vcpkg_path="$($dependency_dl_path)vcpkg\"

# Command to install the dependencies from pip
$pip_modules="cython numpy pillow pygments pyreadline Jinja2"

# Command to install the dependencies from vcpkg
$vcpkg_deps="dirent eigen3 fontconfig freetype harfbuzz libepoxy libogg libpng opus opusfile qt5-base qt5-declarative qt5-quickcontrols sdl2 sdl2-image"


#$cmake build/cpack
$build_config="RelWithDebInfo"

# cmake Visual Studio version
$build_vs_ver = "Visual Studio 15 2017"

# ARCHITECTURE 32/64 bit
## (DEBUG) Set architecture manually
$is64bit=$true

# CHANGEME: Set the environment variable on your system $env:OPENAGEx64 
# to either TRUE (for 64bit builds) or FALSE (for 32bit builds)


# Deal with vcpkg standard triplet for 64bit compilation
if($($env:VCPKG_DEFAULT_TRIPLET) -eq "x64-windows"){
    
    # Set 64bit for us
    $is64bit=$true
    
    # Unset this variable as we have one only for openage-x64
    Remove-Item -Path env:VCPKG_DEFAULT_TRIPLET
}

## Set architecture with environment variables
## Standard is 64-bit
if((($env:OPENAGEx64 -eq $false) -and !($is64bit -eq $true)) -or (($is64bit -eq $false) -and !($env:OPENAGEx64 -eq $true))){
    $is64bit=$false
    $env:OPENAGEx64=$false
    Write-Host "32bit Flag activated! Everything will be downloaded and build in 32bit/x86 architecture!"
    
    #Chocolatey
    $choco_arch="--x86"

    #python-dl-arch
    $python_dl_arch=""

    #git-dl-arch
    $git_dl_arch="-32-bit"

    #cmake-dl-arch
    $cmake_dl_arch="-win32-x86"  
    
    #cmake
    $cmake_arch = "Win32"

    #vcpkg
    $vcpkg_arch=""

}elseif(($is64bit -eq $true) -or ($env:OPENAGEx64 -eq $true)){
    $is64bit=$true
    $env:OPENAGEx64=$true
    Write-Host "64bit Flag activated! Everything will be downloaded and build in 64bit/x64 architecture!"

    #Chocolatey
    $choco_arch=""

    #python-dl-arch
    $python_dl_arch="-amd64"

    #git-dl-arch
    $git_dl_arch="-64-bit"

    #cmake-dl-arch
    $cmake_dl_arch="-win64-x64"

    #cmake
    $cmake_arch = "Win64"

    #vcpkg
    $vcpkg_arch="--triplet x64-windows"

    #relevant config
    $relevant_config_oa="x64-windows"
}

# chocolatey path
$choco_path="C:\ProgramData\chocolatey\choco.exe"

# Links to binaries
# CHANGEME: The BinPath is depending on the structure of the archive itself
# Sometimes these you have to set manually, because the folder structure in
# extracted files will possibly change over time
if($install_method -ne "chocolatey"){

    Write-Host "Portable binaries used!"

    $bin = @(
            [PSCustomObject]@{Name="cmake"; BinDir=""; BinPath="\bin\cmake.exe"}
            [PSCustomObject]@{Name="mingit"; BinDir=""; BinPath="\cmd\git.exe"}
            [PSCustomObject]@{Name="flex"; BinDir=""; BinPath="\win_flex.exe"}
            [PSCustomObject]@{Name="vcpkg"; BinDir=$vcpkg_path; BinPath="$($vcpkg_path)vcpkg.exe"}
            [PSCustomObject]@{Name="cpack"; BinDir=""; BinPath="\bin\cpack.exe"}
            [PSCustomObject]@{Name="nsis"; BinDir=""; BinPath="\NSIS.exe"}
    #       [PSCustomObject]@{Name="pip"; BinDir=""; BinPath=""}
    #       [PSCustomObject]@{Name="python3"; BinDir=""; BinPath=""}
    )
}elseif($install_method -eq "chocolatey"){

    Write-Host "Chocolatey binaries used!"

    $bin = @(
            [PSCustomObject]@{Name="cmake"; BinDir="C:\Program Files\CMake"; BinPath="\bin\cmake.exe"}
            [PSCustomObject]@{Name="mingit"; BinDir="C:\Program Files\Git"; BinPath="\bin\git.exe"}
            [PSCustomObject]@{Name="flex"; BinDir="C:\ProgramData\chocolatey"; BinPath="\bin\win_flex.exe"}
            [PSCustomObject]@{Name="vcpkg"; BinDir=$vcpkg_path; BinPath="vcpkg.exe"}
            [PSCustomObject]@{Name="cpack"; BinDir="C:\Program Files\CMake"; BinPath="\bin\cpack.exe"}
    #       [PSCustomObject]@{Name="nsis"; BinDir=""; BinPath="\NSIS.exe"}
    #       [PSCustomObject]@{Name="pip"; BinDir=""; BinPath=""}
    #       [PSCustomObject]@{Name="python3"; BinDir=""; BinPath=""}
    )

}

# Paths for env:PATH

## not set for normal installation of this script
## $preinstalled_qt_dir = ""

$set_env_path = @(
                [PSCustomObject]@{Path="$($build_dir_arch)libopenage\$($build_config)\";}
                [PSCustomObject]@{Path="$($build_dir_arch)nyan-external\bin\nyan\$($build_config)\";}
                [PSCustomObject]@{Path="$($vcpkg_path)installed\$($relevant_config_oa)\bin\";}
                [PSCustomObject]@{Path="$($build_dir_arch)libopenage\$($build_config)\";}
#               If prebuilt QT5 was installed, not needed with vcpkg 
#               [PSCustomObject]@{Path="$($preinstalled_qt_dir)\bin\";}
)

# Optimized form for env:PATH, just one function call
#$set_env_paths="D:\openage-dl\python3\Scripts;$($build_dir_arch)libopenage\$($build_config)\;$($build_dir_arch)nyan-external\bin\nyan\$($build_config)\;$($vcpkg_path)installed\$($relevant_config_oa)\bin\;$($build_dir_arch)libopenage\$($build_config)\"

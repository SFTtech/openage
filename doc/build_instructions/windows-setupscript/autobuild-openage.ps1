# Copyright 2019 the openage authors. See LICENSE for legal info.
# Licensed under GNU Affero General Public License v3.0

# IMPORTANT NOTE!
# You need to set the following command manually, to run this script on a standard Win10 machine
# don't close the Powershell afterwards because for security reasons scripts are just allowed
# for the current powershell process
# >>> Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope Process


PARAM(
[string]$variables_file="$PSScriptRoot\includes\autobuild-vars.ps1",
[string]$functions_file="$PSScriptRoot\includes\autobuild-functions.ps1"
)


# Evil Dot-Sourcing of file containing variables
. "$($variables_file)"

# Evil Dot-Sourcing of file containing scripting functions
. "$($functions_file)"


## Main
if($progress -eq "dependencies"){
        
  if($install_method -eq "portable"){

        # Import from version file
        $deps = GetFromFile $version_file

        # Create Directory for dependency downloads
        GenerateDepFolders -arr $deps -path $dependency_dl_path

        # Get Latest from Github
        GetVersionLink -arr $deps -path ""

        # Generate FileNames from Link
        GenerateFileNames $deps

        # Download all Dependencies
        DownloadDependencies $deps

        # Extract Dependencies
        ExtractDependencies -arr $deps -path $dependency_dl_path

        # Link config files
        ConnectConfigs $deps

        # Install Dependencies
        InstallDependencies $deps

        # Get Links to Binaries
        GetBinaryLinks $bin

        ### 
        #
        # Installed: Python 3.7.3, VS build tools, NSIS
        # Downloaded and extracted: cmake, (min)git, flex 
        # Python should be in PATH from here (from python installer)
        #
        # NEW: cmake, git, flex, vcpkg should be in env:Path
        #
        # NEW: pip modules should be installed
        #
        ###

      # Continue with vcpkg
      # $progress="vcpkg"

 }elseif($install_method -eq "chocolatey"){

        # Import dep file for chocolatey
        $deps = GetFromFile $choco_dep_file 
                
        # Create Directory for dependency downloads
        GenerateDepFolders -arr $deps -path $dependency_dl_path

        # Get Latest from Github
        GetVersionLink -arr $deps -path ""

        # Generate FileNames from Link
        GenerateFileNames $deps

        # Download all Dependencies
        DownloadDependencies $deps

        # after extraction process the folderstructure should be hashed and saved
        # or at least important files / executables - think through, how to save time
        # space and computing power

        # Extract Dependencies
        ExtractDependencies -arr $deps -path $dependency_dl_path

        # Link config files
        # TODO This should be called in dependency of existing 
        # config-files not every time
        ConnectConfigs $deps

        # Install Dependencies
        # TODO Should test for already installed dependencies first
        # We can use hashes to test in $bin
        # otherwise jump over them or even remove possible conflict files
        InstallDependencies $deps

        # Install chocolatey
        iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
      
        # Allow global confirmation
        ChocoBin -cmd "feature enable" -software "-n allowGlobalConfirmation"

        # DEBUG
        #ChocoBin -cmd uninstall -software "chocolateygui git git.install winflexbison3 nsis.portable dejavufonts doxygen.install Graphviz cmake cmake.install chocolatey"
                
        # Invoke chocolatey to install deps
        ChocoBin -cmd install -software "git.install winflexbison3 nsis.portable dejavufonts doxygen.install Graphviz"
        ChocoBin -cmd install -software "cmake --installargs 'ADD_CMAKE_TO_PATH=System'"
        
        # Import bin file for chocolatey
        $bin = GetFromFile $choco_bin_file

        #Set environment variables
        $bin | %{ SetEnvPath -file_path "$($_.BinDir)$($_.BinPath)" }
        

          ###
          #
          # Installed: Python 3.7.3, VS build tools, NSIS, cmake, git, flex 
          # Python should be in PATH from here (from python installer)
          #
          # NEW: cmake, git, flex, vcpkg should be in env:Path
          #
          # NEW: pip modules should be installed now
          #
          ###

      # Continue with vcpkg
      $progress="vcpkg"

    }
} # End dependencies portable/chocolatey

if("$($progress)" -eq "vcpkg"){
 
 
        # Refresh environment paths
        # refreshenv

        # Install Python pip modules
        Write-Host "Please wait, while we are installing Python modules ..."
        $pip = & Start-Process -FilePath "pip.exe" -ArgumentList "install $($pip_modules)" -Wait
  

        # Test for vcpkg.exe diff
        Write-Host "We are testing for existing vcpkg.exe ..."

        if((Test-Path "$($vcpkg_path)vcpkg.exe.sha512" -PathType Leaf)){
            $vcpkg_hash_comp = Get-FileHash -Path "$($vcpkg_path)vcpkg.exe" -Algorithm SHA512
            $vcpkg_hash_imp = GetFromFile "$($vcpkg_path)vcpkg.exe.sha512"
            $diff = Compare-Object $vcpkg_hash_comp $vcpkg_hash_imp
            
            if($diff){
                 Write-Host "Please wait, while we are compiling vcpkg ..."
                $bat = & "$($vcpkg_path)scripts\bootstrap.ps1"
            }else{ Write-Host "We don't need to recompile." }

        }else { 
                # Clone vcpkg
                Write-Host "Please wait, while we are cloning vcpkg ..."
                GitBin -address "https://github.com/Microsoft/vcpkg.git" -action "clone" -path $vcpkg_path

                Write-Host "Please wait, while we are compiling vcpkg ..."
                $bat = & "$($vcpkg_path)scripts\bootstrap.ps1" 

                # Get Hash of vcpkg.exe
                $vcpkg_hash = Get-FileHash -Path "$($vcpkg_path)vcpkg.exe" -Algorithm SHA512
                WriteToFile -arr $vcpkg_hash -dir $($vcpkg_path) -filename "vcpkg.exe.sha512"

        }
        

        # Integrating vcpkg into OS environment
        $job = & VcpkgBin -cmd "integrate install"

        # Builds defined packages for the defined architecture 
        $job = & VcpkgBin -cmd "install" -software $vcpkg_deps

        ### 
        #
        # Till here everything works out somewhat nicely
        # Installed: Python 3.7.3, VS build tools, NSIS
        # Downloaded and extracted: cmake, (min)git, flex 
        # Python should be in PATH from here (from python installer)
        # cmake, git, flex, vcpkg should be in env:Path
        #
        # NEW: all the Vcpkg-Packages should be built in $arch-Version
        #
        ###

        # Saving binary paths and dependencies
        WriteToFile -arr $bin -dir $($dependency_dl_path) -filename "binaries.json"
        WriteToFile -arr $deps -dir $($dependency_dl_path) -filename "recent-deps.json"


        # Ready
        Write-Host "Here we are ready for cmake configuring and the environment should be set up."

        #Jump to next chapter
        #$progress = "compile"
}

# We will jump here if we are sure that we have all dependencies
# Set flag in the beginning of script
if($progress -eq "compile"){

        Write-Host "Now we are compiling!"

        # TODO
        # Check for choco/flag and write to includes with prefix choco-


        # Import from version file
        # $deps = GetFromFile "$($dependency_dl_path)recent-deps.json"
        # Import binaries from file
        $bin = GetFromFile "$($dependency_dl_path)binaries.json"


        ####
        #
        #
        # Building of openage could start from now on
        # TODO Check to clean the Build-Dir before making new build
        #
        ####

        # Clone openage
        Write-Host "Please wait, while we are cloning openage ..."
        # $openage_src_dir=GitBin -address "https://github.com/SFTtech/openage.git" -action "clone" -path "$($dependency_dl_path)openage"

        # Create build dir depending on building architecture
        $build_dir_arch="$($build_dir)$($cmake_arch)"
        GenerateFolders $build_dir_arch
        Set-Location $build_dir_arch

        # cmake Commands
        $vcpkg_toolchain ="-DCMAKE_TOOLCHAIN_FILE='$($vcpkg_path)scripts\buildsystems\vcpkg.cmake'"
        $build_flag="-G '$($build_vs_ver) $($cmake_arch)'"

        # included forced nyan download due to problems for recompiling with empty build_dir
        $nyan_dl="-DDOWNLOAD_NYAN='YES' -DFORCE_DOWNLOAD_NYAN='YES'"

        if($install_method -eq "chocolatey"){
            # flex binary
            $flex_path = ($bin | ?{$_.Name -eq "flex"} | %{ "$($_.BinDir)$($_.BinPath)" })

        }else{

             # flex binary
             $flex_path = ($bin | ?{$_.Name -eq "flex"} | %{ "$($_.BinPath)" })
    
        }

        $flex_exe="-DFLEX_EXECUTABLE='$($flex_path)'"
        
        CmakeBin -cmd "$($vcpkg_toolchain) $($nyan_dl) $($flex_exe) $($build_flag)" -src_dir "$($openage_src_dir)\" -build_dir $build_dir_arch
 
        # Set-Location $build_dir
        # $job = & Start-Process -FilePath "cmake" -ArgumentList "--build . --config RelWithDebInfo -- /nologo /m /v:m" -Wait
        CmakeBin -cmd "--build . --config $($build_config) -- /nologo /m /v:m" -src_dir "" -build_dir $build_dir_arch

        
        # For testing, prebuilt nyan
        # C:\openage-dl\cmake\cmake-3.14.5-win64-x64\bin\cmake.exe -DCMAKE_TOOLCHAIN_FILE='C:\openage-dl\vcpkg\scripts\buildsystems\vcpkg.cmake' -DNYAN_DIR='C:\openage-build\nyan\' -DFLEX_EXECUTABLE='C:\openage-dl\flex\win_flex_bison-2.5.18\win_flex.exe' -G 'Visual Studio 15 2017 Win64' C:\openage-dl\openage\

        #Jump to next chapter
        #$progress = "run"

}

if($progress -eq "env"){
        
        # Reset build_dir_arch
        $build_dir_arch="$($build_dir)$($cmake_arch)\"

        
        Write-Host "Setting up environment variables ..."

        # Set our environment paths, edit in autobuild-vars.ps1 bottom
        $set_env_path | %{
        
                SetEnvVar -env_var_name "path" -env_var_value $_.Path
        
        }

        # QML2_IMPORT_PATH to $($vcpkg_path)installed\$($relevant_config_oa)\qml
        SetEnvVar -env_var_name "QML2_IMPORT_PATH" -env_var_value "$($vcpkg_path)installed\$($relevant_config_oa)\qml"

        # FONTCONFIG_PATH -> $($vcpkg_path)installed\$($relevant_config_oa)\tools\fontconfig\fonts
        SetEnvVar -env_var_name "FONTCONFIG_PATH" -env_var_value "$($vcpkg_path)installed\$($relevant_config_oa)\tools\fontconfig\fonts"

        # AGE2DIR -> VARIABLE TO SET BY USER? <STD-PATH>
        SetEnvVar -env_var_name "AGE2DIR" -env_var_value $aoe_std_path
        
        # TODO if not chocolatey
        if($install_method -ne "chocolatey"){
            
               # Install DejaVuSerif
               Write-Host "Installing DejaVuSerif ..."

               # Copy ttf/DejaVuSerif*.ttf font files to %WINDIR%/Fonts.
               # \dejavu-fonts-ttf-2.37\ttf\DejaVuSerif*.ttf
               # InstallFont -Path $font_path
        
               # zuntrax style
               # Copy-Item $FontFile $env:windir\fonts
               # New-ItemProperty -Name $FontName -Path "HKLM:\Software\Microsoft\Windows NT\CurrentVersion\Fonts" -PropertyType string -Value $FontFile

               # TODO Fontconfig copying also chocolatey
               # Non-chocolatey: <dependency-path>\dejavufont\dejavu-fonts-ttf-2.37\\fontconfig\57-dejavu-serif.conf to %FONTCONFIG_PATH%/conf.d.

        }elseif($install_method -ne "chocolatey"){
                
               # Install DejaVuSerif
               Write-Host "Installing DejaVuSerif-Fontconfig to ..."

               # TODO Fontconfig copying also chocolatey
               # Chocolatey: C:\ProgramData\chocolatey\lib\dejavufonts\dejavu-fonts-ttf-2.37\fontconfig\57-dejavu-serif.conf to %FONTCONFIG_PATH%/conf.d.

        }


        # Jump to next chapter
        #$progress = "package"
}

if($progress -eq "package"){

        Write-Host "Now we are packaging!"

        # Create build dir depending on building architecture
        $build_dir_arch="$($build_dir)$($cmake_arch)"
        GenerateFolders $build_dir_arch
        Set-Location $build_dir_arch

        # Import from version file
        # $deps = GetFromFile "$($dependency_dl_path)recent-deps.json"

        # Import binaries from file
        $bin = GetFromFile "$($dependency_dl_path)binaries.json"

        #cpack -C RelWithDebInfo -V | Out-File -FilePath "C:\openage-build\cpack-output.txt"
        $cpack_logfile="$($build_dir_arch)cpack-output.txt"
        CpackBin -cmd "-C $($build_config) -V | Out-File -FilePath $($cpack_logfile)" -src_dir "" -build_dir ""

        # Jump to next chapter
        # $progress="cleanup"
        
}

if($progress -eq "cleanup"){

        # TODO Cleanup
        # CleanUp $deps
        # Cleanup environment variables for bin-paths

        # Cleanup old archive versions

}

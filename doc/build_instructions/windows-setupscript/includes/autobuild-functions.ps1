# Copyright 2019 the openage authors. See LICENSE for legal info.
# Licensed under GNU Affero General Public License v3.0

# Functions for autobuildscript-openage

# Write version file
Function WriteToFile($arr, [string]$dir, [string]$filename="versions.json"){
    
    # GenerateFolders
    GenerateFolders $dir

    # Write FileOut
    $arr | ConvertTo-Json -depth 100 | Out-File "$($dir)$($filename)"
    
}

# Import versions from file
Function GetFromFile{
    Param([string]$versionfile_path)
    
    if($versionfile_path){
        Write-Host "File will be imported from $versionfile_path"
        return (Get-Content -Raw -Path $versionfile_path | ConvertFrom-Json)
    }
    
}

# Function to create folderstructure
Function GenerateFolders($path){
    $global:foldPath=$null
    
    foreach($foldername in $path.split("\")){
          $global:foldPath+=($foldername+"\")

          if(!(Test-Path $global:foldPath)){
              New-Item -ItemType Directory -Path $global:foldPath
              Write-Host "$global:foldPath Folder created successfully!"
          }elseif((Test-Path $global:foldPath)){
              # Write-Host "$global:foldPath Folder already exists!"
          }

          
         # elseif($DRY_RUN){
         #        Write-Host "DRYRUN: $global:foldPath folder was not created!"
         #}

    }
   
   # Not sure if we really need that, commented out 
   #return $global:foldPath    
}

# Create subfolder for each dependency and save in $deps
Function GenerateDepFolders{
    Param($arr, [string]$path)

        $arr | ForEach-Object { 
            GenerateFolders "$($path)$($_.Name)"
            $_.HomeDir = "$($path)$($_.Name)\"
        }    
    
}

# Function to download dependency setups
# TODO Do not redownload already extracted deps
Function DownloadDependencies($arr){

     Write-Host "Downloading dependencies ..."
    
     $arr | ForEach-Object {

            # Alternative download command, not working with Github! 
            # Start-BitsTransfer -Source $_.HashFile -Destination $_.HomeDir -Asynchronous

            $source = "$($_.DownloadLink)"
            $output = "$($_.HomeDir)$($_.FileName)"

            # Don't download if file already there
            if( !(Test-Path $output)){
           
                Write-Host "Downloading $($_.Name) $($_.LatestRelease) ..."

                # Debug
                Write-Host $_.DownloadLink
                Write-Host $output


                [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
            
                if(($_.Name) -eq "vs_buildtools"){

                        # Visual Studio 17 CE (advanced options) - Build tools
                        $job = Invoke-WebRequest -Uri $source  -OutFile $output -Headers @{"method"="GET"; "authority"="download.visualstudio.microsoft.com"; "scheme"="https"; "path"="/download/pr/10413969-2070-40ea-a0ca-30f10ec01d1d/414d8e358a8c44dc56cdac752576b402/vs_buildtools.exe"; "upgrade-insecure-requests"="1"; "dnt"="1"; "user-agent"="Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36"; "accept"="text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3"; "referer"="https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=BuildTools&rel=15"; "accept-encoding"="gzip, deflate, br"; "accept-language"="en-US,en;q=0.9"}
                 

                }elseif(($_.Name) -eq "nsis"){
                    
                        # NSIS
                        $job = Invoke-WebRequest -Uri $source -OutFile $output -Headers @{"Upgrade-Insecure-Requests"="1"; "DNT"="1"; "User-Agent"="Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36"; "Accept"="text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3"; "Referer"="https://sourceforge.net/projects/nsis/files/NSIS%203/3.04/nsis-3.04-setup.exe/download?use_mirror=netix&download="; "Accept-Encoding"="gzip, deflate, br"; "Accept-Language"="en-US,en;q=0.9"; "Cookie"="eupubconsent=BOhQ8W7OhQ8W7AKASAENAAAA-AAAAA; euconsent=BOhQ8W7OhQ8W7AKASBENCU-AAAAnd7_______9______9uz_Ov_v_f__33e87_9v_l_7_-___u_-3zd4u_1vf99yfm1-7etr3tp_87ues2_Xur__59__3z3_9phPrsk89r6337A; googlepersonalization=OhQ8W7OhQ8W7gA; _cmpRepromptOptions=OhQ8W7OhQ8W7IA"}
                

                }elseif(($_.Name) -eq "dejavu_font"){

                        # Dejavu-Font
                        Invoke-WebRequest -Uri $source -OutFile $output -Headers @{"Upgrade-Insecure-Requests"="1"; "DNT"="1"; "User-Agent"="Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36"; "Accept"="text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3"; "Referer"="https://sourceforge.net/projects/dejavu/files/dejavu/2.37/dejavu-fonts-ttf-2.37.zip/download"; "Accept-Encoding"="gzip, deflate, br"; "Accept-Language"="en-US,en;q=0.9"; "Cookie"="eupubconsent=BOhQ8W7OhQ8W7AKASAENAAAA-AAAAA; euconsent=BOhQ8W7OhQ8W7AKASBENCU-AAAAnd7_______9______9uz_Ov_v_f__33e87_9v_l_7_-___u_-3zd4u_1vf99yfm1-7etr3tp_87ues2_Xur__59__3z3_9phPrsk89r6337A; googlepersonalization=OhQ8W7OhQ8W7gA; _cmpRepromptOptions=OhQ8W7OhQ8W7IA"}
                       
                }else{
                        # Other dependencies without referrer
                        # But gets config-files for the vs-installer as well
                        $job = Invoke-WebRequest -Uri $source -OutFile $output
                                    
                }

                While ($job.JobState -eq "Transferring") {
                         Sleep -Seconds 3
                }

    
            }elseif(Test-Path $output){
           
              Write-Host "$output already exists! Continuing with already existing file."
    
            }

    }

}

# Function to generate FileName and FilePath from DownloadLink
# TODO Search for function, depreciated?
Function GenerateFileNames($arr){

    Write-Host "Generating Filenames ..."

    $arr | ForEach-Object {
           $_.FileName = "$($_.DownloadLink.SubString($_.DownloadLink.LastIndexOf('/') + 1))"
          
           # Debug
           #Write-Host $_.HomeDir
           #Write-Host $_.FileName
    }
}


# Get the link for the (latest) version of a github repo
# Inspired by https://gist.github.com/f3l3gy/0e89dde158dde024959e36e915abf6bd
Function GetVersionLink{
	Param($arr, [string] $path)
 

    $arr | ForEach-Object {

           # Github
           if(($_.isGit) -eq $true){

               if(!($_.desiredVersion)){

               Write-Host "Get the latest version of Github-Releases ..."

               $releases = "https://api.github.com/repos/$($_.RepoName)/releases"
           
               Write-Host "Determining latest release for $($_.Name)"
               [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
                
               $versionRequest = ((ConvertFrom-Json -InputObject (Invoke-WebRequest -Uri  $releases -UseBasicParsing)) | Where {$_.prerelease -eq $false})[0]

               $_.desiredVersion = $false
               $_.LatestRelease = $versionRequest[0].tag_name

                    Write-Host "The latest release of $($_.Name) is $($_.LatestRelease)."

               }else{

                    Write-Host "Using hardcoded version $($_.desiredVersion) of $($_.Name)..."
                    $_.LatestRelease = $_.desiredVersion
               }

               #$_.DownloadLink = ($versionRequest | Where {$_.content_type -eq "application/zip"})
           

               # ADD NEW SOFTWARE LINK GENERATION HERE
               # set up for architecture-flag
               # >>>
          
               if(($_.Name) -eq "mingit"){
                   #MinGit-2.21.0-64-bit.zip
                   #MinGit-2.21.0-32-bit.zip
                   $name = "$($_.Name)-"
                   $version=($_.LatestRelease.Substring(1))
                   $version= $version.Substring(0,$version.Length-10)
                   $arch=$git_dl_arch
                   $type=".zip"

               }elseif(($_.Name) -eq "cmake"){
                   #cmake-3.14.5-win64-x64.zip 
                   #cmake-3.14.5-win32-x86.zip
                   $name = "$($_.Name)-"  
                   $version=$_.LatestRelease.Split("v")[1]
                   $arch=$cmake_dl_arch
                   $type=".zip"
           
               }elseif(($_.Name) -eq "flex"){
                   #win_flex_bison-2.5.18.zip 
                   $name = "win_$($_.Name)_bison-"  
                   $version=$_.LatestRelease.Split("v")[1]
                   $arch=""
                   $type=".zip"
           
               
               }

	       #ADD NEW SOFTWARE LINK GENERATION FOR GITHUB HERE
               # >>> elseif() <<<

               $file="$name$version$arch$type"
    
               # Debug
               # Write-Host "DownloadLink for $($_.RepoName) is"
               
               $_.DownloadLink="https://github.com/$($_.RepoName)/releases/download/$($_.LatestRelease)/$($file)"
               
               # Debug
               # Write-Host $_.DownloadLink

 

          }elseif(($_.isGit) -eq $false){

                $_.LatestRelease = $_.desiredVersion

                if(($_.Name) -eq "python3"){

                       $arch=$python_dl_arch
                       $_.DownloadLink="https://www.python.org/ftp/python/$($_.desiredVersion)/python-$($_.desiredVersion)$($arch).exe"


                }elseif(($_.Name) -eq "nsis"){

                         # Sourceforge
                         $_.DownloadLink="https://kent.dl.sourceforge.net/project/nsis/NSIS%203/$($_.desiredVersion)/nsis-$($_.desiredVersion)-setup.exe"
                         
            
                }elseif(($_.Name) -eq "dejavu_font"){

                         # Sourceforge
                         $_.DownloadLink="https://kent.dl.sourceforge.net/project/dejavu/dejavu/$($_.desiredVersion)/dejavu-fonts-ttf-$($_.desiredVersion).zip"

                }     


         }
     
    }

}


# Extract Zip-Files and delete archives afterwards
# Sets the HomeDir to the extracted new Folder structure
Function ExtractDependencies{
    Param($arr, [string]$path)

    $arr | ForEach-Object {
    
        if(($_.isZip) -eq $true){
            
            Set-Location -Path $_.HomeDir
          
            $zip = "$($_.HomeDir)$($_.FileName)"

            if(!(Test-Path($zip.Substring(0,$zip.Length-4)))){
                
                if(($_.Name) -eq "flex"){
                    $folder = ($zip.Substring(0,$zip.Length-4))
                    GenerateFolders $folder
                    $_.HomeDir = $folder
                                
                }elseif(($_.Name) -eq "mingit"){
                    $folder = ($zip.Substring(0,$zip.Length-4))
                    GenerateFolders $folder
                    $_.HomeDir = $folder
                                
                } 
                
                Write-Host "Extracting $($_.Name) files"
                Expand-Archive -LiteralPath $zip -DestinationPath $_.HomeDir -Force
               
                # Removes item after extraction DEPRECIATED
                # Remove-Item  $zip  -Force

                # TODO Call archive function, move to archive

                $_.HomeDir = $zip.Substring(0,$zip.Length-4)
            } else {
                Write-Host "Extracted version is already existing! Continuing without extraction!"
                $_.HomeDir = $zip.Substring(0,$zip.Length-4)
            }

            Set-Location -Path $path

            # Debug
            # Write-Host $_.HomeDir 

        }
        
    }

}

# Connect a downloaded config file to a setup
Function ConnectConfigs($arr){

   Write-Host "Connecting Configs ..."
    
   $arr | ForEach-Object {
          if(($_.isConfig) -eq $true){
          
            # LinkName should not be empty
            if($_.LinkName){
                
               $linkname_temp = $_.LinkName
               $cfg_path_temp = "$($_.HomeDir)$($_.FileName)"
                
               $change = ($arr | Where {$($_.Name) -eq $($linkname_temp)})  
               $change.ConfigPath = $cfg_path_temp

            }
          }
    
    }

 }
 
# Install the dependencies
Function InstallDependencies($arr){


    $arr | ForEach-Object {
         if(($_.isInstaller) -eq $true){


           if(($_.Name) -eq "python3"){

             # Installer Routine for Python
             Write-Host "Installing $($_.Name), this can take a longer time. Do not close this window!"

             $setup = Start-Process "$($_.HomeDir)$($_.FileName)" -ArgumentList "/s /passive Include_debug=1 Include_dev=1 Include_lib=1 Include_pip=1 PrependPath=1 CompileAll=1 InstallAllUsers=1 TargetDir=$($_.HomeDir)" -Wait
             if ($setup.exitcode -eq 0){
                # TODO Not Working
                write-host "$($_.Name) installed succesfully."
             }

              # TODO Add install-dir to object
              # $_.InstallDir = "$($_.HomeDir)"

           }elseif(($_.Name) -eq "vs_buildtools"){

             # Installer Routine for VS Buildtools
             Write-Host "Installing $($_.Name), this can take a longer time. Do not close this window!"
             $setup = Start-Process "$($_.HomeDir)$($_.FileName)" -ArgumentList "-p --addProductLang En-us --norestart --noUpdateInstaller --downloadThenInstall --force --config $($_.ConfigPath)" -Wait

             if ($setup.exitcode -eq 0){
                # TODO Not Working
                write-host "$($_.Name) installed succesfully."
             }

              # TODO Add install-dir to object
              # $_.InstallDir = "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools"

           }elseif(($_.Name) -eq "nsis"){

             # Installer Routine for NSIS
             Write-Host "Installing $($_.Name), this can take a longer time. Do not close this window!"
             $setup = Start-Process "$($_.HomeDir)$($_.FileName)" -ArgumentList "/S /D=$($_.HomeDir)$($_.Name)-$($_.LatestRelease)\" -Wait
             
             if ($setup.exitcode -eq 0){
                # TODO Not Working
                write-host "$($_.Name) installed succesfully."
             }
            
             # TODO Add install-dir to object
             # Set InstallDir to directory of install for later use for $bin and $PATH
             # $_.InstallDir = "$($_.HomeDir)$($_.Name)-$($_.LatestRelease)"
         
           }elseif(($_.Name) -eq "dejavu_font"){

             # TODO Installer Routine for dejavu_font
             Write-Host "Installing $($_.Name), this can take a longer time. Do not close this window!"
             #$setup = Start-Process "$($_.HomeDir)$($_.FileName)" -ArgumentList "/S /D=$($_.HomeDir)$($_.Name)-$($_.LatestRelease)\" -Wait
             
             if ($setup.exitcode -eq 0){
                # TODO Not Working
                write-host "$($_.Name) installed succesfully."
             }

           }

         }

    }

}


# Get direktlink to binaries
Function GetBinaryLinks($arr){

      $arr | ForEach-Object {
                
               # search cache
               $is_in_path=$false;

               
               $name_temp = $_.Name
               
               # HACK: Set "cpack" as "cmake" for same directory
               if($($_.Name) -eq "cpack"){
                $name_temp = "cmake"
               }
               $lookup = ($deps | Where {$($_.Name) -eq $($name_temp)})  
               $_.BinDir = $lookup.HomeDir
               $_.BinPath = "$($lookup.HomeDir)$($_.BinPath)"
               
               # Debug for BinaryLinks
               # Write-Host $_.BinPath

               # Add Directory of executable to environent $PATH variable
               SetEnvVar -env_var_name "path" -env_var_value $_.BinPath
                                
     }
    
}

# Archive Function
Function ArchiveController($arr,$action=""){
    
    # $action variable for "search" or "move" or "clean up old archives"

    # TODO Archives doesn't need to be deleted while Testing
    # TODO instead of Remove-Item move to Archive-Folder
    # > and save Version string
    
    # Move succesfully extracted archive to subfolder structure
    # needed vars: $deps array for filenames and folderstructure

    # Search in archive for already downloaded files before download
    # return file-path for already downloaded archive
    # else return $false
    # needed vars: $deps-part for search

    # OTHER OLDER NOTES:
    
        # TODO: Check for already downloaded, extracted installed files
        # Already downloaded files could be compared by filename in archive
        # Successfully extracted files should be moved to archive with archive/name/ 
        # structure and be stored there, then names should be compared 
        # with already downloaded files. If an compressed file is not moved to archive
        # it was not extracted successfully - so delete it

        # So in short: Files in archive dont need to be downloaded again

}

# DEPRECIATED: Set up environment $PATH variable, if not already set
Function SetEnvPath{
    PARAM($file_path)


     Write-Host "This function is depreciated > SetEnvPath"
     Write-Debug "This function is depreciated > SetEnvPath"


     # Split env:Path
     #$split_path = $env:Path.Split(";")
     
     # Get Dir from File
     # INFO Set from +1 to + 0 to strip away also the last backslash
     # Double environment variables come from here, if any
     #$file_dir = "$($file_path.SubString(0,($file_path.LastIndexOf('\') + 0)))"
     
     # Test for the variable already being there
     #$split_path | ?{"$($file_dir)" -eq "$($_)"} | %{ 
                    
                       # Debug
     #                  Write-Host "$($file_dir) is already in env:Path!" 
     #                  $is_in_path = $true
              
     #              }

    #if(!($is_in_path)){
        # Debug
        #Write-Host "Write directory $($file_dir) to $($env:Path)."
        #Set-Item -path env:Path -value "$($env:Path);$($file_dir)"
    #    SetEnvVar -env_var_name "path" -env_var_value "$($env:Path);$($file_dir)"
    #}

}


# Set an environment variable
# IMPORTANT: Always make sure your paths end with '\' or the executable filename
# everything until the last '\' will be stripped away: <dir1>\<dir2>\ {*.exe}-> stripped
# if your forget the last '\' <dir2> would be stripped!
Function SetEnvVar([STRING]$env_var_name,[STRING] $env_var_value, $isbinary=$false, $overwrite=$false){
    
     # Check if $env_var_name is already in env
     if(Test-Path ENV:$env_var_name){ 
        Write-Host "Found the variable!" 
            
            $read_env_val = (Get-ChildItem Env:$env_var_name).Value


            # For testing for more then one value in env:$env_var_name
            $read_env_val_split=$read_env_val.Split(";")
            $read_env_val_count = $read_env_val_split | measure
            $read_env_val_split = ($read_env_val_split | %{

                                        if($_.substring($_.length-1, 1) -eq "\"){ 
                                        "$($_.SubString(0,($_.LastIndexOf('\') + 0)))"
                                        }else{
                                            $_
                                        }
                                   })

   
            # For testing for more then one value in $env_var_value
            $env_var_value_split=$env_var_value.Split(";")
            $env_var_value_count = $env_var_value_split | measure
            $env_var_value_split = ($env_var_value_split | %{

                                        if($_.substring($_.length-1, 1) -eq "\"){ 
                                        "$($_.SubString(0,($_.LastIndexOf('\') + 0)))"
                                        }else{
                                            $_
                                        }
                                   })


            # Debug 
            Write-Host "$($read_env_val_count.Count) already in env:$env_var_name"
            Write-Host "$($env_var_value_count.Count) to test for adding!" 


            $diff = Compare-Object -ReferenceObject $read_env_val_split -DifferenceObject  $env_var_value_split
            $diff_clean = ($diff | ?{$_.sideindicator -eq "=>"} |  %{ $_ })
            $diff_clean_count = ($diff_clean | measure)

            Write-Host "$($diff_clean_count.Count) Values to add"

            if($diff_clean_count.Count -cge 1){

                    $diff_clean | %{ 
                        Write-Host "Adding $($_.InputObject) to env:$env_var_name"
                        # TODO we can just create it!
                        # [System.Environment]::SetEnvironmentVariable("$($env_var_name)", "$($env_var_value)")
                    }
             }

     }else{
     
        Write-Host "Didn't find the variable! Creating it ..."
        Write-Host "Adding $env_var_value to env:$env_var_name"
        # TODO we can just create it!
        # [System.Environment]::SetEnvironmentVariable("$($env_var_name)", "$($env_var_value)")
     }


    # EXAMPLE Set persistent environment variables for user, otherwhise without the third parameter
    # EXAMPLE [System.Environment]::SetEnvironmentVariable('DOCKER_HOST', 'tcp://docker.artofshell.com:2376', [System.EnvironmentVariableTarget]::User)


}


# Git imitating
Function GitBin([string]$address,[string]$action, [string]$path){

    GenerateFolders $path

    
    if($install_method -eq "chocolatey"){
    
        $git = ($bin | Where {$($_.Name) -eq $("mingit")} | %{"$($_.BinDir)$($_.BinPath)"})


    }else{

        # Git Binary
        $git = ($bin | Where {$($_.Name) -eq $("mingit")} |%{ $_.BinPath })

    }
     
        Start-Process -FilePath $git -ArgumentList "$($action) $($address) $($path)" -Wait

        return $path

}

# Vcpkg imitating
Function VcpkgBin([string]$cmd ="/help",[string]$software=""){

    
    if($install_method -eq "chocolatey"){
    
        # vcpkg Binary
        $vcpkg = ($bin | Where {$($_.Name) -eq $("vcpkg")} | %{ "$($_.BinDir)$($_.BinPath)"})

    }else{

        # vcpkg Binary
        $vcpkg = ($bin | Where {$($_.Name) -eq $("vcpkg")} | %{ $_.BinPath } )
    
    }
   

    if($($cmd) -eq "integrate install"){
       
      # integrate in OS for easier use
      $job = & Start-Process -FilePath $vcpkg -ArgumentList "$($cmd)" -Wait

    
    }elseif($($cmd) -eq "install"){

      # install Software in $software with Architecture
      $job = & Start-Process -FilePath $vcpkg -ArgumentList "$($cmd) $($vcpkg_arch) $($software)" -Wait
    }

}

# Cmake imitating
Function CmakeBin([string]$cmd ="/help",[string]$src_dir="",[string]$build_dir=""){


    if($install_method -eq "chocolatey"){
    
          # cmake Binary
          $cmake = ($bin | Where {$($_.Name) -eq $("cmake")} | %{ "$($_.BinDir)$($_.BinPath)"})

    }else{

          # cmake Binary
          $cmake = ($bin | Where {$($_.Name) -eq $("cmake")} | %{ $_.BinPath })
    
    }
   
      #Jump to build directory
      Set-Location $build_dir

      # Call cmake
      # $job = & Start-Process -FilePath $cmake -ArgumentList "$($cmd) $($src_dir)" -Wait

      # DEBUG
      Write-Host "Please manually call the command: '$($cmake)' $($cmd) $($src_dir)"


}

# Cpack imitating
Function CpackBin([string]$cmd ="/help",[string]$src_dir="",[string]$build_dir=""){

    if($install_method -eq "chocolatey"){
    
        $cpack = ($bin | Where {$($_.Name) -eq $("cpack")} | %{ "$($_.BinDir)$($_.BinPath)"})

    }else{

         # cpack Binary
         $cpack = ($bin | Where {$($_.Name) -eq $("cpack")} | %{ $_.BinPath })
    
    }

      
      #Jump to build directory
      Set-Location $build_dir

      # Call cmake
      #$job = & Start-Process -FilePath $cpack -ArgumentList "$($cmd) $($src_dir)" -Wait

      # DEBUG
      Write-Host "Please manually call the command: $($cpack.BinPath) $($cmd) $($src_dir)"

}

# Choco imitating
Function ChocoBin([string]$cmd ="/help",[string]$software=""){
    
    if($($cmd) -eq "uninstall"){
       
          # integrate in OS for easier use
          $job = & Start-Process -FilePath $choco_path -ArgumentList "$($cmd) $($software) -y" -Wait

    
    }elseif($($cmd) -eq "install"){

          # install Software in $software with Architecture
          $job = & Start-Process -FilePath $choco_path -ArgumentList "$($cmd) $($software)  $($choco_arch) --no-progress -y" -Wait

    }elseif($($cmd) -eq "upgrade"){

          # upgrade Software in $software with Architecture
          $job = & Start-Process -FilePath $choco_path -ArgumentList "$($cmd) $($software) $($choco_arch) --no-progress -y" -Wait

    }else{

          # other
          $job = & Start-Process -FilePath $choco_path -ArgumentList "$($cmd) $($software)" -Wait
    
    }

}

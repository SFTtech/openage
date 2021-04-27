@IF [%1] == [] echo "ERROR: Include the path to your site-packages folder as argument to this command" 
@IF [%1] == [] exit /b 
@cd \openage\build\openage\game\RelWithDebInfo
@copy *.pyd %1
@cd \openage\build\openage\log\RelWithDebInfo
@copy *.pyd \openage\build\openage\log\
@cd \openage\build\openage\cppinterface\RelWithDebInfo
@copy *.pyd \openage\build\openage\cppinterface\
@cd \openage\build\openage\cvar\RelWithDebInfo
@copy *.pyd \openage\build\openage\cvar
@cd \openage\build\openage\util\filelike\RelWithDebInfo
@copy *.pyd \openage\build\openage\util\filelike\
@cd \openage\build\openage\util\fslike\RelWithDebInfo
@copy *.pyd \openage\build\openage\util\fslike\
@echo "Successfully moved pyd files"
@cd \openage\build

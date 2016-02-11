cd build
cmake.exe ../openage -DCMAKE_BUILD_TYPE=Debug -DPYTHON=C:/Python34/python.exe -Dopusenc_EXECUTABLE=C:/anandy/Code/libraries/opus-tools-0.1.9-win32/opusenc.exe -DPKG_CONFIG_EXECUTABLE=C:/anandy/Code/libraries/pkg-config-lite-0.28-1/bin/pkg-config.exe -DFREETYPE_LIBRARY=C:/anandy/Code/libraries/freetype-2.4.6-win32-1/lib/freetype246MT.lib -DFREETYPE_INCLUDE_DIRS=C:/anandy/Code/libraries/freetype-2.4.6-win32-1/include -G "Visual Studio 12" %cmake_source_folder%

cd..
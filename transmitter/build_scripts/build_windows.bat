cd ..\build
cmake ..
msbuild Transmitter.vcxproj /p:Configuration=Release /p:Platform=x64

pause
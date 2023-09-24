cd ..\build
cmake ..
msbuild Receiver.vcxproj /p:Configuration=Release /p:Platform=x64

pause
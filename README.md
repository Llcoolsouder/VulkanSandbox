# Vulkan Tutorial
This is just my sandbox for learning and playing with Vulkan.  
I'll build something neat here. At the time of writing this I have no idea what that might be.  

# Building
This project _should_ be set up for cross platform building in CMake, but I'm only testing these builds on Windows.

## Dependencies
It should be obvious that this requires the Vulkan SDK. It might be less obvious that *you need to install the Vulkan SDK with GLM*.

## Build Steps
```bash
git submodule update --recursive --init
mkdir build && cd build
cmake ..
cmake --build .
```
 
# vectrl_proto
Prototype Video Editing Controller software

## Requirements
- CMake 3.8.2 or newer
- C++ compiler
  - Windows: Visual Studio 2022

## Setup
After cloning the repository, run the following command to download all submodules:

```sh
git submodule update --init --recursive
```

## Building
This project uses CMake as its build system. To use it, first create a folder to contain all the files
generated during the build process. For example, from the project's root folder, run:
```sh
md build
```
Then, enter the folder and run CMake, poiting it to the project's root folder as follows:
```sh
cd build
cmake ..
```
Finally, to build the software, run:
```sh
cmake --build .
```

REMINDER: Every time changes are made to CMakeLists.txt, `cmake ..` needs to be rerun from the created
build folder.
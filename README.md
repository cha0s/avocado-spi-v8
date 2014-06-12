# Avocado v8 SPI implementation

# Building

In order to build this project, you will need:

* git (obviously, gotta get the sources somehow!)
* cmake
* g++ (on Ubuntu, do *sudo apt-get install build-essential*)
* Boost (specifically, the filesystem, regex, and system components)

and v8 dependencies:

* svn

First, check out the project:

```
git clone --recursive https://github.com/cha0s/avocado-spi-v8.git
```

(Note the --recursive flag)

Navigate to the avocado-spi-v8 directory and run these commands:

```
mkdir build
cd build
cmake -DAVOCADO_CPP_CORE_DIRECTORY=/the/path/to/avocado-cpp-core ..
make
```

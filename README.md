# libKitsunemimiCpu

![Github workfloat status](https://img.shields.io/github/workflow/status/kitsudaiki/libKitsunemimiCpu/build-and-test/develop?label=build%20and%20test&style=flat-square)
![GitHub tag (latest SemVer)](https://img.shields.io/github/v/tag/kitsudaiki/libKitsunemimiCpu?label=version&style=flat-square)
![GitHub](https://img.shields.io/github/license/kitsudaiki/libKitsunemimiCpu?style=flat-square)
![C++Version](https://img.shields.io/badge/c%2B%2B-17-blue?style=flat-square)
![Platform](https://img.shields.io/badge/platform-Linux--x64-lightgrey?style=flat-square)

## Description

Simple library to read different information of the cpu, like topological information, speed and energy consumption with RAPL, manipulate the speed of single cores of the cpu and read information of the local memory.

## Build

### Requirements

name | repository | version | task
--- | --- | --- | ---
g++ | g++ | >= 8.0 | Compiler for the C++ code.
make | make | >= 4.0 | process the make-file, which is created by qmake to build the programm with g++
qmake | qt5-qmake | >= 5.0 | This package provides the tool qmake, which is similar to cmake and create the make-file for compilation.

Installation on Ubuntu/Debian:

```bash
sudo apt-get install g++ make qt5-qmake
```

IMPORTANT: All my projects are only tested on Linux. 

### Kitsunemimi-repositories

Repository-Name | Version-Tag | Download-Path
--- | --- | ---
libKitsunemimiCommon | v0.27.1 |  https://github.com/kitsudaiki/libKitsunemimiCommon.git

HINT: These Kitsunemimi-Libraries will be downloaded and build automatically with the build-script below.

### build library

In all of my repositories you will find a `build.sh`. You only have to run this script. It doesn't required sudo, because you have to install required tool via apt, for example, by yourself. But if other projects from me are required, it download them from github and build them in the correct version too. This script is also use by the ci-pipeline, so its tested with every commit.


Run the following commands:

```
git clone https://github.com/kitsudaiki/libKitsunemimiCpu.git
cd libKitsunemimiCpu
./build.sh
cd ../result
```

It create automatic a `build` and `result` directory in the directory, where you have cloned the project. At first it build all into the `build`-directory and after all build-steps are finished, it copy the include directory from the cloned repository and the build library into the `result`-directory. So you have all in one single place.

Tested on Debian and Ubuntu. If you use Centos, Arch, etc and the build-script fails on your machine, then please write me a mail and I will try to fix the script.

## Usage by example

### Get and set cpu-information

The functions of the header https://github.com/kitsudaiki/libKitsunemimiCpu/blob/develop/include/libKitsunemimiCpu/cpu.h should be quite self-explaining.

**Use it at your own risk**

- The threadId is the ID of the CPU-thread. So if you have a CPU with 4 cores and Hyperthreading, then you have the Thread-IDs 0-7 available.
- To change the cpu-frequency, the program has to run as root.
- If you don't have a CPU, which can change the frequency of a core separately, then you have to set the desired new CPU-frequecy for ALL Cpu-Threads to enforce the change of the frequency.

### Power-measureing with RAPL

- **Requirements:**
    - Required specific CPU-architecture:
        - **Intel**: 
            - Sandy-Bridge or newer
        - **AMD**: 
            - Zen-Architecture or newer
            - for CPUs of AMD Zen/Zen2 Linux-Kernel of version `5.8` or newer must be used, for Zen3 Linux-Kernel of version `5.11` or newer

    - the `msr`-kernel module has to be loaded with `modeprobe msr`.
    - has to be run as root

```cpp
#include <libKitsunemimiCpu/cpu.h>
#include <libKitsunemimiCpu/rapl.h>
#include <libKitsunemimiCpu/memory.h>
#include <libKitsunemimiCommon/logger.h>
#include <libKitsunemimiCommon/threading/thread.h>

Kitsunemimi::Cpu::Rapl rapl(0);
rapl.initRapl(error);
std::cout<<"info: "<<rapl.getInfo().toString()<<std::endl;

// make firsth request
RaplDiff diff = rapl.calculateDiff();

// sleep 10 seconds
sleep(10);

// make second request
diff = rapl.calculateDiff();
std::cout<<diff.toString()<<std::endl;
// output contains power-consumption per second and total power-consumption within the 10 seconds
```

## Contributing

Please give me as many inputs as possible: Bugs, bad code style, bad documentation and so on.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details

<<<<<<< HEAD
# ToyLArTPC
=======
# ToyLArTPC

A Geant4 simulation of a simplified **Liquid Argon Time Projection Chamber (LArTPC)**, with Qt-based visualization of both detector geometry and event data.

## Requirements
# ToyLArTPC

A Geant4 simulation of a simplified **Liquid Argon Time Projection Chamber (LArTPC)**, with Qt-based visualization of both detector geometry and event data.

## Requirements

| Dependency | Version |
|---|---|
| Geant4 | ≥ 11.0 (built with Qt and OpenGL support) |
| CMake | ≥ 3.16 |
| C++ compiler | C++17 compatible |
| Qt5 or Qt6 | (via Geant4 visualization drivers) |

## Project Structure

```
ToyLArTPC/
├── CMakeLists.txt        # Build system
├── main.cc               # Application entry point
├── include/              # Header files (.hh)
├── src/                  # Source files (.cc)
├── macros/               # Geant4 macro files (.mac)
└── README.md
```

## Building

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Running

**Interactive mode** (opens the Qt visualization):
```bash
./ToyLArTPC
```

**Batch mode** (executes a macro file):
```bash
./ToyLArTPC run.mac
```

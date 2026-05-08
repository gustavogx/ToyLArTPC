# ToyLArTPC

A Geant4 simulation of a simplified **Liquid Argon Time Projection Chamber (LArTPC)** with optical photon detection and Qt-based visualization.

## Overview

This project simulates neutrino interactions in liquid argon using the Geant4 toolkit. It includes:

- **Neutrino event generation** using MARLEY (Monte Carlo Event Generator for Reactor Neutrino Oscillations and other Low Energy Neutrino Applications)
- **Geant4 simulation** of particle interactions in a simplified LArTPC detector
- **Optical photon detection** with configurable scintillation yield
- **Data visualization** and analysis tools

## Requirements

| Dependency | Version | Notes |
|------------|---------|-------|
| Geant4 | ≥ 11.0 | Must be built with Qt and OpenGL support |
| CMake | ≥ 3.16 | Build system |
| C++ compiler | C++17 compatible | GCC, Clang, or MSVC |
| ROOT | ≥ 6.0 | Data analysis framework |
| MARLEY | Latest | Neutrino event generator |
| libpng | Latest | Image generation |

## Project Structure

```
ToyLArTPC/
├── CMakeLists.txt              # Build configuration
├── main.cc                     # Main Geant4 simulation executable
├── generate_marley_events.cc   # MARLEY event generator
├── generate_images.cc          # Image generation from ROOT files
├── include/                    # C++ header files
├── src/                        # C++ source files
├── macros/                     # Geant4 macro files
├── marley_config.js            # MARLEY configuration
└── README.md                   # This file
```

## Building

```bash
# Clone or navigate to the project directory
cd ToyLArTPC

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build all executables
make -j$(nproc)
```

This will create three executables in the `build/` directory:
- `ToyLArTPC` - Main simulation
- `GenerateMarleyEvents` - Event generator
- `GenerateImages` - Image generator

## Usage

### 1. Generate Neutrino Events

First, generate neutrino events using MARLEY:

```bash
./GenerateMarleyEvents marley_config.js <nEvents> [output.root]
```

**Example:**
```bash
./GenerateMarleyEvents marley_config.js 1000 events.root
```

This creates a ROOT file containing pre-generated neutrino events with supernova electron neutrino spectrum.

### 2. Run the Geant4 Simulation

Run the main simulation in either interactive or batch mode:

**Interactive mode** (Qt visualization):
```bash
./ToyLArTPC <events.root>
```

**Batch mode** (headless simulation):
```bash
./ToyLArTPC <events.root> -n <nEvents> [-t <nThreads>] [-full-yield]
```

**Options:**
- `-n <nEvents>`: Number of events to simulate
- `-t <nThreads>`: Number of worker threads (0 = automatic)
- `-full-yield`: Use full scintillation yield (24000 ph/MeV) instead of reduced yield (240 ph/MeV)

**Examples:**
```bash
# Simulate 100 events with 4 threads
./ToyLArTPC events.root -n 100 -t 4

# Simulate with full scintillation yield
./ToyLArTPC events.root -n 1000 -full-yield
```

### 3. Generate Images from Simulation Output

Convert the simulation output to PNG images:

```bash
./GenerateImages <simulation_output.root> [output_dir] [--verbose]
```

**Example:**
```bash
./GenerateImages output.root images/ --verbose
```

This creates a directory of PNG images showing the photon detection patterns for each event.

## Configuration

### MARLEY Configuration

The `marley_config.js` file configures the neutrino source:
- **Target**: Pure ⁴⁰Ar
- **Reaction**: Charged-current electron neutrino scattering
- **Spectrum**: Fermi-Dirac with supernova parameters (T = 3.5 MeV)
- **Direction**: Along +z axis

### Detector Configuration

The detector is defined in `DetectorConstruction.cc`:
- **Dimensions**: Simplified LArTPC geometry
- **Photon detection**: 5×5 grid of optical sensors
- **Scintillation**: Configurable yield (reduced for faster simulation)

### Visualization Macros

Geant4 macro files in `macros/`:
- `init_vis.mac`: Visualization initialization
- `run.mac`: Example batch run configuration

## Output Files

- **Event files**: ROOT files containing MARLEY neutrino events
- **Simulation output**: ROOT files with photon detection data (`PhotonCounts` tree)
- **Images**: PNG files visualizing photon patterns (250×250 pixels, 5×5 sensor grid)

## Physics

The simulation includes:
- **Particle transport**: Using Geant4 FTFP_BERT physics list
- **Optical physics**: Scintillation and optical photon propagation
- **Neutrino interactions**: Via MARLEY event generator
- **Detector response**: Optical photon detection and counting

## Development

### Adding New Features

1. Modify source files in `src/` and headers in `include/`
2. Update `CMakeLists.txt` if new dependencies are needed
3. Rebuild and test

### Visualization

In interactive mode, use Geant4 commands:
```
/vis/viewer/set/viewpointVector 1 1 1
/vis/viewer/set/style wireframe
/run/beamOn 1
```

## Troubleshooting

### Common Issues

1. **Geant4 not found**: Ensure Geant4 is installed with Qt support
2. **MARLEY not found**: Set `MARLEY` environment variable or install MARLEY
3. **ROOT not found**: Ensure ROOT is in PATH
4. **Qt visualization fails**: Check Geant4 was built with Qt

### Environment Variables

```bash
export MARLEY=/path/to/marley/installation
```

## License

This project is for educational and research purposes.

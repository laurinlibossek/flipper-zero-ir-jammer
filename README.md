# Flipper Zero IR Jammer

Infrared jamming application for the Flipper Zero device.

## Functional Overview
Automated transmission of infrared signals across common consumer electronics protocols to disrupt signal synchronization.

### Transmission Modes
- **BASIC:** Rapid transmission of standard infrared timing marks.
- **ENHANCED:** Modified mark and space timings for increased compatibility.
- **SWEEP:** Timing boundary sweep to disrupt signal synchronization.
- **RANDOM:** Continuous transmission of randomized infrared pulses.
- **EMPTY:** Minimal gap timing for experimental low-latency transmission.

### Configuration
- **Density:** Controls the interval between infrared bursts.

### Navigation
- **Up/Down:** Select setting (Status, Freq, Mode, Density, Info).
- **Left/Right:** Modify selected setting.
- **OK/Right (on Status):** Toggle transmission (ACTIVE/PAUSED).
- **Info Menu:** Detailed mode descriptions.

## Build and Deployment
1. Install `ufbt` (micro Flipper Build Tool).
2. Clone repository: `git clone https://github.com/laurinlibossek/flipper-zero-ir-jammer.git`
3. Execute `ufbt` in the project root.
4. Transfer the generated `.fap` file to the Flipper Zero device.

### Flipper App Store Submission
This repository contains metadata for the [Flipper Application Catalog](https://github.com/flipperdevices/flipper-application-catalog). 
Submission instructions are available in [catalog_manifest.yml](./catalog_manifest.yml).
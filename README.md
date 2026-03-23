# Flipper Zero IR Jammer

Hey there! Welcome to my IR Jammer project for the Flipper Zero. 🐬

I built this little application by adapting existing IR protocols and giving it a clean, stripped-down, easy-to-use interface. If you're familiar with the Flipper's native IR capabilities, this takes it a step further by focusing entirely on automated transmission routines (jamming) across common frequencies. 

### Features

This app currently comes packed with a handful of modes depending on what you're trying to achieve:

- **BASIC:** Your bread-and-butter mode. It rapidly sends common, fixed infrared marks that are highly reliable across most systems.
- **ENHANCED:** A slightly more aggressive version that tweaks the mark and space timings for fine-tuning your target.
- **SWEEP:** Slowly sweeps and shifts between boundary timings to effectively break signal synchronization.
- **RANDOM:** Blasts out continuous random IR pulses. Great for fooling smart TVs and modern AC units that might ignore basic repetitive noise!
- **EMPTY:** Very minimal gaps. It's efficient, extremely tight, but mostly experimental.

You also get full control over **Density**. Basically, dialing up the density parameter controls exactly how tightly packed these jamming bursts are transmitted out of your Flipper's infrared bulb. 

### Navigating the App

The UI is built to be clean and simple:
*   Use the **Up/Down** buttons to scroll between Settings (Status, Freq, Mode, Density, Info).
*   Use the **Left/Right** buttons to alter those settings or view the Info menu.
*   Once highlighting the `Status` setting, simply hit the **Right** or **OK** button to toggle the transmission `ACTIVE` or `PAUSED`.
*   Take a look inside the `Info` menu for a scrollable rundown of the modes and everything they do natively. 

### Building for your Flipper

This app sits entirely inside standard `furi` and `furi_hal` structures!

To build it yourself:
1. Make sure you have `ufbt` (micro Flipper Build Tool) installed and configured on your machine.
2. Clone this repo: `git clone https://github.com/laurinlibossek/flipper-zero-ir-jammer.git`
3. Enter the project folder and simply run the command: `ufbt`
4. Deploy the `.fap` directly onto your Flipper, and you're good to go!

### Publishing to Flipper App Store

This repository is prepared for submission to the [Flipper Application Catalog](https://github.com/flipperdevices/flipper-application-catalog). 
If you want to contribute, follow the instructions in [catalog_manifest.yml](./catalog_manifest.yml).

Thanks for checking out the project. Let me know if you discover any fun results from the Random mode out in the wild!
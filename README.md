## Project background
- Brought to you by AMD-V 
- While the project is functional and stable in a way, it serves as a foundational framework designed for others to build upon and expand according to their specific needs. Many features have been simplified or disabled to keep it manageable and stable, and some features are temporarily turned off to avoid instability. It is by no means a final fully featured product, but rather a starting point for anyone interested in virtualization technologies to customize and extend based on their own personal needs and requirements.

- ## Build Guide

### Prerequisites

- CMake 3.21+
- Windows Driver Kit (WDK)

### Building

1. Clone the repository:
   ```
   git clone https://github.com/Hxnter999/vmm.git
   cd vmm
   ```

2. Create and enter build directory:
   ```
   mkdir build && cd build
   ```

3. Configure and build:
   ```
   cmake ..
   cmake --build .
   ```
   To specify the build configuration (e.g., Debug or Release), you can use the --config flag:
   ```
    cmake --build . --config Debug
    cmake --build . --config Release
   ```

### Building Specific Targets

- VMM: `cmake --build . --target vmm`
- Usermode: `cmake --build . --target usermode`

## Todo list
- Fixes:
    - Implement exception handling within the host to safely reflect any potential exceptions back into the guest including msrs instead of strictly injecting an exception or returning nothing
    - Fix unload routine to work for usermode calls, currently only works for cpl0.
    - Fix npt splitting mechanism and ensure proper tlb flushing when hiding pages, perhaps through `invlpgb` to broadcast an npt flush

- QOL:
    - Hide vm overhead if necessary
    - Add a logger class, fmtlib formatting style
	
## Customization
Everything regarding the setup of the guest and the host is within `source/vmm.cpp`. There are separate functions that set up the guest and the host. If you want to customize the guest, then you should start with the `setup_guest` function.

For the vmexit handlers, they're all located in the directory `source/header/vmexit/handlers/`. You can modify or add new handlers there to customize the vmexit handler's behavior according to your needs.
# STM32F105 WiFi Bluetooth Interface Project

This project implements a WiFi/Bluetooth interface for STM32F105 microcontroller with BX310 module communication via SPP (Serial Port Profile).

## Project Overview

- **Target MCU**: STM32F105RCT6 (ARM Cortex-M3)
- **WiFi/BT Module**: BX3105 (Sierra Wireless)
- **Communication**: Bluetooth SPP, CAN bus, UART
- **Development Environment**: VS Code + ARM GCC + OpenOCD
- **Build System**: Makefile

## Prerequisites

### Required Tools

- **ARM GCC Toolchain**: `gcc-arm-none-eabi`
- **Build Tools**: `make`, `build-essential`
- **Debug Tools**: `openocd`, `gdb-multiarch`
- **VS Code Extensions**: C/C++ Extension Pack, Cortex-Debug, ARM Assembly

## Project Structure

```
InterfaceWiFiBluetoothV3 DEBUG/
├── Core/                          # STM32 HAL core files
│   ├── Inc/                       # Header files
│   ├── Src/                       # Source files
│   └── Startup/                   # Startup assembly
├── Drivers/                       # STM32 HAL drivers
│   ├── CMSIS/                     # CMSIS files
│   └── STM32F1xx_HAL_Driver/      # STM32F1 HAL
├── mercura/                       # Application code
│   ├── inc/                       # Application headers
│   └── src/                       # Application sources
│       ├── App/                   # Application layer
│       ├── P_L/                   # Protocol layer
│       ├── Hwi/                   # Hardware interface
│       └── S_S/                   # System services
├── FATFS/                         # FAT filesystem
├── ld/                            # Linker scripts
├── WIFI BLT F105 Debug/           # Build directory
│   ├── Makefile                   # Main Makefile
│   └── objects.list               # Object files list
└── .vscode/                       # VS Code configuration
    ├── c_cpp_properties.json     # IntelliSense config
    ├── tasks.json                 # Build tasks
    └── launch.json                # Debug config
```

## Building the Project

### Command Line Build

1. **Navigate to build directory**
   ```bash
   cd "WIFI BLT F105 Debug"
   ```

2. **Clean build**
   ```bash
   make clean
   ```

3. **Build project**
   ```bash
   make all
   ```

4. **Parallel build (faster)**
   ```bash
   make all -j4
   ```

### Build Output

After successful build, you'll get:
- `Interface_WiFi_Bluetooth.elf` - Executable file
- `Interface_WiFi_Bluetooth.map` - Memory map
- `Interface_WiFi_Bluetooth.list` - Assembly listing

## Debugging with Cortex-Debug

### Hardware Setup

1. Connect ST-Link/V2 to STM32F105:
   - SWDIO → PA13
   - SWCLK → PA14
   - GND → GND
   - 3.3V → 3.3V

### Debug Configuration

The project includes pre-configured debug settings in `.vscode/launch.json`:

```json
{
    "name": "Debug STM32F105",
    "type": "cortex-debug",
    "request": "launch",
    "servertype": "openocd",
    "cwd": "${workspaceFolder}",
    "executable": "./WIFI BLT F105 Debug/Interface_WiFi_Bluetooth.elf",
    "configFiles": [
        "interface/stlink.cfg",
        "target/stm32f1x.cfg"
    ]
}
```

## Configuration

### Compiler Defines

The project uses these key defines (in `.vscode/c_cpp_properties.json`):

```json
"defines": [
    "USE_HAL_DRIVER",
    "STM32F105xC",
    "_WATCHDOG",
    "_GESTION_ODOMETRE", 
    "_BLUETOOTH",
    "_RAMPE_AUTONOME",
    "_PROJECTEUR_BT",
    "_DATA_BT_CALSSIQUE_SPP",
    "_BT_CLASSIQUE",
    "_WIFI_BT_ARRET_SPA",
    "_DEBUG_WIFI_BT"
]
```

### Memory Configuration

- **Flash**: 256KB
- **RAM**: 64KB
- **Linker script**: `ld/stm32f105rb_flash_debug.ld`

## Development Notes

### Key Application Files

- `mercura/src/App/app_manage_BT.c` - Bluetooth management
- `mercura/src/P_L/p_l_bx310x.c` - BX310 module control
- `mercura/src/Hwi/F105/` - STM32F105 hardware interface

### Communication Protocol

- **Bluetooth SPP**: AT commands to BX310
- **CAN Bus**: Internal communication
- **Frame Format**: Custom protocol 
- **Security**: Key-based authentication
# Bluetooth Module Architecture Documentation

## Overview

This document explains the roles and responsibilities of the two main Bluetooth communication components in the STM32F105 project:

- **`app_manage_BT`**: Application layer Bluetooth management
- **`pl_bx310`**: Platform layer BX310 module driver

## 1. app_manage_BT Module

### Purpose
The `app_manage_BT` module handles the **application-level Bluetooth communication logic** for the STM32F105 system. It manages the business logic, protocol implementation, and data processing for Bluetooth SPP (Serial Port Profile) communications.

### Key Responsibilities



#### 1.1 Communication Protocol
- **Frame Processing**: Handles different command types:
  - `WRD_KEY_ASK_BT` ('key?'): Key request from remote device
  - `WRD_KEY_BT` ('key.'): Key response with authentication data
  - `WRD_KEY_CAPP_BT` ('capp'): Application command frames
  - `WRD_DATA_1_BT` ('da1='): Data transmission frames
  - `WRD_BTN_BT` ('btn='): Button command frames

#### 1.2 Data Management
- **Power Control**: Manages power output commands for up to 4 channels (`HWI_NB_POWER_BT_CMD_RAMPE`)
- **CAN Bus Integration**: Bridges Bluetooth commands to CAN bus messages via `P_L_Can_Send_*` functions
- **Status Monitoring**: Tracks device states, fault conditions, and analog measurements

#### 1.3 State Machine Management
- **Send State Machine**: Controls data transmission flow with states:
  - `BT_SEND_STATE_ASK_KEY`: Request authentication key
  - `BT_SEND_STATE_WAIT_KEY`: Wait for key response
  - `BT_SEND_STATE_SEND_DATA`: Transmit actual data
  - `BT_SEND_STATE_FINISH`: Complete transmission
  - `BT_SEND_STATE_WAIT`: Idle state

#### 1.4 Timeout and Safety Features
- **Communication Timeouts**: Implements safety timeouts for Bluetooth communications
- **Automatic Disconnection**: Handles lost connections and timeout recovery
- **Force Send Mechanisms**: Provides emergency communication pathways

### Key Functions

```c
void APP_BT_Manage_Send_Data(void)               // Handle outgoing data transmission
void APP_Gestion_BT_DataRx(U16 data_len, U8 *data) // Process incoming Bluetooth data
```

### Data Structures

```c
// Button command structure
typedef struct {
    U8 id;   // Button ID
    U8 cmd;  // Command value
} APP_BT_COMMAND_BOUTON_TYPE;
```

## 2. pl_bx310 Module

### Purpose
The `pl_bx310` module serves as the **platform layer driver** for the Sierra Wireless BX310 WiFi/Bluetooth module. It handles low-level AT command communication, module initialization, and hardware interface management.

### Key Responsibilities

#### 2.1 Hardware Interface Management
- **UART Communication**: Manages serial communication with BX310 module at 115200 baud
- **Flow Control**: Implements RTS/CTS hardware flow control (`AT&K3`)
- **Module Power Control**: Handles module power on/off and reset sequences

#### 2.2 AT Command Processing
- **Command Transmission**: Sends AT commands to configure BX310 module
- **Response Parsing**: Processes AT command responses and status messages          

#### 2.3 Bluetooth Configuration
- **Bluetooth System Setup**: Configures Bluetooth Classic mode (`AT+SRBTSYSTEM=2`)
- **Device Identity**: Sets Bluetooth name and retrieves MAC address
- **SPP Profile**: Manages Serial Port Profile connections

#### 2.4 State Machine Management
The module uses a state machine with states including:

```c
typedef enum {
    Bx310x_state_RESET_module,        // Module reset
    Bx310x_state_CONTROL_FLUX,        // Flow control setup
    Bx310x_state_RWCFG,              // Read WiFi configuration
    Bx310x_state_SRBTSYSTEM_CFG,     // Bluetooth system configuration
    Bx310x_state_SRBTADDR,           // Bluetooth address query
    Bx310x_state_SRBTNAME,           // Bluetooth name setup
    Bx310x_state_SRBTCSECPARAMS,     // Bluetooth security parameters
    Bx310x_state_SRBTSTATE,          // Bluetooth state management
    Bx310x_state_FINIT               // Initialization complete
} P_L_BX310X_STATE;
```

#### 2.5 Connection Management
- **SPP Connection Tracking**: Monitors Bluetooth SPP connection status
- **Client Management**: Tracks connected devices and session IDs
- **Automatic Reconnection**: Handles connection recovery and maintenance

### Key Functions

```c
// Main functions in p_l_bx310x.c
void P_L_Bx310x_Init(BOOL_TYPE init_clef_wep)    // Initialize BX310 module
void P_L_Bx310x_Manage_State(void)               // State machine management
void P_L_Bx310x_Rx(U16 ptr, U8 *data)           // Process received AT responses
void P_L_BT_Tx_dataDirect(U32 id, U8 len, U8 *data) // Direct Bluetooth data transmission
static void P_L_Bx310x_cmd_system_*()            // Various AT command functions
```

### AT Command Examples

```c
// Bluetooth configuration commands
"AT+SRBTSYSTEM=2"      // Enable Bluetooth Classic only
"AT+SRBTADDR?"         // Query Bluetooth MAC address  
"AT+SRBTNAME=<name>"   // Set Bluetooth device name
"AT+SRBTCSECPARAMS=0"  // Set security parameters (no PIN)
"AT+SRSPPCFG=1,0"      // Configure SPP profile
"AT+SRSPPCNX=<id>"     // Start SPP connection
```

## 3. Module Interaction

### Communication Flow

1. **Initialization**:
   - `pl_bx310` initializes BX310 hardware and configures Bluetooth
   - `app_manage_BT` sets up application-level data structures

2. **Connection Establishment**:
   - `pl_bx310` handles SPP connection setup with remote device
   - Connection status reported via `P_L_BT_spp_rampe_state`

3. **Data Exchange**:
   - `app_manage_BT` prepares data frames with authentication
   - `pl_bx310` transmits data via `P_L_BT_Tx_dataDirect()`
   - Incoming data flows: `pl_bx310` → `APP_Gestion_BT_DataRx()`

4. **Authentication Sequence**:
   - Remote device sends 'key?' request
   - `app_manage_BT` generates time-based key and responds
   - Subsequent communications include authentication data


## 4. Bluetooth Frame Structure

### General Frame Structure

A complete Bluetooth frame consists of several fields in this order:

#### 4.1 Command Keyword (4 bytes)
- **Position**: bytes 0-3
- **Role**: Identifies the frame type
- **Examples**:
  - `WRD_KEY_ASK_BT` → "key?" (key request)
  - `WRD_KEY_BT` → "key." (key transmission)
  - `WRD_DATA_1_BT` → "da1=" (PWM command data)
  - `WRD_KEY_CAPP_BT` → "capp" (application command)

#### 4.2 Data Size (2 bytes)
- **Position**: bytes 4-5
- **Format**: Big-endian (MSB then LSB)
- **Content**: Total size = Unlockkey (8) + Useful data + Checksum (2)

#### 4.3 Unlock Key (8 bytes)
- **Position**: bytes 6-13
- **Role**: Security/authentication
- **Note**: Currently simplified to 8 × 0x00 for testing

#### 4.4 Useful Data (variable)
- **Position**: starting from byte 14
- **Content**: Depends on frame type

#### 4.5 Checksum (2 final bytes)
TODO:
- **Checksum 1**: Simple sum of all previous bytes (modulo 256)
- **Checksum 2**: Weighted sum (byte × position) (modulo 256)

### Frame Example: Application Command (CAPP)
Trames are transmited  in Hexadecimal ASCII character
| Hexadecimal | Description                          |
|-------------|--------------------------------------|
| 44 41 54 31 | "CAPP" (Application Command)         |
| 00 16       | Size = 22 bytes (8 + 12 + 2)        |
| 02 02 02 02 02 02 02 02 | Unlock key (8 × 0x00) |
| 0 0         | BUTTON_ID:0 cmd: 0   ID: 8 bits CMD: 8 bits |    
| 1 2         | BUTTON_ID:1 cmd: 2               |
| 2 2         | BUTTON_ID:2 cmd: 1               |
| 3 1         | BUTTON_ID:3 cmd: 1               |
| 2B B7       | Calculated checksums (TODO)                 |

## 5. Security Features

### Authentication Protocol
1. **Key Request**: Send key? to the app
2. **App response key.**: App send key. with the password
3. **Keep alive da1=**: App need to receive  bluetooth command each 3s from the STM32 or the app close the communication
### Security Parameters
- **No PIN Required**: Configured with `AT+SRBTCSECPARAMS=0`
- **Trame key**: In each bluetooth trame there is 8 bytes of frameKey.


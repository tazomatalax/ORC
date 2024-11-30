# PCB Control System

This project implements a comprehensive bioreactor control system with multiple interacting control loops, data management, and safety features.

## System Overview

### Core Control Systems

#### pH Control
- Measurement frequency: 1 second
- Control action: Every 2 minutes
- Trigger: 0.2 pH below setpoint
- Interfaces: Acid/base pump
- Database logging for additions

#### Dissolved Oxygen (DO) Control
- Measurement frequency: 1 second
- Control action: Every 30 seconds
- Cascading control:
  1. Stirrer speed adjustment
  2. Gas flow rate adjustment
- Configurable cascade priority

#### Temperature Control
- Measurement frequency: 1 second
- Control action: Every 10-30 seconds (configurable)
- Advanced PID control implementation:
  - 12-bit PWM resolution (0-4095)
  - 1 kHz PWM frequency
  - Heater control via MOSFET on pin PB10
- Multi-sensor temperature monitoring:
  - pH probe temperature sensor
  - DO probe temperature sensor
  - Intelligent averaging of valid readings
- Fallback mechanisms:
  - Continues with single sensor if one fails
  - Maintains last known temperature if both fail
- Safety features integrated with SafetyManager
- Interfaces: PWM-controlled heating jacket

#### Pressure Control
- Measurement frequency: 1 second
- Control action: Every 5-10 seconds
- Interfaces: Backpressure controller, gas mixing system

#### Gas Mixing and Flow Control
- Measurement frequency: 5 seconds
- Control action: Every 30 seconds
- Coordinates with DO control
- Interfaces: DO control, pressure control

#### Nutrient Feed Control
- Measurement frequency: 1 minute
- Control action: Every 5-15 minutes
- Adaptive feed profiles based on:
  - Growth phase
  - Metabolic demand
- Interfaces: Peristaltic pumps, biomass monitoring

#### Harvest/Perfusion Control
- Measurement frequency: 1 minute
- Control action: Every 15-30 minutes
- Modes: Continuous or semi-continuous
- Interfaces: Harvest pump, biomass monitoring

#### Stirrer Speed Control
- Measurement frequency: 1 second
- Control action: Every 30 seconds
- Adaptive control based on:
  - DO levels
  - Shear sensitivity
- Interfaces: DO control, biomass monitoring

### Data Management

#### Data Flow Architecture
- Logging frequency: 1 second
- Pipeline: Microcontroller → MQTT → Python script → Database
- MQTT topics organized by sensor/control system
- Standardized JSON data format

#### Database Implementation
- Time-series database (InfluxDB)
- Data retention policies
- SQL database backup integration
- Optimized time-based queries

#### Digital Twin
- Update frequency: 5-10 seconds (configurable)
- Real-time sensor data integration
- Predictive modeling
- Bi-directional system communication

#### User Interface
- Update frequency: 5 seconds
- Features:
  - Real-time visualization
  - Setpoint management
  - PID tuning
  - Control timing adjustments
  - Alarm configuration
  - Min/max value settings
- Web-based remote access

### Safety and Alarm System
- Continuous monitoring (1 second)
- 5-second alarm confirmation
- Notification system: Email/SMS
- Emergency shutdown protocols
- Predictive alarm triggering via digital twin

## TODO List

### 1. Sensor Integration
- [ ] Implement pH sensor reading function
  - Hardware: Atlas Scientific pH sensor
  - Interface: I2C
  - Function: `readPHSensor()` in `ph_controller.h`

- [ ] Implement DO sensor reading function
  - Hardware: PreSens DO sensor
  - Interface: RS485
  - Function: `readDOSensor()` in `do_controller.h`

- [ ] Implement temperature sensor reading
  - Hardware: PT100 RTD sensors
  - Interface: ADC
  - Function: `readTemperatureSensor()` in `temperature_controller.h`

- [ ] Implement pressure sensor reading
  - Hardware: Honeywell pressure transducer
  - Interface: Analog
  - Function: `readPressureSensor()` in `pressure_controller.h`

### 2. Hardware Control Implementation
- [ ] Implement acid/base pump control
  - Hardware: Peristaltic pumps
  - Interface: PWM
  - Function: `actuatePump()` in `ph_controller.h`

- [ ] Implement stirrer speed control
  - Hardware: BLDC motor
  - Interface: PWM + Encoder
  - Function: `adjustStirrerSpeed()` in `do_controller.h`

- [ ] Implement gas flow control
  - Hardware: Mass flow controllers
  - Interface: RS485
  - Function: `adjustGasFlow()` in `do_controller.h`

- [ ] Implement heating jacket control
  - Hardware: Heating element
  - Interface: SSR
  - Function: `adjustHeatingJacket()` in `temperature_controller.h`

### 3. MQTT Configuration
- [ ] Set up Mosquitto MQTT broker
  ```bash
  sudo apt-get install mosquitto mosquitto-clients
  sudo systemctl enable mosquitto
  ```
- [ ] Configure MQTT credentials in `mqtt_handler.h`
  ```cpp
  const char* MQTT_SERVER = "your-broker-ip";
  const char* MQTT_USER = "your-username";
  const char* MQTT_PASSWORD = "your-password";
  ```
- [ ] Set up MQTT topics structure
  ```
  bioreactor/
    ├── data/
    │   ├── ph
    │   ├── do
    │   ├── temperature
    │   └── pressure
    └── control/
        ├── setpoints
        └── commands
  ```

### 4. InfluxDB Setup
- [ ] Install InfluxDB
  ```bash
  curl -s https://repos.influxdata.com/influxdb.key | sudo apt-key add -
  echo "deb https://repos.influxdata.com/debian stable main" | sudo tee /etc/apt/sources.list.d/influxdb.list
  sudo apt-get update && sudo apt-get install influxdb
  sudo systemctl enable influxdb
  ```
- [ ] Create database and user
  ```sql
  CREATE DATABASE bioreactor
  CREATE USER "bioreactor" WITH PASSWORD 'your-password'
  GRANT ALL ON "bioreactor" TO "bioreactor"
  ```
- [ ] Configure retention policies
  ```sql
  CREATE RETENTION POLICY "raw_data" ON "bioreactor" DURATION 7d REPLICATION 1 DEFAULT
  CREATE RETENTION POLICY "aggregated" ON "bioreactor" DURATION 52w REPLICATION 1
  ```
- [ ] Update credentials in `database_manager.h`

### 5. Web Interface Development
- [ ] Create responsive dashboard layout
  - Main display panel
  - Control panel
  - Alarm panel
  - Trends panel

- [ ] Implement real-time data visualization
  - [ ] Use Chart.js for trend plots
  - [ ] Create gauge displays for current values
  - [ ] Add alarm indicators

- [ ] Add control interface
  - [ ] Setpoint adjustment forms
  - [ ] PID tuning interface
  - [ ] Mode selection (Manual/Auto)
  - [ ] Emergency stop button

- [ ] Implement user authentication
  - [ ] Login system
  - [ ] Role-based access control
  - [ ] Audit logging

### 6. Testing and Calibration
- [ ] Create calibration procedures
  - [ ] pH sensor calibration
  - [ ] DO sensor calibration
  - [ ] Temperature sensor calibration
  - [ ] Pressure sensor calibration

- [ ] Implement automated testing
  - [ ] Unit tests for control algorithms
  - [ ] Integration tests for communication
  - [ ] System tests for safety features

## Recent Updates

### Temperature Control Implementation (Latest)
1. Enhanced Temperature Sensing
   - Integrated temperature readings from both pH and DO probes
   - Smart averaging system for reliable temperature data
   - Automatic sensor validity checking

2. PWM Heater Control
   - Implemented on SAMD51 pin PB10 (pin 32)
   - 12-bit resolution for precise control (4096 steps)
   - 1 kHz switching frequency for efficient heating
   - MOSFET-based power control

3. Control Architecture
   - PID control with configurable parameters
   - Adjustable control interval (10-30 seconds)
   - Integrated with central SensorManager
   - Safety system integration

4. System Integration
   - Unified through ControllerManager
   - Automatic sensor data handling
   - Seamless integration with existing control systems
   - Real-time monitoring and adjustment capabilities

## System Architecture

### SAMD51 (Main Controller)
- Handles sensor interfaces (RS485, PT100s)
- Controls stepper motors
- Manages PWM outputs
- Communicates with RP2040 via SPI

### RP2040 (Network Controller)
- Ethernet connectivity
- Web server
- MicroSD card logging
- MQTT client for data transmission

## Project Structure
```
pcb_control_system/
├── samd51/                 # SAMD51 firmware
│   ├── src/               # Source files
│   │   ├── controllers/   # Control system implementations
│   │   ├── sensors/       # Sensor interfaces
│   │   └── safety/        # Safety and alarm systems
│   ├── include/           # Header files
│   └── platformio.ini     # PlatformIO configuration
├── rp2040/                # RP2040 firmware
│   ├── src/               # Source files
│   │   ├── network/       # Network communication
│   │   ├── data/         # Data management
│   │   └── web/          # Web interface
│   ├── include/           # Header files
│   └── platformio.ini     # PlatformIO configuration
└── README.md              # This file
```

## Building and Flashing

This project uses PlatformIO for build management. To build:

1. Install PlatformIO
2. Navigate to either samd51/ or rp2040/ directory
3. Run `pio run` to build
4. Run `pio run -t upload` to flash

## Dependencies

### SAMD51
- Arduino core for SAMD51
- PID control library
- Sensor libraries (specified in platformio.ini)

### RP2040
- Arduino core for RP2040
- Ethernet library
- SD card library
- MQTT client library
- Web server library

## Getting Started

1. Set up the hardware connections according to the PCB documentation
2. Configure the network settings in the RP2040 firmware
3. Set up the MQTT broker and database system
4. Flash both microcontrollers
5. Access the web interface for system configuration and monitoring

## Contributing

Please read CONTRIBUTING.md for details on our code of conduct and the process for submitting pull requests.

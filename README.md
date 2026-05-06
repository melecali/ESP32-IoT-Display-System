# ESP32-IoT-Display-System

Embedded IoT project developed using ESP32, WiFi communication, HTTP requests, and JSON data processing.

The system retrieves real-time data from a public REST API and displays selected information on a dual 7-segment display using dynamic text scrolling and multiplexing techniques.

## Features

- WiFi network scanning and connection
- HTTP API requests using ESP32
- JSON data parsing with ArduinoJson
- Dynamic text scrolling on dual 7-segment display
- Custom alphanumeric character encoding
- Serial terminal interaction for dynamic user input
- Real-time embedded data visualization
- Display multiplexing implementation

## Technologies Used

- ESP32
- Arduino IDE
- C++
- WiFi.h
- HTTPClient.h
- ArduinoJson

## System Workflow

1. Scan available WiFi networks
2. Connect to selected network
3. Receive user input through Serial Monitor
4. Send HTTP request to external API
5. Parse JSON response
6. Display processed information on 7-segment display
7. Output additional data through serial communication

## Embedded Concepts

- Serial communication
- API integration
- JSON parsing
- Display multiplexing
- Timing management
- State-based logic
- Real-time data processing

## Future Improvements

- OLED/LCD display support
- Non-blocking architecture
- Better UI and animations
- Multiple API support
- FreeRTOS task implementation


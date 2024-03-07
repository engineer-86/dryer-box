# Filament Dryer Project

![Dryer schematic](/assets/dryer_box_Steckplatine.png "Fritzing schematic")

## Motivation

This project was initiated after my Eibos Cyclopes filament dryer stopped functioning. Instead of discarding the entire unit, I opted to repurpose the PTC heating element and the case to construct a bespoke filament dryer. This approach not only recycles parts that are still usable but also delivers a customized solution to meet my specific 3D printing requirements.

<img src="/assets/wired_01.jpg" alt="Inside" width="200"/><img src="/assets/wired_02.jpg" alt="Inside" width="200"/>

## Components

- **Microcontroller**: ESP8266 (Wemos Mini), selected for its WiFi capabilities and Arduino code compatibility. [More Info](https://shorturl.at/hnuOP)
- **Sensor**: DHT11, employed for monitoring the humidity and temperature within the dryer.
- **Relays**: Utilizes two relays for precise control over the PTC heating element and the fan, ensuring optimal temperature and airflow. [More Info](https://shorturl.at/mALQ7)
- **PTC Heater**: A 220V PTC heating element repurposed from the original filament dryer. [More Info](https://shorturl.at/tvNV9)
- **Power Converter**: Converts 220V AC to 5V DC, powering the ESP8266 and other low-voltage components. [More Info](https://shorturl.at/dekt5)

## Credentials Template

Sensitive information such as WiFi and MQTT broker credentials are managed through a template mechanism to ensure security. Here's the template structure:

```cpp
#ifndef CREDENTIALS_H
#define CREDENTIALS_H
// creds for wifi and mqtt
class Credentials
{
private:
    const char *_wifi_ssid = "YOUR_SSID";
    const char *_wifi_password = "YOUR_WIFI_PASSWORD";
    const char *_broker_ip = "YOUR_BROKER_IP";
    const int _broker_port = 1883; // Default MQTT port
    const char *_broker_user = "YOUR_BROKER_USERNAME";
    const char *_broker_password = "YOUR_BROKER_PASSWORD";

public:
    static const char *getWifiSSID() { return "YOUR_SSID"; }
    static const char *getWifiPassword() { return "YOUR_WIFI_PASSWORD"; }
    static const char *getBrokerIP() { return "YOUR_BROKER_IP"; }
    static int getBrokerPort() { return 1883; }
    static const char *getBrokerUsername() { return "YOUR_BROKER_USERNAME"; }
    static const char *getBrokerPassword() { return "YOUR_BROKER_PASSWORD"; }
};

#endif /* CREDENTIALS_H */

```

## WiFi and MQTT Integration

The project leverages the ESP8266's WiFi capabilities to connect to a local network and communicate with an MQTT broker. This setup enables remote monitoring and control of the filament dryer's operations.

### WiFi Connectivity

The ESP8266 connects to a predefined WiFi network using credentials specified in the `credentials.hpp` template. This step is crucial for enabling internet access and, subsequently, MQTT communication. The WiFi library initializes the connection in the setup phase of the program, ensuring the device maintains connectivity throughout its operation.

### MQTT Communication

The filament dryer utilizes MQTT (Message Queuing Telemetry Transport) for effective communication regarding the dryer's status, and to receive commands for operational control. It is configured to both publish its sensor data and subscribe to topics for remote commands.

- **Subscribed Topics**:

  - `cmnd/dryer/filament`: This topic listens for commands related to the filament. Commands published to this topic can control aspects like which filament to use or when to start drying.
  - `cmnd/dryer/heater`: Through this topic, the dryer receives commands to control the heater's operation, such as turning the heater on or off and setting the desired temperature.
  - `cmnd/dryer/fan`: This topic is used for controlling the fan's functionality within the dryer, allowing for adjustments in airflow to ensure optimal drying conditions.
    cts to an MQTT broker, specified in the `credentials.hpp`, to publish sensor readings and subscribe to control topics.

- **Publishing Data**: The device periodically publishes temperature and humidity readings to the MQTT topic `tele/heater/state`. This allows for remote monitoring of the filament dryer's environment through any MQTT client subscribed to the same topic.

  ```cpp
  connected_mqtt_client.publish("tele/dryer/state", payload);
  ```

  ## Filament Drying Times and MQTT Message Handling

### Filament Settings

The `FilamentSettings.hpp` file contains definitions for various 3D printing filaments, specifying the recommended drying temperature and time. These settings are crucial for ensuring that each filament type is dried under optimal conditions to preserve its quality and printing characteristics.

Here is a summary of the drying settings for different filament materials:

- PLA: 50°C for 4 hours
- ABS: 60°C for 2 hours
- PETG: 65°C for 2 hours
- NYLON: 70°C for 2 hours
- PC: 70°C for 8 hours
- TPU: 55°C for 4 hours
- PVA: 50°C for 4 hours
- ASA: 60°C for 4 hours
- PP: 55°C for 6 hours

The drying time is calculated in milliseconds using the `hoursToMilliseconds` function, providing a precise control mechanism for the drying process.

### MQTT Message Handling

The system subscribes to specific MQTT topics to receive commands that control the drying process. Here are the key topics and their expected messages:

- `cmnd/dryer/filament`: Receives the filament material name. The system then applies the corresponding drying settings for temperature and time.

  - Sending a filament name (e.g., "PLA") sets the dryer to the specific settings for that filament.
  - Sending "RESET" resets the dryer to a default state, turning off the heater and activating the fan.

- `cmnd/dryer/heater`: Controls the state of the heater.

  - Sending "ON" activates the heater (manual override).
  - Sending "OFF" deactivates the heater (manual override).

- `cmnd/dryer/fan`: Manages the operation of the fan.
  - Sending "ON" turns the fan on (considering Normally Closed - NC - logic).
  - Sending "OFF" turns the fan off (NC logic).

Each command is processed in the `mqttCallback` function, which adjusts the dryer's operation according to the received message. This allows for remote monitoring and control over the drying process, ensuring that filaments are prepared optimally for 3D printing.

## PlatformIO Configuration

The `platformio.ini` file is configured for the ESP8266 with essential libraries for DHT sensor reading, MQTT communication, NTP for time synchronization, and JSON for data handling.

```ini
[env:nodemcuv2]
platform = espressif8266
board = nodemcuv2
framework = arduino
monitor_speed = 115200
monitor_port = COM9
upload_port = COM9
upload_speed = 115200
lib_deps = adafruit/DHT sensor library@^1.4.6
           adafruit/Adafruit Unified Sensor@^1.1.4
           knolleary/PubSubClient@^2.8
           arduino-libraries/NTPClient@^3.2.1
           bblanchon/ArduinoJson@^6.19.4
```

## Future Enhancements

The filament dryer project is continuously evolving, with plans to integrate additional features and improvements to enhance functionality and user experience. Here are some of the planned enhancements:

- [ ] **Checkboxes Integration**: Implementing a web interface with checkboxes for selecting the filament type and setting the drying parameters. This will allow for an easier and more intuitive control over the drying process.

- [ ] **Push Button Support**: Adding physical push buttons to the dryer for manual control. This will include buttons for starting/stopping the drying cycle, selecting filament types, and manually overriding temperature and time settings.

- [ ] **OLED Display Output**: Incorporating an OLED display to show real-time information about the drying process. This display will provide feedback on the current temperature, humidity, drying time remaining, and selected filament type, making it easier for users to monitor the dryer's status.

- [ ] **Stepper Motor Control for Filament Rotation**: Implementing stepper motor control to rotate the filament spools during the drying process. This feature aims to ensure even drying by periodically turning the spools, preventing moisture from settling in any one area of the filament.
      Like the `printables procject from warpster` [printables link](https://www.printables.com/model/336958)

These enhancements aim to improve the usability and flexibility of the filament dryer, making it more accessible and informative for users. Stay tuned for updates as these features are developed and integrated into the project.

## License

This project is made available under the MIT License. This license allows everyone to use, modify, distribute, and privately or commercially exploit the project, under the condition that the original copyright notice and permission notice are included in all copies or substantial portions of the software.

The MIT License is a permissive license that is short and to the point. It lets people do anything they want with your code as long as they provide attribution back to you and don’t hold you liable.

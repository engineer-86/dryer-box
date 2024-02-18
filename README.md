# Filament Dryer Project

## Motivation

This project was initiated as a response to my Eibos Cyclopes filament dryer breaking down. Instead of discarding the entire unit, I decided to repurpose the PTC heating element and the case to create a new, custom filament dryer. This initiative not only recycles parts that are still functional but also provides a tailored solution to my 3D printing needs.

## Components

- **Microcontroller**: ESP8266, chosen for its WiFi capabilities and compatibility with Arduino code.
- **Sensor**: DHT11, used for measuring humidity and temperature within the dryer.
- **Relays**: Two relays are used, one to control the PTC heating element and another for the fan, ensuring precise temperature and airflow control.

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

MQTT (Message Queuing Telemetry Transport) is used for sending and receiving messages related to the dryer's status, including temperature, humidity, and operational commands. The device connects to an MQTT broker, specified in the `credentials.hpp`, to publish sensor readings and subscribe to control topics.

- **Publishing Data**: The device periodically publishes temperature and humidity readings to the MQTT topic `tele/heater/state`. This allows for remote monitoring of the filament dryer's environment through any MQTT client subscribed to the same topic.

  ```cpp
  connected_mqtt_client.publish("tele/heater/state", payload);
  ```

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

## License

This project is made available under the MIT License. This license allows everyone to use, modify, distribute, and privately or commercially exploit the project, under the condition that the original copyright notice and permission notice are included in all copies or substantial portions of the software.

The MIT License is a permissive license that is short and to the point. It lets people do anything they want with your code as long as they provide attribution back to you and don’t hold you liable.



#include "Provisioning.hpp"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

// ---------------------------------------------------------------------------
// HTML pages stored in program flash to keep heap free
// ---------------------------------------------------------------------------

static const char SETUP_HTML[] PROGMEM = R"html(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <title>Dryer Setup</title>
  <style>
    *{box-sizing:border-box}
    body{font-family:sans-serif;background:#f4f4f4;margin:0;padding:20px}
    .card{background:#fff;border-radius:8px;padding:24px;max-width:420px;margin:0 auto;box-shadow:0 2px 8px rgba(0,0,0,.12)}
    h1{margin:0 0 4px;color:#e05a00;font-size:1.4rem}
    p{margin:0 0 20px;color:#666;font-size:.9rem}
    h3{margin:16px 0 6px;font-size:.95rem;color:#333;border-bottom:1px solid #eee;padding-bottom:4px}
    label{display:block;font-size:.82rem;font-weight:600;color:#444;margin-top:10px}
    input{display:block;width:100%;padding:8px 10px;margin-top:3px;border:1px solid #ccc;border-radius:4px;font-size:.95rem}
    input:focus{outline:none;border-color:#e05a00}
    button{display:block;width:100%;margin-top:24px;padding:12px;background:#e05a00;color:#fff;border:none;border-radius:6px;font-size:1rem;cursor:pointer;font-weight:600}
    button:hover{background:#c24d00}
    .hint{font-size:.75rem;color:#999;margin-top:2px}
  </style>
</head>
<body>
<div class="card">
  <h1>Dryer Setup</h1>
  <p>Configure WiFi and MQTT broker, then save to restart.</p>
  <form method="POST" action="/save">
    <h3>WiFi</h3>
    <label>SSID
      <input name="ssid" type="text" required placeholder="Network name" autocomplete="off">
    </label>
    <label>Password
      <input name="pass" type="password" placeholder="Leave blank if open network">
    </label>
    <h3>MQTT Broker</h3>
    <label>IP Address
      <input name="broker_ip" type="text" required placeholder="192.168.1.100">
    </label>
    <label>Port
      <input name="broker_port" type="number" value="1883" required min="1" max="65535">
    </label>
    <label>Username
      <input name="broker_user" type="text" placeholder="optional">
    </label>
    <label>Password
      <input name="broker_pass" type="password" placeholder="optional">
    </label>
    <button type="submit">Save &amp; Restart</button>
  </form>
</div>
</body>
</html>
)html";

static const char SAVED_HTML[] PROGMEM = R"html(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <title>Saved</title>
  <style>
    body{font-family:sans-serif;display:flex;align-items:center;justify-content:center;height:100vh;margin:0;background:#f4f4f4}
    .card{background:#fff;border-radius:8px;padding:32px;text-align:center;box-shadow:0 2px 8px rgba(0,0,0,.12);max-width:300px}
    h2{color:#2a9d2a;margin:0 0 8px}
    p{color:#666;margin:0}
  </style>
</head>
<body>
<div class="card">
  <h2>Saved!</h2>
  <p>Dryer is restarting and connecting to your network&hellip;</p>
</div>
</body>
</html>
)html";

// ---------------------------------------------------------------------------

constexpr const char* Provisioning::CREDENTIALS_FILE;
constexpr const char* Provisioning::BOOT_COUNT_FILE;
constexpr const char* Provisioning::AP_SSID;
constexpr uint8_t     Provisioning::DNS_PORT;
constexpr uint8_t     Provisioning::RESET_BOOT_COUNT;

Provisioning::Provisioning() : server(80) {}

bool Provisioning::begin() {
    if (!LittleFS.begin()) {
        Serial.println("LittleFS mount failed — formatting");
        LittleFS.format();
        LittleFS.begin();
    }

    if (!loadCredentials()) {
        // No credentials stored yet — go straight to AP mode without touching
        // the boot counter (counter is only meaningful during normal operation).
        writeBootCount(0);
        Serial.println("No credentials found — entering AP mode.");
        startAPMode();
        return false; // unreachable
    }

    // Credentials exist: count this boot and check for rapid power-cycle reset.
    // main loop must call clearBootCounter() after ~10 s of normal operation.
    uint8_t boots = readBootCount() + 1;
    writeBootCount(boots);
    Serial.printf("Boot count: %d/%d\n", boots, RESET_BOOT_COUNT);

    if (boots >= RESET_BOOT_COUNT) {
        writeBootCount(0);
        clearCredentials();
        Serial.println("Factory reset triggered by 5x rapid power cycle — entering AP mode.");
        startAPMode();
        return false; // unreachable
    }

    Serial.println("Credentials loaded from storage.");
    return true;
}

uint8_t Provisioning::readBootCount() {
    if (!LittleFS.exists(BOOT_COUNT_FILE)) return 0;
    File f = LittleFS.open(BOOT_COUNT_FILE, "r");
    uint8_t count = f.parseInt();
    f.close();
    return count;
}

void Provisioning::writeBootCount(uint8_t count) {
    File f = LittleFS.open(BOOT_COUNT_FILE, "w");
    f.print(count);
    f.close();
}

void Provisioning::clearBootCounter() {
    writeBootCount(0);
}

bool Provisioning::loadCredentials() {
    if (!LittleFS.exists(CREDENTIALS_FILE)) return false;

    File f = LittleFS.open(CREDENTIALS_FILE, "r");
    if (!f) return false;

    StaticJsonDocument<512> doc;
    DeserializationError err = deserializeJson(doc, f);
    f.close();
    if (err) return false;

    credentials.wifiSSID       = doc["ssid"]        | "";
    credentials.wifiPassword   = doc["pass"]        | "";
    credentials.brokerIP       = doc["broker_ip"]   | "";
    credentials.brokerPort     = doc["broker_port"] | 1883;
    credentials.brokerUser     = doc["broker_user"] | "";
    credentials.brokerPassword = doc["broker_pass"] | "";

    return credentials.isValid();
}

void Provisioning::saveCredentials(const NetworkCredentials& creds) {
    StaticJsonDocument<512> doc;
    doc["ssid"]        = creds.wifiSSID;
    doc["pass"]        = creds.wifiPassword;
    doc["broker_ip"]   = creds.brokerIP;
    doc["broker_port"] = creds.brokerPort;
    doc["broker_user"] = creds.brokerUser;
    doc["broker_pass"] = creds.brokerPassword;

    File f = LittleFS.open(CREDENTIALS_FILE, "w");
    serializeJson(doc, f);
    f.close();
    Serial.println("Credentials saved to LittleFS.");
}

void Provisioning::clearCredentials() {
    if (LittleFS.begin()) {
        LittleFS.remove(CREDENTIALS_FILE);
        Serial.println("Credentials cleared.");
    }
}

void Provisioning::startAPMode() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID);
    IPAddress apIP = WiFi.softAPIP();

    // Redirect every DNS query to the device so browsers open the setup page
    dnsServer.start(DNS_PORT, "*", apIP);

    server.on("/",     HTTP_GET,  [this]() { handleRoot(); });
    server.on("/save", HTTP_POST, [this]() { handleSave(); });
    server.onNotFound(            [this]() { handleNotFound(); });
    server.begin();

    Serial.print("AP started — connect to WiFi \"");
    Serial.print(AP_SSID);
    Serial.print("\" and open http://");
    Serial.println(apIP.toString());

    while (true) {
        dnsServer.processNextRequest();
        server.handleClient();
        yield();
    }
}

void Provisioning::handleRoot() {
    server.send_P(200, "text/html", SETUP_HTML);
}

void Provisioning::handleSave() {
    NetworkCredentials creds;
    creds.wifiSSID       = server.arg("ssid");
    creds.wifiPassword   = server.arg("pass");
    creds.brokerIP       = server.arg("broker_ip");
    creds.brokerPort     = server.arg("broker_port").toInt();
    creds.brokerUser     = server.arg("broker_user");
    creds.brokerPassword = server.arg("broker_pass");

    if (!creds.isValid()) {
        server.send(400, "text/plain", "SSID and broker IP are required.");
        return;
    }

    saveCredentials(creds);
    server.send_P(200, "text/html", SAVED_HTML);
    delay(2000);
    ESP.restart();
}

void Provisioning::handleNotFound() {
    // Captive-portal redirect: send any unknown URL back to the setup page
    server.sendHeader("Location", "http://" + WiFi.softAPIP().toString() + "/");
    server.send(302, "text/plain", "");
}

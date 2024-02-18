#ifndef CREDENTIALS_H
#define CREDENTIALS_H
// creds for wifi and mqtt
class Credentials
{
private:
    const char *_wifi_ssid = "BLACKNET";
    const char *_wifi_password = "SarahKonrad2018!";
    const char *_broker_ip = "192.168.10.96";
    const int _broker_port = 1883;
    const char *_broker_user = "arox";
    const char *_broker_password = "UgeNDbMR1ljR1ill3MxRoR";

public:
    static const char *getWifiSSID() { return "BLACKNET"; }
    static const char *getWifiPassword() { return "SarahKonrad2018!"; }
    static const char *getBrokerIP() { return "192.168.10.96"; }
    static int getBrokerPort() { return 1883; }
    static const char *getBrokerUsername() { return "arox"; }
    static const char *getBrokerPassword() { return "UgeNDbMR1ljR1ill3MxRoR"; }
};

#endif /* CREDENTIALS_H */
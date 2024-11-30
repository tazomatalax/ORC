#pragma once
#include <Arduino.h>
#include <Ethernet.h>

class NetworkManager {
public:
    void begin();
    void update();
    bool isConnected();
    IPAddress getIP();
    
private:
    byte mac[6];
    IPAddress ip;
    bool connected;
    
    void initEthernet();
    void checkConnection();
};

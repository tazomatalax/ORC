#pragma once
#include <Arduino.h>
#include <EthernetServer.h>
#include <ArduinoJson.h>

class WebServerManager {
public:
    void begin();
    void handleClients();
    
private:
    EthernetServer server;
    static const int MAX_CLIENTS = 4;
    EthernetClient clients[MAX_CLIENTS];
    
    void handleClient(EthernetClient& client);
    void processRequest(EthernetClient& client, String& request);
    void sendResponse(EthernetClient& client, const JsonDocument& response);
};

// web_server.cpp
#include "web_server.h"

#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h> 

#include "index.h"

AsyncWebServer server(80);
DNSServer dns;

void initWebServer() {
    AsyncWiFiManager wifiManager(&server,&dns);
    wifiManager.autoConnect("Trafficlights", "password");

    Serial.println("Connected to WiFi");

    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // Serve the HTML page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        String htmlPage = MAIN_page;
        htmlPage.replace("WEBSOCKET_IP", WiFi.localIP().toString());
        request->send(200, "text/html", htmlPage);
    });

    server.begin();
}
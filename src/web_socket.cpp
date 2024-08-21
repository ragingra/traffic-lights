// web_socket.cpp
#include "web_socket.h"
#include <WebSocketsServer.h>

WebSocketsServer webSocket = WebSocketsServer(81);

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connection from %s\n", num, ip.toString().c_str());
            break;
        }
        case WStype_TEXT:
            Serial.printf("[%u] Text: %s\n", num, payload);
            break;
        case WStype_ERROR:
            Serial.printf("[%u] Error: %s\n", num, payload);
            break;
    }
}

void initWebSocket() {
    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);
}

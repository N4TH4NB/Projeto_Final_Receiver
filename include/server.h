#ifndef SERVER_H
#define SERVER_H

#include <ESPAsyncWebServer.h>

// Declaração de variáveis globais
extern AsyncWebServer server;
extern AsyncWebSocket ws;

// Declaração de funções
void setupServer();
void handleWebSocketEvents(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

#endif // SERVER_H

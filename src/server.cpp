#include "server.h"
#include "LittleFS.h"

// Declaração das variáveis globais
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Callback para tratar eventos do WebSocket
void handleWebSocketEvents(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    if (type == WS_EVT_CONNECT)
    {
        Serial.printf("Cliente conectado: %u\n", client->id());
    }
    else if (type == WS_EVT_DISCONNECT)
    {
        Serial.printf("Cliente desconectado: %u\n", client->id());
    }
}

// Função para configurar o servidor
void setupServer()
{
    // Configura o WebSocket
    ws.onEvent(handleWebSocketEvents);
    server.addHandler(&ws);

    // Configura as rotas do servidor
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/index.html", "text/html"); });

    server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/styles.css", "text/css"); });

    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/script.js", "text/javascript"); });

    // Resposta para arquivos não encontrados
    server.onNotFound([](AsyncWebServerRequest *request)
                      { request->send(404, "text/plain", "File not found"); });

    // Inicia o servidor
    server.begin();
    Serial.println("Servidor iniciado");
}

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <esp_now.h>
#include "LittleFS.h"
#include "wifi_cred.h"

// Definições de Wi-Fi
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

// Servidor e WebSocket
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Estrutura para dados recebidos
typedef struct struct_message {
    float temp;
    float press;
    float alt;
    float pressMar;
    int lum;
    float tensao;
    int chuva;
    long lon;
    long lat;
    char data[20];
} struct_message;

struct_message myData;
AsyncWebSocketClient *client = nullptr;

// Prototipação
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);
void handleWebSocketEvents(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

// Função de configuração
void setup() {
    // Inicializa o monitor serial
    Serial.begin(115200);

    // Inicializa o sistema de arquivos
    if (!LittleFS.begin()) {
        Serial.println("Erro ao montar LittleFS");
        return;
    }
    Serial.println("LittleFS montado com sucesso");

    // Configura Wi-Fi
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando ao Wi-Fi...");
    }
    Serial.println("Wi-Fi conectado.");
    Serial.printf("Endereço IP: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("Canal Wi-Fi: %d\n", WiFi.channel());

    // Inicializa ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Erro ao inicializar ESP-NOW");
        return;
    }
    esp_now_register_recv_cb(OnDataRecv);

    // Configura WebSocket
    ws.onEvent(handleWebSocketEvents);
    server.addHandler(&ws);

    // Configura rotas do servidor
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html", "text/html");
    });
    server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/styles.css", "text/css");
    });
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/script.js", "text/javascript");
    });
    server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "File not found");
    });

    // Inicia o servidor
    server.begin();
    Serial.println("Servidor iniciado");
}

// Função principal de loop
void loop() {
    ws.cleanupClients();
}

// Eventos do WebSocket
void handleWebSocketEvents(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.printf("Cliente conectado: %u\n", client->id());
    } else if (type == WS_EVT_DISCONNECT) {
        Serial.printf("Cliente desconectado: %u\n", client->id());
    }
}

// Callback de recebimento de dados ESP-NOW
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    // Copia os dados recebidos
    memcpy(&myData, incomingData, sizeof(myData));

    // Exibe os dados no monitor serial
    Serial.printf("Bytes recebidos: %d\n", len);
    Serial.print("MAC Address: ");
    for (int i = 0; i < 6; ++i) {
        Serial.printf("%02X", mac[i]);
        if (i < 5) Serial.print(":");
    }
    Serial.println();

    // Cria um documento JSON
    JsonDocument jsonDoc;
    jsonDoc["temp"] = myData.temp;
    jsonDoc["press"] = myData.press;
    jsonDoc["lum"] = myData.lum;
    jsonDoc["tensao"] = myData.tensao;
    jsonDoc["chuva"] = myData.chuva;
    jsonDoc["hora"] = myData.data;
    jsonDoc["lon"] = myData.lon;
    jsonDoc["lat"] = myData.lat;

    // Serializa e envia os dados via WebSocket
    size_t lenJson = measureJson(jsonDoc);
    AsyncWebSocketMessageBuffer *buffer = ws.makeBuffer(lenJson);
    if (buffer) {
        serializeJson(jsonDoc, (char *)buffer->get(), lenJson + 1);
        if (client) {
            client->text(buffer);
        } else {
            ws.textAll(buffer);
        }
    }
}

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <IOXhop_FirebaseESP32.h>
#include <esp_now.h>
#include "LittleFS.h"
#include "wifi_cred.h"

#define FIREBASE_HOST "https://projeto-final-4de38-default-rtdb.firebaseio.com/"    
#define FIREBASE_AUTH "senha"

// Definições de Wi-Fi
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
const unsigned long intervaloEnvio = 1000;
unsigned long previousMillis = millis();

// Servidor e WebSocket
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Estrutura para dados recebidos
typedef struct struct_message
{
    float temp;
    float press;
    float alt;
    int lum;
    float tensao;
    int chuva;
    long lon;
    long lat;
    unsigned short ano;
    unsigned char mes;
    unsigned char dia;
    unsigned char hora;
    unsigned char minuto;
    unsigned char segundo;
} struct_message;

struct_message myData;

// Eventos do WebSocket
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("Cliente WebSocket #%u conectado de %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("Cliente WebSocket #%u desconectado\n", client->id());
        break;
    case WS_EVT_DATA:
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
}

void sendDataWs()
{
    // Cria um documento JSON
    JsonDocument jsonDoc;
    jsonDoc["temp"] = myData.temp;
    jsonDoc["press"] = myData.press;
    jsonDoc["lum"] = myData.lum;
    jsonDoc["tensao"] = myData.tensao;
    jsonDoc["chuva"] = myData.chuva;
    jsonDoc["lon"] = myData.lon;
    jsonDoc["lat"] = myData.lat;

    char datetime[20];
    snprintf(datetime, sizeof(datetime), "%02d-%02d-%04dT%02d:%02d:%02d",
             myData.dia, myData.mes, myData.ano,
             myData.hora, myData.minuto, myData.segundo);
    jsonDoc["hora"] = datetime;

    // Serializa e envia os dados via WebSocket
    size_t lenJson = measureJson(jsonDoc);
    AsyncWebSocketMessageBuffer *buffer = ws.makeBuffer(lenJson);
    if (buffer)
    {
        serializeJson(jsonDoc, (char *)buffer->get(), lenJson + 1);
        ws.textAll(buffer);
    }
}

// Callback de recebimento de dados ESP-NOW
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    // Copia os dados recebidos
    memcpy(&myData, incomingData, sizeof(myData));

    // Exibe os dados no monitor serial
    Serial.printf("Bytes recebidos: %d\n", len);
    Serial.print("MAC Address: ");
    for (int i = 0; i < 6; ++i)
    {
        Serial.printf("%02X", mac[i]);
        if (i < 5)
            Serial.print(":");
    }
    Serial.println();
}

// Inicializa o sistema de arquivos
void initFS()
{
    if (!LittleFS.begin())
    {
        Serial.println("Erro ao montar LittleFS");
        return;
    }
    Serial.println("LittleFS montado com sucesso");
}

// Configura Wi-Fi
void initWifi()
{
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Conectando ao Wi-Fi...");
    }
    Serial.println("Wi-Fi conectado.");
    Serial.printf("Endereço IP: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("Canal Wi-Fi: %d\n", WiFi.channel());

    // Inicializa ESP-NOW
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Erro ao inicializar ESP-NOW");
        return;
    }
    esp_now_register_recv_cb(OnDataRecv);
}

// Configura Servidor
void initWeb()
{
    ws.onEvent(onEvent);
    server.addHandler(&ws);

    // Configura caminho dos arquivos do servidor
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/index.html", "text/html"); });
    server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/styles.css", "text/css"); });
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/script.js", "text/javascript"); });
    server.onNotFound([](AsyncWebServerRequest *request)
                      { request->send(404, "text/plain", "File not found"); });

    // Inicia o servidor
    server.begin();
    Serial.println("Servidor iniciado");
}

void setup()
{
    Serial.begin(115200);
    initFS();
    initWifi();
    initWeb();
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

// Função principal de loop
void loop()
{
    ws.cleanupClients();

    if (millis() - previousMillis >= intervaloEnvio)
    {
        sendDataWs();
        previousMillis = millis();
    }
}

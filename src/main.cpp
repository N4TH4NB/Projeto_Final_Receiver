#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include "wifi_espnow.h"
#include "data_handler.h"

AsyncWebServer server(80);

void setup()
{
  // Inicializa o monitor serial
  Serial.begin(115200);

  // Inicializa o sistema de arquivos LittleFS
  if (!LittleFS.begin())
  {
    Serial.println("Erro ao montar LittleFS");
    return;
  }
  Serial.println("LittleFS montado com sucesso");

  // Inicializa o ESP-NOW e Wi-Fi
  inicializaWifi();

  // Configura rotas do servidor
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", "text/html"); });

  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/styles.css", "text/css"); });

  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/script.js", "text/javascript"); });

  // Rota para retornar dados em JSON
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              String jsonData = convertToJson();
              request->send(200, "application/json", jsonData); });

  // Resposta para arquivos não encontrados
  server.onNotFound([](AsyncWebServerRequest *request)
                    { request->send(404, "text/plain", "File not found"); });

  // Inicia o servidor
  server.begin();
  Serial.println("Servidor iniciado");
}

void loop()
{
  // O loop pode ser mantido vazio ou usado para outras tarefas
}

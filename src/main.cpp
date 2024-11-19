#include <Arduino.h>
#include "server.h"
#include "wifi_espnow.h"
#include "LittleFS.h"

void setup()
{
  // Inicializa o monitor serial
  Serial.begin(115200);

  // Inicializa o sistema de arquivos
  if (!LittleFS.begin())
  {
    Serial.println("Erro ao montar LittleFS");
    return;
  }
  Serial.println("LittleFS montado com sucesso");

  // Configura o Wi-Fi e ESP-NOW
  inicializaWifi();

  // Configura o servidor
  setupServer();

  // Passa o WebSocket para o ESP-NOW
  //setWebSocket(&ws);
}

void loop()
{
  // Manter o loop vazio ou adicionar tarefas
}

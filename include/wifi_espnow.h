#ifndef WIFI_ESPNOW_H
#define WIFI_ESPNOW_H

#include <esp_now.h>
#include <WiFi.h>

// Estrutura da mensagem recebida
typedef struct struct_message
{
  float temp;
  float press;
  float alt;
  float pressMar;
  int lum;
  float tensao;
  int chuva;
  unsigned long hora;
  long lon;
  long lat;
} struct_message;

// Variáveis globais
extern struct_message myData;
extern const char *ssid;
extern const char *password;

// Declaração de funções
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);
void inicializaWifi();
//void setWebSocket(AsyncWebSocket *websocket);

#endif // WIFI_ESPNOW_H

#include "wifi_espnow.h"
#include "wifi_cred.h"
#include <ArduinoJson.h>

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
struct_message myData;

// Função para associar o WebSocket
/*void setWebSocket(AsyncWebSocket *websocket)
{
  ws = websocket;
}*/

// Função de callback para receber dados
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&myData, incomingData, sizeof(myData));

  Serial.print("Bytes recebidos: ");
  Serial.println(len);

  Serial.print("MAC Address: ");
  for (int i = 0; i < 6; ++i)
  {
    Serial.printf("%02X", mac[i]);
    if (i < 5)
      Serial.print(":");
  }
  Serial.println();

  // Envia os dados recebidos via WebSocket
  JsonDocument jsonDoc;
  jsonDoc["Temperatura"] = myData.temp;
  jsonDoc["Pressão"] = myData.press;
  jsonDoc["Altitude"] = myData.alt;
  jsonDoc["PressãoMar"] = myData.pressMar;
  jsonDoc["Luminosidade"] = myData.lum;
  jsonDoc["Tensão"] = myData.tensao;
  jsonDoc["Chuva"] = myData.chuva;
  jsonDoc["Hora"] = myData.hora;
  jsonDoc["Longitude"] = myData.lon;
  jsonDoc["Latitude"] = myData.lat;

  size_t leng = measureJson(jsonDoc);
  AsyncWebSocketMessageBuffer *buffer = ws.makeBuffer(leng); //  creates a buffer (len + 1) for you.
  if (buffer)
  {
    serializeJson(jsonDoc, (char *)buffer->get(), leng + 1);
      ws.textAll(buffer);
  }
  Serial.print("Temperatura = ");
  Serial.print(myData.temp);
  Serial.print(", Pressão = ");
  Serial.print(myData.press);
  Serial.print(", Altitude = ");
  Serial.print(myData.alt);
  Serial.print(", PressãoMar = ");
  Serial.print(myData.pressMar);
  Serial.print(", Luminosidade = ");
  Serial.print(myData.lum);
  Serial.print(", Tensão = ");
  Serial.print(myData.tensao);
  Serial.print(", Chuva = ");
  Serial.print(myData.chuva);
  Serial.print(", Hora = ");
  Serial.print(myData.hora);
  Serial.print(", Longitude = ");
  Serial.print(myData.lon);
  Serial.print(", Latitude = ");
  Serial.println(myData.lat);
}

// Inicialização do Wi-Fi e ESP-NOW
void inicializaWifi()
{
  // Configura o modo Wi-Fi
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Conectando ao Wi-Fi...");
  }
  Serial.println("Wi-Fi conectado.");

  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Canal Wi-Fi: ");
  Serial.println(WiFi.channel());

  // Inicializa o ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Erro ao inicializar ESP-NOW");
    return;
  }

  // Registra o callback de recebimento
  esp_now_register_recv_cb(OnDataRecv);
}

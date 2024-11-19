#include "wifi_espnow.h"
#include "data_handler.h"

const char *ssid = "SSID";
const char *password = "PASSWORD";

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

  // Converte e exibe os dados como JSON
  String jsonData = convertToJson();
  Serial.println("Dados em JSON:");
  Serial.println(jsonData);
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

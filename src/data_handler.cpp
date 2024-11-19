#include "data_handler.h"

struct_message myData;

// Função para converter dados em JSON
String convertToJson()
{
    JsonDocument jsonDoc;

    jsonDoc["temp"] = myData.temp;
    jsonDoc["press"] = myData.press;
    jsonDoc["alt"] = myData.alt;
    jsonDoc["pressMar"] = myData.pressMar;
    jsonDoc["lum"] = myData.lum;
    jsonDoc["tensao"] = myData.tensao;
    jsonDoc["chuva"] = myData.chuva;
    jsonDoc["hora"] = myData.hora;
    jsonDoc["lon"] = myData.lon;
    jsonDoc["lat"] = myData.lat;

    String jsonString;
    serializeJson(jsonDoc, jsonString);
    return jsonString;
}

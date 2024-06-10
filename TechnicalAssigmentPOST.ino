#include <WiFi.h>
#include <HTTPClient.h>
#include <DS18B20.h>
#include <time.h>

const char* ssid = "admin";
const char* password = "JhonFKennedy67";
const char* serverName = "http://192.168.1.12:5000/sensor/data";
DS18B20 ds(18);

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // Mengambil Waktu Indonesia
  configTime(0, 0, "0.id.pool.ntp.org", "1.id.pool.ntp.org");

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Gagal Mengambil Waktu");
    while (1) {
      delay(500);
    }
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Gagal Mengambil Waktu");
      return;
    }
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%dT%H:%M:%S", &timeinfo);
    float temperature = ds.getTempC();
    String Message = "{\"temperature\":" + String(temperature) + ",\"time\":\"" + String(timeStr) + "\"}";

    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(Message);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.println("Response: ");
      Serial.println(response);
    } else {
      Serial.print("Gagal Mengirim POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("Wifi Error");
  }

  delay(5000);
}

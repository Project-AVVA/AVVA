#include <PubSubClient.h>
#include <WiFi.h>
#include <Adafruit_BME280.h>
Adafruit_BME280 bme280;
#include <SparkFun_SGP30_Arduino_Library.h>
#include <Wire.h>
#define PIN_HCSR501 2
SGP30 mySensor;
const char* ssid = "RTF_IoT"; // Указать название сети
const char* password = "rtf-1234"; // Указать пароль сети
IPAddress server(192, 168, 88, 100); // Указать IP-адрес MQTT-сервера
bool hjk = 0;
bool okno = 0;
void callback(char* topic, byte* payload, unsigned int length) {
Serial.print("Message arrived [");
Serial.print(topic);
Serial.print("] ");
for (int i = 0; i < length; i++) {
Serial.print((char)payload[i]);
}
Serial.println();
}

WiFiClient espClient;
PubSubClient client(espClient);
void reconnect() { // Цикл подклчения к MQTT
while (!client.connected()) {
Serial.print("Attempting MQTT connection..."); // Попытка подключения // Поменять значение arduinoClient на любое другое (уникальное)
if (client.connect("second comlukter")) {
Serial.println("connected");
// После подключения опубликуется сообщение
client.subscribe("inTopic");
} else {
Serial.print("failed, rc=");
Serial.print(client.state());
Serial.println(" try again in 5 seconds"); // Ожидание в 5 секунд для повторения попытки подключения
delay(5000);
}
}
}
void setup() {
Serial.begin(115200);
Wire.begin();
bool bme_status = bme280.begin();
if (!bme_status)
Serial.println("Could not find a valid BME280 sensor, check wiring!");
delay(10);
Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());
client.setServer(server, 1883);
client.setCallback(callback);
delay(1500);
}
void loop() {
if (!client.connected()) {
reconnect();
}
Serial.println("TEST!");
client.loop();
float t = bme280.readTemperature();
// Вывод измеренных значений в терминал
Serial.println("Air temperature = " + String(t, 1) + " *C");
delay(250);
// Установка занчений температуры для открытия и закрытия окна
if (t >= 25 && okno == 0) {
client.publish("/class/stand<1>/actuator", "open", 1);
Serial.println("open");
okno = 1;
} else {
if (t < 25 && okno == 1) {
client.publish("/class/stand<1>/actuator", "close", 1);
Serial.println("close");
okno = 0;
}
}
if (hjk == 1) {
client.publish("/class/stand<1>/actuator", "on", 1);
Serial.println("on");
hjk = 0;
}
}

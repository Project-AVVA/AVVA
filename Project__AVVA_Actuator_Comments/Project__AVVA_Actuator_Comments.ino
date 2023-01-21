#define PWMA 21
#define PWMB 22
#include <Wire.h>
#include "TLC59108.h"
#include <PubSubClient.h>
#include <WiFi.h>
const char* ssid = "RTF_IoT"; // Указать название сети
const char* password = "rtf-1234"; // Указать пароль сети
IPAddress server(192, 168, 88, 100); // Указать IP-адрес MQTT-сервера
//обозначение пинов подключения реле, которое отвечает за приведение в действие моторов
#define PUMP 16
#define WIND 17
#define HW_RESET_PIN 0 // Только программный сброс
#define I2C_ADDR TLC59108::I2C_ADDR::BASE

TLC59108 leds(I2C_ADDR + 7);
// Массивы для определения команд
char mas1[2] = {'o', 'n'};
char mas2[4] = {'o', 'p', 'e', 'n'};
char mas3[5] = {'c', 'l', 'o', 's', 'e'};
int j = 30000;

void callback(char* topic, byte* payload, unsigned int length) {
Serial.print("Message arrived [");
Serial.print(topic);
Serial.print("] ");
//Цикл для определения актуатором полученной команды и выполнение соответствующего действия
for (int i = 0; i < length; i++) {
Serial.print((char)payload[i]);
}
Serial.println(length);
int counter = 0;
if (length == 4) {
for (int i = 0; i < length; i++) {
if ((char)payload[i] == mas2[i]) counter++;
}
if (counter == 4) {
digitalWrite(PUMP, HIGH), digitalWrite(WIND, HIGH);
Serial.println("open");
}
}
else if (length == 5) {

for (int i = 0; i < length; i++) {
if ((char)payload[i] == mas3[i]) counter++;
}
if (counter == 5) {
digitalWrite(PUMP, LOW), digitalWrite(WIND, LOW);
Serial.println("close");
}
}
else if (length == 2)
{ for (int i = 0; i < length; i++) {
if ((char)payload[i] == mas1[i]) counter++;
}
if (counter == 2) {
for (byte pwm = 0; pwm < 0xff; pwm++)
{
leds.setBrightness(3, pwm);
delay(10000);
}

}
else if (length == 5)
{
digitalWrite(PUMP, LOW), digitalWrite(WIND, LOW);
Serial.println("close");
}
}
Serial.println();
}
void Null() {}
int m;
WiFiClient espClient;
PubSubClient client(espClient);
void reconnect() {
// //Цикл подключения к MQTT
while (!client.connected()) {
Serial.print("Attempting MQTT connection...");
client.publish("/class/stand<1>/error", "I'll be back...");
// Попытка подключения
// Поменять значение arduinoClient на любое другое (уникальное)
if (client.connect("NSLR5", "/class/stand<1>/error", 2, Null, "I'll be back...")) {
Serial.println("connected");
// После подключения публикуется сообщение...
client.publish("outTopic", "Actuator");
// повторная подписка
client.subscribe("/class/stand<1>/actuator");
} else {
Serial.print("failed, rc=");
client.publish("/class/stand<1>/error", "I'll be back...");
Serial.print(client.state());
Serial.println(" try again in 5 seconds");
// Задержка в 5 секунд для повторной попытки
}
}
}

void setup() {
// Инициализация последовательного порта
Serial.begin(115200);
// Инициализация выхода реле
pinMode(PUMP, OUTPUT);
pinMode(WIND, OUTPUT);
// поместите сюда свой установочный код, чтобы запустить его один раз:
Wire.begin();
leds.init(HW_RESET_PIN);
leds.setLedOutputMode(TLC59108::LED_MODE::PWM_IND);
Serial.begin(115200);
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
client.loop();
}

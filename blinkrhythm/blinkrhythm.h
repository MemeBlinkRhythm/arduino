#include <Metro.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

//受信
Metro udpReceiveMetro = Metro(100);

//送信
Metro udpSendMetro = Metro(100);

//WiFi AP情報
#define WIFI_SSID "blinkrhythm"
#define WIFI_PWD "jinsmeme"
IPAddress ip(192,168,10,1);
IPAddress subnet(255,255,255,0);

WiFiUDP udp;
#define UDP_PORT 2390
#define UDP_BROADCAST_IP "192.168.10.255"

const int BUFF_SIZE = 3;
char packetBuffer[BUFF_SIZE];

//LEDは12,13,14pin
const int LED_PIN_ARRAY_SIZE = 3;
const int LED_PIN[LED_PIN_ARRAY_SIZE] = {12,13,14};

#define LED_ON LOW
#define LED_OFF HIGH

//Servo 2
#define SERVO_PIN 2
Servo servo;

//SW 0
#define SW_PIN 0

//受信データを扱いやすくするための構造体
struct CmdPack {
  char id;
  char blink;
  char sec;
};

//送信パケット
struct BroadcastPack {
  char status;
  char o;
  char u1;
  char u2;
};

struct BroadcastPack sendBroadcastPack;


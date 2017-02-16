#include "blinkrhythm.h"

void setup() {
  //シリアルを初期化
  Serial.begin(115200);

  //スイッチ初期化
  swInit();
  
  //Wi-FiをAPとして初期化
  initAP();

  //LEDピンを初期化
  ledInit();
  
  //UDP初期化
  udpInit();
}

void loop() {  
  //受信処理
  if(udpReceiveMetro.check() == 1){
    udpReceive();
  }

  //送信処理
  if(udpSendMetro.check() == 1){
    //送信処理
    udpBroadcast();
  }
}

//スイッチ初期化
void swInit(){  
  pinMode(SW_PIN, INPUT);
  attachInterrupt(SW_PIN, clickSw, FALLING);
}

//Wi-Fi 初期化
void initAP(){
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(ip, ip, subnet);
  WiFi.softAP(WIFI_SSID, WIFI_PWD);
}

//受信処理
void udpReceive(){
  //Serial.println("udpReceive");
  int size = udp.parsePacket();
  if(size > 0){
    //Serial.print("receive size ");
    //Serial.println(size);

    memset(packetBuffer, 0, sizeof(packetBuffer));
    int len = udp.read(packetBuffer, BUFF_SIZE);
    if(len > 0){
      //構造体キャストして扱いやすくする
      CmdPack* pack = (CmdPack *)&packetBuffer;
      Serial.println("----");
      Serial.println(pack->id,HEX);
      Serial.println(pack->blink,HEX);
      Serial.println(pack->sec,HEX);
      Serial.println("----");
      
      //idにあわせた LEDを点灯 させる
      if(pack->blink == 1){
        ledBlink(LED_PIN[pack->id]);
        //瞬き回数をカウントする
        if(pack->id == 0){
          sendBroadcastPack.p++;    
        }
        else if(pack->id == 1){
          sendBroadcastPack.c1++;    
        }
        else if(pack->id == 2){
          sendBroadcastPack.c2++;    
        }
      }
    }
  }
}

//LEDピンを初期化
void ledInit(){
  //LEDピンをデジタル出力として初期化
  for(int i=0;i<LED_PIN_ARRAY_SIZE;i++)
    pinMode(LED_PIN[i], OUTPUT);
}

//LEDを点灯後に消灯
void ledBlink(int led){
  digitalWrite(led, LED_ON);
  delay(300);
  digitalWrite(led, LED_OFF); 
}

//UDP初期化
void udpInit(){
  //UDP受信ができるように設定
  udp.begin(UDP_PORT);
}

//サーボ初期化
void servoInit(){
  servo.attach(SERVO_PIN);
  servo.write(0);
}

void udpBroadcast(){
  if(sendBroadcastPack.status > 0){
    //測定結果の比較
    if(sendBroadcastPack.status == 2){
      if((sendBroadcastPack.p == sendBroadcastPack.c1)
        ||(sendBroadcastPack.p == sendBroadcastPack.c2)){
        //どちらかの回数が一致した場合 サーボを動かす
        //TODO 実機に合わせて動きの修正が必要    
        servo.write(0);
        delay(300);
        servo.write(45);
        delay(300);
        servo.write(0);
      }
    }
    
    //結果を Broadcast 送信する
    sendBroadcast( sendBroadcastPack );

    if(sendBroadcastPack.status == 2){
      //ブロードキャストしたので sendBroadcastPackを初期値へ戻す
      sendBroadcastPack.status = 0;
      sendBroadcastPack.p = 0;
      sendBroadcastPack.c1 = 0;
      sendBroadcastPack.c2 = 0;
    }
  }
}

//ブロードキャストする
void sendBroadcast(BroadcastPack pack){
  //Serial.println("sendBroadcast");
  udp.beginPacket(UDP_BROADCAST_IP, UDP_PORT);
  udp.write((char*)&pack,sizeof(BroadcastPack));
  udp.endPacket();  
}

//SWが押されたとき
void clickSw(){
  Serial.println("clickSw");
  if(sendBroadcastPack.status == 0){
    Serial.println("start");
    //ゲーム スタート
    sendBroadcastPack.status = 1;
  }
  else{
    Serial.println("end");
    //ゲーム 終了
    sendBroadcastPack.status = 2;
  }
}


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>

// WiFi 设置
// WiFi 网络名称和密码
const char* ssid = "Yunshu_Drwells";
const char* password = "yzy@0203yzy@0203";

//const char* ssid = "DUOBAO";
//const char* password = "yunshu666";

// 指定的消息
const char* expectedMessage0 = "GET_IP";
const char* expectedMessage1 = "MasterLight_ON";
const char* expectedMessage2 = "MasterLight_OFF";
const char* expectedMessage3 = "CONNECT_NEW_WIFI";

// 创建一个 Web 服务器实例
ESP8266WebServer server(80);

// GPIO 设置
#define LED_PIN    0  // GPIO2 (D4) on ESP8266-01S
#define NUM_LEDS   7 // LED的数量

// 创建NeoPixel对象
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void connect_wifi(const char* ssid, const char* password){
  // 连接到 WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  // 初始化串口通信
  Serial.begin(115200);
  
  //连接wifi
  connect_wifi(ssid, password);
  
  // 设置处理函数
  server.on("/", handleRoot);
  server.on("/get_ip", handleGetIP);
  server.on("/masterlight_on", handleMasterLightOn);
  server.on("/masterlight_off", handleMasterLightOff);
  server.on("/connect", handleConnectToNewWifi);

  // 开始服务器
  server.begin();
  Serial.println("HTTP server started");

  // 初始化 GPIO
  pinMode(LED_PIN, OUTPUT);

  // 初始化LED条
  strip.begin();
  // 设置所有LED为关闭状态
  strip.show();
}

void loop() {
  // 处理客户端请求
  server.handleClient();
}

void handleRoot() {
  String message = "MasterLight";
  server.send(200, "text/plain", message);
}

void handleGetIP() {
  if (server.arg("msg") == expectedMessage0) {
    String ip = WiFi.localIP().toString();
    server.send(200, "text/plain", ip);
    Serial.print("get ip:");
    Serial.println(ip);
  } else {
    server.send(400, "text/plain", "Invalid request");
  }
}

void handleMasterLightOn() {
  if (server.arg("msg") == expectedMessage1) {
    //获取灯光颜色
    int R = server.arg("R").toInt();
    int G = server.arg("G").toInt();
    int B = server.arg("B").toInt();
    //设置灯光颜色
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, strip.Color(R, G, B));
    }
    strip.show();

    server.send(200, "text/plain", "masterlight on");
    Serial.println("masterlight on");
    //Serial.println("set color to R:%d G:%d B:%d", R, G, B);
  } else {
    server.send(400, "text/plain", "Invalid request");
  }
}

void handleMasterLightOff() {
  if (server.arg("msg") == expectedMessage2) {
    //关闭主灯
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();

    server.send(200, "text/plain", "masterlight off");
    Serial.println("masterlight off");
  } else {
    server.send(400, "text/plain", "Invalid request");
  }
}

void handleConnectToNewWifi() {
  if (server.arg("msg") == expectedMessage3) {
     // 获取参数
    String ssid = server.arg("ssid");
    String password = server.arg("pwd");
    // 连接到新 WiFi
    connect_wifi(ssid.c_str(), password.c_str());
  } else {
    server.send(400, "text/plain", "Invalid request");
  }
}

//在浏览器中访问 http://<ESP8266_IP>/,将返回模块信息
//http://<ESP8266_IP>/get_ip?msg=GET_IP，将返回 ESP8266 的 IP 地址。
//http://<ESP8266_IP>/masterlight_on?msg=MasterLight_ON&R=<>&G=<>&B=<>，开主灯并设置颜色。
//http://<ESP8266_IP>/masterlight_off?msg=MasterLight_OFF，将关闭主灯。
//http://<ESP8266_IP>/connect?msg=CONNECT_NEW_WIFI&ssid=<wifi name>&pwd=<password>，将连接新的wifi。

/*
http://192.168.2.40/
http://192.168.2.40/get_ip?msg=GET_IP
http://192.168.2.40/masterlight_on?msg=MasterLight_ON&R=255&G=0&B=0
http://192.168.2.40/masterlight_off?msg=MasterLight_OFF
http://192.168.2.40/connect?msg=CONNECT_NEW_WIFI&ssid=Yunshu_Drwells&pwd=yzy@0203yzy@0203
*/

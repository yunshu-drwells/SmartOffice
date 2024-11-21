#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// WiFi 设置
// WiFi 网络名称和密码
const char* ssid = "Yunshu_Drwells";
const char* password = "yzy@0203yzy@0203";

//const char* ssid = "DUOBAO";
//const char* password = "yunshu666";

// 指定的消息
const char* expectedMessage0 = "GET_IP";
const char* expectedMessage1 = "FAN_ON";
const char* expectedMessage2 = "FAN_OFF";
const char* expectedMessage3 = "CONNECT_NEW_WIFI";

// 创建一个 Web 服务器实例
ESP8266WebServer server(80);

// GPIO 设置
const int Pin0 = 0; // GPIO0

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
  // 初始化 GPIO
  pinMode(Pin0, OUTPUT);
  digitalWrite(Pin0, LOW); // 初始状态低电平

  // 初始化串口通信
  Serial.begin(115200);
  
  //连接wifi
  connect_wifi(ssid, password);
  
  // 设置处理函数
  server.on("/", handleRoot);
  server.on("/get_ip", handleGetIP);
  server.on("/fan_on", handleFanOn);
  server.on("/fan_off", handleFanOff);
  server.on("/connect", handleConnectToNewWifi);
  
  
  // 开始服务器
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // 处理客户端请求
  server.handleClient();
}

void handleRoot() {
  String message = "FAN";
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

void handleFanOn() {
  if (server.arg("msg") == expectedMessage1) {
    //开启风扇
    digitalWrite(Pin0, HIGH); // 拉高 GPIO0
    server.send(200, "text/plain", "fan on");
    Serial.println("fan on");
  } else {
    server.send(400, "text/plain", "Invalid request");
  }
}

void handleFanOff() {
  if (server.arg("msg") == expectedMessage2) {
    //关闭风扇
    digitalWrite(Pin0, LOW); // 拉低 GPIO0
    server.send(200, "text/plain", "fan off");
    Serial.println("fan off");
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
//http://<ESP8266_IP>/fan_on?msg=FAN_ON，将开启风扇。
//http://<ESP8266_IP>/fan_off?msg=FAN_OFF，将关闭风扇。
//http://<ESP8266_IP>/connect?msg=CONNECT_NEW_WIFI&ssid=<wifi name>&pwd=<password>，将连接新的wifi。

/*
http://192.168.2.37/
http://192.168.2.37/get_ip?msg=GET_IP
http://192.168.2.37/fan_on?msg=FAN_ON
http://192.168.2.37/fan_off?msg=FAN_OFF
http://192.168.2.37/connect?msg=CONNECT_NEW_WIFI&ssid=Yunshu_Drwells&pwd=yzy@0203yzy@0203
*/
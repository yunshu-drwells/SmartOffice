#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>

// WiFi 设置
// WiFi 网络名称和密码
const char* ssid0 = "Yunshu_Drwells";
const char* password0 = "yzy@0203yzy@0203";

const char* ssid1 = "DUOBAO";
const char* password1 = "yunshu666";

// UDP 设置
WiFiUDP udp;
unsigned int localUdpPort = 8080;  // 本地端口号
char incomingPacket[255];  // 用于存储接收到的数据包
//char replyPacket[] = "Fan";  // 设备名称

// 指定的消息
//const char* expectedMessage0 = "GET_IP";
//const char* expectedMessage = "DISCOVER_DEVICES";
const char* expectedMessage0 = "SpotLight_ON";  //SpotLight_ON&R=<>&G=<>&B=<>
const char* expectedMessage1 = "SpotLight_OFF";
const char* expectedMessage2 = "CONNECT_NEW_WIFI";  //CONNECT_NEW_WIFI&ssid=Yunshu_Drwells&pwd=yzy@0203yzy@0203

// GPIO 设置
#define LED_PIN    0  // GPIO2 (D4) on ESP8266-01S
#define NUM_LEDS   3 // LED的数量

// 创建NeoPixel对象
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

#include <ESP8266WiFi.h>

bool connect_wifi(const char* ssid, const char* password, int timeout) {
  // 连接到 WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  int elapsed_time = 0;
  while (WiFi.status() != WL_CONNECTED && elapsed_time < timeout) {
    delay(100);
    Serial.print(".");
    elapsed_time += 100;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    Serial.println("");
    Serial.println("WiFi connection failed");
    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL:
        Serial.println("SSID not found");
        break;
      case WL_CONNECT_FAILED:
        Serial.println("Connection failed");
        break;
      case WL_CONNECTION_LOST:
        Serial.println("Connection lost");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
    return false;
  }
}

void beginListening(int localUdpPort){
  // 开始监听UDP广播
  udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
}

void setup() {
  Serial.begin(115200);
  
  // 连接WiFi
  if(false == connect_wifi(ssid0, password0, 10000)){
    connect_wifi(ssid1, password1, 10000);
  }
  
  //监听端口
  beginListening(localUdpPort);

  // 初始化 GPIO
  pinMode(LED_PIN, OUTPUT);

  // 初始化LED条
  strip.begin();
  // 设置所有LED为关闭状态
  strip.show();
}

void loop() {
  //监听8080端口广播，收到“DISCOVER_DEVICES”消息，就向发送这个消息的ip发起tcp连接请求，并发送自己的设备名和ip地址
  int packetSize = udp.parsePacket();
  if (packetSize) {
    // 接收UDP数据包
    int len = udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = 0;
    }
    
    Serial.printf("UDP packet contents: %s\n", incomingPacket);
    
    // 检查是否是指定消息
    if (strstr(incomingPacket, expectedMessage0)) {  //MasterLight_ON&R=<>&G=<>&B=<>
      //开启主灯
      char* st0 = strstr(incomingPacket, "&R=");
      char* st1 = strstr(incomingPacket, "&G=");
      char* st2 = strstr(incomingPacket, "&B=");

      char R_str[4] = {0};
      char G_str[4] = {0};
      char B_str[4] = {0};

      strncpy(R_str, st0+3, st1-(st0+3));
      //Serial.println(R_str);
      strncpy(G_str, st1+3, st2-(st1+3));
      //Serial.println(G_str);
      strncpy(B_str, st2+3, strlen(st2+3));
      //Serial.println(B_str);

      //获取灯光颜色
      int R = String(R_str).toInt();
      int G = String(G_str).toInt();
      int B = String(B_str).toInt();

      //设置灯光颜色
      for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, strip.Color(R, G, B));
      }
      strip.show();
      Serial.println("masterlight on");
    }
    if (strcmp(incomingPacket, expectedMessage1) == 0) {
      //关闭射灯
      for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
      strip.show();
      Serial.println("masterlight off");
    }
    if(strstr(incomingPacket, expectedMessage2)){  //CONNECT_NEW_WIFI&ssid=Yunshu_Drwells&pwd=yzy@0203yzy@0203
      //获取新wifi的名称和密码
      char* ssid_ = strstr(incomingPacket, "&ssid=");
      char* pwd_ = strstr(incomingPacket, "&pwd=");

      char ssid_str[32] = {0};
      char pwd_str[32] = {0};

      strncpy(ssid_str, ssid_+6, pwd_-(ssid_+6));
      //Serial.println(R_str);
      strncpy(pwd_str, pwd_+5, strlen(pwd_+5));
      
      Serial.println(ssid_str);
      Serial.println(pwd_str);

    // 连接新WiFi
    connect_wifi(ssid_str, pwd_str, 10000);

    //监听端口
    beginListening(localUdpPort);
    }
  }
  //监听80端口，用于连接服务器发过来的tcp请求
}

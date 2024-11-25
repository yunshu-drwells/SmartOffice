#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

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
char replyPacket[] = "Fan";  // 设备名称

// 指定的消息
const char* expectedMessage = "DISCOVER_DEVICES";
const char* expectedMessage0 = "FAN_ON";
const char* expectedMessage1 = "FAN_OFF";
const char* expectedMessage2 = "CONNECT_NEW_WIFI";  //CONNECT_NEW_WIFI&ssid=Yunshu_Drwells&pwd=yzy@0203yzy@0203

// GPIO 设置
const int Pin0 = 0; // GPIO0

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

  // 初始化 GPIO
  pinMode(Pin0, OUTPUT);
  digitalWrite(Pin0, LOW); // 初始状态低电平
  
  // 连接WiFi
  if(false == connect_wifi(ssid1, password1, 10000)){
    connect_wifi(ssid0, password0, 10000);
  }
  
  //监听端口
  beginListening(localUdpPort);
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
    /*
    if (strcmp(incomingPacket, expectedMessage) == 0) {
      // 获取发送方的IP和端口
      IPAddress remoteIp = udp.remoteIP();
      Serial.println("Remote ip:");
      Serial.println(remoteIp);
      
      // 建立TCP连接
      WiFiClient client;
      if (client.connect(remoteIp, 80)) {
        Serial.println("Connected to device");
        
        // 发送设备名称:本设备ip地址
        // 拼接字符串
        String message = String(replyPacket) + ":" + WiFi.localIP().toString();
        client.println(message);
        
        // 等待响应
        while (client.connected()) {
          if (client.available()) {
            String line = client.readStringUntil('\n');
            Serial.println("Received: " + line);
            // 在这里处理接收到的数据
            break;
          }
        }
        
        // 关闭连接
        client.stop();
        Serial.println("Disconnected");
      } else {
        Serial.println("Connection failed");
      }
    }
    */
    if (strcmp(incomingPacket, expectedMessage0) == 0) {
      //开启风扇
      digitalWrite(Pin0, HIGH); // 拉高 GPIO0
      Serial.println("fan on");
    }
    if (strcmp(incomingPacket, expectedMessage1) == 0) {
      //关闭风扇
      digitalWrite(Pin0, LOW); // 拉低 GPIO0
      Serial.println("fan off"); 
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

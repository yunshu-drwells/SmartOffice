/*
使用socket通信来处理web请求
*/

#include "lwip/sockets.h"

#include "fatfs.h"  //SDFatFS、USERFatFS
#include "mymalloc.h"  //mymalloc
#include "main.h"  //fmout_sd fmout_norflash
#include "web.h"  //webstinfo
#include "norflash.h"  //norflash_read
#include "esp8266_web.h"  //ESP8266_sendBroadcastCmd
#include "cJSON.h"  // 用于 JSON 编码

int server_socket;
struct sockaddr_in server_addr;

//extern variables
//LightingMasterDLG.c
extern uint16_t MasterLight_R_slider;
extern uint16_t MasterLight_G_slider;
extern uint16_t MasterLight_B_slider;
extern uint8_t MasterLight_RGBchanged;
//SpotlightDLC.c
extern uint16_t SpolightLight_R_slider;
extern uint16_t SpolightLight_G_slider;
extern uint16_t SpolightLight_B_slider;
extern uint8_t SpolightLight_RGBchanged;
//freertos.c
extern uint16_t temperature;  //温度
extern uint16_t humidity;  //湿度
extern uint16_t adcx;  //亮度
//FanDLG.c
extern int Fan_status;  //风扇状态
extern int Fan_status_changed;  //风扇状态改变
//AlarmDLG.c
extern int Alarm_status;
extern int Alram_status_changed;
  
void WebServer(void);
void Listen(void);
void Listen_Thread(void);

/*
创建socket套接字
*/
void WebServer(){
    // 创建一个 TCP 套接字
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        // 处理错误
        while (1) {
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5); // 闪烁 LED 表示错误
            HAL_Delay(500);
        }
    }

    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(80); // HTTP 端口

    // 绑定套接字
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        // 处理错误
        while (1) {
            printf("socket error\n");
        }
    }
}

/*
获取Content-Type
*/
static char* get_Content_Type(char* type){
	//文本类型
	if(!strcmp(".txt", type)){
		return "text/plain";
	}else if(!strcmp(".html", type)){
		return "text/html";
	}else if(!strcmp(".css", type)){
		return "text/css";
	}else if(!strcmp(".js", type)){
		return "text/javascript";
	}
	/*
	else if(!strcmp(".xml", type)){
		return "text/xml";
	}
	*/
	//图像类型
	else if(!strcmp(".jpg", type)){
		return "image/jpeg";
	}
	else if(!strcmp(".png", type)){
		return "image/png";
	}
	else if(!strcmp(".gif", type)){
		return "image/gif";
	}
	else if(!strcmp(".svg", type)){
		return "image/svg+xml";
	}
	//音频类型
	else if(!strcmp(".mp3", type)){
		return "audio/mpeg";
	}
	else if(!strcmp(".wav", type)){
		return "audio/wav";
	}
	else if(!strcmp(".aac", type)){
		return "audio/aac";
	}
	//视频类型
	else if(!strcmp(".mp4", type)){
		return "video/mp4";
	}
	else if(!strcmp(".webm", type)){
		return "video/webm";
	}
	else if(!strcmp(".mov", type) || !strcmp(".qt", type)){
		return "video/quicktime";
	}
	//应用类型
	else if(!strcmp(".json", type)){
		return "application/json";
	}
	else if(!strcmp(".pdf", type)){
		return "application/pdf";
	}
	else if(!strcmp(".xml", type)){
		return "application/xml";  //application/xml 更通用。可以用于表示任何形式的 XML 数据，包括 XML 文档。因此，XML 文档也可以使用 application/xml 类型。
	}
	/*
	else if(!strcmp(".webm", type)){
		return "application/x-www-form-urlencoded";
	}
	*/
	else if(!strcmp(".bin", type) || !strcmp(".dat", type)){
		return "application/octet-stream";  //二进制数据流
	}
	//多部分类型
	/*
	else if(!strcmp(".", type)){
		return "multipart/form-data";  //表单数据
	}
	*/
	//其他类型
	/*
	else if(!strcmp(".woff", type)){
		return "font/woff";  //WOFF 字体
	}
	else if(!strcmp(".ttf", type)){
		return "font/ttf";  //TrueType 字体
	}
	else if(!strcmp(".woff", type)){
		return "font/x-font-woff";  //WOFF 字体（旧版）
	}
	else if(!strcmp(".ttf", type)){
		return "font/x-font-ttf";  //TrueType 字体（旧版）
	}
	*/
	return "";
}

#define BUFFER_SIZE 1024
//#define BUFFER_SIZE 4096  //前端静态资源的访问太慢了，增大缓冲区，提高分段传输的大小，加速传输
/*
发送文件的方法
*/
static void send_file(const char* file_path, int client_socket){
	uint8_t res = 0;			
			FIL *fftemp;
			fftemp = (FIL *)mymalloc(SRAMCCM, sizeof(FIL));  // 给文件描述符开辟空间
			res = f_open(fftemp, file_path, FA_READ);
			//printf("NORFlash f_open return :%d\n", res);
			if (res != FR_OK) {
				printf("Failed to open file:%s return:%d\n", file_path, res);
			}
			
			FILINFO* fileInfo = mymalloc(SRAMCCM, sizeof(FILINFO));     // 文件信息结构体
			// 获取文件状态信息
			res = f_stat(file_path, fileInfo);
			if (res != FR_OK) {
				printf("Failed to get file:%s status: %d\n", file_path, res);
			}
			
			// 打印文件大小
			uint32_t total_bytes_read = 0;
			if(NULL != fileInfo){
				total_bytes_read = fileInfo->fsize;  //文件总大小
				//printf("File size: %u bytes\n", (unsigned int)fileInfo->fsize);
			}			
			myfree(SRAMCCM, fileInfo);

			char* response_header = mymalloc(SRAMCCM, 256);
			
			//获取文件格式
			
			char* type = strstr(file_path, ".");
			char* Type = get_Content_Type(type);

			// 构建响应头
			snprintf(response_header, 256,
					 "HTTP/1.1 200 OK\r\n"
					 "Content-Type: %s\r\n"
					 "Content-Length: %d\r\n"
					 "Connection: keep-alive\r\n"
					 "Cache-Control: public, max-age=86400\r\n"			
					 "\r\n"
					 , Type, total_bytes_read);

			// 发送响应头
			send(client_socket, response_header, strlen(response_header), 0);

			char* buffer = mymalloc(SRAMCCM, BUFFER_SIZE);
			size_t bytes_read = 1;
            while (bytes_read) {
				f_read(fftemp, buffer, BUFFER_SIZE, &bytes_read);
				//printf("bytes_read:%d\n", bytes_read);
				//测试发送过程
				/*
				if (bytes_read > 0) {
					size_t bytes_sent = 0;
					while (bytes_sent < bytes_read) {
						//ssize_t sent = send(client_socket, buffer + bytes_sent, bytes_read - bytes_sent, 0);
						ssize_t sent = send(client_socket, buffer, bytes_read, 0);
						if (sent <= 0) {
							perror("send failed");
							break;
						}
						bytes_sent += sent;
						//printf("bytes_sent:%zu\n", bytes_sent);
					}
				} else {
					//printf("No more data to read.\n");
				}
				*/
                ssize_t sent = send(client_socket, buffer, bytes_read, 0);
				if (sent <= 0) {
					perror("send failed");
					break;
				} else {
					//printf("No more data to read.\n");
				}
            }
			f_close(fftemp);
			// 释放内存
			myfree(SRAMCCM, response_header);
			myfree(SRAMCCM, fftemp);
			myfree(SRAMCCM, buffer);
}

/*
响应POST请求，表示成功
返回一个 HTTP 状态码和响应数据
*/
static void Send_POST_OK(const char* str, int client_socket){
	char* response_header = mymalloc(SRAMCCM, 256);

	// 构建响应头
	snprintf(response_header, 256,
			"HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s",
			str
			 );

	// 发送响应头
	send(client_socket, response_header, strlen(response_header), 0);
	myfree(SRAMCCM, response_header);
}
/*
区分请求方法并响应
*/
const char* handle_request(const char* request, int client_socket)
{
    // 检查请求方法
    if (strncmp(request, "GET", 3) == 0) {
        // 处理 GET 请求
        if (strstr(request, "GET / HTTP/") != NULL) {  //GET / HTTP/1.1
			//之前使用fatfs读写挂载的磁盘失败是因为FATFS描述符被内存覆写，同样使用norflash_read失败的原因同样是文件信息结构体内存被覆写
			//在Touch_Task任务执行之后，外扩内存池占用率激增至SRAMEX   USED: 98.0% (约826KB),我的FreeRTOS采用了heap5的堆管理算法
			//而在定义xHeapRegions数组时只预分配了40KB的空间，因此猜测堆空间不够时，会使用pvPortMalloc来向外扩SRAM申请，但是覆写了一些其它的在外扩SRAM中申请的结构体或变量
			//内存覆写的原因是FreeRTOS采用heap5堆管理算法时，我并没有找到重定向pvPortMalloc的方法，这应该就是核心原因，最终解决办法是，将这些重要的结构体或变量分配在CCM空间
			//现在可以使用fatfs读写挂载的磁盘，或者直接使用norflash_read读写NORFlash
			
            // 根目录请求
            //const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Welcome to the Root Directory!</h1></body></html>";
            //send(client_socket, response, strlen(response), 0);
		
			//不能通过临界段保护index.html的发送，index.html发送过程太缓慢，在index.html发送期间也会收到GET请求bootstrap.css、jquery-1.11.0.min.js、bootstrap.min.js等其它请求
			//taskENTER_CRITICAL();           // 进入临界段
			// 读取文件内容
			//uint32_t total_bytes_read = webstinfo->index_html_size;  //文件总大小

			uint8_t res = 0;			
			FIL *fftemp;
			fftemp = (FIL *)mymalloc(SRAMCCM, sizeof(FIL));  // 给文件描述符开辟空间
			res = f_open(fftemp, "1:SmartOfficeWeb/index.html", FA_READ);
			//printf("NORFlash f_open return :%d\n", res);
			if (res != FR_OK) {
				printf("Failed to open file: %d\n", res);
				return NULL;
			}
			
			FILINFO* fileInfo = mymalloc(SRAMCCM, sizeof(FILINFO));     // 文件信息结构体
			// 获取文件状态信息
			res = f_stat("1:SmartOfficeWeb/index.html", fileInfo);
			if (res != FR_OK) {
				printf("Failed to get file status: %d\n", res);
				return NULL;
			}
			
			// 打印文件大小
			uint32_t total_bytes_read = 0;
			if(NULL != fileInfo){
				total_bytes_read = fileInfo->fsize;  //文件总大小
				printf("File size: %u bytes\n", (unsigned int)fileInfo->fsize);
			}			
			myfree(SRAMCCM, fileInfo);

			char* response_header = mymalloc(SRAMCCM, 256);

			// 构建响应头 设置静态资源超时时间24h
			snprintf(response_header, 256,
					 "HTTP/1.1 200 OK\r\n"
					 "Content-Type: text/html\r\n"
					 "Content-Length: %d\r\n"
					 "Connection: keep-alive\r\n"
					 "Cache-Control: public, max-age=86400\r\n"
					 "\r\n",
					 total_bytes_read);

			// 发送响应头
			send(client_socket, response_header, strlen(response_header), 0);

			// 发送响应正文
			//send(client_socket, response_body, total_bytes_read, 0);
			// 逐块读取文件并发送
			/*
            char* buffer = mymalloc(SRAMIN, 1024);;
            while (total_bytes_read > BUFFER_SIZE) {
				norflash_read((uint8_t *)buffer, webstinfo->index_html_addr, BUFFER_SIZE);
                send(client_socket, buffer, BUFFER_SIZE, 0);
				total_bytes_read -= BUFFER_SIZE;
            }
			norflash_read((uint8_t *)buffer, webstinfo->index_html_addr, total_bytes_read);
            send(client_socket, buffer, total_bytes_read, 0);
			*/
			char* buffer = mymalloc(SRAMCCM, BUFFER_SIZE);
			size_t bytes_read = 1;
            while (bytes_read) {
				f_read(fftemp, buffer, BUFFER_SIZE, &bytes_read);
				//printf("bytes_read:%d\n", bytes_read);
				//测试读取
				/*
				for(uint16_t i=0; i<BUFFER_SIZE; i++){
					printf("%c", buffer[i]);
				}
				printf("\n");
				*/
				//测试发送过程
				/*
				if (bytes_read > 0) {
					size_t bytes_sent = 0;
					while (bytes_sent < bytes_read) {
						//ssize_t sent = send(client_socket, buffer + bytes_sent, bytes_read - bytes_sent, 0);
						ssize_t sent = send(client_socket, buffer, bytes_read, 0);
						if (sent <= 0) {
							perror("send");
							break;
						}
						bytes_sent += sent;
						//printf("bytes_sent:%zu\n", bytes_sent);
					}
				} else {
					//printf("No more data to read.\n");
				}
				*/
				
                send(client_socket, buffer, bytes_read, 0);
            }
			f_close(fftemp);

			// 释放内存
			myfree(SRAMCCM, response_header);
			myfree(SRAMCCM, fftemp);
			myfree(SRAMCCM, buffer);
			//taskEXIT_CRITICAL();            // 出临界区 
			
            return NULL;
        } else if(strstr(request, "GET /css/bootstrap.css") != NULL){  //GET /css/bootstrap.css
			send_file("1:SmartOfficeWeb/css/bootstrap.css", client_socket);
		} else if(strstr(request, "GET /js/jquery-1.11.0.min.js") != NULL){  //GET /js/jquery-1.11.0.min.js
			send_file("1:SmartOfficeWeb/js/jquery-1.11.0.min.js", client_socket);
		} else if(strstr(request, "GET /js/bootstrap.min.js") != NULL){  //GET /js/bootstrap.min.js
			send_file("1:SmartOfficeWeb/js/bootstrap.min.js", client_socket);
		} else if(strstr(request, "GET /images/left-title.png") != NULL){  //GET /images/left-title.png
			send_file("1:SmartOfficeWeb/images/left-title.png", client_socket);
		} else if(strstr(request, "GET /images/01Temperature.jpg") != NULL){  //GET /images/01Temperature.jpg
			send_file("1:SmartOfficeWeb/images/01Temperature.jpg", client_socket);
		} else if(strstr(request, "GET /images/02Humidity.jpg") != NULL){  //GET /images/02Humidity.jpg
			send_file("1:SmartOfficeWeb/images/02Humidity.jpg", client_socket);
		} else if(strstr(request, "GET /images/03Brightnesss.jpg") != NULL){  //GET /images/03Brightnesss.jpg
			send_file("1:SmartOfficeWeb/images/03Brightnesss.jpg", client_socket);
		} else if(strstr(request, "GET /images/04LightingMasterOn.jpg") != NULL){  //GET /images/04LightingMasterOn.jpg
			send_file("1:SmartOfficeWeb/images/04LightingMasterOn.jpg", client_socket);
		} else if(strstr(request, "GET /images/05SpotlightOn.jpg") != NULL){  //GET /images/05SpotlightOn.jpg
			send_file("1:SmartOfficeWeb/images/05SpotlightOn.jpg", client_socket);
		} else if(strstr(request, "GET /images/06FanOn.jpg") != NULL){  //GET /images/06FanOn.jpg
			send_file("1:SmartOfficeWeb/images/06FanOn.jpg", client_socket);
		} else if(strstr(request, "GET /images/07AlarmOn.jpg") != NULL){  //GET /images/07AlarmOn.jpg
			send_file("1:SmartOfficeWeb/images/07AlarmOn.jpg", client_socket);
		} else if(strstr(request, "GET /images/08LoRaOn.jpg") != NULL){  //GET /images/08LoRaOn.jpg
			send_file("1:SmartOfficeWeb/images/08LoRaOn.jpg", client_socket);
		} else if(strstr(request, "GET /images/an-off.png") != NULL){  //GET /images/an-off.png
			send_file("1:SmartOfficeWeb/images/an-off.png", client_socket);
		} else if(strstr(request, "GET /images/an-on.png") != NULL){  //GET /images/an-on.png
			send_file("1:SmartOfficeWeb/images/an-on.png", client_socket);
		} else if(strstr(request, "GET /images/sgbj-off.png") != NULL){  //GET /images/sgbj-off.png
			send_file("1:SmartOfficeWeb/images/sgbj-off.png", client_socket);
		} else if(strstr(request, "GET /images/sgbj-on.gif") != NULL){  //GET /images/sgbj-on.gif
			send_file("1:SmartOfficeWeb/images/sgbj-on.gif", client_socket);
		} else if(strstr(request, "GET /images/sys-bg.jpg") != NULL){  //GET /images/sys-bg.jpg
			send_file("1:SmartOfficeWeb/images/sys-bg.jpg", client_socket);
		} else if(strstr(request, "GET /images/sys-bg-off.jpg") != NULL){  //GET /images/sys-bg-off.jpg
			send_file("1:SmartOfficeWeb/images/sys-bg-off.jpg", client_socket);
		} else if(strstr(request, "GET /images/null.png") != NULL){  //GET /images/null.png
			send_file("1:SmartOfficeWeb/images/null.png", client_socket);
		} else if(strstr(request, "GET /images/fan-on.png") != NULL){  //GET /images/fan-on.png
			send_file("1:SmartOfficeWeb/images/fan-on.png", client_socket);
		} else if(strstr(request, "GET /images/fan-off.png") != NULL){  //GET /images/fan-off.png 
			send_file("1:SmartOfficeWeb/images/fan-off.png", client_socket);
		} else if(strstr(request, "GET /images/bg-fan-on.png") != NULL){  //GET /images/bg-fan-on.png
			send_file("1:SmartOfficeWeb/images/bg-fan-on.png", client_socket);
		} else if(strstr(request, "GET /images/bg-fan-off.png") != NULL){  //GET /images/bg-fan-off.png 
			send_file("1:SmartOfficeWeb/images/bg-fan-off.png", client_socket);
		} else if(strstr(request, "GET /favicon.ico") != NULL){  //GET /favicon.ico 
			send_file("1:SmartOfficeWeb/favicon.ico", client_socket);
		} else if(strstr(request, "GET /music/alarm.mp3") != NULL){  //GET /music/alarm.mp3
			send_file("1:SmartOfficeWeb/music/alarm.mp3", client_socket);
		} else {
            // 其他 GET 请求
            //const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Other GET Request</h1></body></html>";
            //send(client_socket, response, strlen(response), 0);
            return NULL;
        }
    } else if (strncmp(request, "POST", 4) == 0) {
        // 处理 POST 请求
        //const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>POST Request Received</h1></body></html>";
        //send(client_socket, response, strlen(response), 0);
        if (strstr(request, "POST /CMD/MasterLight_On") != NULL) {  //POST /CMD/MasterLight_On HTTP/1.1
			//printf("MasterLight_On\n");
			char* cmd = mymalloc(SRAMCCM, 35); 
			MasterLight_R_slider = 255;
			MasterLight_G_slider = 255;
			MasterLight_B_slider = 255;
			MasterLight_RGBchanged = 1;
			sprintf(cmd, "MasterLight_ON&R=%d&G=%d&B=%d", MasterLight_R_slider, MasterLight_G_slider, MasterLight_B_slider);
			//printf("%s", cmd);
			ESP8266_sendBroadcastCmd(cmd);
			myfree(SRAMCCM, cmd);
			Send_POST_OK("MasterLight Opened!", client_socket);
		} else if(strstr(request, "POST /CMD/MasterLight_Off") != NULL){  //POST /CMD/MasterLight_Off HTTP/1.1
			//printf("MasterLight_Off\n");
			MasterLight_R_slider = 0;
			MasterLight_G_slider = 0;
			MasterLight_B_slider = 0;
			SpolightLight_RGBchanged = 1;
			ESP8266_sendBroadcastCmd("MasterLight_OFF");
			Send_POST_OK("MasterLight Closed!", client_socket);
		} else if(strstr(request, "POST /CMD/SpotLight_On") != NULL){  //PPOST /CMD/SpotLight_On HTTP/1.1
			//printf("SpotLight_On\n");
			char* cmd = mymalloc(SRAMCCM, 35); 
			SpolightLight_R_slider = 255;
			SpolightLight_G_slider = 255;
			SpolightLight_B_slider = 255;
			SpolightLight_RGBchanged = 1;
			sprintf(cmd, "SpotLight_ON&R=%d&G=%d&B=%d", SpolightLight_R_slider, SpolightLight_G_slider, SpolightLight_B_slider);
			//printf("%s", cmd);
			ESP8266_sendBroadcastCmd(cmd);
			myfree(SRAMCCM, cmd);
			Send_POST_OK("SpotLight Opened!", client_socket);
		} else if(strstr(request, "POST /CMD/SpotLight_Off") != NULL){  //POST /CMD/SpotLight_Off HTTP/1.1
			//printf("SpotLight_Off\n");
			SpolightLight_R_slider = 0;
			SpolightLight_G_slider = 0;
			SpolightLight_B_slider = 0;
			SpolightLight_RGBchanged = 1;
			ESP8266_sendBroadcastCmd("SpotLight_OFF");
			Send_POST_OK("SpotLight Closed!", client_socket);
		} else if(strstr(request, "POST /CMD/Fan_On") != NULL){  //POST /CMD/Fan_On HTTP/1.1
			//printf("Fan_On\n");
			Fan_status = 1;
			Fan_status_changed = 1;
			ESP8266_sendBroadcastCmd("FAN_ON");
			Send_POST_OK("Fan Opened!", client_socket);
		} else if(strstr(request, "POST /CMD/Fan_Off") != NULL){  //POST /CMD/Fan_Off HTTP/1.1
			//printf("Fan_Off\n");
			Fan_status = 0;
			Fan_status_changed = 1;
			ESP8266_sendBroadcastCmd("FAN_OFF");
			Send_POST_OK("Fan Closed!", client_socket);
		} else if(strstr(request, "POST /CMD/Alarm_On") != NULL){  //POST /CMD/Alarm_On HTTP/1.1
			//printf("Alarm_On\n");
			Alarm_status = 1;
			Alram_status_changed = 1;
			//alarm on
			HAL_GPIO_WritePin(Beep_GPIO_Port, Beep_Pin, GPIO_PIN_SET);
			Send_POST_OK("Alarm Opened!", client_socket);
		} else if(strstr(request, "POST /CMD/Alarm_Off") != NULL){  //POST /CMD/Alarm_Off HTTP/1.1
			//printf("Alarm_Off\n");
			Alarm_status = 0;
			Alram_status_changed = 1;
			//alarm off
			HAL_GPIO_WritePin(Beep_GPIO_Port, Beep_Pin, GPIO_PIN_RESET);
			Send_POST_OK("Alarm Closed!", client_socket);
		} else if(strstr(request, "POST /CMD/MagnetismLock_On") != NULL){  //POST /CMD/MagnetismLock_On HTTP/1.1
			//printf("MagnetismLock_On\n");		
		} else if(strstr(request, "POST /DATA/Sensor") != NULL){  //POST /DATA/Sensor
			//printf("POST /DATA/Sensor\n");
			// 构建 JSON 响应
			cJSON *root = cJSON_CreateObject();
			cJSON_AddNumberToObject(root, "temperature", temperature);
			cJSON_AddNumberToObject(root, "humidity", humidity);
			cJSON_AddNumberToObject(root, "light", adcx);

			char *response_json = cJSON_PrintUnformatted(root);
			cJSON_Delete(root);
			printf("response_json:%s\n", response_json);

			// 构建完整的 HTTP 响应
			char* response_header = mymalloc(SRAMCCM, 256);
			snprintf(response_header, sizeof(response_header),
					 "HTTP/1.1 200 OK\r\n"
					 "Content-Type: application/json\r\n"
					 "Connection: close\r\n"
					 "\r\n"
					 "%s", response_json);

			// 发送响应
			send(client_socket, response_header, strlen(response_header), 0);
			myfree(SRAMCCM, response_header);
			free(response_json);
		} else{  //其它POST请求
			return NULL;
		}
        return NULL;
    } else {
        // 其他请求
        //const char* response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Bad Request</h1></body></html>";
        //send(client_socket, response, strlen(response), 0);
        return NULL;
    }
	return NULL;
}

/*
测试函数
接收web请求
*/
void Listen(){
	int client_socket;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
	
    char buffer[1024];
	
	// 监听连接
    if (listen(server_socket, 1) < 0) {
        // 处理错误
        while (1) {
            printf("listen error\n");
        }
    }

	while(1){
		// 接受客户端连接
		client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
		if (client_socket < 0) {
			// 处理错误
			continue;
		}

		// 接收客户端请求
		int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
		if (bytes_received > 0) {
			buffer[bytes_received] = '\0';
			// 处理接收到的请求
			// 这里可以解析 HTTP 请求并生成响应
			printf("Received request: %s\n", buffer);
			
            // 处理接收到的请求(区分根目录请求，POST和GET请求)
            handle_request(buffer, client_socket);

            // 发送响应
            //send(client_socket, response, strlen(response), 0);
			
			// 发送响应
			//const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello, World!</h1></body></html>";
			//send(client_socket, response, strlen(response), 0);
		}

		// 关闭客户端连接
		close(client_socket);
	}
}

#include "FreeRTOS.h"
#include "task.h"

/*
处理单个客户端请求
*/
#define MAX_CLIENTS 10
#define CLIENT_BUFFER_SIZE 512

static void handle_client(void *pvParameters) {
    int client_socket = (int)pvParameters;
    //char buffer[CLIENT_BUFFER_SIZE];
	char* buffer = mymalloc(SRAMCCM, CLIENT_BUFFER_SIZE);
    int bytes_received;

    // 接收客户端请求
    bytes_received = recv(client_socket, buffer, CLIENT_BUFFER_SIZE, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        // 处理接收到的请求
        printf("Received request: %s\n", buffer);
		
		//fmout_norflash(1); //挂载norflash
		//挂载之后程序直接奔溃
		
		//printf("total_bytes_read:%u\n", webstinfo->index_html_size);

        // 处理接收到的请求
        const char* response = handle_request(buffer, client_socket);

        // 发送响应
        //send(client_socket, response, strlen(response), 0);
    }
	myfree(SRAMCCM, buffer);
    // 关闭客户端连接
    close(client_socket);
    vTaskDelete(NULL); // 删除任务
}

void Listen_Thread(){
	int client_socket;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
	
	// 监听连接
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        // 处理错误
        while (1) {
            printf("listen error\n");
        }
    }

    while (1) {
        // 接受客户端连接
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket < 0) {
            // 处理错误
            continue;
        }

        // 为每个客户端创建一个新任务
        xTaskCreate(
            handle_client,          // 任务处理函数
            "ClientHandler",        // 任务名称
            2048,                   // 任务堆栈大小
            (void *)client_socket,  // 任务参数
            1,                      // 任务优先级
            NULL                    // 任务句柄
        );
    }
}

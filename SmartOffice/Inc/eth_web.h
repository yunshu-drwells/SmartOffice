/*
使用socket通信来处理web请求
*/

#include "lwip/sockets.h"

#include "fatfs.h"  //SDFatFS、USERFatFS
#include "mymalloc.h"  //mymalloc
#include "main.h"  //fmout_sd fmout_norflash
#include "web.h"  //webstinfo
#include "norflash.h"  //norflash_read

int server_socket;
struct sockaddr_in server_addr;
  
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
区分请求方法并响应
*/
#define BUFFER_SIZE 1024
const char* handle_request(const char* request, int client_socket)
{
    // 检查请求方法
    if (strncmp(request, "GET", 3) == 0) {
        // 处理 GET 请求
        if (strstr(request, "GET / HTTP/1.1") != NULL) {
			//之前使用fatfs读写挂载的磁盘失败是因为FATFS描述符被内存覆写，同样使用norflash_read失败的原因同样是文件信息结构体内存被覆写
			//在Touch_Task任务执行之后，外扩内存池占用率激增至SRAMEX   USED: 98.0% (约826KB),我的FreeRTOS采用了heap5的堆管理算法
			//而在定义xHeapRegions数组时只预分配了40KB的空间，因此猜测堆空间不够时，会使用pvPortMalloc来向外扩SRAM申请，但是覆写了一些其它的在外扩SRAM中申请的结构体或变量
			//内存覆写的原因是FreeRTOS采用heap5堆管理算法时，我并没有找到重定向pvPortMalloc的方法，这应该就是核心原因，最终解决办法是，将这些重要的结构体或变量分配在CCM空间
			
            // 根目录请求
            //const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Welcome to the Root Directory!</h1></body></html>";
            //send(client_socket, response, strlen(response), 0);
			
			//fmout_norflash(1); //挂载norflash
			//挂载之后程序直接奔溃
			
			//taskENTER_CRITICAL();           // 进入临界段
			//uint8_t res = 0;
			// 检查文件状态
			/*
			FILINFO fno;
			res = f_stat("1:AlarmOn.bin", &fno);
			if (res != FR_OK) {
				printf("f_stat return for index.html: %d (", res);
				printf(")\n");
			} else {
				printf("File exists and is accessible\n");
			}
			*/			
			//FIL *fftemp;
			//fftemp = (FIL *)mymalloc(SRAMEX, sizeof(FIL));  // 给文件描述符开辟空间
			//res = f_open(fftemp, "1:SmartOfficeWeb/index.html", FA_READ);
			//printf("NORFlash f_open return :%d\n", res);
			//taskEXIT_CRITICAL();            // 出临界区 
			

			// 读取文件内容
			//int total_bytes_read = webstinfo->index_html_size;
			//norflash_read((uint8_t *)response, webstinfo->index_html_addr, webstinfo->index_html_size);
			
			taskENTER_CRITICAL();           // 进入临界段
			// 读取文件内容
			char* response_header = mymalloc(SRAMEX, 256);
			uint32_t total_bytes_read = webstinfo->index_html_size;
			//char* response_body = mymalloc(SRAMEX, total_bytes_read+1024);
			
			//norflash_read((uint8_t *)response_body, webstinfo->index_html_addr, total_bytes_read);
			
			printf("webstinfo:%p\n", webstinfo);
			printf("webstinfo->index_html_size:%u\n", (uint32_t)webstinfo->index_html_size);
			//for(uint16_t i = 0; i<1024; i++){
			//	printf("%c", response_body[i]);
			//}
			
			/*
			char* response_header = mymalloc(SRAMEX, 256);
			char* response_body = "<html><body><h1>Welcome to the Root Directory!</h1></body></html>";
			int total_bytes_read = 65;
			*/

			// 构建响应头
			snprintf(response_header, 256,
					 "HTTP/1.1 200 OK\r\n"
					 "Content-Type: text/html\r\n"
					 "Content-Length: %d\r\n"
					 "Connection: keep-alive\r\n"
					 "\r\n",
					 total_bytes_read);

			// 发送响应头
			send(client_socket, response_header, strlen(response_header), 0);

			// 发送响应正文
			//send(client_socket, response_body, total_bytes_read, 0);

			// 释放内存
			myfree(SRAMEX, response_header);
			//myfree(SRAMEX, response_body);
			taskEXIT_CRITICAL();            // 出临界区 
			
            return NULL;
        } else if (strstr(request, "GET /image.jpg HTTP/1.1") != NULL) {
            // 图片资源请求
            const char* file_path = "/path/to/image.jpg";
            FILE* file = fopen(file_path, "rb");
            if (file == NULL) {
                const char* response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";
                send(client_socket, response, strlen(response), 0);
                return NULL;
            }

            // 发送响应头
            const char* header = "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\n\r\n";
            send(client_socket, header, strlen(header), 0);

            // 逐块读取文件并发送
            char buffer[BUFFER_SIZE];
            size_t bytes_read;
            while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
                send(client_socket, buffer, bytes_read, 0);
            }

            fclose(file);
            return NULL;
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
        return NULL;
    } else {
        // 其他请求
        //const char* response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Bad Request</h1></body></html>";
        //send(client_socket, response, strlen(response), 0);
        return NULL;
    }
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
#define BUFFER_SIZE 1024

static void handle_client(void *pvParameters) {
    int client_socket = (int)pvParameters;
    char buffer[BUFFER_SIZE];
    int bytes_received;

    // 接收客户端请求
    bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
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
            4096,                   // 任务堆栈大小
            (void *)client_socket,  // 任务参数
            1,                      // 任务优先级
            NULL                    // 任务句柄
        );
    }
}

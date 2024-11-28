/*
使用socket通信来处理web请求
*/

#include "lwip/sockets.h"

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
            // 根目录请求
            const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Welcome to the Root Directory!</h1></body></html>";
            send(client_socket, response, strlen(response), 0);
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
            const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Other GET Request</h1></body></html>";
            send(client_socket, response, strlen(response), 0);
            return NULL;
        }
    } else if (strncmp(request, "POST", 4) == 0) {
        // 处理 POST 请求
        const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>POST Request Received</h1></body></html>";
        send(client_socket, response, strlen(response), 0);
        return NULL;
    } else {
        // 其他请求
        const char* response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Bad Request</h1></body></html>";
        send(client_socket, response, strlen(response), 0);
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
            1024,                   // 任务堆栈大小
            (void *)client_socket,  // 任务参数
            1,                      // 任务优先级
            NULL                    // 任务句柄
        );
    }
}

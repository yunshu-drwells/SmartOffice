#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "httpserver-socket.h"
#include "string.h"
#include "fatfs.h"
#include "protocol.h"

//extern uint8_t SensorData[3];
//extern ProtocolDataType ProtocolData;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//extern _SYNC_t fatfs_sem;

extern int SensorValue[3];

extern DSTATUS SD_initialize(BYTE);
const char Sensor[] = "Sensor";
const char Light[] = "Light";
const char Fan[] = "Fan";
const char Alarm[] = "Alarm";
const char On[] = "On";
const char Off[] = "Off";

u32_t nPageHits = 0;
unsigned char Request_Buf[1600];

static void Get_SensorValue(void *buffer, void *value);
static void Post_Cmd_Light(void *buffer, void *value);
static void Post_Cmd_Fan(void *buffer, void *value);
static void Post_Cmd_Alarm(void *buffer, void *value);

typedef struct
{
	char Method;				 //请求方法： GET 、HEAD 、POST
	char *URL;					 //URL
	char FileType;				 //文件类型
	char *Post_Data;			 //POST数据
	unsigned int Content_Length; //POST数据的长度
} Http_Request_MSG_Type;		 //定义HTTP请求报文消息结构体

#define HOMEPAGE_DEFAULT "index.html"

#define INVALID_CMD "HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\nConnection: close\r\nContent-Length: 16\r\n\r\ninvalid cmd!"
#define POST_REQUEST_OK "HTTP/1.0 200 OK\r\nServer: WFM-Control\r\nConnection: close\r\nContent-Type: text/css\r\nCache-control: no-cache\r\nExpires: Thu, 15 Apr 2000 20:00:00 GMT\r\nContent-Length: 18\r\n\r\nPOST Successfully!"
#define POST_REQUEST_FAIL "HTTP/1.0 200 OK\r\nServer: WFM-Control\r\nConnection: close\r\nContent-Type: text/css\r\nCache-control: no-cache\r\nExpires: Thu, 15 Apr 2000 20:00:00 GMT\r\nContent-Length: 13\r\n\r\nPOST Failure!"
#define RETURN_cmd_OK "HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\nCache-Control: no-cache, no-store, max-age=0\r\nExpires: 1L\r\nConnection: close\r\nContent-Length: "
/* HTML Doc. for ERROR */
const char ERROR_HTML_PAGE[] = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nContent-Length: 78\r\n\r\n<HTML>\r\n<BODY>\r\nSorry, the page you requested was not found.\r\n</BODY>\r\n</HTML>\r\n\0";
const char ERROR_REQUEST_PAGE[] = "HTTP/1.0 500 Pafe Not Found\r\nConnection: close\r\nContent-Type: text/html\r\nContent-Length: 50\r\n\r\n<HTML>\r\n<BODY>\r\nInvalid request.\r\n</BODY>\r\n</HTML>\r\n\0";
const char RETURN_CGI_PAGE[] = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nContent-Length: 59\r\n\r\n<HTML>\r\n<BODY>\r\nCGI cmd was executed.\r\n</BODY>\r\n</HTML>\r\n\0";
/* Response header for HTML*/
const char RES_HTMLHEAD_OK[] = "HTTP/1.0 200 OK\r\nServer: WFM-Control\r\nConnection: close\r\nContent-Type: text/html\r\nCache-control: max-age=315360000000\r\nExpires: Thu, 15 Apr 2100 20:00:00 GMT\r\nContent-Length: ";
/* Response head for TEXT */
const char RES_TEXTHEAD_OK[] = "HTTP/1.0 200 OK\r\nServer: WFM-Control\r\nConnection: close\r\nContent-Type: text/plain\r\nCache-control: max-age=315360000000\r\nExpires: Thu, 15 Apr 2100 20:00:00 GMT\r\nContent-Length: ";
/* Response head for GIF */
const char RES_GIFHEAD_OK[] = "HTTP/1.0 200 OK\r\nServer: WFM-Control\r\nConnection: close\r\nContent-Type: image/gif\r\nCache-control: max-age=315360000000\r\nExpires: Thu, 15 Apr 2100 20:00:00 GMT\r\nContent-Length: ";
/* Response head for JPEG */
const char RES_JPEGHEAD_OK[] = "HTTP/1.0 200 OK\r\nServer: WFM-Control\r\nConnection: close\r\nContent-Type: image/jpeg\r\nCache-control: max-age=315360000000\r\nExpires: Thu, 15 Apr 2100 20:00:00 GMT\r\nContent-Length: ";
/* Response head for MPEG */
const char RES_PNGHEAD_OK[] = "HTTP/1.0 200 OK\r\nServer: WFM-Control\r\nConnection: close\r\nContent-Type: image/png\r\nCache-control: max-age=315360000000\r\nExpires: Thu, 15 Apr 2100 20:00:00 GMT\r\nContent-Length: ";
const char RES_MP3HEAD_OK[] = "HTTP/1.0 200 OK\r\nServer: WFM-Control\r\nConnection: close\r\nContent-Type: audio/mpeg\r\nContent-Range: bytes 0-40123/40124\r\nCache-control: max-age=315360000000\r\nExpires: Thu, 15 Apr 2100 20:00:00 GMT\r\nContent-Length: ";
const char RES_JSHEAD_OK[] = "HTTP/1.0 200 OK\r\nServer: WFM-Control\r\nConnection: close\r\ncontent-type:application/x-javascript\r\nCache-control: max-age=315360000000\r\nExpires: Thu, 15 Apr 2100 20:00:00 GMT\r\nContent-Length: ";
const char RES_ICOHEAD_OK[] = "HTTP/1.0 200 OK\r\nServer: WFM-Control\r\nConnection: close\r\nContent-Type: image/x-icon\r\nCache-control: max-age=315360000000\r\nExpires: Thu, 15 Apr 2100 20:00:00 GMT\r\nContent-Length: ";
const char RES_CSSHEAD_OK[] = "HTTP/1.0 200 OK\r\nServer: WFM-Control\r\nConnection: close\r\nContent-Type: text/css\r\nCache-control: max-age=315360000000\r\nExpires: Thu, 15 Apr 2100 20:00:00 GMT\r\nContent-Length: ";
const char RES_APP_OK[] = "HTTP/1.0 200 OK\r\nServer: WFM-Control\r\nConnection: close\r\nContent-Type: application/octet-stream\r\nCache-control: max-age=315360000000\r\nExpires: Thu, 15 Apr 2100 20:00:00 GMT\r\nContent-Length: ";

WEB_Server_Struct SENSOR_WEB_DATA_GET = {NULL, Sensor, Get_SensorValue};
WEB_Server_Struct CONTROL_LIGHT_CMD_POST = {NULL, Light, Post_Cmd_Light};
WEB_Server_Struct CONTROL_FAN_CMD_POST = {NULL, Fan, Post_Cmd_Fan};
WEB_Server_Struct CONTROL_ALARM_CMD_POST = {NULL, Alarm, Post_Cmd_Alarm};

/****************************************************************************************************
 函数原型：void Login_authentication(void *buffer,void *value)
 入口参数：发送缓冲区指针，设定值指针
 出口参数：无
 函数功能：登录验证
 ****************************************************************************************************/
static void Get_SensorValue(void *buffer, void *value)
{

	sprintf(buffer, "{\"temperature\":\"%d\",\"humidity\":\"%d\",\"light\":\"10021.1\"}", SensorValue[0], SensorValue[1]);
}

/****************************************************************************************************
 函数原型：void Post_Cmd_Light(void *buffer,void *value)
 入口参数：发送缓冲区指针，设定值指针
 出口参数：无
 函数功能：登录验证
 ****************************************************************************************************/
static void Post_Cmd_Light(void *buffer, void *value)
{

	if (strstr(value, On))
	{
		sprintf(buffer, "{\"Status\":\"On\"}");
		HAL_GPIO_WritePin(GPIOF, D6_Pin | D7_Pin | D8_Pin | D9_Pin, GPIO_PIN_RESET);
	}
	else if (strstr(value, Off))
	{
		sprintf(buffer, "{\"Status\":\"Off\"}");
		HAL_GPIO_WritePin(GPIOF, D6_Pin | D7_Pin | D8_Pin | D9_Pin, GPIO_PIN_SET);
	}
	else
	{
		sprintf(buffer, "{\"Status\":\"Error\"}");
	}
}

/****************************************************************************************************
 函数原型：void Post_Cmd_Fan(void *buffer,void *value)
 入口参数：发送缓冲区指针，设定值指针
 出口参数：无
 函数功能：登录验证
 ****************************************************************************************************/
static void Post_Cmd_Fan(void *buffer, void *value)
{
	if (strstr(value, On))
	{
		sprintf(buffer, "{\"Status\":\"On\"}");
		FanControl(0x01);
	}
	else if (strstr(value, Off))
	{
		sprintf(buffer, "{\"Status\":\"Off\"}");
		FanControl(0x0);
	}
	else
	{
		sprintf(buffer, "{\"Status\":\"Error\"}");
	}
}

/****************************************************************************************************
 函数原型：void Post_Cmd_Alarm(void *buffer,void *value)
 入口参数：发送缓冲区指针，设定值指针
 出口参数：无
 函数功能：登录验证
 ****************************************************************************************************/
static void Post_Cmd_Alarm(void *buffer, void *value)
{
	if (strstr(value, On))
	{
		sprintf(buffer, "{\"Status\":\"On\"}");
		HAL_GPIO_WritePin(BUZ_GPIO_Port, BUZ_Pin, GPIO_PIN_SET);
	}
	else if (strstr(value, Off))
	{
		sprintf(buffer, "{\"Status\":\"Off\"}");
		HAL_GPIO_WritePin(BUZ_GPIO_Port, BUZ_Pin, GPIO_PIN_RESET);
	}
	else
	{
		sprintf(buffer, "{\"Status\":\"Error\"}");
	}
}

/*******************************************************************************
* 函数名称:  void WEB_Service_Registration(WEB_Server_Struct *next)
* 函数说明： WEB数据服务注册，与应用程序间的映射建立。
* 输入参数： 相应应用程序的链表类型指针
* 返回参数： 无
*******************************************************************************/
void WEB_Service_Registration(WEB_Server_Struct *next)
{
	WEB_Server_Struct *f = WEB_Registry_Head;
	next->next = NULL;
	while (f->next != NULL)
	{
		f = f->next;
	}
	f->next = next;
}

/*******************************************************************************
* 函数名称:  char Search_match_the_analytical_method(const char *cmd , char *body_Buf)
* 函数说明： 根据命令搜寻匹配解析方法
* 输入参数： WBE网页发来的命令
* 返回参数： 搜寻匹配成功返回0  ； 未找到匹配命令返回1
*******************************************************************************/
char Search_match_the_analytical_method(const char *cmd, char *body_Buf)
{
	WEB_Server_Struct *f = WEB_Registry_Head;
	char *p;

	printf("CMD:\r\n");
	printf("%s\r\n", cmd);
	for (f = WEB_Registry_Head; f != NULL; f = f->next)
	{
		p = strstr(cmd, f->cmd);
		if (p != NULL)
		{
			p = (char *)cmd;
			p = p + strlen(f->cmd) + 1;
			f->function(body_Buf, p); //此函数不可重入，所以停止任务调度
			return 1;
		}
	}
	return 0;
}
/*******************************************************************************
* 函数名称:  char POST_Search_match_the_analytical_method(char *cmd,char *dat)
* 函数说明： POST方式下获取的命令和数据，根据命令搜寻匹配解析方法
* 输入参数： WBE网页POST发来的命令和数据包
* 返回参数： 搜寻匹配成功返回0  ； 未找到匹配命令返回1
*******************************************************************************/
char POST_Search_match_the_analytical_method(char *cmd, char *body_buf, char *dat)
{
	WEB_Server_Struct *f = WEB_Registry_Head;

	printf("CMD:\r\n");
	printf("%s\r\n", dat);
	for (f = WEB_Registry_Head; f != NULL; f = f->next)
	{
		if (strstr(cmd, f->cmd))
		{
			f->function((char *)body_buf, dat); //此函数不可重入，所以停止任务调度
			return 1;
		}
	}
	return 0;
}
/*******************************************************************************
* 函数名称:  char Parse_URL_File_Type(const char *URL) 
* 函数说明： 解析URL路径中文件名的文件类型
* 输入参数： URL指针
* 返回参数： 文件类型
*******************************************************************************/
char Parse_URL_File_Type(const char *URL)
{
	char type;
	if (strstr(URL, ".htm") || strstr(URL, ".HTM"))
		type = PTYPE_HTML;
	else if (strstr(URL, ".gif") || strstr(URL, ".GIF"))
		type = PTYPE_GIF;
	else if (strstr(URL, ".txt") || strstr(URL, ".TXT"))
		type = PTYPE_TEXT;
	else if (strstr(URL, ".cfg") || strstr(URL, ".CFG"))
		type = PTYPE_TEXT;
	else if (strstr(URL, ".jpg") || strstr(URL, ".JPG"))
		type = PTYPE_JPEG;
	else if (strstr(URL, ".png") || strstr(URL, ".PNG"))
		type = PTYPE_PNG;
	else if (strstr(URL, ".css") || strstr(URL, ".CSS"))
		type = PTYPE_CSS;
	else if (strstr(URL, ".js") || strstr(URL, ".JS"))
		type = PTYPE_JS;
	else if (strstr(URL, ".ico") || strstr(URL, ".ICO"))
		type = PTYPE_ICO;
	else if (strstr(URL, ".mp3") || strstr(URL, ".MP3"))
		type = PTYPE_MP3;
	else
		type = PTYPE_APP;
	return type;
}
/*******************************************************************************
* 函数名称:  void Make_http_response_head(unsigned char * buf, char type, int len)
* 函数说明： 生成HTTP响应报文
* 输入参数： 响应报文指针* buf；回应类型type；响应报文长度 len
* 返回参数： 无
*******************************************************************************/
void Make_http_response_head(char *buf, char type, unsigned int len)
{
	const char *head;
	if (type == PTYPE_HTML)
		head = RES_HTMLHEAD_OK;
	else if (type == PTYPE_GIF)
		head = RES_GIFHEAD_OK;
	else if (type == PTYPE_TEXT)
		head = RES_TEXTHEAD_OK;
	else if (type == PTYPE_JPEG)
		head = RES_JPEGHEAD_OK;
	else if (type == PTYPE_PNG)
		head = RES_PNGHEAD_OK;
	else if (type == PTYPE_JS)
		head = RES_JSHEAD_OK;
	else if (type == PTYPE_CSS)
		head = RES_CSSHEAD_OK;
	else if (type == PTYPE_APP)
		head = RES_APP_OK;
	else if (type == PTYPE_ICO)
		head = RES_ICOHEAD_OK;
	else if (type == PTYPE_MP3)
		head = RES_MP3HEAD_OK;
#ifdef WEB_DEBUG
	else
		printf("\r\n\r\n-MAKE HEAD UNKNOWN-\r\n");
#endif
	sprintf(buf, "%s%u\r\n\r\n", head, len);
}
/*******************************************************************************
* 函数名称:  void Send_Web_Service_Data(SOCKET ch,char*body_buf) 
* 函数说明： 发送Web回应数据包 
* 输入参数： SOCKET 通道号 ,body_buf 要发送的数据包
* 返回参数： 无
*******************************************************************************/
void Send_Web_Service_Data(char ch, char *body_buf)
{
	char *buf;
	char *P_index;
	short length = 0;
	length = strlen((const char *)body_buf);
	buf = (char *)pvPortMalloc(length + sizeof(RETURN_cmd_OK) + 50); //开辟内存空间
	sprintf(buf, "%s%u\r\n\r\n%s", RETURN_cmd_OK, length, body_buf);
	length = strlen((const char *)buf);
	P_index = buf;
	while (length > 1500)
	{
		write(ch, (const unsigned char *)P_index, 1500);
		length -= 1500;
		P_index += 1500;
	}
	if (length > 0)
	{
		write(ch, (const unsigned char *)P_index, length);
	}
	vPortFree(buf); //释放内存空间
}

/*******************************************************************************
* 函数名称:  void Send_Response_File(SOCKET ch,Http_Request_MSG_Type *Http_Request_MSG) 
* 函数说明： 回应请求的文件
* 输入参数： SOCKET 通道号  ,Http_Request_MSG_Type  文件类型信息
* 返回参数： 无
*******************************************************************************/
void Send_Response_File(char ch, Http_Request_MSG_Type *Http_Request_MSG)
{
	FRESULT res;
	FIL *f;
	unsigned int bytes_ret;
	unsigned char *buf;
	unsigned char *buf1;
	uint32_t fSize;
	f = (FIL *)pvPortMalloc(sizeof(FIL));		//开辟内存空间
	buf = (unsigned char *)pvPortMalloc(1500);	//开辟内存空间
	buf1 = (unsigned char *)pvPortMalloc(1500); //开辟内存空间
	if (f == NULL || buf == NULL || buf1 == NULL)
	{
		printf("内存分配失败\r\n");
		vPortFree(f);			 //释放内存空间
		vPortFree(buf);			 //释放内存空间
		vPortFree(buf1);		 //释放内存空间
		//释放信号量
		write(ch, (const unsigned char *)ERROR_REQUEST_PAGE, sizeof(ERROR_REQUEST_PAGE));
		return;
	}
	res = f_open(f, Http_Request_MSG->URL, FA_OPEN_EXISTING | FA_READ);
	fSize = f_size(f);
	printf("http file size is %d\r\n", fSize);
	if (res == FR_OK)
	{
		Http_Request_MSG->FileType = Parse_URL_File_Type(Http_Request_MSG->URL); //分析请求URL中包含的文件的文件类型
		Make_http_response_head((char *)buf, Http_Request_MSG->FileType, fSize); //生成HTTP报文
		write(ch, (const unsigned char *)buf, strlen((const char *)buf));
		while (1)
		{
			res = f_read(f, buf, 1500, &bytes_ret);
			if (res != FR_OK)
			{
				printf("读取文件失败！文件名：%s，错误代码：0x%02x 文件大小：%d\r\n", (const char *)Http_Request_MSG->URL, res, bytes_ret);
				
				break;
			}
			if (bytes_ret == 0)
				break;
			write(ch, (const unsigned char *)buf, bytes_ret);
			res = f_read(f, buf1, 1500, &bytes_ret);
			if (res != FR_OK)
			{
				printf("读取文件失败！文件名：%s，错误代码：0x%02x 文件大小：%d\r\n", (const char *)Http_Request_MSG->URL, res, bytes_ret);
				
				break;
			}
			if (bytes_ret == 0)
				break;
			write(ch, (const unsigned char *)buf1, bytes_ret);
		}
		f_close(f);
		vPortFree(f);			 //释放内存空间
		vPortFree(buf);			 //释放内存空间
		vPortFree(buf1);		 //释放内存空间
		//释放信号量
	}
	else
	{
		f_close(f);
		vPortFree(f);			 //释放内存空间
		vPortFree(buf);			 //释放内存空间
		vPortFree(buf1);		 //释放内存空间
		//释放信号量
		write(ch, (const unsigned char *)ERROR_REQUEST_PAGE, sizeof(ERROR_REQUEST_PAGE));
		printf("打开文件失败！文件名：%s，错误代码：0x%02x\r\n", (const char *)Http_Request_MSG->URL, res);
		
	}
}

/*******************************************************************************
* 函数名称:  void Parse_Http_Request(char * Request_Msg ,Http_Request_MSG_Type *Http_Request_MSG)
* 函数说明： 解析HTTP请求
* 输入参数： 请求数据包指针：Request_Msg ；  请求信息类型 ：Http_Request_MSG
* 返回参数： 无
*******************************************************************************/
void Parse_Http_Request(char *Request_Msg, Http_Request_MSG_Type *Http_Request_MSG)
{
	char *thisstart = NULL;
	char *nextstart = NULL;

	thisstart = strtok_r(Request_Msg, " ", &nextstart);
	if (thisstart == NULL)
	{
		Http_Request_MSG->Method = METHOD_ERR;
		Http_Request_MSG->URL = NULL;
		return;
	}
	//解析请求所用的方法：GET,HEAD,POST
	if (!strcmp(thisstart, "GET") || !strcmp(thisstart, "get"))
	{
		Http_Request_MSG->Method = METHOD_GET;
	}
	else if (!strcmp(thisstart, "HEAD") || !strcmp(thisstart, "head"))
	{
		Http_Request_MSG->Method = METHOD_HEAD;
	}
	else if (!strcmp(thisstart, "POST") || !strcmp(thisstart, "post"))
	{
		Http_Request_MSG->Method = METHOD_POST;
	}
	else
	{
		Http_Request_MSG->Method = METHOD_ERR;
		Http_Request_MSG->URL = NULL;
		return;
	}

	if (nextstart == NULL)
	{
		Http_Request_MSG->Method = METHOD_ERR;
		Http_Request_MSG->URL = NULL;
		return;
	}

	Http_Request_MSG->URL = strtok_r(NULL, " ?", &nextstart);				 //解析URL
	if (Http_Request_MSG->URL[0] == '/' && Http_Request_MSG->URL[1] == '\0') //如果url仅是一个“/”，则默认为主页
	{
		Http_Request_MSG->URL = HOMEPAGE_DEFAULT; //设置默认页
	}
	Http_Request_MSG->Post_Data = nextstart; //保存下一字符串指针
}

/*******************************************************************************
* 函数名称:  void Respond_Http_Request(SOCKET ch ,char* Request_Msg)
* 函数说明： 响应HTTP请求
* 输入参数： socket 端口：ch ；HTTP请求数据包指针	：Request_Msg
* 返回参数： 无
*******************************************************************************/
void Respond_Http_Request(char ch, char *Request_Msg)
{
	Http_Request_MSG_Type Http_Request_MSG;
	char *thisstart = NULL;
	char *nextstart = NULL;
	char *buf;
	int length = 0;
	Parse_Http_Request(Request_Msg, &Http_Request_MSG); //解析HTTP请求类型
	switch (Http_Request_MSG.Method)
	{
	case METHOD_ERR:
		write(ch, (const unsigned char *)ERROR_REQUEST_PAGE, sizeof(ERROR_REQUEST_PAGE));
		break;
	case METHOD_HEAD:
	case METHOD_GET:
		if (strstr((const char *)Http_Request_MSG.URL, "/DATA/")) //判断是否是通讯指令，并解析指令帧
		{
			char *buf;
			buf = (char *)pvPortMalloc(128);													   //开辟内存空间--原先2048
			if (Search_match_the_analytical_method((const char *)(Http_Request_MSG.URL + 6), buf)) //匹配解析方法，匹配成功，执行相应动作
			{
				Send_Web_Service_Data(ch, buf); //回应请求的数据
			}
			else
			{
				write(ch, INVALID_CMD, sizeof(INVALID_CMD)); //无此指令，回应无效请求
			}
			vPortFree(buf); //释放内存空间
		}
		else
		{
			Send_Response_File(ch, &Http_Request_MSG); //发送请求的文件
		}
		break;
	case METHOD_POST:
		//获取POST内容
		thisstart = strstr(Http_Request_MSG.Post_Data, "Content-Length:");
		if (thisstart != NULL)
		{
			Http_Request_MSG.Content_Length = atoi(thisstart + 15); //获取POST内容的大小
		}
		thisstart = strstr(thisstart, "\r\n\r\n") + 4;
		Http_Request_MSG.Post_Data = thisstart;
		length = strlen(thisstart); //修改bug

		//解析POST内存
		if (strstr((const char *)Http_Request_MSG.URL, "/CMD/")) //判断是否是通讯指令，并解析指令帧
		{
			//接收完整的POST数据包
			nextstart = thisstart + length;
			while ((nextstart - thisstart) < Http_Request_MSG.Content_Length) //可能数据量很大，获取POST完整内容
			{																  //后期添加边获取边存储方式，应对大文件保存
				length = read(ch, (unsigned char *)nextstart, 1500);
				if (length > 0)
					nextstart += length;
				else if (length < 0)
					break;
			}
			*nextstart = '\0';
			Http_Request_MSG.Content_Length = nextstart - thisstart;

			//解析数据包
			buf = (char *)pvPortMalloc(128); //开辟内存空间--原先2048
			if (POST_Search_match_the_analytical_method(Http_Request_MSG.URL + 5, buf, Http_Request_MSG.Post_Data))
			{
				Send_Web_Service_Data(ch, buf);
			}
			else
			{
				write(ch, INVALID_CMD, sizeof(INVALID_CMD));
			}
			vPortFree(buf);
		}
		else
		{
			FRESULT res;
			FIL *f;
			unsigned int bytes_ret;
			int len;
			unsigned char *buf;
			f = (FIL *)pvPortMalloc(sizeof(FIL));	   //开辟内存空间
			buf = (unsigned char *)pvPortMalloc(6144); //开辟内存空间
			if (f == NULL || buf == NULL)
			{
				printf("内存分配失败\r\n");
				vPortFree(f);			 //释放内存空间
				vPortFree(buf);			 //释放内存空间
				//释放信号量
				write(ch, POST_REQUEST_FAIL, sizeof(POST_REQUEST_FAIL));
				return;
			}
			res = f_open(f, Http_Request_MSG.URL, FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
			if (res == FR_OK)
			{
				unsigned int index;						 //看看是不是应该改成int-2017年8月17日
				for (index = 0; index < length; index++) //拷贝数据到临时缓存区
				{
					buf[index] = thisstart[index];
				}
				while (1)
				{
					if (length == 0 || Http_Request_MSG.Content_Length > 1000)
						len = read(ch, buf + index, 1500); //修改BUG
					else
						len = 0; //修改BUG length为0表示已经接受超过1500
					if (len > 0)
					{
						index += len;
						length += len;
					}
#ifdef WEB_DEBUG
					printf("len=%d ; Content_Length=%d ; length=%d \r\n", len, Http_Request_MSG.Content_Length, length);
#endif
					if (index >= 3000 || len < 0 || length >= Http_Request_MSG.Content_Length)
					{
						res = f_write(f, buf, index, &bytes_ret);
						if (res != FR_OK)
						{
							printf("FATFS 写入文件失败！文件名：%s，错误代码：0x%02x\r\n", (const char *)Http_Request_MSG.URL, res);
							break;
						}
						if (index > bytes_ret)
						{
							printf("FLASH 存储空间已满！");
							break;
						}
						index = 0;
					}
					if (len < 0 || length >= Http_Request_MSG.Content_Length)
					{
						break;
					}
				}
				f_close(f);
				vPortFree(f);			 //释放内存空间
				vPortFree(buf);			 //释放内存空间
				//释放信号量
				write(ch, POST_REQUEST_OK, sizeof(POST_REQUEST_OK));
			}
			else
			{
				f_close(f);
				vPortFree(f);			 //释放内存空间
				vPortFree(buf);			 //释放内存空间
				//释放信号量
				printf("打开文件失败！文件名：%s，错误代码：0x%02x\r\n", (const char *)Http_Request_MSG.URL, res);
				write(ch, POST_REQUEST_FAIL, sizeof(POST_REQUEST_FAIL));
			}
		}
		break;
	default:
		break;
	}
	printf("\r\nURL: %s", Http_Request_MSG.URL);
#ifdef WEB_DEBUG
	printf("\r\nURL: %s", Http_Request_MSG.URL);
	switch (Http_Request_MSG.Method)
	{
	case METHOD_ERR:
		break;
	case METHOD_HEAD:
		printf("\r\nMethod: HEAD");
		break;
	case METHOD_GET:
		printf("\r\nMethod: GET");
		break;
	case METHOD_POST:
		printf("\r\nMethod: POST");
		break;
	default:
		break;
	}
	printf("\r\nContent_Length: %u", Http_Request_MSG.Content_Length);
	printf("\r\nPOST_DATA:\r\n%s\r\n", Http_Request_MSG.Post_Data);
#endif
}
/**
  * @brief serve tcp connection  
  * @param conn: connection socket 
  * @retval None
  */
void http_server_serve(int conn)
{
	int ret;
	/* Read in the request */
	ret = read(conn, (unsigned char *)Request_Buf, 1500);

	if (ret < 0)
		return;
	else
	{
		*(Request_Buf + ret) = 0;
#ifdef WEB_DEBUG
		printf("\r\nWEB服务器，接收请求，内容:\r\n%s\r\n", (const char *)Request_Buf);
#endif
		Respond_Http_Request(conn, (char *)Request_Buf);
	}

	close(conn);
}
/**
  * @brief  http server thread 
  * @param arg: pointer on argument(not used here) 
  * @retval None
  */
void http_server_socket_thread(void *p_arg)
{
	int sock, newconn, size;
	struct sockaddr_in address, remotehost;

	/* create a TCP socket */
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("http_server can not create socket");
		return;
	}

	/* bind to port 80 at any interface */
	address.sin_family = AF_INET;
	address.sin_port = htons(80);
	address.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock, (struct sockaddr *)&address, sizeof(address)) == -1)
	{
		printf("http_server can not bind socket");
		return;
	}

	/* listen for incoming connections (TCP listen backlog = 5) */
	listen(sock, 5);

	size = sizeof(remotehost);

	printf("\r\n--------------Web_Server_Task----------------\r\n");
	WEB_Service_Registration(&CONTROL_LIGHT_CMD_POST);
	WEB_Service_Registration(&CONTROL_FAN_CMD_POST);
	WEB_Service_Registration(&CONTROL_ALARM_CMD_POST);

	while (1)
	{
		newconn = accept(sock, (struct sockaddr *)&remotehost, (socklen_t *)&size);
		http_server_serve(newconn);
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

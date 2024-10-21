
# 软件和库版本

- cubemx: **5.3.0**

- - **STM32Cube_FW_F4_V1.26.0**

- keil 5: **5.23.0.0**

- CodeBlocks 20.03

# 硬件

- MCU: STM32F407ZGT6
- LCD: SD1963+GT911
- ETH: YT8512C
- SRAM: IS62WV51216BLL-55TLT
- norflash: W25Q128
- EEPROM: 24c02
- 温湿度传感器：DHT11

通过FSMC模拟8080控制LCD的显示和驱动外扩SRAM。使用内存池和块表项组织管理EXSRAM以及INSRAM(128KB)，将FreeRTOS和LwIP以及STemWin的堆映射到外扩SRAM，以缓解内部SRAM空间紧张的问题。实现方式分别是：FreeRTOS采用heap_5+动态管理的方式，将heap5的堆区地址映射到外扩SRAM的前40KB；LwIP在初始化的时候就声明堆大小MEM_SIZE为16KB，通过在lwipopts.h声明LWIP_RAM_HEAP_POINTER指向紧跟着FreeRTOS后面的地址，就完了紧挨着FreeRTOS后面的16KB空间的映射。STemWin只需要在GUIConf.c修改宏定义USE_EXRAM为1，同时在GUI_X_Config函数中将分配内存指向自定义实现的内存池分配函数，则StemWin的堆就会在外扩SRAM中分配。FreeRTOS中实现STemWin的GUI任务栈总共只分配了512Words = 2KB就够用，可见使用自定义内存池管理是很有效果的。

项目中有挑战性且重要的地方是根据IS62WV51216 SRAM的读写时序图来计算ADDSET和DATAST，如果两个值太小则会影响数据在数据线上的保持时间，会导致数据出错，如果值太大，读写性能会受影响，同时也会出错。然后就是ETH的YT8512C芯片需要通过HAL_ETH_ReadPHYRegister函数来计算phyreg的值，否则PHY无法正常驱动。还有LCD显示IC SD1963需要根据数据手册的时序图，确定是否要开启扩展模式，读时序周期比写周期明显长，如果不分开控制读写容易造成显示异常，这也是很重要的点。

然后就是STemWin开发页面过程中收获最大的点，就是理解了重要的事件有哪些，以及如何处理。比如在环境亮度子页面中如何去实时的根据外部变量去更新组件值或重绘组件，比较可靠且好用的办法就是，在页面创建之后，就开辟一个任务去间隔的去处理，在子页面退出的时候记得销毁任务。

最后一点就是实现息屏和唤醒功能。重点就是如何让屏幕背光之后，不允许触发按键或者其它组件的事件，同时又可以处理触摸事件来让背光再次开启。我的处理思路就是在背光关闭之后，让所有组件处理响应的时候判断背光是否关闭，关闭的话不允许执行自己应该响应的事件，则就需要开启背光，并跳过自己的事件。然后让父容器window去处理WM_TOUCH事件，只要背光是关闭的，触摸事件便能唤醒背光，页面中的所有组件便可以正常响应正常逻辑。之所以这样做，是因为我发现BUTTON_SetFocusable或者WM_WindowDisable之类的方法并不好用。

项目效果展示可以参考视频：<a href="https://www.bilibili.com/video/BV1RdyiYjEVu/">bilibili</a>

===========================================================================================
LCD实验(例程)
===========================================================================================
实验目的：1. 理解液晶显示器的工作原理。
          2. 掌握STM32H7 FMC库函数的使用。
          3. 完成LCD程序编写。
===========================================================================================
实验准备：
一个eeLab板卡，一个MiniUSB线
一个DC 12V电源
===========================================================================================
实验步骤：
1）将MiniUSB线的USB端连接电脑，另一端连接节点的调试接口。
2）在Keil软件菜单栏选择Flash->Download将程序下载到eeLab板卡，观察实验现象。
3）在Keil软件菜单栏选择Debug->Start/Stop Debug Session可进入调试状态。
4）退出调试状态：再次选择菜单栏中的Debug->Start/Stop Debug Session停止程序运行。
5）此时程序已经固化到节点板的Flash中，重启或重新上电，程序将直接运行。

注意：
1）进入Debug状态若出错，请检查电脑的设备管理器是否正确识别仿真器设备，如果未成功识别则安装ST-Link驱动后重试。
2）如果以上方法均无效请更换MiniUSB线再试。
===========================================================================================
实验现象：
程序运行后屏幕上会显示实验名称、实验描述、实验现象等描述
3秒后LCD屏上轮询显示"Hello IOT!  物联网开放平台！"和"STM32H743ZIT6"
间隔时间为两秒.
D3、D4以0.5hz的频率闪烁
===========================================================================================


直流电机测速实验（例程）
===========================================================================================
实验目的：1. 了解直流电机测速的原理结构。
          2. 学习定时器的溢出中断代码。
===========================================================================================
实验准备：
一个eeLab板卡，一个MiniUSB线
一个DC 12V电源
===========================================================================================
实验步骤：
1）将MiniUSB线的USB端连接电脑，另一端连接节点的调试接口。
2）用MiniUSB线连接PC与节点，打开串口助手设置波特率为115200，数据位8，校验0,停止位1,给节点通上电源，点击打开串口。
3）在Keil软件菜单栏选择Flash->Download将程序下载到eeLab板卡，观察实验现象。
4）在Keil软件菜单栏选择Debug->Start/Stop Debug Session可进入调试状态。
5）退出调试状态：再次选择菜单栏中的Debug->Start/Stop Debug Session停止程序运行。
6）此时程序已经固化到节点板的Flash中，重启或重新上电，程序将直接运行。

注意：
1）进入Debug状态若出错，请检查电脑的设备管理器是否正确识别仿真器设备，如果未成功识别则安装ST-Link驱动后重试。
2）如果以上方法均无效请更换MiniUSB线再试。
===========================================================================================
实验现象：
电机的测速是实时测量的，通过触发按键控制直流电机的正反转，完成直流电机的测速。每隔一段时间
LCD和串口都会更新实时速度。
===========================================================================================


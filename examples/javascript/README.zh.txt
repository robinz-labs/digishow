在 DigiShow 中使用 JavaScript 表达式和脚本
=======================================

expression.dgs
在输入/输出端上绑定JS表达式来动态改变它们的信号数值。如此例中，通过表达式将普通按键改造成不同类型的模式切换按钮，这在交互装置中会被经常用于触发逻辑。

script.dgs
该工程附带脚本文件 script.scriptable.txt, 其中包含了自定义函数 onValueChanged() 的代码，工程中就可以通过在输入/输出端绑定表达式来调用这个函数。如此例中，移动 Input 推杆到不同位置，即可改变 RGB 灯的预设颜色。

time.dgs
使用 JavaScipt 中的日期时间函数来为工程中的信号赋值，在DigiShow中实现时钟和定时触发功能。

shell_command
编写 JavaScipt 代码，在 DigiShow 中调用操作系统的命令行工具。如此例中，演示了控制 Windows 安全关机的方法。

http_request
编写 JavaScipt 代码，发送特定的 HTTP 请求。如此例，用于遥控外部媒体播放软件 Kodi。

code_sender
编写 JavaScipt 代码，通过串口、TCP 或 UDP 发送 G-code 指令给目标设备。

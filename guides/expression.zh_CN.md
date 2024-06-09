# DigiShow JavaScript 表达式参考
 
JS 表达式即一个能动态改变信号数值的公式，在这个用 JavaScript 语法写成的表达式中可以使用如下变量和函数。 
 
 
## 表达式中的常用变量
 
**value** 
此信号未经表达式计算前的原始数值 
 
**range** 
此信号在取值范围中的最大数值 
 
**lastValue** 
此信号中寄存的经表达式计算后的最后数值 
 
 
## 表达式中的常用函数
 
**inputValueOf(name)** 
获取标题为 name 的信号链接条的输入端数值 
 
**outputValueOf(name)** 
获取标题为 name 的信号链接条的输出端数值 
 
**alert(message)** 
显示文字内容为 message 的信息对话框，在按“确定”按钮后关闭 
 
**toast(message)** 
显示文字内容为 message 的通知提示框，停留数秒后会自动消失 
 
表达式中也可以包含多个语句，语句与语句之间需要用 ; 分号来进行分割，最末一句用于返回由表达式计算获得的数值。如：
alert("value = " + value); value 
 
另外，还有更多函数可以在表达式和 DigiShow 项目的附加脚本文件中被调用。 
 
 
## 更多函数
 
**setOutputValueOf(name, value)** 
改变标题为 name 的信号链接条的输出端数值为 value。调用成功返回 true，失败返回 false 
 
**appVersion()** 
返回 DigiShow 应用程序版本号 
 
**appFilePath(filename)** 
返回存在于 DigiShow 项目文件所在目录中的名为 filename 的文件的绝对路径 
 
**appDataPath(filename)** 
返回存在于 DigiShow 配置数据目录中的名为 filename 的文件的绝对路径 
 
( 更多请参见 digishow_environment.h 头文件中带有 Q_INVOKABLE 修饰的函数，它们都可以直接在 JavaScript 脚本中被调用 ) 
 
 
## app 对象的成员函数
 
**app.restart()** 
重启当前 DigiShow 项目的运行 
 
**app.startLaunch(name)** 
启动指定的 preset (launch) 项，参数 name 通常可以是 'launch1', 'launch2', 'launch3' ... 
 
( 更多 app 对象的用法请参见 digishow_app.h 头文件中带有 Q_INVOKABLE 修饰的函数，它们都可以直接在 JavaScript 脚本中被调用 ) 
 

## utilities 对象的成员函数
 
**utilities.runInShell(program, arguments)** 
在操作系统 shell 中运行程序 program, 并在数组 arguments 中指定运行程序的各个参数 
如：utilities.runInShell('shutdown', ['-f', '-s', '-t', '0']) 
即相当于在命令行中运行 shutdown -f -s -t 0 
 
**utilities.httpRequest(url)**
以 GET 方法向目标 url 发起 http 请求，并获得由服务器返回的内容 
 
**utilities.httpRequest(url, method, contents, timeout)** 
以 GET / POST / PUT 方法向目标 url 发起 http 请求，并获得由服务器返回的内容 
参数 method 可以是 'get'、'post' 或 'put' 
参数 contents 是被 post 或 put 到 http 服务器的数据内容 
参数 timeout 以毫秒为单位，当在等待 http 请求回复超时后，函数将返回一个空字符串 
 
**utilities.delay(timeout)** 
程序停顿特定时长后继续执行，参数 timeout 以毫秒数为单位。在停顿期间 DigiShow 主程序始终保持活动状态，信号正常收发 
 
( 更多 utilities 对象的用法请参见 app_utilities.h 头文件中带有 Q_INVOKABLE 修饰的函数，它们都可以直接在 JavaScript 脚本中被调用 ) 
 
 
## metronome 对象的属性
 
可以通过 metronome 对象来实现对节拍生成器的各种控制。
 
**metronome.isRunning** 
节拍生成器的运行状态，取值可以是 true 或 false，该属性可读可写 
 
**metronome.isSoundEnabled** 
节拍生成器是否在打拍子时发出声音，取值可以是 true 或 false，该属性可读可写 
 
**metronome.isLinkEnabled** 
节拍生成器是否与其他支持 Ableton Link 接口的软件保持节拍同步，取值可以是 true 或 false，该属性可读可写 
 
**metronome.bpm**
节拍生成器的 tempo 状态，bpm 数值用于标记每分钟的节拍数，该属性可读可写 
 
**metronome.quantum**
节拍生成器每小节的拍数，默认为 4，该属性可读可写
 
**metronome.beat**
运行中的节拍生成器当前的总拍子计数，该属性只读
 
**metronome.phase**
运行中的节拍生成器在当前小节中的拍子数，如：1、2、3、4，该属性只读
 
( 更多 metronome 对象的用法请参见 digishow_metronome.h 头文件中带有 Q_INVOKABLE 修饰的函数，它们都可以直接在 JavaScript 脚本中被调用 ) 



远程虚拟管道
==========

我们提供一些示例用来演示如何运用远程虚拟管道打通多个 DigiShow 应用实例之间的信号数据同步，这些 DigiShow 应用实例还可以运行在网络中的不同的电脑上。

运用 Web Socket 通信，也可以将自己开发的 HTML5 或 Python 等程序接入到 DigiShow 所提供的远程虚拟管道中，同样可以实现信号数据同步。

1. 在 DigiShow 中打开工程文件 remote_pipe_demo.dgs，并启动。

2. 在 DigiShow 菜单中选择“新建实例”，在新 DigiShow 实例中打开 client 目录中的工程文件 remote_pipe_client.dgs，启动后即可实现两个 DigiShow 应用实例之间的虚拟管道互通和信号数据同步。

3. 在 client/html5 目录中有一个最简单的 html 网页示例，在 client/python 目录中有一个最简单的 python 脚本示例，它们用于接入之前启动的 remote_pipe_demo 工程，可以对 DigiShow 虚拟管道中的信号数据进行读写。

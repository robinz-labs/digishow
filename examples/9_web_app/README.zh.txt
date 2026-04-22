为 DigiShow 创建 Web app
=======================

当你需要编写 web app 来与 DigiShow 协同工作时，请首先阅读 guide 目录中的 README.md，还能参考 pipe_demo 和 smile_finder 目录中的示例。


AI Coding
=========

如果你希望通过 AI vibe coding 来生成 web app，同样可以让 AI 智能体来阅读 guide 目录中的 README.txt 。

方法：
1. 将 guide/README.md 文件发送给你的 AI agent

2. 在提示词中输入类似文字：“创建 web app：用电脑摄像头识别微笑，并把标识是否微笑的状态变量数值传输给 DigiShow 虚拟管道中的 开关量 通道1。”，并生成网页。

3. 在 DigiShow 中的接口管理器中将虚拟管道的 “远程链接服务” 选择为 “启动(多连接)”，并在主界面中的信号链接中加入一个输入信号，设置为：虚拟管道 → 开关量 → 通道1。

4. 启动 DigiShow 工程，然后打开 Web app 网页。
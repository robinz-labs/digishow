使用 AI 控制 DigiShow 中的灯光
===========================

本项目演示了如何利用 AI 智能体或 vibe coding 工具生成 Python 脚本，通过 OSC 协议控制 DigiShow 中的灯光。

项目文件：
1. light_osc_server.dgs - DigiShow 项目文件，包含 OSC 服务器配置及灯光控制设置
2. AI_prompt.txt - 用于 AI 代码生成的提示词
3. light_animation.py - AI 生成的灯光动画 Python 脚本（你也可以创建自己的版本）

演示步骤：
1. 在 DigiShow 中打开并启动 light_osc_server.dgs
2. 将 AI_prompt.txt 中的提示词输入 AI 工具，将生成的 Python 脚本保存为 light_animation.py
3. 通过命令行运行生成的 Python 脚本：

python light_animation.py


注意：要在 Python 脚本中使用 OSC 功能，通常需要安装额外的包（如 python-osc）：

pip install python-osc

详细说明请参考 AI 对话内容
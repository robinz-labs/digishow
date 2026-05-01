# loopMIDI 在 Windows 11 上的问题及解决方法

## 问题描述

Windows 11 的系统更新 **KB5077241** 引入了新的 **MIDI 2.0 服务**，导致与 loopMIDI 的兼容性被破坏。旧的虚拟驱动程序无法被新系统识别，因此你的 DAW 看不到 loopMIDI 端口。

## 快速解决方法

**1. 重启 MIDI 服务（临时解决）**
以管理员身份运行：
```
net stop midisrv && net start midisrv
```
该方法有效直到下次重启电脑。

**2. 使用 Windows 原生回环端口（永久解决）**
- 安装 Windows MIDI Services SDK：`winget install Microsoft.WindowsMIDIServicesSDK`
- 下载 **Windows MIDI Port Creator**（从 Steinberg 论坛获取）
- 导入你现有的 loopMIDI 端口
- 启用开机自动重建端口功能

## 总结

Windows 原生 MIDI 1.0 回环端口可以完全替代 loopMIDI。使用 Port Creator 工具，只需 2 分钟即可迁移你现有的端口。
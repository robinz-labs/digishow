DigiShow 与 Ableton Live 一起使用
===============================

demo1_dancing_ink
跳舞的墨水(磁流体)装置，演示在电子音乐中加入更多表达维度

1. 在 Ableton 中打开工程 dancing_ink.als。轨道 Drums 和 Zheng 用于产生音乐，而轨道 Electromagnets 上的 MIDI CC 信号曲线 (CC21 - CC24) 用于控制电磁铁。 这些 MIDI 信号会输出到电脑中的 IAC/loopMIDI 总线上。 

2. 在 DigiShow 中打开工程 dancing_ink.dgs。其中的4对信号链接，将从 IAC/loopMIDI 总线接收到由 Ableton 发送而来的 MIDI CC 信号，并把它们映射到 Arduino 输出管脚上，以带动4路电磁铁。即可实现在 Ableton 中播放音乐的同时驱动电磁铁来操控磁流体的运动。

（注意：在此示例中 Ableton 中的 MIDI 轨道输出端口与 DigiShow 中的 MIDI 输入端口必须保持一致）

--

demo2_jam_with_things
"以物即兴"，演示创造自己的实验乐器

1. 在 DigiShow 中打开工程 jam_with_things.dgs。将连接在 Arduino 上的传感器输入信号映射为 MIDI 音符信号，这些MIDI信号会传输到电脑中的 IAC/loopMIDI 总线上。 

2. 在 Ableton 中打开工程 jam_with_things.als。其中包含一系列的 MIDI 轨道，它们都各自加载了不同的乐器音色。当从 IAC/loopMIDI 总线接收到由 DigiShow 发送而来的音符信号时，Ableton 便会作为一台合成器来演奏你的音乐。

（在此示例中 DigiShow 中的 MIDI 输出端口与 Ableton 中的 MIDI 轨道输入端口必须保持一致）

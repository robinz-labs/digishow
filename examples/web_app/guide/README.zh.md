# DigiShow Web App 虚拟管道使用指南

通过 WebSocket 虚拟管道，实现 Web 网页与 DigiShow 工程之间的双向信号通信。

---

## 1. DigiShow 软件端配置

1. 打开 DigiShow 软件，进入 **Interface Manager** 的 **Pipe** 配置页。
2. 将 **Remote Link Service** 设置为：
   - **Enabled**：允许单个 Web 应用接入。
   - **Enabled (multiple)**：允许多个 Web 应用同时接入。

---

## 2. Web 网页端编程

在 HTML5 页面中，通过 WebSocket 与 DigiShow 虚拟管道进行通信。

### WebSocket 连接地址

```
ws://127.0.0.1:50000
```

> 适用于 Web 应用与 DigiShow 运行在同一台电脑的场景。

---

## 3. 消息格式

采用文本格式的 `dgss` 消息，格式如下：

```
dgss,<channel>,<signal_type>,<a_value>,<a_range>,<b_value>
```

**示例：**

```
dgss,1,65,1024,65535,0
```

### 参数说明

| 字段 | 说明 |
|------|------|
| `dgss` | 固定消息头，表示 DigiShow Signal |
| `<channel>` | 通道号 |
| `<signal_type>` | 信号类型：`65`（A模拟量）、`66`（B开关量）、`78`（N音符） |
| `<a_value>` | 信号数值（根据信号类型含义不同） |
| `<a_range>` | 取值范围（通常默认 65535） |
| `<b_value>` | 辅助值（根据信号类型含义不同） |

---

## 4. 不同信号类型的用法

### 模拟量信号（signal_type = 65）

- `<a_value>`：信号数值
- `<a_range>`：取值范围（通常为 65535）
- `<b_value>`：保持为 `0`

### 开关量信号（signal_type = 66）

- `<a_value>`：保持为 `0`
- `<a_range>`：保持为 `0`
- `<b_value>`：`1`（开）或 `0`（关）

### 音符信号（signal_type = 78）

- `<a_value>`：音符强度（0–127）
- `<a_range>`：取值范围（固定为 `127`）
- `<b_value>`：`1`（音符开）或 `0`（音符关）

---

## 5. 代码示例

```javascript
// 连接 DigiShow
var ws = new WebSocket("ws://127.0.0.1:50000");

// 设置通道1模拟量信号为 50%
ws.send('dgss,1,65,32767,65535,0');

// 设置通道2开关量信号为 ON
ws.send('dgss,2,66,0,0,1');

// 设置通道3音符信号为 ON，强度 50%
ws.send('dgss,3,78,63,127,1');

// 设置通道3音符信号为 OFF
ws.send('dgss,3,78,0,127,0');

// 接收来自 DigiShow 的消息
ws.onmessage = function (evt) {
    var msgIn = evt.data;
    var fields = msgIn.split(',');

    if (fields[0] === 'dgss' && fields.length === 6) {
        var channel = fields[1];
        var signalType = fields[2];
        var aValue = fields[3];
        var aRange = fields[4];
        var bValue = fields[5];

        // 根据业务逻辑处理接收到的信号
        console.log(`通道 ${channel} 信号更新`);
    }
};
```

---

## 6. 双向通信说明

`dgss` 消息支持 **双向收发**：
- Web 端发送：控制 DigiShow 中的信号值。
- Web 端接收：监听 DigiShow 中信号值的变化。

通过这种方式，Web 应用可以与 DigiShow 工程实现实时的信号联动。

---

## 7. 注意事项

- 确保 DigiShow 软件的 Remote Link Service 已正确开启。
- WebSocket 连接地址端口默认为 `50000`，请确认未被占用。
- 消息字段数量必须为 6 个，且以 `dgss` 开头，否则将被忽略。
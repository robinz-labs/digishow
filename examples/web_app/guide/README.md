# DigiShow Web App Virtual Pipe Guide

Bidirectional signal communication between a web page and a DigiShow project via WebSocket virtual pipe.

---

## 1. DigiShow Software Configuration

1. Open DigiShow software and navigate to **Interface Manager** > **Pipe** configuration page.
2. Set **Remote Link Service** to:
   - **Enabled**: Allows a single web application to connect.
   - **Enabled (multiple)**: Allows multiple web applications to connect simultaneously.

---

## 2. Web Page Programming

Communicate with the DigiShow virtual pipe via WebSocket in an HTML5 page.

### WebSocket Connection URL

```
ws://127.0.0.1:50000
```

> Use this when the web application and DigiShow are running on the same computer.

---

## 3. Message Format

Messages are text-based `dgss` messages formatted as follows:

```
dgss,<channel>,<signal_type>,<a_value>,<a_range>,<b_value>
```

**Example:**

```
dgss,1,65,1024,65535,0
```

### Parameter Description

| Field | Description |
|-------|-------------|
| `dgss` | Fixed message header, indicates DigiShow Signal |
| `<channel>` | Channel number |
| `<signal_type>` | Signal type: `65` (A = Analog), `66` (B = Binary), `78` (N = Note) |
| `<a_value>` | Signal value (meaning varies by signal type) |
| `<a_range>` | Value range (typically 65535 by default) |
| `<b_value>` | Auxiliary value (meaning varies by signal type) |

---

## 4. Usage by Signal Type

### Analog Signal (signal_type = 65)

- `<a_value>`: Signal value
- `<a_range>`: Value range (typically 65535)
- `<b_value>`: Keep as `0`

### Binary Signal (signal_type = 66)

- `<a_value>`: Keep as `0`
- `<a_range>`: Keep as `0`
- `<b_value>`: `1` (on) or `0` (off)

### Note Signal (signal_type = 78)

- `<a_value>`: Note velocity (0–127)
- `<a_range>`: Value range (fixed at `127`)
- `<b_value>`: `1` (note on) or `0` (note off)

---

## 5. Code Example

```javascript
// Connect to DigiShow
var ws = new WebSocket("ws://127.0.0.1:50000");

// Set channel 1 analog signal to 50%
ws.send('dgss,1,65,32767,65535,0');

// Set channel 2 binary signal to ON
ws.send('dgss,2,66,0,0,1');

// Set channel 3 note signal to ON with 50% velocity
ws.send('dgss,3,78,63,127,1');

// Set channel 3 note signal to OFF
ws.send('dgss,3,78,0,127,0');

// Receive messages from DigiShow
ws.onmessage = function (evt) {
    var msgIn = evt.data;
    var fields = msgIn.split(',');

    if (fields[0] === 'dgss' && fields.length === 6) {
        var channel = fields[1];
        var signalType = fields[2];
        var aValue = fields[3];
        var aRange = fields[4];
        var bValue = fields[5];

        // Handle received signals based on business logic
        console.log(`Channel ${channel} signal updated`);
    }
};
```

---

## 6. Bidirectional Communication

`dgss` messages support **bidirectional transmission**:
- Sent from web: Controls signal values in DigiShow.
- Received by web: Listens for signal value changes in DigiShow.

This enables real-time signal interaction between the web application and the DigiShow project.

---

## 7. Important Notes

- Ensure the Remote Link Service in DigiShow software is properly enabled.
- The default WebSocket connection port is `50000`; verify it is not occupied.
- The message must contain exactly 6 fields and start with `dgss`; otherwise, it will be ignored.
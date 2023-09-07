#!/usr/bin/python3

# pip install websocket-client

import websocket
from digishow import DGSSignal

def on_message(ws, message):
    signalIn = DGSSignal()
    if signalIn.from_message(message) == False :
        return

    if signalIn.type == ord('N') and signalIn.channel == 1 :
        
        print("received a note signal", signalIn.a_value, signalIn.b_value)

        signalOutA = DGSSignal.analog(1, round((signalIn.a_value / signalIn.a_range) * 0xffff))
        ws.send(signalOutA.to_message())

        print("sent an analog signal", signalOutA.a_value)

        signalOutB = DGSSignal.binary(2, signalIn.b_value)
        ws.send(signalOutB.to_message())

        print("sent a binary signal", signalOutB.b_value)


def on_error(ws, error):
    print(error)

def on_close(ws):
    print("### closed ###")

def on_open(ws):
    print("### open ###")


if __name__ == "__main__":
    websocket.enableTrace(False)
    ws = websocket.WebSocketApp("ws://127.0.0.1:50000/",
                              on_message = on_message,
                              on_error = on_error,
                              on_close = on_close,
                              on_open = on_open)
    ws.run_forever()


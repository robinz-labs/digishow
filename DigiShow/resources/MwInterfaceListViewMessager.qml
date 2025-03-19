import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

MwInterfaceListView {

    id: interfaceListView

    interfaceType: "messager"

    dataModel: ListModel {

       ListElement {
           name: "_new"
           type: ""
           mode: ""
           comPort: ""
           comBaud: 0
           comParity: ""
           tcpHost: ""
           tcpPort: 0
           udpHost: ""
           udpPort: 0
       }
    }

    delegate: Rectangle {

        width: gridView.cellWidth
        height: gridView.cellHeight
        color: "transparent"

        MwInterfaceListItem {

            Item {

                id: interfaceListItem

                anchors.fill: parent
                visible: model.name !== "_new"

                COptionButton {
                    id: buttonMode
                    width: buttonComPort.width
                    anchors.left: parent.left
                    anchors.bottom: buttonComPort.top
                    anchors.margins: 20
                    anchors.bottomMargin: 36
                    text: menuMode.findOptionTextByTag(model.mode)

                    onClicked: {
                        menuMode.selectOptionWithTag(model.mode)
                        menuMode.showOptions()
                    }

                    COptionMenu {
                        id: menuMode

                        optionItems: [
                            { text: qsTr("COM (Serial)"), value: 0, tag: "com" },
                            { text: qsTr("TCP"),          value: 1, tag: "tcp" },
                            { text: qsTr("UDP Input"   ), value: 2, tag: "udp_in" },
                            { text: qsTr("UDP Output"  ), value: 3, tag: "udp_out" }

                        ]

                        onOptionClicked: {
                            var options = { mode: selectedItemTag }

                            if (options["mode"] === "com") {

                                options["comPort"] = ""
                                options["comBaud"] = 9600
                                options["comParity"] = "8N1"
                                options["tcpHost"] = undefined
                                options["tcpPort"] = undefined
                                options["udpHost"] = undefined
                                options["udpPort"] = undefined

                            } else if (options["mode"] === "tcp") {

                                options["comPort"] = undefined
                                options["comBaud"] = undefined
                                options["comParity"] = undefined
                                options["tcpHost"] = "127.0.0.1"
                                options["tcpPort"] = 50000
                                options["udpHost"] = undefined
                                options["udpPort"] = undefined

                            } else if (options["mode"] === "udp_in") {

                                options["comPort"] = undefined
                                options["comBaud"] = undefined
                                options["comParity"] = undefined
                                options["tcpHost"] = undefined
                                options["tcpPort"] = undefined
                                options["udpHost"] = undefined
                                options["udpPort"] = 50000

                            } else if (options["mode"] === "udp_out") {

                                options["comPort"] = undefined
                                options["comBaud"] = undefined
                                options["comParity"] = undefined
                                options["tcpHost"] = undefined
                                options["tcpPort"] = undefined
                                options["udpHost"] = "127.0.0.1"
                                options["udpPort"] = 50000
                            }

                            updateInterface(model.index, options)
                        }
                    }

                    Label {
                        anchors.left: parent.left
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 10
                        font.pixelSize: 11
                        text: qsTr("Mode")
                    }
                }

                COptionButton {
                    id: buttonComPort
                    width: 150
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    anchors.margins: 20
                    text: model.comPort===undefined ? "" : model.comPort

                    visible: model.mode === "com"

                    onClicked: {
                        menuComPort.selectOptionWithTag(model.comPort===undefined ? "" : model.comPort)
                        menuComPort.showOptions()
                    }

                    COptionMenu {
                        id: menuComPort
                        width: interfaceListItem.width - 40

                        optionItems: {
                            var items = listOnline["messager"]
                            var options = []
                            var i = 0
                            for (var n=0 ; n<items.length ; n++) {
                                if (items[n]["mode"] === "com") {
                                    options.push({ text: items[n]["comPort"], value: i, tag: items[n]["comPort"]})
                                    i++
                                }
                            }
                            return options
                        }

                        onOptionSelected: {
                            var options = { comPort: selectedItemTag }
                            updateInterface(model.index, options)
                        }
                    }

                    Label {
                        anchors.left: parent.left
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 10
                        font.pixelSize: 11
                        text: qsTr("Serial Port")
                    }
                }

                COptionButton {
                    id: buttonComParity
                    anchors.left: buttonMode.right
                    anchors.right: parent.right
                    anchors.verticalCenter: buttonMode.verticalCenter
                    anchors.margins: 20
                    anchors.leftMargin: 10
                    text: menuComParity.findOptionTextByTag(model.comParity)

                    visible: model.mode === "com"

                    onClicked: {
                        menuComParity.selectOptionWithTag(model.comParity)
                        menuComParity.showOptions()
                    }

                    COptionMenu {
                        id: menuComParity

                        optionItems: [
                            { text: qsTr("8-N-1"), value: 0, tag: "8N1" },
                            { text: qsTr("8-E-1"), value: 1, tag: "8E1" },
                            { text: qsTr("8-O-1"), value: 2, tag: "8O1" },
                            { text: qsTr("7-E-1"), value: 3, tag: "7E1" },
                            { text: qsTr("7-O-1"), value: 4, tag: "7O1" },
                            { text: qsTr("8-N-2"), value: 5, tag: "8N2" }
                        ]

                        onOptionSelected: {
                            var options = { comParity: selectedItemTag }
                            updateInterface(model.index, options)
                        }
                    }

                    Label {
                        anchors.left: parent.left
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 10
                        font.pixelSize: 11
                        text: qsTr("Parity")
                    }
                }

                COptionButton {
                    id: buttonComBaud
                    anchors.left: buttonComPort.right
                    anchors.right: parent.right
                    anchors.verticalCenter: buttonComPort.verticalCenter
                    anchors.margins: 20
                    anchors.leftMargin: 10
                    text: menuComBaud.findOptionTextByValue(model.comBaud)

                    visible: model.mode === "com"

                    onClicked: {
                        menuComBaud.selectOption(model.comBaud)
                        menuComBaud.showOptions()
                    }

                    COptionMenu {
                        id: menuComBaud

                        optionItems: [
                            { text: qsTr("4800"),   value: 4800 },
                            { text: qsTr("9600"),   value: 9600 },
                            { text: qsTr("14400"),  value: 14400 },
                            { text: qsTr("19200"),  value: 19200 },
                            { text: qsTr("38400"),  value: 38400 },
                            { text: qsTr("57600"),  value: 57600 },
                            { text: qsTr("115200"), value: 115200 }
                        ]

                        onOptionSelected: {
                            var options = { comBaud: selectedItemValue }
                            updateInterface(model.index, options)
                        }
                    }

                    Label {
                        anchors.left: parent.left
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 10
                        font.pixelSize: 11
                        text: qsTr("Baud Rate")
                    }
                }

                CTextInputBox {
                    id: textTcpHost
                    width: 150
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    anchors.margins: 20
                    text: model.tcpHost === undefined ? "" : model.tcpHost

                    visible: model.mode === "tcp"

                    onVisibleChanged: {
                        if (visible) input.forceActiveFocus()
                    }

                    onEditingFinished: {
                        if (visible) {
                            var options = { tcpHost: text }
                            updateInterface(model.index, options)
                        }
                    }

                    Label {
                        anchors.left: parent.left
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 10
                        font.pixelSize: 11
                        text: qsTr("Device IP")
                    }
                }

                CTextInputBox {
                    id: textTcpPort
                    anchors.left: textTcpHost.right
                    anchors.right: parent.right
                    anchors.verticalCenter: buttonComPort.verticalCenter
                    anchors.margins: 20
                    anchors.leftMargin: 10
                    validator: IntValidator {
                        bottom: 0
                        top: 65535
                    }
                    text: model.tcpPort === undefined ? "" : model.tcpPort

                    visible: model.mode === "tcp"

                    onEditingFinished: {
                        if (visible) {
                            var options = { tcpPort: parseInt(text) }
                            updateInterface(model.index, options)
                        }
                    }

                    Label {
                        anchors.left: parent.left
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 10
                        font.pixelSize: 11
                        text: qsTr("TCP Port")
                    }
                }

                CTextInputBox {
                    id: textUdpHost
                    width: 150
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    anchors.margins: 20
                    text: model.udpHost === undefined ? "" : model.udpHost

                    visible: model.mode === "udp_out"

                    onVisibleChanged: {
                        if (visible) input.forceActiveFocus()
                    }

                    onEditingFinished: {
                        if (visible) {
                            var options = { udpHost: text }
                            updateInterface(model.index, options)
                        }
                    }

                    Label {
                        anchors.left: parent.left
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 10
                        font.pixelSize: 11
                        text: qsTr("Device IP")
                    }
                }


                CTextInputBox {
                    id: textUdpPort
                    anchors.left: model.mode === "udp_in" ? parent.left : textUdpHost.right
                    anchors.right: parent.right
                    anchors.verticalCenter: buttonComPort.verticalCenter
                    anchors.margins: 20
                    anchors.leftMargin: model.mode === "udp_in" ? 20 : 10
                    validator: IntValidator {
                        bottom: 0
                        top: 65535
                    }
                    text: model.udpPort === undefined ? "" : model.udpPort

                    visible: model.mode === "udp_in" || model.mode === "udp_out"

                    onEditingFinished: {
                        if (visible) {
                            var options = { udpPort: parseInt(text) }
                            updateInterface(model.index, options)
                        }
                    }

                    Label {
                        anchors.left: parent.left
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 10
                        font.pixelSize: 11
                        text: qsTr("UDP Port")
                    }
                }

            }
        }
    }

    Component.onCompleted: {

    }
}



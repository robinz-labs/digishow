import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

MwInterfaceListView {

    id: interfaceListView

    interfaceType: "modbus"

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
                            { text: qsTr("Modbus RTU"),          value: 0, tag: "rtu" },
                            { text: qsTr("Modbus TCP"),          value: 1, tag: "tcp" },
                            { text: qsTr("Modbus RTU over TCP"), value: 2, tag: "rtuovertcp" }
                        ]

                        onOptionClicked: {
                            var options = { mode: selectedItemTag }

                            if (options["mode"] === "rtu") {

                                options["comPort"] = ""
                                options["comBaud"] = 9600
                                options["comParity"] = "8N1"
                                options["tcpHost"] = undefined
                                options["tcpPort"] = undefined

                            } else {

                                options["comPort"] = undefined
                                options["comBaud"] = undefined
                                options["comParity"] = undefined
                                options["tcpHost"] = "127.0.0.1"
                                options["tcpPort"] = 502
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

                    visible: model.mode === "rtu"

                    onClicked: {
                        menuComPort.selectOptionWithTag(model.comPort===undefined ? "" : model.comPort)
                        menuComPort.showOptions()
                    }

                    COptionMenu {
                        id: menuComPort
                        width: interfaceListItem.width - 40

                        optionItems: {
                            var items = listOnline["modbus"]
                            var options = []
                            var i = 0
                            for (var n=0 ; n<items.length ; n++) {
                                if (items[n]["mode"] === "rtu") {
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

                    visible: model.mode === "rtu"

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

                    visible: model.mode === "rtu"

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

                    visible: model.mode !== "rtu"

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

                    visible: model.mode !== "rtu"

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
            }
        }
    }

    Component.onCompleted: {

    }
}



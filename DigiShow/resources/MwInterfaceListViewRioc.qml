import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

MwInterfaceListView {

    id: interfaceListView

    interfaceType: "rioc"

    dataModel: ListModel {

       ListElement {
           name: "_new"
           type: ""
           mode: ""
           comPort: ""
       }
    }

    delegate: Rectangle {

        width: gridView.cellWidth
        height: gridView.cellHeight
        color: "transparent"

        MwInterfaceListItem {

            Item {
                anchors.fill: parent
                visible: model.name !== "_new"

                COptionButton {
                    id: buttonRiocComPort
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: buttonRiocMode.top
                    anchors.margins: 20
                    anchors.bottomMargin: 36
                    text: {
                        if (model.comPort===undefined || model.comPort==="") return qsTr("Automatic")
                        return model.comPort
                    }

                    onClicked: {
                        menuRiocComPort.selectOptionWithTag(model.comPort===undefined ? "" : model.comPort)
                        menuRiocComPort.showOptions()
                    }

                    COptionMenu {
                        id: menuRiocComPort

                        optionItems: {
                            var items = listOnline["rioc"]
                            var options = [ { text:  qsTr("Automatic"), value: -1, tag: "" } ]
                            for (var n=0 ; n<items.length ; n++) {
                                options[n+1] = {
                                    text: items[n]["comPort"],
                                    value: n,
                                    tag: items[n]["comPort"] + "\t" + items[n]["mode"]
                                }
                            }
                            return options
                        }

                        onOptionSelected: {
                            var items = selectedItemTag.split("\t")
                            var options = { comPort: items[0] }
                            if (items.length===2) options["mode"] = items[1]
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
                    id: buttonRiocMode
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.margins: 20
                    text: menuRiocMode.findOptionTextByTag(model.mode)

                    onClicked: {
                        menuRiocMode.selectOptionWithTag(model.mode)
                        menuRiocMode.showOptions()
                    }

                    COptionMenu {
                        id: menuRiocMode

                        optionItems: [
                            { text:  qsTr("General"),              value: 0, tag: "general" },
                            { text:  qsTr("Arduino UNO / Nano"),   value: 1, tag: "arduino_uno" },
                            { text:  qsTr("Arduino MEGA"),         value: 2, tag: "arduino_mega" },
                            { text:  qsTr("Arduino Leonardo / Micro"),
                                                                   value: 3, tag: "arduino_micro" },
                            { text:  "-",                          value:-1 },
                            { text:  qsTr("Aladdin Micro"),        value: 4, tag: "aladdin1" },
                            { text:  qsTr("Aladdin MEGA"),         value: 5, tag: "aladdin2" },
                            { text:  qsTr("Aladdin ESP32"),        value: 6, tag: "aladdin3" },
                            { text:  qsTr("Aladdin"),              value: 7, tag: "aladdin" },
                            { text:  qsTr("Arduino PLC Model 1"),  value: 8, tag: "plc1" },
                            { text:  qsTr("Arduino PLC Model 2"),  value: 9, tag: "plc2" }
                        ]

                        onOptionSelected: {
                            var options = {
                                mode: selectedItemTag
                            }
                            updateInterface(model.index, options)
                        }
                    }

                    Label {
                        anchors.left: parent.left
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 10
                        font.pixelSize: 11
                        text: qsTr("Model")

                    }
                }
            }
        }
    }

    Component.onCompleted: {

    }
}



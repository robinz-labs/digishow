import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

MwInterfaceListView {

    id: interfaceListView

    interfaceType: "artnet"

    dataModel: ListModel {

       ListElement {
           name: "_new"
           type: ""
           mode: ""
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
                anchors.fill: parent
                visible: model.name !== "_new"

                COptionButton {
                    id: buttonMode
                    width: textUdpHost.width
                    anchors.left: parent.left
                    anchors.bottom: textUdpHost.top
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
                            { text: qsTr("Input"),  value: 0, tag: "input" },
                            { text: qsTr("Output"), value: 1, tag: "output" }
                        ]

                        onOptionClicked: {
                            var options = { mode: selectedItemTag }

                            if (options["mode"] === "input") {

                                options["udpHost"] = undefined
                                options["udpPort"] = 6454

                            } else if (options["mode"] === "output") {

                                options["udpHost"] = "255.255.255.255"
                                options["udpPort"] = 6454
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

                CTextInputBox {
                    id: textUdpHost
                    width: 135
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    anchors.margins: 20
                    text: model.udpHost === undefined ? "" : model.udpHost

                    visible: model.mode === "output"

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
                        text: qsTr("IP Address")
                    }
                }

                CTextInputBox {
                    id: textUdpPort
                    anchors.left: textUdpHost.visible ? textUdpHost.right : textUdpHost.left
                    anchors.right: textUdpHost.visible ? parent.right : textUdpHost.right
                    anchors.verticalCenter: textUdpHost.verticalCenter
                    anchors.margins: 20
                    anchors.leftMargin: textUdpHost.visible ? 10 : 0
                    anchors.rightMargin: textUdpHost.visible ? 20 : 0
                    validator: IntValidator {
                        bottom: 0
                        top: 65535
                    }
                    text: model.udpPort === undefined ? "" : model.udpPort

                    onEditingFinished: {
                        var options = { udpPort: parseInt(text) }
                        updateInterface(model.index, options)
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



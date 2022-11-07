import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

MwInterfaceListView {

    id: interfaceListView

    interfaceType: "pipe"

    dataModel: ListModel {

       ListElement {
           name: "_new"
           type: ""
           mode: ""
           comment: ""
           acceptRemote: 0
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
                anchors.fill: parent
                visible: model.name !== "_new"

                COptionButton {
                    id: buttonMode
                    width: textTcpHost.width
                    anchors.left: parent.left
                    anchors.bottom: textTcpHost.top
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
                            { text: qsTr("Local Pipe"),  value: 0, tag: "local" },
                            { text: qsTr("Remote Pipe"), value: 1, tag: "remote" }
                        ]

                        onOptionClicked: {
                            var options = { mode: selectedItemTag }

                            if (options["mode"] === "local") {

                                options["acceptRemote"] = 0
                                options["tcpHost"] = undefined
                                options["tcpPort"] = undefined
                                options["outputInterval"] = undefined

                            } else if (options["mode"] === "remote") {

                                options["acceptRemote"] = undefined
                                options["tcpHost"] = "127.0.0.1"
                                options["tcpPort"] = 50000
                                options["outputInterval"] = 20
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
                    id: textTcpHost
                    width: 135
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    anchors.margins: 20
                    text: model.tcpHost === undefined ? "" : model.tcpHost

                    visible: model.mode === "remote"

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
                        text: qsTr("Remote IP")
                    }
                }

                CTextInputBox {
                    id: textTcpPort
                    anchors.left: textTcpHost.right
                    anchors.right: parent.right
                    anchors.verticalCenter: textTcpHost.verticalCenter
                    anchors.margins: 20
                    anchors.leftMargin: 10
                    validator: IntValidator {
                        bottom: 0
                        top: 65535
                    }
                    text: model.tcpPort === undefined ? "" : model.tcpPort

                    visible: model.mode === "remote" || (model.mode === "local" && model.acceptRemote)

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

                COptionButton {
                    id: buttonAcceptRemote
                    width: textTcpHost.width
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    anchors.margins: 20
                    text: menuAcceptRemote.findOptionTextByValue(model.acceptRemote)

                    visible: model.mode === "local"

                    onClicked: {
                        menuAcceptRemote.selectOptionWithTag(model.mode)
                        menuAcceptRemote.showOptions()
                    }

                    COptionMenu {
                        id: menuAcceptRemote

                        optionItems: [
                            { text: qsTr("Disabled (default)"), value: 0 },
                            { text: qsTr("Enabled" ), value: 1 },
                            { text: qsTr("Enabled (multiple)" ), value: 2 }
                        ]

                        onOptionClicked: {
                            var options = { acceptRemote: selectedItemValue }

                            if (options["acceptRemote"] > 0) {

                                options["tcpPort"] = 50000
                                options["outputInterval"] = 20

                            } else {

                                options["tcpPort"] = undefined
                                options["outputInterval"] = undefined
                            }

                            updateInterface(model.index, options)
                        }
                    }

                    Label {
                        anchors.left: parent.left
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 10
                        font.pixelSize: 11
                        text: qsTr("Remote Link Service")
                    }
                }


                CTextInputBox {
                    id: textComment
                    anchors.left: buttonMode.right
                    anchors.right: parent.right
                    anchors.verticalCenter: buttonMode.verticalCenter
                    anchors.margins: 20
                    anchors.leftMargin: 10
                    text: model.comment === undefined ? "" : model.comment

                    onEditingFinished: {
                        var options = { comment: text }
                        updateInterface(model.index, options)
                    }

                    Label {
                        anchors.left: parent.left
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 10
                        font.pixelSize: 11
                        text: qsTr("Comment")
                    }
                }
            }
        }
    }

    Component.onCompleted: {

    }
}



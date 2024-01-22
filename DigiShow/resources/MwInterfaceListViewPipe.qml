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
           pipeId: ""
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
                            { text: qsTr("Local Pipe" ), value: 0, tag: "local" },
                            { text: qsTr("Remote Pipe"), value: 1, tag: "remote" },
                            { text: qsTr("Cloud Pipe" ), value: 2, tag: "cloud" }
                        ]

                        onOptionClicked: {
                            var options = { mode: selectedItemTag }

                            if (options["mode"] === "local") {

                                options["acceptRemote"] = 0
                                options["tcpHost"] = undefined
                                options["tcpPort"] = undefined
                                options["outputInterval"] = undefined
                                options["pipeId"] = undefined

                            } else if (options["mode"] === "remote") {

                                options["acceptRemote"] = undefined
                                options["tcpHost"] = "127.0.0.1"
                                options["tcpPort"] = 50000
                                options["outputInterval"] = 20
                                options["pipeId"] = undefined

                            } else if (options["mode"] === "cloud") {

                                options["acceptRemote"] = undefined
                                options["tcpHost"] = undefined
                                options["tcpPort"] = undefined
                                options["outputInterval"] = 20
                                options["pipeId"] = "1"
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

                CButton {
                    id: buttonCloudSignIn
                    height: 28
                    width: 100 //135
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    anchors.margins: 20
                    label.font.bold: false
                    label.font.pixelSize: 11
                    label.text: qsTr("Sign In ...")
                    box.radius: 3

                    visible: model.mode === "cloud"

                    onClicked: cloudSignIn()
                }

                COptionButton {
                    id: buttonCloudPipeId
                    anchors.left: buttonMode.right
                    anchors.right: parent.right
                    anchors.verticalCenter: buttonMode.verticalCenter
                    anchors.margins: 20
                    anchors.leftMargin: 10
                    text: menuCloudPipeId.findOptionTextByTag(model.pipeId)

                    visible: model.mode === "cloud"

                    onClicked: {
                        menuCloudPipeId.selectOptionWithTag(model.pipeId)
                        menuCloudPipeId.showOptions()
                    }

                    COptionMenu {
                        id: menuCloudPipeId

                        optionItems: [
                            { text: "1", value: 1, tag: "1" },
                            { text: "2", value: 2, tag: "2" },
                            { text: "3", value: 3, tag: "3" },
                            { text: "4", value: 4, tag: "4" },
                            { text: "5", value: 5, tag: "5" },
                            { text: "6", value: 6, tag: "6" },
                            { text: "7", value: 7, tag: "7" },
                            { text: "8", value: 8, tag: "8" },
                            { text: "9", value: 9, tag: "9" },
                            { text: "10", value: 10, tag: "10" },
                            { text: "11", value: 11, tag: "11" },
                            { text: "12", value: 12, tag: "12" },
                            { text: "13", value: 13, tag: "13" },
                            { text: "14", value: 14, tag: "14" },
                            { text: "15", value: 15, tag: "15" },
                            { text: "16", value: 16, tag: "16" },
                            { text: "17", value: 17, tag: "17" },
                            { text: "18", value: 18, tag: "18" },
                            { text: "19", value: 19, tag: "19" },
                            { text: "20", value: 20, tag: "20" }
                        ]

                        onOptionClicked: {
                            var options = { pipeId: selectedItemTag }
                            updateInterface(model.index, options)
                        }
                    }

                    Label {
                        anchors.left: parent.left
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 10
                        font.pixelSize: 11
                        text: qsTr("Pipe ID")
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

                    visible: model.mode !== "cloud"

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

    DigishowCloud {
        id: cloud
    }

    Timer {
        id: timerCloudCheckSignIn

        interval: 1500
        repeat: true
        running: false

        onTriggered: {

            var status = cloud.checkSignInStatus()
            switch (status) {
            case DigishowCloud.SignInSuccess: messageBox.stopWaiting(2); break
            case DigishowCloud.SignInError:   messageBox.stopWaiting(3); break
            }
        }
    }


    Component.onCompleted: {

    }

    function cloudSignIn() {

        var sessionInfo = cloud.getSessionInfo()

        if (Object.keys(sessionInfo).length === 0) {

            // the client has not signed in yet

            timerCloudCheckSignIn.start()

            var r = messageBox.showQrAndWait(
                        cloud.getSignInCode(),
                        qsTr("Please use DigiShow Outlet app on your phone to scan the QR code here to sign in to the cloud service:"),
                        qsTr("Not Now"))

            timerCloudCheckSignIn.stop()

            switch (r) {
            case 2: messageBox.show(qsTr("Succeeded to sign in to DigiShow Outlet cloud service."    ), qsTr("OK")); break
            case 3: messageBox.show(qsTr("Can't connect or sign in to DigiShow Outlet cloud service."), qsTr("OK")); break
            }

        } else {

            // the client has already signed in

            if (messageBox.showAndWait(
                    qsTr("The computer has signed in to DigiShow Outlet cloud service.\r\n\r\nUser: %1\r\nConnection: %2")
                        .arg(sessionInfo["userName"]).arg(sessionInfo["clientName"]),
                    qsTr("Sign Out"),
                    qsTr("Cancel")) === 1) {

                cloud.signOut();
            }

        }


    }
}


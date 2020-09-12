import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

MwInterfaceListView {

    id: interfaceListView

    interfaceType: "hue"

    dataModel: ListModel {

       ListElement {
           name: "_new"
           type: ""
           mode: ""
           serial: ""
           username: ""
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

                CTextInputBox {
                    id: textSerial
                    width: textUsername.width
                    anchors.left: parent.left
                    anchors.bottom: textUsername.top
                    anchors.margins: 20
                    anchors.bottomMargin: 36
                    text: model.serial === undefined ? "" : model.serial

                    onEditingFinished: {
                        var options = { serial: text }
                        updateInterface(model.index, options)
                    }

                    Label {
                        anchors.left: parent.left
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 10
                        font.pixelSize: 11
                        text: qsTr("Hue Bridge ID")
                    }
                }

                CButton {
                    id: buttonSerial
                    height: 28
                    anchors.left: textSerial.right
                    anchors.right: parent.right
                    anchors.verticalCenter: textSerial.verticalCenter
                    anchors.margins: 20
                    anchors.leftMargin: 10
                    label.font.bold: false
                    label.font.pixelSize: 11
                    label.text: qsTr("Find")
                    box.radius: 3

                    onClicked: {

                        // find bridge by UPnP
                        messageBox.show(qsTr("Finding Hue bridge on the local area network ..."))
                        var rsp = utilities.upnpWaitResponse("hue-bridgeid: ", 3000)
                        messageBox.close()
                        if (rsp === "") {
                            messageBox.show(qsTr("Unable to find any Hue bridge on your local area network."), qsTr("OK"))
                            return
                        }

                        // extract bridge id from the UPnP response data
                        var serial = null
                        var lines = rsp.split("\r\n")
                        for (var n=0 ; n<lines.length ; n++) {
                            var line = lines[n]
                            if (line.startsWith("hue-bridgeid: ")) {
                                serial = line.split(" ")[1]
                            }
                        }

                        if (serial !== null) {
                            textSerial.text = serial
                            var options = { serial: serial }
                            updateInterface(model.index, options)
                        }
                    }
                }

                CTextInputBox {
                    id: textUsername
                    width: 140
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    anchors.margins: 20
                    text: model.username === undefined ? "" : model.username

                    onEditingFinished: {
                        var options = { username: text }
                        updateInterface(model.index, options)
                    }

                    Label {
                        anchors.left: parent.left
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 10
                        font.pixelSize: 11
                        text: qsTr("Authorized User PIN")
                    }
                }

                CButton {
                    id: buttonUsername
                    height: 28
                    anchors.left: textUsername.right
                    anchors.right: parent.right
                    anchors.verticalCenter: textUsername.verticalCenter
                    anchors.margins: 20
                    anchors.leftMargin: 10
                    label.font.bold: false
                    label.font.pixelSize: 11
                    label.text: qsTr("Generate...")
                    box.radius: 3

                    onClicked: {

                        if (model.serial === undefined || model.serial === "") {
                            messageBox.show(qsTr("Please specify the Hue bridge id first."), qsTr("OK"))
                            return
                        }

                        // find bridge ip by UPnP
                        messageBox.show(qsTr("Finding Hue bridge on the local area network ..."))
                        var rsp = utilities.upnpWaitResponse("hue-bridgeid: " + model.serial, 3000)
                        messageBox.close()
                        var ip = rsp.split("\r\n")[0]
                        if (ip === "") {
                            messageBox.show(qsTr("Unable to find the Hue bridge with the specified id %1.").arg(model.serial), qsTr("OK"))
                            return
                        }

                        // get ready to generate user
                        if (messageBox.showAndWait(
                                    qsTr("In order to generate an authorized user for DigiShow connections, please press the physical link button on your Hue bridge and click Generate in this dialog box."),
                                    qsTr("Generate"),
                                    qsTr("Cancel")) !== 1) return

                        // call new user api
                        rsp = utilities.httpRequest(
                                    "http://" + ip + "/api",
                                    "post",
                                    JSON.stringify({"devicetype":"digishow_app#interface_hue"}));

                        console.log("http://" + ip + "/api", rsp)

                        try {
                            var username = JSON.parse(rsp)[0]["success"]["username"]
                            textUsername.text = username
                            var options = { username: username }
                            updateInterface(model.index, options)
                        } catch (e) {
                            messageBox.show(qsTr("Failed to generate the user on your Hue bridge %1, please confirm you have pressed the link button on the Hue bridge.").arg(model.serial), qsTr("OK"))
                            return
                        }
                    }
                }


            }
        }
    }

    Component.onCompleted: {

    }

}



import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

Dialog {
    id: dialog

    width: 480
    height: 360
    anchors.centerIn: parent
    modal: true
    focus: true

    background: Rectangle {
        anchors.fill: parent
        color: "#333333"
        radius: 5
        border.width: 2
        border.color: "#cccccc"
    }

    // @disable-check M16
    Overlay.modal: Rectangle {
        color: "#cc000000"
    }

    Column {
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20
        spacing: 20

        Label {
            text: qsTr("Security key for the file encryption (optional)")
            color: "white"
            font.pixelSize: 12
        }

        CTextInputBox {
            id: textFileKey
            width: parent.width
            input.echoMode: TextInput.Password
            onReturnPressed: buttonSave.clicked()
        }

        Item {
            width: parent.width
            height: 10
        }

        CheckBox {
            id: checkLocalUseOnly
            width: parent.width
            text: qsTr("Only allow the encrypted file to be opened on this machine")
            font.pixelSize: 12
            checked: true
        }
    }

    CButton {
        id: buttonSave
        width: 150
        height: 32
        anchors.left: parent.left
        anchors.leftMargin: 150
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        label.font.bold: true
        label.font.pixelSize: 12
        label.text: qsTr("Encrypt and Save")
        box.radius: 3
        colorNormal: Material.accent
        onClicked: {
            dialog.accept()
        }
    }

    CButton {
        id: buttonCancel
        width: 90
        height: 32
        anchors.left: buttonSave.right
        anchors.leftMargin: 15
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        label.font.bold: true
        label.font.pixelSize: 12
        label.text: qsTr("Cancel")
        box.radius: 3
        onClicked: {
            dialog.reject()
        }
    }

    function show() {
        textFileKey.text = ""
        textFileKey.input.forceActiveFocus()
        dialog.open()
    }

    function getValues() {
        return {
            "fileKey": textFileKey.text,
            "localUseOnly": checkLocalUseOnly.checked
        }
    }

}

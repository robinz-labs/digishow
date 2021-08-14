import QtQuick 2.12
import QtQuick.Controls 2.12

Popup {

    id: messageBox

    property int buttonCount: 3
    property int buttonClickedAt: -1

    readonly property bool noButton: !button1.visible && !button2.visible && !button3.visible

    signal buttonClicked(int buttonID)

    width: 640
    height: (noButton ? 220 : 320)
    anchors.centerIn: parent

    modal: false
    dim: true
    visible: false
    focus: true
    closePolicy: Popup.NoAutoClose

    background: Rectangle {
        anchors.fill: parent
        color: "#111111"
        opacity: 0.9
        border.color: "#ffffff"
        border.width: 2
        radius: 12
    }

    contentItem: Item {

        anchors.fill: parent
        focus: true

        Keys.onPressed: {
            if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
                messageBox.buttonClickedAt = 1
                messageBox.close()
                buttonClicked(1)

            } else if (event.key === Qt.Key_Escape) {
                messageBox.buttonClickedAt = 0
                messageBox.close()
                buttonClicked(0)
            }
        }

        Label {
            id: labelMessage
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: noButton ? parent.bottom : rowButtons.top
            anchors.margins: 40
            font.bold: true
            font.pixelSize: 18
            lineHeight: 1.5
            text: ""
            wrapMode: Text.WordWrap
            horizontalAlignment: Label.AlignHCenter
            verticalAlignment: Label.AlignVCenter
        }

        Row {
            id: rowButtons
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottomMargin: 50
            spacing: 20

            CButton {
                id: button1
                width: 130
                height: 42
                label.text: "Button 1"
                onClicked: {
                    messageBox.buttonClickedAt = 1
                    messageBox.close()
                    buttonClicked(1)
                }
            }

            CButton {
                id: button2
                width: 130
                height: 42
                label.text: "Button 2"
                onClicked: {
                    messageBox.buttonClickedAt = 2
                    messageBox.close()
                    buttonClicked(2)
                }
            }

            CButton {
                id: button3
                width: 130
                height: 42
                label.text: "Button 3"
                onClicked: {
                    messageBox.buttonClickedAt = 3
                    messageBox.close()
                    buttonClicked(3)
                }
            }
        }
    }

    // @disable-check M16
    Overlay.modeless: Rectangle {
        color: "#55000000"

        MouseArea {
            anchors.fill: parent
            onClicked: {
                messageBox.buttonClickedAt = 0
                messageBox.close()
                buttonClicked(0)
            }
        }
    }

    function show(message, buttonText1, buttonText2, buttonText3) {

        messageBox.buttonCount = 0

        if (buttonText1 !== undefined) {
            button1.visible = true
            button1.label.text = buttonText1
            messageBox.buttonCount++
        } else {
            button1.visible = false
        }

        if (buttonText2 !== undefined) {
            button2.visible = true
            button2.label.text = buttonText2
            messageBox.buttonCount++
        } else {
            button2.visible = false
        }

        if (buttonText3 !== undefined) {
            button3.visible = true
            button3.label.text = buttonText3
            messageBox.buttonCount++
        } else {
            button3.visible = false
        }

        if (messageBox.buttonCount > 0) {
            rowButtons.anchors.bottomMargin = 50
        } else {
            rowButtons.anchors.bottomMargin = 0
        }

        labelMessage.text = message

        messageBox.buttonClickedAt = -1
        messageBox.open()

        contentItem.forceActiveFocus()
    }

    function showAndWait(message, buttonText1, buttonText2, buttonText3) {

        show(message, buttonText1, buttonText2, buttonText3)

        while (messageBox.buttonClickedAt == -1) {
            utilities.doEvents()
        }

        return messageBox.buttonClickedAt

    }
}

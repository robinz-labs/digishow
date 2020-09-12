import QtQuick 2.12
import QtQuick.Controls 2.12

Item {

    id: inputBox

    property alias text: input.text
    property alias validator: input.validator
    property alias input: input
    property alias box: box

    signal editingFinished()
    signal textEdited()

    width: 150
    height: 28

    Rectangle {
        id: box
        anchors.fill: parent
        color: "#181818"
        border.color: "#383838"
        border.width: 1
        radius: 3
    }

    TextInput {
        id: input
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        color: "#cccccc"
        selectionColor: "#666666"
        selectedTextColor: "#ffffff"
        horizontalAlignment: Qt.AlignLeft
        verticalAlignment: Qt.AlignVCenter
        selectByMouse: true
        font.pixelSize: 11
        clip: true
        focus: true

        onEditingFinished: inputBox.editingFinished() // emit signal
        onTextEdited: inputBox.textEdited() // emit signal
    }

}

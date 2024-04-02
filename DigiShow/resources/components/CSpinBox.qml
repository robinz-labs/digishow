import QtQuick 2.12
import QtQuick.Controls 2.12

SpinBox {

    id: spinBox

    property string unit: ""

    width: 100
    height: 28
    editable: true
    font.pixelSize: 12
    focus: true
    value: 0

    background: Rectangle {
        color: "#181818"
        border.color: "#383838"
        border.width: 1
        radius: 3
    }

    contentItem: Item {

        anchors.fill: parent
        anchors.leftMargin: spinBox.height
        anchors.rightMargin: spinBox.height

        TextInput {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            text: spinBox.textFromValue(spinBox.value)
            font: spinBox.font
            color: "#cccccc"
            selectionColor: "#666666"
            selectedTextColor: "#ffffff"
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            selectByMouse: true
            readOnly: !spinBox.editable
            //validator: spinBox.validator
            inputMethodHints: Qt.ImhDigitsOnly
            clip: true
            onEditingFinished: {
                spinBox.value = spinBox.valueFromText(text)
                valueModified() // emit signal
            }
        }
    }

    textFromValue: function(value) {
        if (unit !== "")
            return value + " " + unit
        else
            return value
    }

    valueFromText: function(text) {
        return parseInt(text)
    }
}

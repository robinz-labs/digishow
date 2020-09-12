import QtQuick 2.12
import QtQuick.Controls 2.12

SpinBox {

    id: spinBox

    width: parent.width
    height: parent.height
    visible: false
    editable: true
    font.pixelSize: 12
    value: 0

    background: Rectangle {
        color: "#181818"
        border.color: "#383838"
        border.width: 1
        radius: 3
    }

    contentItem: TextInput {
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        text: spinBox.value //spinBox.textFromValue(spinBox.value, spinBox.locale)
        font: spinBox.font
        color: "#cccccc"
        selectionColor: "#666666"
        selectedTextColor: "#ffffff"
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
        selectByMouse: true
        readOnly: !spinBox.editable
        validator: spinBox.validator
        inputMethodHints: Qt.ImhDigitsOnly
        onEditingFinished: spinBox.visible = false
        onVisibleChanged: if (visible) forceActiveFocus()
    }

    onValueChanged: {
        common.setAncestorProperty(spinBox, "isModified", true)
    }
}

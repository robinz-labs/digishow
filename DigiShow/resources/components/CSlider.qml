import QtQuick 2.12
import QtQuick.Controls 2.12

Slider {
    id: slider

    property color color: "#006699"
    property bool inverted: false

    // customize control style
    background: Rectangle {
        height: 4
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        color: "#333333"
        radius: 2
    }

    handle: Rectangle {
        width: 8
        height: 20
        anchors.left: parent.left
        anchors.leftMargin: (parent.width - width) * slider.value / (slider.to - slider.from)
        anchors.verticalCenter: parent.verticalCenter
        color: slider.pressed ? "#cccccc" : slider.color
        radius: 3
    }

    Rectangle {
        height: 4
        anchors.left: parent.left
        anchors.right: slider.handle.left
        anchors.rightMargin: -3
        anchors.verticalCenter: parent.verticalCenter
        color: slider.color
        radius: 2
        opacity: 0.5
        visible: !inverted
    }

    Rectangle {
        height: 4
        anchors.right: parent.right
        anchors.left: slider.handle.right
        anchors.leftMargin: -3
        anchors.verticalCenter: parent.verticalCenter
        color: slider.color
        radius: 2
        opacity: 0.5
        visible: inverted
    }
}

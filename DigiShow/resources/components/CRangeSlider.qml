import QtQuick 2.12
import QtQuick.Controls 2.12

RangeSlider {
    id: slider

    property color color: "#006699"

    // customize control style
    background: Rectangle {
        height: 4
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        color: "#333333"
        radius: 2
    }

    first.handle: Rectangle {
        width: 8
        height: 20
        anchors.left: parent.left
        anchors.leftMargin: (parent.width - width) * slider.first.value / (slider.to - slider.from)
        anchors.verticalCenter: parent.verticalCenter
        color: slider.first.pressed ? "#cccccc" : slider.color
        radius: 3
    }

    second.handle: Rectangle {
        width: 8
        height: 20
        anchors.left: parent.left
        anchors.leftMargin: (parent.width - width) * slider.second.value / (slider.to - slider.from)
        anchors.verticalCenter: parent.verticalCenter
        color: slider.second.pressed ? "#cccccc" : slider.color
        radius: 3
    }

    Rectangle {
        height: 4
        anchors.left: slider.first.handle.right
        anchors.leftMargin: -3
        anchors.right: slider.second.handle.left
        anchors.rightMargin: -3
        anchors.verticalCenter: parent.verticalCenter
        color: slider.color
        radius: 2
        opacity: 0.5
    }
}

import QtQuick 2.12
import QtQuick.Controls.Material 2.12

Rectangle {

    property alias text: textTip.text
    property int offset: 60

    width: 36
    height: 36
    anchors.top: parent.bottom
    anchors.topMargin: offset
    anchors.horizontalCenter: parent.horizontalCenter
    radius: 18
    color: Material.accent

    Rectangle {
        width: 1
        height: offset
        anchors.bottom: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        color: Material.accent
    }

    Rectangle {
        width: 5
        height: 5
        anchors.bottom: parent.top
        anchors.bottomMargin: offset - 3
        anchors.horizontalCenter: parent.horizontalCenter
        radius: 3
        color: Material.accent
    }

    Text {

        id: textTip

        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#ffffff"
        text: ""
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 14
        font.bold: true
    }

    Behavior on opacity { NumberAnimation { duration: 300 } }
}

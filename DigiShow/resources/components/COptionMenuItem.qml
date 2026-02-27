import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

MenuItem {

    id: menuItem

    property int index: 0
    property bool secondary: false
    property bool showIcon: false

    signal itemSelected(int index)

    font.pixelSize: 12
    leftPadding: 8
    rightPadding: 4
    height: 28

    contentItem: Text {
        text: menuItem.text
        color: "#ffffff"
        font: menuItem.font
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        implicitHeight: 28
        color: menuItem.highlighted ? Material.accent :
               menuItem.secondary ? "#383838" : "transparent"
        radius: menuItem.highlighted ? 2 : 0

        Image {
            id: imageIcon
            width: 16
            height: 16
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 4
            source: menuItem.secondary ? "qrc:///images/icon_adjust_white.png" : "qrc:///images/icon_arrow_white.png"
            opacity: menuItem.secondary ? 0.4 : 1.0
            visible: showIcon
        }
    }

    onTriggered: {

        itemSelected(index)
    }
}

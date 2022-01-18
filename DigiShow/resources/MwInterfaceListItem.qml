import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

Rectangle {

    id: interfaceItem

    anchors.fill: parent
    anchors.margins: 6
    radius: 6
    color: "#222222"
    border.color: "#555555"
    border.width: gridView.currentIndex === model.index ? 1 : 0

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {

            gridView.currentIndex = model.index

            if (mouse.button === Qt.RightButton && model.name !== "_new") {
                menuInterface.x = mouse.x
                menuInterface.y = mouse.y
                menuInterface.open()
            }
        }

        CMenu {
            id: menuInterface
            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

            CMenuItem {
                text: qsTr("Delete Interface")
                onTriggered: {
                    menuInterface.close()
                    deleteInterface(model.index)
                }
            }
        }
    }

    CButton {

        anchors.fill: parent
        icon.width: 64
        icon.height: 64
        icon.source: "qrc:///images/icon_add_white.png"
        colorNormal: "#222222"

        visible: model.name === "_new"

        onClicked: {
            addNewInterface()
        }
    }

    CButton {
        width: 24
        height: 24
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: -4
        icon.width: 24
        icon.height: 24
        icon.source: "qrc:///images/icon_close_white.png"
        box.radius: 12
        box.border.width: 0
        colorNormal: Material.accent

        visible: model.name !== "_new" &&  gridView.currentIndex === model.index

        onClicked: {
            deleteInterface(model.index)
        }
    }

    Image {
        width: 32
        height: 32
        anchors.top: parent.top
        anchors.topMargin: 16
        anchors.left: parent.left
        anchors.leftMargin: 18
        opacity: 0.2
        source: {
            if (model.name === "_new") return ""
            return "qrc:///images/icon_interface_" + model.type + "_white.png"
        }

        Label {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.right
            anchors.margins: 20
            font.bold: true
            text: {

                if (model.name === "_new") return ""

                var typeName = ""
                if      (model.type === "midi"  ) typeName = qsTr("MIDI")
                else if (model.type === "dmx"   ) typeName = qsTr("DMX")
                else if (model.type === "osc"   ) typeName = qsTr("OSC")
                else if (model.type === "artnet") typeName = qsTr("ArtNet")
                else if (model.type === "modbus") typeName = qsTr("Modbus")
                else if (model.type === "rioc"  ) typeName = qsTr("Arduino")                
                else if (model.type === "hue"   ) typeName = qsTr("Hue")
                else if (model.type === "screen") typeName = qsTr("Screen")
                else if (model.type === "pipe"  ) typeName = qsTr("Virtual Pipe")

                return typeName + " " + (model.index + 1)
            }
        }
    }

}

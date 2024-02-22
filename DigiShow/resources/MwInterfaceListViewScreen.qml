import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

MwInterfaceListView {

    id: interfaceListView

    interfaceType: "screen"

    dataModel: ListModel {

       ListElement {
           name: "_new"
           type: ""
           mode: ""
           screen: 0
       }
    }

    delegate: Rectangle {

        width: gridView.cellWidth
        height: gridView.cellHeight
        color: "transparent"

        MwInterfaceListItem {

            Item {
                anchors.fill: parent
                visible: model.name !== "_new"

                COptionButton {
                    id: buttonScreen
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.margins: 20
                    //anchors.bottom: buttonMedia.top
                    //anchors.bottomMargin: 36
                    text: screenName(model.screen)

                    onClicked: {
                        menuScreen.selectOption(model.screen)
                        menuScreen.showOptions()
                    }

                    COptionMenu {
                        id: menuScreen

                        optionItems: {
                            var items = listOnline["screen"]
                            var options = []
                            for (var n=-1 ; n<=Math.max(4, items.length) ; n++) {
                                options.push({
                                    text: screenName(n) + (n>=1 && n<=items.length ? " (" + items[n-1]["width"] + "x" + items[n-1]["height"] + ")" : ""),
                                    value: n
                                })
                            }
                            return options
                        }

                        onOptionSelected: {
                            var options = { screen: selectedItemValue }
                            updateInterface(model.index, options)
                        }
                    }

                    Label {
                        anchors.left: parent.left
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 10
                        font.pixelSize: 11
                        text: qsTr("Screen")
                    }
                }

                /*
                CButton {
                    id: buttonMedia
                    height: 28
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.margins: 20
                    label.font.bold: false
                    label.font.pixelSize: 11
                    label.text: qsTr("Edit Media Library ...")
                    box.radius: 3

                    onClicked: {
                        // TODO:
                    }

                    Label {
                        anchors.left: parent.left
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 10
                        font.pixelSize: 11
                        text: qsTr("Preloaded Media")
                    }
                }
                */
            }
        }
    }

    Component.onCompleted: {

    }

    function screenName(n) {
        switch (n) {
        case -1: return qsTr("Preview Window")
        case  0: return qsTr("Default")
        default: return qsTr("Screen") + " " + n
        }
    }
}



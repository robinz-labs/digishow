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
           screen: ""
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
                    text: model.screen===undefined || model.screen==="" ? qsTr("Default") : model.screen

                    onClicked: {
                        menuScreen.selectOptionWithTag(model.screen===undefined ? "" : model.screen)
                        menuScreen.showOptions()
                    }

                    COptionMenu {
                        id: menuScreen

                        optionItems: {
                            var items = listOnline["screen"]
                            var options = [ { text: qsTr("Default"), value: 0, tag: "" } ]
                            for (var n=0 ; n<items.length ; n++) {
                                options.push({
                                    text: items[n]["screen"] + " (" + items[n]["width"] + "x" + items[n]["height"] + ")",
                                    value: n+1,
                                    tag: items[n]["screen"]
                                })
                            }
                            return options
                        }

                        onOptionSelected: {
                            var options = { screen: selectedItemTag }
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
}



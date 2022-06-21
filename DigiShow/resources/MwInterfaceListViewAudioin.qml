import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

MwInterfaceListView {

    id: interfaceListView

    interfaceType: "audioin"

    dataModel: ListModel {

       ListElement {
           name: "_new"
           type: ""
           mode: ""
           port: ""
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
                    id: buttonPort
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.margins: 20
                    text: model.port===undefined || model.port==="" ? qsTr("Default") : model.port

                    onClicked: {
                        menuPort.selectOptionWithTag(model.port===undefined ? "" : model.port)
                        menuPort.showOptions()
                    }

                    COptionMenu {
                        id: menuPort

                        optionItems: {
                            var items = listOnline["audioin"]
                            var options = [ { text: qsTr("Default"), value: 0, tag: "" } ]
                            for (var n=0 ; n<items.length ; n++) {
                                options.push({
                                    text: items[n]["port"],
                                    value: n+1,
                                    tag: items[n]["port"]
                                })
                            }
                            return options
                        }

                        onOptionSelected: {
                            var options = { port: selectedItemTag }
                            updateInterface(model.index, options)
                        }
                    }

                    Label {
                        anchors.left: parent.left
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 10
                        font.pixelSize: 11
                        text: qsTr("Audio Input Device")
                    }
                }
            }
        }
    }

    Component.onCompleted: {

    }
}



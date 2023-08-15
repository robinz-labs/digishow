import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

MwInterfaceListView {

    id: interfaceListView

    interfaceType: "mplay"

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
                    id: buttonMidiPort
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.margins: 20
                    text: {
                        if (model.port === undefined || model.port === "") return ""
                        return model.port
                    }

                    onClicked: {
                        menuMidiPort.selectOptionWithTag(model.port === undefined || model.port === "" ? "" : model.port)
                        menuMidiPort.showOptions()
                    }

                    COptionMenu {
                        id: menuMidiPort

                        optionItems: {
                            var items = listOnline["midi"]
                            var options = [ { text: "", value: -1, tag: "" } ]
                            for (var n=0 ; n<items.length ; n++) {
                                if (items[n]["mode"] === "output") {
                                    options.push({
                                        text: items[n]["port"],
                                        value: n,
                                        tag: items[n]["port"]
                                    })
                                }
                            }
                            return options
                        }

                        onOptionSelected: {
                            if (selectedItemTag !== "") {
                                var options = {
                                    port: selectedItemTag
                                }
                                updateInterface(model.index, options)
                            }
                        }
                    }

                    Label {
                        anchors.left: parent.left
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 10
                        font.pixelSize: 11
                        text: qsTr("MIDI Output Port")

                    }
                }
            }
        }
    }

    Component.onCompleted: {

    }
}



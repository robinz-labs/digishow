import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

MwInterfaceListView {

    id: interfaceListView

    interfaceType: "midi"

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
                        return model.port + " (" + getModeName(model.mode) + ")"
                    }

                    onClicked: {
                        menuMidiPort.selectOptionWithTag(model.port === undefined || model.port === "" ? "" : model.port+ "\t" + model.mode)
                        menuMidiPort.showOptions()
                    }

                    COptionMenu {
                        id: menuMidiPort

                        optionItems: {
                            var items = listOnline["midi"]
                            var options = [ { text: "", value: -1, tag: "" } ]
                            for (var n=0 ; n<items.length ; n++) {
                                options[n] = {
                                    text: items[n]["port"] + " (" + getModeName(items[n]["mode"]) + ")" ,
                                    value: n,
                                    tag: items[n]["port"] + "\t" + items[n]["mode"]
                                }
                            }
                            return options
                        }

                        onOptionSelected: {
                            if (selectedItemTag !== "") {
                                var items = selectedItemTag.split("\t")
                                var options = {
                                    port: items[0],
                                    mode: items[1]
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
                        text: qsTr("MIDI Port")

                    }
                }
            }
        }
    }

    Component.onCompleted: {

    }

    function getModeName(mode) {
        if      (mode === "input" ) return qsTr("Input")
        else if (mode === "output") return qsTr("Output")
        else return mode
    }
}



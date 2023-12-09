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
                    id: buttonMode
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: buttonMidiPortIn.top
                    anchors.margins: 20
                    anchors.bottomMargin: 36
                    text: menuMode.findOptionTextByTag(model.mode)

                    onClicked: {
                        menuMode.selectOptionWithTag(model.mode)
                        menuMode.showOptions()
                    }

                    COptionMenu {
                        id: menuMode

                        optionItems: [
                            { text: qsTr("Input"),  value: 0, tag: "input" },
                            { text: qsTr("Output"), value: 1, tag: "output" }
                        ]

                        onOptionClicked: {
                            var options = { mode: selectedItemTag, port: "" }
                            updateInterface(model.index, options)
                        }
                    }

                    Label {
                        anchors.left: parent.left
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 10
                        font.pixelSize: 11
                        text: qsTr("Mode")
                    }
                }


                COptionButton {
                    id: buttonMidiPortIn
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.margins: 20

                    visible: model.mode === "input"

                    text: {
                        if (model.port === undefined || model.port === "" || model.mode !== "input") return ""
                        return model.port
                    }

                    onClicked: {
                        menuMidiPortIn.selectOptionWithTag(model.port === undefined || model.port === "" ? "" : model.port)
                        menuMidiPortIn.showOptions()
                    }

                    COptionMenu {
                        id: menuMidiPortIn

                        optionItems: {
                            var items = listOnline["midi"]
                            var options = []
                            for (var n=0 ; n<items.length ; n++) {
                                if (items[n]["mode"] === "input") {
                                    options.push({ text: items[n]["port"], value: n, tag: items[n]["port"] })
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
                        text: qsTr("MIDI Port")

                    }
                }

                COptionButton {
                    id: buttonMidiPortOut
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.margins: 20

                    visible: model.mode === "output"

                    text: {
                        if (model.port === undefined || model.port === "" || model.mode !== "output") return ""
                        return model.port
                    }

                    onClicked: {
                        menuMidiPortIn.selectOptionWithTag(model.port === undefined || model.port === "" ? "" : model.port)
                        menuMidiPortIn.showOptions()
                    }

                    COptionMenu {
                        id: menuMidiPortOut

                        optionItems: {
                            var items = listOnline["midi"]
                            var options = []
                            for (var n=0 ; n<items.length ; n++) {
                                if (items[n]["mode"] === "output") {
                                    options.push({ text: items[n]["port"], value: n, tag: items[n]["port"] })
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
                        text: qsTr("MIDI Port")

                    }
                }

            }
        }
    }

    Component.onCompleted: {

    }
}



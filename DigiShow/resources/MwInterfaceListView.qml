import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

Item {

    id: interfaceListView

    property string interfaceType: ""
    property alias gridView: gridView
    property alias delegate: gridView.delegate
    property ListModel dataModel: ListModel {

        ListElement {
            name: "_new"
            type: ""
            mode: ""
        }
    }

    MwInterfaceListGrid {

        id: gridView
        model: dataModel

        delegate: Rectangle {

            width: gridView.cellWidth
            height: gridView.cellHeight
            color: "transparent"

            MwInterfaceListItem {

            }
        }
    }

    function refresh() {

        dataModel.clear()
        var interfaceCount = app.interfaceCount();
        for (var n=0 ; n<interfaceCount ; n++) {
            var interfaceOptions = digishow.getInterfaceConfiguration(n)["interfaceOptions"]
            if (interfaceOptions["type"] === interfaceType) {
                dataModel.append(interfaceOptions)
            }
        }
        var itemAddNew = { name: "_new" }
        dataModel.append(itemAddNew)
    }

    function addNewInterface() {
        var i = app.newInterface(interfaceType)
        var interfaceOptions = digishow.getInterfaceConfiguration(i)["interfaceOptions"]
        dataModel.insert(dataModel.count - 1, interfaceOptions)
        gridView.currentIndex = dataModel.count - 2
    }

    function deleteInterface(index) {
        var interfaceName = dataModel.get(index).name;
        var interfaceIndex = digishow.findInterfaceWithName(interfaceName)
        if (interfaceIndex === -1) return

        if (app.deleteInterface(interfaceIndex)) {
            gridView.currentIndex = -1
            dataModel.remove(index)
        } else {
            messageBox.show(qsTr("Unable to delete the interface that is employed."), qsTr("OK"))
        }
    }

    function updateInterface(index, options) {

        var interfaceName = dataModel.get(index).name;
        var interfaceIndex = digishow.findInterfaceWithName(interfaceName)
        if (interfaceIndex === -1) return

        var keys = Object.keys(options)
        for (var n=0 ; n<keys.length ; n++) {
            var key = keys[n]
            var val = options[key]
            app.interfaceAt(interfaceIndex).setInterfaceOption(key, val)
            if (val !== undefined) {
                dataModel.setProperty(index, key, val)
            } else {
                var properties = dataModel.get(index)
                delete properties[key]
                dataModel.set(index, properties)
            }

        }
        app.interfaceAt(interfaceIndex).updateMetadata()
    }
}



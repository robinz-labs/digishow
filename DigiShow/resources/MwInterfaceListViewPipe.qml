import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

MwInterfaceListView {

    id: interfaceListView

    interfaceType: "pipe"

    dataModel: ListModel {

       ListElement {
           name: "_new"
           type: ""
           mode: ""
           comment: ""
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

                CTextInputBox {
                    id: textComment
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.margins: 20
                    text: model.comment === undefined ? "" : model.comment

                    onEditingFinished: {
                        var options = { comment: text }
                        updateInterface(model.index, options)
                    }

                    Label {
                        anchors.left: parent.left
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 10
                        font.pixelSize: 11
                        text: qsTr("Comment")
                    }
                }
            }
        }
    }

    Component.onCompleted: {

    }
}



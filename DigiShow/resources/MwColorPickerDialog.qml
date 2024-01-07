import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"
import "components/CColor.js" as CColor

Dialog {
    id: dialog

    property color color: "#000000"

    width: 400
    height: 400
    anchors.centerIn: parent
    modal: true
    focus: true

    background: Rectangle {
        anchors.fill: parent
        color: "#333333"
        radius: 8
        border.width: 2
        border.color: "#cccccc"
    }

    // @disable-check M16
    Overlay.modal: Rectangle {
        color: "#55000000"
    }

    ListModel {

        id: dataModel

        ListElement { color: "#000000" }
    }

    GridView {
        id: gridView

        model: dataModel
        anchors.fill: parent
        anchors.topMargin: 5
        cellWidth: width / 4
        cellHeight: height / 4
        clip: true
        focus: true

        delegate: Rectangle {

            width: gridView.cellWidth
            height: gridView.cellHeight
            color: "transparent"

            CButton {
                id: buttonLaunch

                anchors.fill: parent
                anchors.margins: 3
                colorNormal: model.color
                colorActivated: "#000000"
                box.border.color: "#ffffff"
                box.border.width: (dialog.color.toString() === model.color) || mouseOver ? 2 : 0
                box.radius: 8

                onClicked: {
                    dialog.color = model.color
                    dialog.close()
                    accept() // emit signal
                }
            }
        }
    }

    Component.onCompleted: {

        dataModel.clear()
        dataModel.append({ color: CColor.Cherry   })
        dataModel.append({ color: CColor.HotPink  })
        dataModel.append({ color: CColor.RosePink })
        dataModel.append({ color: CColor.Salmon   })
        dataModel.append({ color: CColor.Orange   })
        dataModel.append({ color: CColor.Curry    })
        dataModel.append({ color: CColor.Mint     })
        dataModel.append({ color: CColor.Lime     })
        dataModel.append({ color: CColor.Lake     })
        dataModel.append({ color: CColor.Sky      })
        dataModel.append({ color: CColor.Ocean    })
        dataModel.append({ color: CColor.Iris     })
        dataModel.append({ color: "#aaaaaa" })
        dataModel.append({ color: "#888888" })
        dataModel.append({ color: "#666666" })
        dataModel.append({ color: "#444444" })
    }
}

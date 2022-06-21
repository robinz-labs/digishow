import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

GridView {
    id: gridView
    height: 2*cellHeight
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    cellWidth: 300
    cellHeight: 210
    clip: true
    snapMode: ListView.SnapToItem
    focus: true

    ScrollBar.vertical: ScrollBar {
        width: 8
        policy: ScrollBar.AsNeeded
    }
}



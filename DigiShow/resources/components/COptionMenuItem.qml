import QtQuick 2.12
import QtQuick.Controls 2.12

MenuItem {

    property int index: 0

    signal itemSelected(int index)

    font.pixelSize: 12
    leftPadding: 8
    rightPadding: 4
    height: 28

    onTriggered: {

        itemSelected(index)
    }
}

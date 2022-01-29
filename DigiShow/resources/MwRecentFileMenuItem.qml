import QtQuick 2.12
import QtQuick.Controls 2.12

MenuItem {
    property string filepath: ""

    onTriggered: {
        window.fileOpen(filepath)
    }
}

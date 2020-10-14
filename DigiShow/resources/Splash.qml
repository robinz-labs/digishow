import QtQuick 2.12
import QtQuick.Window 2.12
import DigiShow 1.0

Window {
    id: splash
    width: 640
    height: 480
    color: "transparent"
    modality: Qt.ApplicationModal
    flags: Qt.SplashScreen

    Image {
        anchors.fill: parent
        source: "qrc:///images/background_about.png"
    }

    Timer {
        interval: 2000
        running: true
        repeat: false
        onTriggered: splash.hide()
    }

    Component.onCompleted: splash.showNormal()
}

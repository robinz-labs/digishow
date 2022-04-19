import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

Dialog {
    id: dialog

    width: 640
    height: 480
    anchors.centerIn: parent
    modal: false
    dim: true
    focus: true

    onVisibleChanged: {
        textSlogan.visible = false
        textSlogan.reset()
        timerSlogan.stop()
    }

    background: Image {
        anchors.fill: parent
        source: "qrc:///images/background_about.png"
    }

    // @disable-check M16
    Overlay.modeless: Rectangle {
        color: "#cc000000"

        MouseArea {
            anchors.fill: parent
            onClicked: dialog.close()
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (!timerSlogan.running) {
                textSlogan.scale = 10
                textSlogan.opacity = 0
                textSlogan.visible = true
                timerSlogan.start()
            }
        }
    }

    Text {
        anchors.top: parent.top
        anchors.topMargin: 60
        anchors.left: parent.left
        anchors.leftMargin: 45
        color: Material.accent
        font.bold: false
        font.pixelSize: 18
        text: digishow.appName() + (digishow.appExperimental() ? " +" : "" )
    }

    Text {
        id: textAppVersion
        anchors.top: parent.top
        anchors.topMargin: 110
        anchors.left: parent.left
        anchors.leftMargin: 45
        color: "#999999"
        lineHeight: 1.2
        font.pixelSize: 14
        text: qsTr("app version: ") + digishow.appVersion() + "\r\n" +
              qsTr("app build date: ") + digishow.appBuildDate() + "\r\n\r\n" +
              qsTr("qt version: ") + digishow.appQtVersion() + "\r\n" +
              qsTr("rtmidi version: ") + digishow.appRtMidiVersion()
    }

    Text {
        id: textSlogan
        anchors.top: textAppVersion.bottom
        anchors.bottom: textCopyright.top
        anchors.left: parent.left
        anchors.leftMargin: 45
        color: "#cccccc"
        lineHeight: 1.5
        verticalAlignment: Text.AlignVCenter
        font.bold: true
        font.pixelSize: 14
        scale: 1
        rotation: 0
        text: qsTr("Jam with All Things Digital")
        visible: false

        Behavior on color   { ColorAnimation  { duration: 300 } }
        Behavior on scale   { NumberAnimation { duration: 3000; easing.type: Easing.OutCubic } }
        Behavior on opacity { OpacityAnimator { duration: 3000; easing.type: Easing.OutCubic } }

        Timer {
            id: timerSlogan

            property int tick: 0

            interval: 500
            repeat: true
            running: false

            onRunningChanged: tick = 0

            onTriggered: {
                tick++
                if (tick < 6) {
                    textSlogan.color = Qt.rgba(Math.random(), Math.random(), Math.random())
                } else if (tick == 6) {
                    textSlogan.reset()
                } else if (tick > 12) {
                    stop()
                }
            }
        }

        function reset() {
            scale = 1
            rotation = 0
            color = "#cccccc"
            opacity = 1
        }
    }

    Text {
        id: textCopyright
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 45
        color: "#999999"
        lineHeight: 1.2
        font.pixelSize: 12
        text: qsTr("© 2020-2021 Robin Zhang & Labs") + (!digishow.appExperimental() ? "\r\n\r\nThe software is provided on an 'AS IS' \r\nBASIS, WITHOUT WARRANTIES \r\nOR CONDITIONS OF ANY KIND, \r\neither express or implied." : "")
    }

}

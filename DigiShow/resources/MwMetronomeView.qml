import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

Item {

    id: metronomeView

    property bool opened: height > 0

    width: 710
    height: 100

    Behavior on height { NumberAnimation { duration: 120 } }

    Rectangle {
        anchors.fill: parent
        color: "#181818"

        Rectangle {
            anchors.fill: parent
            anchors.topMargin: 20
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            color: "transparent"
            border.color: "#333333"
            border.width: 1
            radius: 3

            Text {
                id: textMetronome
                anchors.top: parent.top
                anchors.topMargin: 45
                anchors.left: parent.left
                anchors.right: buttonRun.left
                horizontalAlignment: Text.AlignHCenter
                color: "#dddddd"
                font.pixelSize: 12
                font.bold: true
                text: qsTr("Beat Maker")
            }

            CButton {
                id: buttonRun

                width: 30
                height: 28
                anchors.verticalCenter: textMetronome.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 100

                label.text: metronome.isRunning ? "ON" : "OFF"
                label.font.bold: true
                label.font.pixelSize: 9
                box.radius: 3
                colorNormal: metronome.isRunning ? Material.accent : "#666666"
                onClicked: {
                    metronome.isRunning = !metronome.isRunning
                    isModified = true
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.top
                    anchors.bottomMargin: 10
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("Run")
                }
            }

            CSpinBox {
                id: spinBPM

                width: 90
                anchors.verticalCenter: textMetronome.verticalCenter
                anchors.left: buttonRun.right
                anchors.leftMargin: 20

                from: 20
                to: 600
                stepSize: 1
                value: 120
                onValueModified: {
                    metronome.bpm = value
                    isModified = true
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.top
                    anchors.bottomMargin: 10
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("BPM")
                }

                Component.onCompleted: {
                    metronome.bpmChanged.connect(function() {
                        spinBPM.value = Math.round(metronome.bpm)
                    })
                }
            }

            CButton {
                id: buttonTap
                width: 45
                height: 28
                anchors.left: spinBPM.right
                anchors.leftMargin: 5
                anchors.verticalCenter: textMetronome.verticalCenter
                label.text: metronome.tapCount>0 ? metronome.tapCount : qsTr("Tap")
                label.font.bold: false
                label.font.pixelSize: 12
                box.radius: 3
                box.border.width: 1
                colorNormal: "transparent"
                colorActivated: metronome.tapCount>0 ? "darkRed" : "#383838"

                onClicked: metronome.tap()
            }

            Rectangle {
                id: rectBeats

                height: 28
                anchors.verticalCenter: textMetronome.verticalCenter
                anchors.left: buttonTap.right
                anchors.leftMargin: 20
                anchors.right: parent.right
                anchors.rightMargin: 217
                color: "transparent"
                border.color: "#383838"
                border.width: 1
                radius: 3

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.top
                    anchors.bottomMargin: 10
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("Beats")
                }

                Text {
                    anchors.right: parent.right
                    anchors.bottom: parent.top
                    anchors.bottomMargin: 10
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: !metronome.isRunning ? "" :
                          (Math.floor(metronome.beat / metronome.quantum) + 1).toString() + " : " +
                          (Math.floor(metronome.beat % metronome.quantum) + 1).toString()
                }

                Canvas {
                    id: canvasBeats

                    anchors.fill: parent

                    onPaint: {
                        var ctx = getContext('2d')
                        ctx.clearRect(0, 0, width, height)

                        // draw bar
                        if (metronome.isRunning) {
                            ctx.fillStyle = Material.accent
                            var w
                            if (metronome.quantum > 1)
                                w = width * (Math.floor(metronome.phase) + 1) / metronome.quantum
                            else
                                w = (Math.floor(metronome.beat) % 2 === 0 ? width : 0)
                            ctx.fillRect(3, 3, Math.min(Math.max(w - 3, 0), width - 6), height - 6)
                        }

                        // draw grid
                        ctx.strokeStyle = "#383838"
                        ctx.lineWidth = 1

                        ctx.beginPath()
                        var q = metronome.quantum
                        for (var n=0 ; n<q-1 ; n++) {
                            var x = (n + 1) * width / q
                            ctx.moveTo(x, 0)
                            ctx.lineTo(x, height)
                        }
                        ctx.stroke()
                    }
                }

                Component.onCompleted: {
                    metronome.beatChanged.connect(function() {
                        canvasBeats.requestPaint()
                    })

                    metronome.isRunningChanged.connect(function() {
                        canvasBeats.requestPaint()
                    })
                }
            }

            CSpinBox {
                id: spinQuantum

                width: 75
                anchors.verticalCenter: textMetronome.verticalCenter
                anchors.left: rectBeats.right
                anchors.leftMargin: 5

                from: 1
                to: 12
                stepSize: 1
                value: 4
                onValueModified: {
                    metronome.quantum = value
                    canvasBeats.requestPaint()
                    isModified = true
                }

                Component.onCompleted: {
                    metronome.quantumChanged.connect(function() {
                        spinQuantum.value = Math.round(metronome.quantum)
                        canvasBeats.requestPaint()
                    })
                }
            }

            CButton {
                id: buttonSound

                width: 30
                height: 28
                anchors.verticalCenter: textMetronome.verticalCenter
                anchors.left: spinQuantum.right
                anchors.leftMargin: 20
                label.text: metronome.isSoundEnabled ? "ON" : "OFF"
                label.font.bold: true
                label.font.pixelSize: 9
                box.radius: 3
                colorNormal: metronome.isSoundEnabled ? Material.accent : "#666666"
                onClicked: {
                    metronome.isSoundEnabled = !metronome.isSoundEnabled
                    isModified = true
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.top
                    anchors.bottomMargin: 10
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("Sound")
                }
            }

            CButton {
                id: buttonLink

                width: 30
                height: 28
                anchors.verticalCenter: textMetronome.verticalCenter
                anchors.left: buttonSound.right
                anchors.leftMargin: 20
                label.text: metronome.isLinkEnabled ? "ON" : "OFF"
                label.font.bold: true
                label.font.pixelSize: 9
                box.radius: 3
                colorNormal: metronome.isLinkEnabled ? Material.accent : "#666666"
                onClicked: {
                    metronome.isLinkEnabled = !metronome.isLinkEnabled
                    isModified = true
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.top
                    anchors.bottomMargin: 10
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("Link")

                    CButton {
                        width: 18
                        height: 18
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.right
                        anchors.leftMargin: 10
                        label.text: "?"
                        label.font.pixelSize: 11
                        label.font.bold: true
                        box.radius: 9

                        onClicked: {
                            if (messageBox.showAndWait(
                                    qsTr("Enable 'Link' to use Ableton's technology for synchronizing beats with other music / DJ apps and devices."),
                                    qsTr("OK"), qsTr("More Info")) === 2) {
                                Qt.openUrlExternally("https://www.ableton.com/en/link/")
                            }
                        }
                    }
                }
            }

        }
    }

    Component.onCompleted: {

    }

    function open() { height = 100 }

    function close() { height = 0 }

    function refresh() {

    }
}

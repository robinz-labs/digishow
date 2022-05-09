import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
//import QtAudioEngine 1.1
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
                anchors.leftMargin: 20
                color: "#dddddd"
                font.pixelSize: 12
                font.bold: true
                text: qsTr("Metronome")
            }

            CheckBox {
                id: checkRun

                height: 28
                anchors.verticalCenter: textMetronome.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 95
                checked: metronome.isRunning
                onClicked: {
                    metronome.isRunning = checked
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
                anchors.left: checkRun.right
                anchors.leftMargin: 15

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
                label.text: qsTr("Tap")
                label.font.bold: false
                label.font.pixelSize: 12
                box.radius: 3
                box.border.width: 1
                colorNormal: "transparent"
                colorActivated: "#383838"

                property var timestamps: ([])

                onClicked: {

                    var now = Date.now()

                    if (timestamps.length > 0) {
                        var lastTime = timestamps[timestamps.length-1]
                        if (now - lastTime > 3000) timestamps = []
                    }
                    timestamps.push(now)

                    if (timestamps.length > 2) {
                        var firstTime = timestamps[0]
                        var milliseconds = (now - firstTime) / (timestamps.length - 1)
                        var bpm = 60000 / milliseconds

                        metronome.bpm = Math.round(bpm)
                        isModified = true
                    }
                }
            }

            Rectangle {
                id: rectBeats

                height: 28
                anchors.verticalCenter: textMetronome.verticalCenter
                anchors.left: buttonTap.right
                anchors.leftMargin: 20
                anchors.right: parent.right
                anchors.rightMargin: 210
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

                            var w = width * (Math.floor(metronome.phase) + 1) / metronome.quantum

                            ctx.fillStyle = Material.accent
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

                /*
                AudioEngine {
                    id:audioengine

                    AudioSample {
                        name:"metro1"
                        source: "qrc:///sounds/metro1.wav"
                    }
                    AudioSample {
                        name:"metro2"
                        source: "qrc:///sounds/metro2.wav"
                    }

                    Sound {
                        name:"metro1"
                        PlayVariation {
                            sample:"metro1"
                        }
                    }
                    Sound {
                        name:"metro2"
                        PlayVariation {
                            sample:"metro2"
                        }
                    }
                }
                */

                Component.onCompleted: {
                    metronome.phaseChanged.connect(function() {
                        //if (metronome.isSoundEnabled) audioengine.sounds[ metronome.phase < 1 ? "metro1": "metro2" ].play();
                        canvasBeats.requestPaint()
                    })

                    metronome.isRunningChanged.connect(function() {
                        canvasBeats.requestPaint()
                    })
                }
            }

            CSpinBox {
                id: spinQuantum

                width: 65
                anchors.verticalCenter: textMetronome.verticalCenter
                anchors.left: rectBeats.right
                anchors.leftMargin: 5

                from: 1
                to: 32
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
                    })
                }
            }

            CheckBox {
                id: checkSound

                height: 28
                anchors.verticalCenter: textMetronome.verticalCenter
                anchors.left: spinQuantum.right
                anchors.leftMargin: 15
                checked: metronome.isSoundEnabled
                onClicked: {
                    metronome.isSoundEnabled = checked
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

            CheckBox {
                id: checkLink

                height: 28
                anchors.verticalCenter: textMetronome.verticalCenter
                anchors.left: checkSound.right
                anchors.leftMargin: 10
                checked: metronome.isLinkEnabled
                onClicked: {
                    metronome.isLinkEnabled = checked
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
                                    qsTr("Select the 'Link' checkbox to synchronize the beats with Ableton or other music apps running on your computer."),
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

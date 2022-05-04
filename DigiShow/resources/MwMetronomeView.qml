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
                checked: false

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

                from: 1
                to: 999
                value: 120
                stepSize: 1

                Text {
                    anchors.left: parent.left
                    anchors.bottom: parent.top
                    anchors.bottomMargin: 10
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("Tempo / BPM")
                }
            }

            CSpinBox {
                id: spinQuantum

                width: 90
                anchors.verticalCenter: textMetronome.verticalCenter
                anchors.left: spinBPM.right
                anchors.leftMargin: 20

                from: 1
                to: 32
                value: 4
                stepSize: 1
                onValueChanged: canvasBeats.requestPaint()

                Text {
                    anchors.left: parent.left
                    anchors.bottom: parent.top
                    anchors.bottomMargin: 10
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("Quantum")
                }
            }

            Rectangle {
                id: rectBeats

                height: 28
                anchors.verticalCenter: textMetronome.verticalCenter
                anchors.left: spinQuantum.right
                anchors.leftMargin: 20
                anchors.right: parent.right
                anchors.rightMargin: 140
                color: "transparent"
                border.color: "#333333"
                border.width: 1
                radius: 3

                Canvas {
                    id: canvasBeats

                    anchors.fill: parent

                    onPaint: {
                        var ctx = getContext('2d')

                        //ctx.fillStyle = "#333333"
                        //ctx.fillRect(0, 0, width, height)

                        ctx.clearRect(0, 0, width, height)

                        ctx.strokeStyle = "#333333"
                        ctx.lineWidth = 1

                        ctx.beginPath()
                        var q = spinQuantum.value
                        for (var n=0 ; n<q-1 ; n++) {
                            var x = (n + 1) * width / q
                            ctx.moveTo(x, 0)
                            ctx.lineTo(x, height)
                        }
                        ctx.stroke()
                    }

                }


                Text {
                    anchors.left: parent.left
                    anchors.bottom: parent.top
                    anchors.bottomMargin: 10
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("Beats")
                }
            }

            CheckBox {
                id: checkSound

                height: 28
                anchors.verticalCenter: textMetronome.verticalCenter
                anchors.left: rectBeats.right
                anchors.leftMargin: 15
                checked: false

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
                checked: true

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
                                    qsTr("Select the 'Link' checkbox to synchronize the beats with Ableton or other applications running on your computer."),
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

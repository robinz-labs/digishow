import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

Dialog {

    id: dialog

    width: 900
    height: 660
    anchors.centerIn: parent
    modal: true
    focus: true
    padding: 10

    background: Rectangle {
        anchors.fill: parent
        color: "#333333"
        radius: 5
    }

    // @disable-check M16
    Overlay.modal: Rectangle {
        color: "#cc000000"
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex
        interactive: false
        orientation: Qt.Vertical
        clip: true

        Rectangle {
            id: rectMidi
            color: "transparent"

            Label {
                anchors.left: parent.left
                anchors.leftMargin: 25
                anchors.top: parent.top
                anchors.topMargin: 10
                color: Material.accent
                font.bold: true
                font.pixelSize: 16
                text: qsTr("MIDI Interfaces")
            }

            Label {
                id: labelMidiInfo
                anchors.left: parent.left
                anchors.leftMargin: 25
                anchors.right: parent.right
                anchors.rightMargin: 25
                anchors.top: parent.top
                anchors.topMargin: 40
                wrapMode: Label.Wrap
                lineHeight: 1.3
                text: qsTr("MIDI interface is typically for connecting your digital musical instruments, controllers, sequencers, as well as Ableton Live or other digital audio workstation apps running on your computer.")
            }

            MwInterfaceListViewMidi {
                id: interfaceListViewMidi
                anchors.top: labelMidiInfo.bottom
                anchors.topMargin: 20
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.bottom: parent.bottom
                anchors.right: parent.right
            }
        }

        Rectangle {
            id: rectDmx
            color: "transparent"

            Label {
                anchors.left: parent.left
                anchors.leftMargin: 25
                anchors.top: parent.top
                anchors.topMargin: 10
                color: Material.accent
                font.bold: true
                font.pixelSize: 16
                text: qsTr("DMX Interfaces")
            }

            Label {
                id: labelDmxInfo
                anchors.left: parent.left
                anchors.leftMargin: 25
                anchors.right: parent.right
                anchors.rightMargin: 25
                anchors.top: parent.top
                anchors.topMargin: 40
                wrapMode: Label.Wrap
                lineHeight: 1.3
                text: qsTr("DMX interface is typically for connecting your lights and fixtures on the stage. An Enttec DMX USB Pro adapter or compatible is required.")
            }

            MwInterfaceListViewDmx {
                id: interfaceListViewDmx
                anchors.top: labelDmxInfo.bottom
                anchors.topMargin: 20
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.bottom: parent.bottom
                anchors.right: parent.right
            }

        }

        Rectangle {
            id: rectOsc
            color: "transparent"

            Label {
                anchors.left: parent.left
                anchors.leftMargin: 25
                anchors.top: parent.top
                anchors.topMargin: 10
                color: Material.accent
                font.bold: true
                font.pixelSize: 16
                text: qsTr("OSC Interfaces")
            }

            Label {
                id: labelOscInfo
                anchors.left: parent.left
                anchors.leftMargin: 25
                anchors.right: parent.right
                anchors.rightMargin: 25
                anchors.top: parent.top
                anchors.topMargin: 40
                wrapMode: Label.Wrap
                lineHeight: 1.3
                text: qsTr("OSC interface is typically for connecting your interactive media control and creation applications, such as TouchOSC, TouchDesigner, etc.")
            }

            MwInterfaceListViewOsc {
                id: interfaceListViewOsc
                anchors.top: labelOscInfo.bottom
                anchors.topMargin: 20
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.bottom: parent.bottom
                anchors.right: parent.right
            }

        }

        Rectangle {
            id: rectArtnet
            color: "transparent"

            Label {
                anchors.left: parent.left
                anchors.leftMargin: 25
                anchors.top: parent.top
                anchors.topMargin: 10
                color: Material.accent
                font.bold: true
                font.pixelSize: 16
                text: qsTr("ArtNet Interfaces")
            }

            Label {
                id: labelArtnetInfo
                anchors.left: parent.left
                anchors.leftMargin: 25
                anchors.right: parent.right
                anchors.rightMargin: 25
                anchors.top: parent.top
                anchors.topMargin: 40
                wrapMode: Label.Wrap
                lineHeight: 1.3
                text: qsTr("ArtNet interface is typically for connecting your show lights, such as LED strips or matrices, through an IP network.")
            }

            MwInterfaceListViewArtnet {
                id: interfaceListViewArtnet
                anchors.top: labelArtnetInfo.bottom
                anchors.topMargin: 20
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.bottom: parent.bottom
                anchors.right: parent.right
            }

        }

        Rectangle {
            id: rectModbus
            color: "transparent"

            Label {
                anchors.left: parent.left
                anchors.leftMargin: 25
                anchors.top: parent.top
                anchors.topMargin: 10
                color: Material.accent
                font.bold: true
                font.pixelSize: 16
                text: qsTr("Modbus Interfaces")
            }

            Label {
                id: labelModbusInfo
                anchors.left: parent.left
                anchors.leftMargin: 25
                anchors.right: parent.right
                anchors.rightMargin: 25
                anchors.top: parent.top
                anchors.topMargin: 40
                wrapMode: Label.Wrap
                lineHeight: 1.3
                text: qsTr("Modbus interface is typically for connecting your industrial automations, robots and machines.")
            }

            MwInterfaceListViewModbus {
                id: interfaceListViewModbus
                interfaceType: "modbus"
                anchors.top: labelModbusInfo.bottom
                anchors.topMargin: 20
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.bottom: parent.bottom
                anchors.right: parent.right
            }
        }

        Rectangle {
            id: rectRioc
            color: "transparent"

            Label {
                anchors.left: parent.left
                anchors.leftMargin: 25
                anchors.top: parent.top
                anchors.topMargin: 10
                color: Material.accent
                font.bold: true
                font.pixelSize: 16
                text: qsTr("Arduino IO Controls")
            }

            Label {
                id: labelRiocInfo
                anchors.left: parent.left
                anchors.leftMargin: 25
                anchors.right: parent.right
                anchors.rightMargin: 25
                anchors.top: parent.top
                anchors.topMargin: 40
                wrapMode: Label.Wrap
                lineHeight: 1.3
                text: qsTr("Arduino is an open source electronic controller for making homebrew instruments, lights or gadgets for your interactive shows. A freeware named RIOC is employed for remote IO controls by DigiShow, you need upload the RIOC sketch to your Arduino UNO or MEGA boards.")
            }

            MwInterfaceListViewRioc {
                id: interfaceListViewRioc
                anchors.top: labelRiocInfo.bottom
                anchors.topMargin: 20
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.bottom: parent.bottom
                anchors.right: parent.right
            }
        }

        Rectangle {
            id: rectHue
            color: "transparent"

            Label {
                anchors.left: parent.left
                anchors.leftMargin: 25
                anchors.top: parent.top
                anchors.topMargin: 10
                color: Material.accent
                font.bold: true
                font.pixelSize: 16
                text: qsTr("Philips Hue Smart Lights")
            }

            Label {
                id: labelHueInfo
                anchors.left: parent.left
                anchors.leftMargin: 25
                anchors.right: parent.right
                anchors.rightMargin: 25
                anchors.top: parent.top
                anchors.topMargin: 40
                wrapMode: Label.Wrap
                lineHeight: 1.3
                text: qsTr("DigiShow is able to connect Philips Hue smart home bridge and control lights wirelessly.")
            }

            MwInterfaceListViewHue {
                id: interfaceListViewHue
                anchors.top: labelHueInfo.bottom
                anchors.topMargin: 20
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.bottom: parent.bottom
                anchors.right: parent.right
            }
        }

        Rectangle {
            id: rectScreen
            color: "transparent"

            Label {
                anchors.left: parent.left
                anchors.leftMargin: 25
                anchors.top: parent.top
                anchors.topMargin: 10
                color: Material.accent
                font.bold: true
                font.pixelSize: 16
                text: qsTr("Screen Presentations")
            }

            Label {
                id: labelScreenInfo
                anchors.left: parent.left
                anchors.leftMargin: 25
                anchors.right: parent.right
                anchors.rightMargin: 25
                anchors.top: parent.top
                anchors.topMargin: 40
                wrapMode: Label.Wrap
                text: qsTr("DigiShow enables to accept control signals for presenting videos, pictures and web contents on multiple screens.")
            }

            MwInterfaceListViewScreen {
                id: interfaceListViewScreen
                interfaceType: "screen"
                anchors.top: labelScreenInfo.bottom
                anchors.topMargin: 20
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.bottom: parent.bottom
                anchors.right: parent.right
            }
        }

        Rectangle {
            id: rectPipe
            color: "transparent"

            Label {
                anchors.left: parent.left
                anchors.leftMargin: 25
                anchors.top: parent.top
                anchors.topMargin: 10
                color: Material.accent
                font.bold: true
                font.pixelSize: 16
                text: qsTr("Virtual Pipes")
            }

            Label {
                id: labelPipeInfo
                anchors.left: parent.left
                anchors.leftMargin: 25
                anchors.right: parent.right
                anchors.rightMargin: 25
                anchors.top: parent.top
                anchors.topMargin: 40
                wrapMode: Label.Wrap
                lineHeight: 1.3
                text: qsTr("DigiShow enables to set up virtual pipes to transfer and duplicate signals between separate links. Remote pipe can be used when need to transfer signals between several DigiShow LINK apps running on different computers.")
            }

            MwInterfaceListViewPipe {
                id: interfaceListViewPipe
                interfaceType: "pipe"
                anchors.top: labelPipeInfo.bottom
                anchors.topMargin: 20
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.bottom: parent.bottom
                anchors.right: parent.right
            }
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        background: Rectangle {
            anchors.fill: parent
            color: "#222222"
            radius: 5
        }

        TabButton {
            text: qsTr("MIDI")
            font.capitalization: Font.MixedCase
            icon.source: "qrc:///images/icon_interface_midi_white.png"
            icon.width: 48
            icon.height: 48
            display: AbstractButton.TextUnderIcon
        }
        TabButton {
            text: qsTr("DMX")
            font.capitalization: Font.MixedCase
            icon.source: "qrc:///images/icon_interface_dmx_white.png"
            icon.width: 48
            icon.height: 48
            display: AbstractButton.TextUnderIcon
        }
        TabButton {
            text: qsTr("OSC")
            font.capitalization: Font.MixedCase
            icon.source: "qrc:///images/icon_interface_osc_white.png"
            icon.width: 48
            icon.height: 48
            display: AbstractButton.TextUnderIcon
        }
        TabButton {
            text: qsTr("ArtNet")
            font.capitalization: Font.MixedCase
            icon.source: "qrc:///images/icon_interface_artnet_white.png"
            icon.width: 48
            icon.height: 48
            display: AbstractButton.TextUnderIcon
        }
        TabButton {
            text: qsTr("Modbus")
            font.capitalization: Font.MixedCase
            icon.source: "qrc:///images/icon_interface_modbus_white.png"
            icon.width: 48
            icon.height: 48
            display: AbstractButton.TextUnderIcon
        }
        TabButton {
            text: qsTr("Arduino")
            font.capitalization: Font.MixedCase
            icon.source: "qrc:///images/icon_interface_rioc_white.png"
            icon.width: 48
            icon.height: 48
            display: AbstractButton.TextUnderIcon
        }
        TabButton {
            text: qsTr("Hue")
            font.capitalization: Font.MixedCase
            icon.source: "qrc:///images/icon_interface_hue_white.png"
            icon.width: 48
            icon.height: 48
            display: AbstractButton.TextUnderIcon
        }
        TabButton {
            text: qsTr("Screen")
            font.capitalization: Font.MixedCase
            icon.source: "qrc:///images/icon_interface_screen_white.png"
            icon.width: 48
            icon.height: 48
            display: AbstractButton.TextUnderIcon
        }
        TabButton {
            text: qsTr("Virtual Pipe")
            font.capitalization: Font.MixedCase
            icon.source: "qrc:///images/icon_interface_pipe_white.png"
            icon.width: 48
            icon.height: 48
            display: AbstractButton.TextUnderIcon
        }
    }

    CButton {
        width: 28
        height: 28
        anchors.top: parent.top
        anchors.topMargin: -30
        anchors.right: parent.right
        anchors.rightMargin: -20
        icon.width: 24
        icon.height: 24
        icon.source: "qrc:///images/icon_close_white.png"
        box.radius: 14
        box.border.width: 2
        box.border.color: "#cccccc"
        colorNormal: "#000000"

        onClicked: dialog.close()
    }

    Component.onCompleted: {
        swipeView.onCurrentIndexChanged()
    }

    function refresh() {

        interfaceListViewMidi.refresh()
        interfaceListViewDmx.refresh()
        interfaceListViewOsc.refresh()
        interfaceListViewArtnet.refresh()
        interfaceListViewModbus.refresh()
        interfaceListViewRioc.refresh()
        interfaceListViewHue.refresh()
        interfaceListViewScreen.refresh()
        interfaceListViewPipe.refresh()
    }

    function show() {
        showTab(0)
    }

    function showTab(index) {
        app.stop()

        listOnline = digishow.listOnline()

        tabBar.currentIndex = index
        dialog.open()

        slotListView.highlightedIndex = -1
        slotDetailView.refresh()

        window.isModified = true
    }
}

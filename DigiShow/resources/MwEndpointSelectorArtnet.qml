import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.0

import DigiShow 1.0

import "components"

Item {
    id: itemArtnet

    property alias spinUnit: spinArtnetUnit
    property alias spinChannel: spinArtnetChannel
    property alias menuType: menuArtnetType
    property alias menuMediaControl: menuMediaControl
    property alias textMediaUrl: textMediaUrl

    property bool forMedia: menuArtnetType.selectedItemValue === DigishowEnvironment.EndpointArtnetMedia

    COptionButton {
        id: buttonArtnetType
        width: 100
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: menuArtnetType.selectedItemText
        onClicked: menuArtnetType.showOptions()

        COptionMenu {
            id: menuArtnetType

            onOptionSelected: refreshMoreOptions()
        }
    }

    COptionButton {
        id: buttonArtnetUnit
        width: 100
        height: 28
        anchors.left: buttonArtnetType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: qsTr("Universe") + " " + spinArtnetUnit.value
        visible: !forMedia
        onClicked: spinArtnetUnit.visible = true

        COptionSpinBox {
            id: spinArtnetUnit
        }
    }

    COptionButton {
        id: buttonArtnetChannel
        width: 100
        height: 28
        anchors.left: buttonArtnetUnit.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: qsTr("Channel") + " " + spinArtnetChannel.value
        visible: !forMedia
        onClicked: spinArtnetChannel.visible = true

        COptionSpinBox {
            id: spinArtnetChannel
        }
    }

    COptionButton {
        id: buttonMediaControl
        width: 100
        height: 28
        anchors.left: buttonArtnetType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuMediaControl.selectedItemText
        visible: forMedia
        onClicked: menuMediaControl.showOptions()

        COptionMenu {
            id: menuMediaControl

            onOptionSelected: refreshMoreOptions()
        }
    }

    CButton {
        id: buttonMediaSelect
        width: 70
        height: 28
        anchors.left: buttonMediaControl.right
        anchors.leftMargin: 20
        anchors.top: parent.top
        label.font.bold: false
        label.font.pixelSize: 11
        label.text: qsTr("File ...")
        box.radius: 3
        visible: textMediaUrl.visible

        onClicked: {
            dialogSelectFile.setDefaultFileUrl(textMediaUrl.text)
            dialogSelectFile.open()
        }
    }

    CButton {
        id: buttonMediaOptions
        width: 70
        height: 28
        anchors.left: buttonMediaSelect.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        label.font.bold: false
        label.font.pixelSize: 11
        label.text: qsTr("Options ...")
        box.radius: 3
        visible: textMediaUrl.visible &&
                 menuMediaControl.selectedItemValue === DigishowEnvironment.ControlMediaStart

        onClicked: popupMediaOptions.open()
    }

    CTextInputBox {

        id: textMediaUrl

        anchors.bottom: parent.top
        anchors.bottomMargin: 10
        anchors.left: buttonMediaSelect.left
        anchors.right: parent.right
        anchors.rightMargin: 38
        text: "file://"
        //input.anchors.rightMargin: 30
        visible: forMedia &&
                 menuMediaControl.selectedItemValue !== DigishowEnvironment.ControlMediaStopAll

        onTextEdited: isModified = true
        onEditingFinished: if (text === "") text = "file://"
    }

    MwFileDialog {
        id: dialogSelectFile
        title: qsTr("Select Video File")
        folder: shortcuts.home
        selectExisting: true
        nameFilters: [ qsTr("Video files") + " (*.avi *.mp4 *.mov)",
                       qsTr("Image files") + " (*.bmp *.png *.jpg)",
                       qsTr("Image Sequence") + " (*.ini)",
                       qsTr("All files") + " (*)" ]
        onAccepted: {
            console.log("select video file: ", fileUrl)
            textMediaUrl.text = fileUrl
            isModified = true
        }
    }

    Popup {
        id: popupMediaOptions

        width: 280
        height: 620
        x: 100
        y: -height-10
        transformOrigin: Popup.BottomRight
        modal: true
        focus: true

        //onVisibleChanged: if (!visible) isModified = true

        background: Rectangle {
            anchors.fill: parent
            color: "#333333"
            radius: 5
            border.width: 1
            border.color: "#999999"
        }

        // @disable-check M16
        Overlay.modal: Item {}

        Column {
            anchors.fill: parent
            anchors.leftMargin: 16
            spacing: 10

            Text {
                height: 30
                verticalAlignment: Text.AlignBottom
                color: "#cccccc"
                font.pixelSize: 12
                font.bold: true
                text: qsTr("Pixel Mapping")
            }

            COptionButton {
                id: buttonMediaPixelMode
                width: 120
                height: 28
                anchors.left: parent.left
                anchors.leftMargin: 105
                text: menuMediaPixelMode.selectedItemText
                focus: true
                onClicked: {
                    menuMediaPixelMode.showOptions()
                    isModified = true
                }

                COptionMenu {
                    id: menuMediaPixelMode

                    onOptionClicked: {
                        if (selectedItemTag === "mono" && spinMediaPixelCount.value === 170)
                            spinMediaPixelCount.value = 512
                        else if (selectedItemTag !== "mono" && spinMediaPixelCount.value === 512)
                            spinMediaPixelCount.value = 170
                    }
                }

                Text {
                    anchors.right: parent.left
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("Pixel Mode")
                }
            }

            CSpinBox {
                id: spinMediaPixelCount

                width: 120
                anchors.left: parent.left
                anchors.leftMargin: 105
                from: 1
                to: 99999999
                value: 170
                stepSize: 1

                onValueModified: isModified = true

                Text {
                    anchors.right: parent.left
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("Pixel Count")
                }
            }

            CSpinBox {
                id: spinMediaPixelOffset

                width: 120
                anchors.left: parent.left
                anchors.leftMargin: 105
                from: 1
                to: 99999999
                value: 1
                stepSize: 1

                onValueModified: isModified = true

                Text {
                    anchors.right: parent.left
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("From Pixel")
                }
            }

            CSpinBox {
                id: spinMediaUnit

                width: 120
                anchors.left: parent.left
                anchors.leftMargin: 105
                from: spinArtnetUnit.from
                to: spinArtnetUnit.to
                value: spinArtnetUnit.value
                stepSize: spinArtnetUnit.stepSize

                onValueModified: spinArtnetUnit.value = spinMediaUnit.value

                Text {
                    anchors.right: parent.left
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("To Universe")
                }
            }

            CSpinBox {
                id: spinMediaChannel

                width: 120
                anchors.left: parent.left
                anchors.leftMargin: 105
                from: spinArtnetChannel.from
                to: spinArtnetChannel.to
                value: spinArtnetChannel.value
                stepSize: spinArtnetChannel.stepSize

                onValueModified: spinArtnetChannel.value = spinMediaChannel.value

                Text {
                    anchors.right: parent.left
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("To Channel")
                }
            }

            Text {
                height: 30
                verticalAlignment: Text.AlignBottom
                color: "#cccccc"
                font.pixelSize: 12
                font.bold: true
                text: qsTr("Playback Options")
            }

            CheckBox {
                id: checkMediaAlone

                height: 28
                anchors.left: parent.left
                anchors.leftMargin: 105
                padding: 0
                checked: true

                onClicked: isModified = true

                Text {
                    anchors.right: parent.left
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("Play Alone")
                }
            }

            CSpinBox {
                id: spinMediaFadeIn

                width: 120
                anchors.left: parent.left
                anchors.leftMargin: 105
                from: 0
                to: 5000
                value: 300
                stepSize: 10
                unit: "ms"

                onValueModified: isModified = true

                Text {
                    anchors.right: parent.left
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("Fade In")
                }
            }

            CSpinBox {
                id: spinMediaVolume

                width: 120
                anchors.left: parent.left
                anchors.leftMargin: 105
                from: 0
                to: 100
                value: 100
                stepSize: 5
                unit: "%"

                onValueModified: isModified = true

                Text {
                    anchors.right: parent.left
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("Volume")
                }
            }

            CSpinBox {
                id: spinMediaSpeed

                width: 120
                anchors.left: parent.left
                anchors.leftMargin: 105
                from: 10
                to: 400
                value: 100
                stepSize: 5
                unit: qsTr("%")

                onValueModified: isModified = true

                Text {
                    anchors.right: parent.left
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("Speed")
                }
            }

            CSpinBox {
                id: spinMediaPosition

                width: 120
                anchors.left: parent.left
                anchors.leftMargin: 105
                from: 0
                to: 99999000
                value: 0
                stepSize: 1000
                unit: qsTr("ms")

                onValueModified: isModified = true

                Text {
                    anchors.right: parent.left
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("Position")
                }
            }

            CSpinBox {
                id: spinMediaDuration

                width: 120
                anchors.left: parent.left
                anchors.leftMargin: 105
                from: 0
                to: 99999999
                value: 0
                stepSize: 1000
                unit: qsTr("ms")

                onValueModified: isModified = true

                Text {
                    anchors.right: parent.left
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("Duration")
                }
            }

            CheckBox {
                id: checkMediaRepeat

                height: 28
                anchors.left: parent.left
                anchors.leftMargin: 105
                padding: 0
                checked: false

                onClicked: isModified = true

                Text {
                    anchors.right: parent.left
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("Repeat")
                }
            }

            Item {
                width: 5
                height: 5
            }

            CButton {
                width: 100
                height: 28
                anchors.left: parent.left
                anchors.leftMargin: 105
                label.font.bold: false
                label.font.pixelSize: 11
                label.text: qsTr("Defaults")
                box.radius: 3

                onClicked: {

                    spinArtnetUnit.value = 0
                    spinArtnetChannel.value = 1

                    setEndpointMediaOptions({})
                    isModified = true
                }
            }

        }

        Component.onCompleted: {

        }
    }


    function refresh() {

        var items
        var n
        var v

        // init artnet universe option spinbox
        spinArtnetUnit.from = 0
        spinArtnetUnit.to = 32767
        spinArtnetUnit.visible = false

        // init artnet channel option spinbox
        spinArtnetChannel.from = 1
        spinArtnetChannel.to = 512
        spinArtnetChannel.visible = false

        // init artnet type option menu
        if (menuArtnetType.count === 0) {
            items = []
            items.push({ text: qsTr("Dimmer"), value: DigishowEnvironment.EndpointArtnetDimmer, tag: "dimmer" })
            items.push({ text: qsTr("Pixels"), value: DigishowEnvironment.EndpointArtnetMedia, tag: "media" })

            menuArtnetType.optionItems = items
            menuArtnetType.selectedIndex = 0
        }

        // init media control option menu
        if (menuMediaControl.count === 0) {
            items = []

            v = DigishowEnvironment.ControlMediaStart;   items.push({ text: digishow.getMediaControlName(v), value: v })
            v = DigishowEnvironment.ControlMediaStop;    items.push({ text: digishow.getMediaControlName(v), value: v })
            v = DigishowEnvironment.ControlMediaStopAll; items.push({ text: digishow.getMediaControlName(v), value: v })

            menuMediaControl.optionItems = items
            menuMediaControl.selectedIndex = 0
        }

        // init media pixel mode option menu
        if (menuMediaPixelMode.count === 0) {
            items = []

            items.push({ text: qsTr("Mono"), value: 1,  tag: "mono" })
            items.push({ text: qsTr("RGB"),  value: 2,  tag: "rgb" })
            items.push({ text: qsTr("RBG"),  value: 3,  tag: "rbg" })
            items.push({ text: qsTr("GRB"),  value: 4,  tag: "grb" })
            items.push({ text: qsTr("GBR"),  value: 5,  tag: "gbr" })
            items.push({ text: qsTr("BRG"),  value: 6,  tag: "brg" })
            items.push({ text: qsTr("BGR"),  value: 7,  tag: "bgr" })

            menuMediaPixelMode.optionItems = items
            menuMediaPixelMode.selectedIndex = 1
        }

        // init more options
        refreshMoreOptions()
    }

    function setEndpointMediaOptions(options) {

        var v
        v = options["mediaAlone"];    checkMediaAlone.checked  = (v === undefined ? true  : v )
        v = options["mediaFadeIn"];   spinMediaFadeIn.value    = (v === undefined ? 300   : v )
        v = options["mediaVolume"];   spinMediaVolume.value    = (v === undefined ? 100   : v / 100)
        v = options["mediaSpeed"];    spinMediaSpeed.value     = (v === undefined ? 100   : v / 100)
        v = options["mediaPosition"]; spinMediaPosition.value  = (v === undefined ? 0     : v )
        v = options["mediaDuration"]; spinMediaDuration.value  = (v === undefined ? 0     : v )
        v = options["mediaRepeat"];   checkMediaRepeat.checked = (v === undefined ? false : v )

        v = options["mediaPixelMode"];   menuMediaPixelMode.selectOptionWithTag(v === undefined ? "rgb" : v )
        v = options["mediaPixelCount"];  spinMediaPixelCount.value  = (v === undefined ? 170 : v )
        v = options["mediaPixelOffset"]; spinMediaPixelOffset.value = (v === undefined ? 1   : v + 1 )
    }

    function getEndpointMediaOptions() {

        var options = {
            mediaAlone:    checkMediaAlone.checked,
            mediaFadeIn:   spinMediaFadeIn.value,
            mediaVolume:   spinMediaVolume.value * 100,
            mediaSpeed:    spinMediaSpeed.value * 100,
            mediaPosition: spinMediaPosition.value,
            mediaDuration: spinMediaDuration.value,
            mediaRepeat:   checkMediaRepeat.checked,

            mediaPixelMode:   menuMediaPixelMode.selectedItemTag,
            mediaPixelCount:  spinMediaPixelCount.value,
            mediaPixelOffset: spinMediaPixelOffset.value - 1
        }

        return options
    }

    function refreshMoreOptions() {

        var endpointType = menuArtnetType.selectedItemValue
        var enables = {}

        if (endpointType === DigishowEnvironment.EndpointArtnetDimmer) {

            enables["optInitialDmx"] = true

        } else if (endpointType === DigishowEnvironment.EndpointArtnetMedia) {

            enables["optInitialB"] = true
        }

        moreOptions.resetOptions()
        moreOptions.enableOptions(enables)
        buttonMoreOptions.visible = (Object.keys(enables).length > 0)
    }

}


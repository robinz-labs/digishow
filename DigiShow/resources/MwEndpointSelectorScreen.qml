import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.0

import DigiShow 1.0

import "components"

Item {
    id: itemScreen

    property alias menuType:          menuScreenType
    property alias menuLightControl:  menuLightControl
    property alias menuMediaControl:  menuMediaControl
    property alias menuCanvasControl: menuCanvasControl
    property alias textMediaUrl:      textMediaUrl

    COptionButton {
        id: buttonScreenType
        width: 100
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: menuScreenType.selectedItemText
        onClicked: menuScreenType.showOptions()

        COptionMenu {
            id: menuScreenType

            onOptionSelected: refreshMoreOptions()
        }
    }

    COptionButton {
        id: buttonLightControl
        width: 100
        height: 28
        anchors.left: buttonScreenType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuLightControl.selectedItemText
        visible: menuScreenType.selectedItemValue === DigishowEnvironment.EndpointScreenLight
        onClicked: menuLightControl.showOptions()

        COptionMenu {
            id: menuLightControl

            onOptionSelected: refreshMoreOptions()
        }
    }

    COptionButton {
        id: buttonCanvasControl
        width: 100
        height: 28
        anchors.left: buttonScreenType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuCanvasControl.selectedItemText
        visible: menuScreenType.selectedItemValue === DigishowEnvironment.EndpointScreenCanvas
        onClicked: menuCanvasControl.showOptions()

        COptionMenu {
            id: menuCanvasControl

            onOptionSelected: refreshMoreOptions()
        }
    }

    COptionButton {
        id: buttonMediaControl
        width: 100
        height: 28
        anchors.left: buttonScreenType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuMediaControl.selectedItemText
        visible: menuScreenType.selectedItemValue === DigishowEnvironment.EndpointScreenMedia
        onClicked: menuMediaControl.showOptions()

        COptionMenu {
            id: menuMediaControl

            onOptionSelected: refreshMoreOptions()
        }
    }

    CButton {
        id: buttonMediaSelect
        width: 50
        height: 28
        anchors.left: buttonMediaControl.right
        anchors.leftMargin: 20
        anchors.top: parent.top
        label.font.bold: false
        label.font.pixelSize: 11
        label.text: qsTr("File ...")
        box.radius: 3
        visible: textMediaUrl.visible

        onClicked: dialogSelectFile.open()
    }

    CButton {
        id: buttonMediaOptions
        width: 50
        height: 28
        anchors.left: buttonMediaSelect.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        label.font.bold: false
        label.font.pixelSize: 11
        label.text: qsTr("Opt ...")
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
        input.anchors.rightMargin: 30
        visible: menuScreenType.selectedItemValue === DigishowEnvironment.EndpointScreenMedia &&
                 menuMediaControl.selectedItemValue !== DigishowEnvironment.ControlMediaStopAll

        onTextEdited: isModified = true
        onEditingFinished: if (text === "") text = "file://"
    }

    CButton {
        id: buttonMediaTip
        width: 20
        height: 20
        anchors.right: textMediaUrl.right
        anchors.rightMargin: 4
        anchors.verticalCenter: textMediaUrl.verticalCenter
        label.font.bold: false
        label.font.pixelSize: 11
        label.text: qsTr("?")
        box.radius: 3
        visible: textMediaUrl.visible

        onClicked: messageBox.show(
                       qsTr("Media Clip\r\nJPG, PNG, SVG pictures and MP4 videos, as well as any online or local web pages can be presented dynamically on the screen. Please enter the url of your online media file, or click button 'File...' to select a local media file.") ,
                       qsTr("OK"))

    }

    FileDialog {
        id: dialogSelectFile
        title: qsTr("Select Media File")
        folder: shortcuts.home
        selectExisting: true
        nameFilters: [ qsTr("Video and picture") + " (*.mp4 *.mov *.jpg *.png *.svg)",
                       qsTr("Web page file") +" (*.htm *.html)" ]
        onAccepted: {
            console.log("select media file: ", fileUrl)
            textMediaUrl.text = fileUrl
            isModified = true
        }
    }

    Popup {
        id: popupMediaOptions

        width: 280
        height: 655
        x: 60
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
                height: 25
                verticalAlignment: Text.AlignBottom
                color: "#cccccc"
                font.pixelSize: 12
                font.bold: true
                text: qsTr("Media Clip Display Options")
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
                    text: qsTr("Show Alone")
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
                id: spinMediaOpacity

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
                    text: qsTr("Opacity")
                }
            }

            CSpinBox {
                id: spinMediaScale

                width: 120
                anchors.left: parent.left
                anchors.leftMargin: 105
                from: 0
                to: 100
                value: 50
                stepSize: 5
                unit: "%"

                onValueModified: isModified = true

                Text {
                    anchors.right: parent.left
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("Scale")
                }
            }

            CSpinBox {
                id: spinMediaRotation

                width: 120
                anchors.left: parent.left
                anchors.leftMargin: 105
                from: 0
                to: 360
                value: 0
                stepSize: 1
                unit: "º"

                onValueModified: isModified = true

                Text {
                    anchors.right: parent.left
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("Rotation")
                }
            }

            CSpinBox {
                id: spinMediaXOffset

                width: 120
                anchors.left: parent.left
                anchors.leftMargin: 105
                from: 0
                to: 100
                value: 50
                stepSize: 1
                unit: "%"

                onValueModified: isModified = true

                Text {
                    anchors.right: parent.left
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("X Offset")
                }
            }

            CSpinBox {
                id: spinMediaYOffset

                width: 120
                anchors.left: parent.left
                anchors.leftMargin: 105
                from: 0
                to: 100
                value: 50
                stepSize: 1
                unit: "%"

                onValueModified: isModified = true

                Text {
                    anchors.right: parent.left
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("Y Offset")
                }
            }

            Text {
                height: 30
                verticalAlignment: Text.AlignBottom
                color: "#cccccc"
                font.pixelSize: 12
                font.bold: true
                text: qsTr("Options for Video Clip Only")
            }

            CheckBox {
                id: checkVideoRepeat

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
                    text: qsTr("Repeat")
                }
            }

            CSpinBox {
                id: spinVideoVolume

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
                id: spinVideoSpeed

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
                id: spinVideoPosition

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

            Text {
                height: 30
                verticalAlignment: Text.AlignBottom
                color: "#cccccc"
                font.pixelSize: 12
                font.bold: true
                text: qsTr("Options for Web Clip Only")
            }

            CTextInputBox {
                id: textWebJavascript

                width: 120
                anchors.left: parent.left
                anchors.leftMargin: 105
                text: ""

                onTextEdited: isModified = true

                Text {
                    anchors.right: parent.left
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#cccccc"
                    font.pixelSize: 12
                    text: qsTr("Run Java Script")
                }
            }

            Item {
                width: 5
                height: 5
            }

            CButton {
                width: 90
                height: 28
                anchors.left: parent.left
                anchors.leftMargin: 105
                label.font.bold: false
                label.font.pixelSize: 11
                label.text: qsTr("Defaults")
                box.radius: 3

                onClicked: {
                    setMediaOptions({})
                    isModified = true
                }
            }

        }

    }

    function refresh() {

        var items
        var n
        var v

        // init screen type option menu
        if (menuScreenType.count === 0) {
            items = []
            items.push({ text: qsTr("Backlight" ), value: DigishowEnvironment.EndpointScreenLight,  tag:"light" })
            items.push({ text: qsTr("Media Clip"), value: DigishowEnvironment.EndpointScreenMedia,  tag:"media" })
            items.push({ text: qsTr("Canvas"    ), value: DigishowEnvironment.EndpointScreenCanvas, tag:"canvas" })
            menuScreenType.optionItems = items
            menuScreenType.selectedIndex = 0
        }

        // init screen light control option menu
        if (menuLightControl.count === 0) {
            items = []
            v = DigishowEnvironment.ControlLightR; items.push({ text: digishow.getLightControlName(v), value: v })
            v = DigishowEnvironment.ControlLightG; items.push({ text: digishow.getLightControlName(v), value: v })
            v = DigishowEnvironment.ControlLightB; items.push({ text: digishow.getLightControlName(v), value: v })
            v = DigishowEnvironment.ControlLightW; items.push({ text: digishow.getLightControlName(v), value: v })
            menuLightControl.optionItems = items
            menuLightControl.selectedIndex = 0
        }

        // init screen canvas option menu
        if (menuCanvasControl.count === 0) {
            items = []

            v = DigishowEnvironment.ControlMediaOpacity;  items.push({ text: digishow.getMediaControlName(v), value: v })
            v = DigishowEnvironment.ControlMediaScale;    items.push({ text: digishow.getMediaControlName(v), value: v })
            v = DigishowEnvironment.ControlMediaRotation; items.push({ text: digishow.getMediaControlName(v), value: v })
            v = DigishowEnvironment.ControlMediaXOffset;  items.push({ text: digishow.getMediaControlName(v), value: v })
            v = DigishowEnvironment.ControlMediaYOffset;  items.push({ text: digishow.getMediaControlName(v), value: v })

            menuCanvasControl.optionItems = items
            menuCanvasControl.selectedIndex = 0
        }

        // init screen media control option menu
        if (menuMediaControl.count === 0) {
            items = []

            v = DigishowEnvironment.ControlMediaStart;    items.push({ text: digishow.getMediaControlName(v, true), value: v })
            v = DigishowEnvironment.ControlMediaStop;     items.push({ text: digishow.getMediaControlName(v, true), value: v })
            v = DigishowEnvironment.ControlMediaStopAll;  items.push({ text: digishow.getMediaControlName(v, true), value: v })

            items.push({ text: "-", value: -1 })

            v = DigishowEnvironment.ControlMediaOpacity;  items.push({ text: digishow.getMediaControlName(v), value: v })
            v = DigishowEnvironment.ControlMediaScale;    items.push({ text: digishow.getMediaControlName(v), value: v })
            v = DigishowEnvironment.ControlMediaRotation; items.push({ text: digishow.getMediaControlName(v), value: v })
            v = DigishowEnvironment.ControlMediaXOffset;  items.push({ text: digishow.getMediaControlName(v), value: v })
            v = DigishowEnvironment.ControlMediaYOffset;  items.push({ text: digishow.getMediaControlName(v), value: v })

            menuMediaControl.optionItems = items
            menuMediaControl.selectedIndex = 0
        }

        // init more options
        refreshMoreOptions()
    }

    function setMediaOptions(options) {

        var v
        v = options["mediaFadeIn"];        spinMediaFadeIn.value    = (v === undefined ? 300  : v )
        v = options["mediaOpacity"];       spinMediaOpacity.value   = (v === undefined ? 100  : v / 100)
        v = options["mediaScale"];         spinMediaScale.value     = (v === undefined ? 50   : v / 100)
        v = options["mediaRotation"];      spinMediaRotation.value  = (v === undefined ? 0    : v / 10 )
        v = options["mediaXOffset"];       spinMediaXOffset.value   = (v === undefined ? 50   : v / 100)
        v = options["mediaYOffset"];       spinMediaYOffset.value   = (v === undefined ? 50   : v / 100)

        // deprecated !!!
        v = options["mediaShowAlone"];     checkMediaAlone.checked  = (v === undefined ? true : v )
        v = options["mediaVideoRepeat"];   checkVideoRepeat.checked = (v === undefined ? true : v )
        v = options["mediaVideoVolume"];   spinVideoVolume.value    = (v === undefined ? 100  : v / 100)
        v = options["mediaVideoSpeed"];    spinVideoSpeed.value     = (v === undefined ? 100  : v / 100)
        v = options["mediaVideoPosition"]; spinVideoPosition.value  = (v === undefined ? 0    : v * 1000)
        v = options["mediaWebJavascript"]; textWebJavascript.text   = (v === undefined ? ""   : v )

        // replaced with
        v = options["mediaAlone"];         if (v !== undefined) checkMediaAlone.checked =  v
        v = options["mediaRepeat"];        if (v !== undefined) checkVideoRepeat.checked =  v
        v = options["mediaVolume"];        if (v !== undefined) spinVideoVolume.value = v / 100
        v = options["mediaSpeed"];         if (v !== undefined) spinVideoSpeed.value = v / 100
        v = options["mediaPosition"];      if (v !== undefined) spinVideoPosition.value = v
        v = options["mediaScript"];        if (v !== undefined) textWebJavascript.text =  v
    }

    function getMediaOptions() {

        var options = {
            mediaAlone:     checkMediaAlone.checked,
            mediaFadeIn:    spinMediaFadeIn.value,
            mediaOpacity:   spinMediaOpacity.value * 100,
            mediaScale:     spinMediaScale.value * 100,
            mediaRotation:  spinMediaRotation.value * 10,
            mediaXOffset:   spinMediaXOffset.value * 100,
            mediaYOffset:   spinMediaYOffset.value * 100,
            mediaRepeat:    checkVideoRepeat.checked,
            mediaVolume:    spinVideoVolume.value * 100,
            mediaSpeed:     spinVideoSpeed.value * 100,
            mediaPosition:  spinVideoPosition.value,
            mediaScript:    textWebJavascript.text
        }

        return options
    }    

    function refreshMoreOptions() {

        var endpointType = menuScreenType.selectedItemValue
        var lightControl = menuLightControl.selectedItemValue
        var mediaControl = menuMediaControl.selectedItemValue
        var enables = {}

        if (endpointType === DigishowEnvironment.EndpointScreenLight) {

            enables["optInitialA"] = true

        } else if (endpointType === DigishowEnvironment.EndpointScreenMedia) {

            if (mediaControl === DigishowEnvironment.ControlMediaStart ||
                mediaControl === DigishowEnvironment.ControlMediaStop ||
                mediaControl === DigishowEnvironment.ControlMediaStopAll) {

                enables["optInitialB"] = true
            } else {
                enables["optInitialA"] = true
            }
        }

        moreOptions.resetOptions()
        moreOptions.enableOptions(enables)
        buttonMoreOptions.visible = (Object.keys(enables).length > 0)
    }

}


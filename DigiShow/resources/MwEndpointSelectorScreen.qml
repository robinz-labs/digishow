import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.0

import DigiShow 1.0

import "components"

Item {
    id: itemScreen

    COptionButton {
        id: buttonType
        width: 100
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: menuType.selectedItemText
        onClicked: menuType.showOptions()

        COptionMenu {
            id: menuType

            onOptionSelected: refreshMoreOptions()
        }
    }

    COptionButton {
        id: buttonLightControl
        width: 100
        height: 28
        anchors.left: buttonType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuLightControl.selectedItemText
        visible: menuType.selectedItemValue === DigishowEnvironment.EndpointScreenLight
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
        anchors.left: buttonType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuCanvasControl.selectedItemText
        visible: menuType.selectedItemValue === DigishowEnvironment.EndpointScreenCanvas
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
        anchors.left: buttonType.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuMediaControl.selectedItemText
        visible: menuType.selectedItemValue === DigishowEnvironment.EndpointScreenMedia
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
        visible: textMediaUrl.visible && (
                 menuMediaControl.selectedItemValue === DigishowEnvironment.ControlMediaStart ||
                 menuMediaControl.selectedItemValue === DigishowEnvironment.ControlMediaRestart)

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
        visible: menuType.selectedItemValue === DigishowEnvironment.EndpointScreenMedia &&
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

    MwFileDialog {
        id: dialogSelectFile
        title: qsTr("Select Media File")
        folder: shortcuts.home
        selectExisting: true
        nameFilters: [ qsTr("Video and picture") + " (*.mp4 *.mov *.png *.jpg *.jpeg *.bmp *.svg)",
                       qsTr("Web page file") +" (*.htm *.html)" ,
                       qsTr("All files") + " (*)" ]
        onAccepted: {
            console.log("select media file: ", fileUrl)
            textMediaUrl.text = fileUrl
            isModified = true
        }
    }

    Popup {
        id: popupMediaOptions

        width: 530
        height: 400
        x: -64
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

        CButton {
            width: 20
            height: 20
            anchors.top: parent.top
            anchors.topMargin: -8
            anchors.right: parent.right
            anchors.rightMargin: -8
            icon.width: 16
            icon.height: 16
            icon.source: "qrc:///images/icon_close_white.png"
            box.radius: 3
            colorNormal: "#484848"

            onClicked: popupMediaOptions.close()
        }

        Row {
            anchors.fill: parent
            anchors.leftMargin: 16
            spacing: 0

            Column {
                width: 250
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
            }

            Column {
                width: 250
                spacing: 10

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

                CheckBox {
                    id: checkVideoABRepeat

                    height: 28
                    anchors.left: parent.left
                    anchors.leftMargin: 105
                    padding: 0
                    checked: false

                    onClicked: {
                        if (!checked) {
                            spinVideoPositionA.value = 0
                            spinVideoPositionB.value = 0
                            isModified = true
                        }
                    }

                    Text {
                        anchors.right: parent.left
                        anchors.rightMargin: 15
                        anchors.verticalCenter: parent.verticalCenter
                        color: "#cccccc"
                        font.pixelSize: 12
                        text: qsTr("A-B Repeat")
                    }
                }

                CSpinBox {
                    id: spinVideoPositionA

                    width: 120
                    anchors.left: parent.left
                    anchors.leftMargin: 105
                    from: 0
                    to: 99999000
                    value: 0
                    stepSize: 1000
                    unit: qsTr("ms")

                    visible: checkVideoABRepeat.checked

                    onValueModified: isModified = true

                    Text {
                        anchors.right: parent.left
                        anchors.rightMargin: 15
                        anchors.verticalCenter: parent.verticalCenter
                        color: "#cccccc"
                        font.pixelSize: 12
                        text: qsTr("Starting Point A")
                    }
                }

                CSpinBox {
                    id: spinVideoPositionB

                    width: 120
                    anchors.left: parent.left
                    anchors.leftMargin: 105
                    from: 0
                    to: 99999000
                    value: 0
                    stepSize: 1000
                    unit: qsTr("ms")

                    visible: checkVideoABRepeat.checked

                    onValueModified: isModified = true

                    Text {
                        anchors.right: parent.left
                        anchors.rightMargin: 15
                        anchors.verticalCenter: parent.verticalCenter
                        color: "#cccccc"
                        font.pixelSize: 12
                        text: qsTr("Ending Point B")
                    }
                }

                Item {
                    width: 5
                    height: checkVideoABRepeat.checked ? 20 : 96
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
                        setEndpointMediaOptions({})
                        isModified = true
                    }
                }

            }

        }
    }

    function refresh() {

        var items
        var n
        var v

        // init screen type option menu
        if (menuType.count === 0) {
            items = []
            items.push({ text: qsTr("Backlight" ), value: DigishowEnvironment.EndpointScreenLight,  tag:"light" })
            items.push({ text: qsTr("Media Clip"), value: DigishowEnvironment.EndpointScreenMedia,  tag:"media" })
            items.push({ text: qsTr("Canvas"    ), value: DigishowEnvironment.EndpointScreenCanvas, tag:"canvas" })
            menuType.optionItems = items
            menuType.selectedIndex = 0
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
            v = DigishowEnvironment.ControlMediaRestart;  items.push({ text: digishow.getMediaControlName(v, true), value: v })
            v = DigishowEnvironment.ControlMediaStop;     items.push({ text: digishow.getMediaControlName(v, true), value: v })
            v = DigishowEnvironment.ControlMediaStopAll;  items.push({ text: digishow.getMediaControlName(v, true), value: v })

            items.push({ text: "-", value: -1 })

            v = DigishowEnvironment.ControlMediaOpacity;  items.push({ text: digishow.getMediaControlName(v), value: v })
            v = DigishowEnvironment.ControlMediaScale;    items.push({ text: digishow.getMediaControlName(v), value: v })
            v = DigishowEnvironment.ControlMediaRotation; items.push({ text: digishow.getMediaControlName(v), value: v })
            v = DigishowEnvironment.ControlMediaXOffset;  items.push({ text: digishow.getMediaControlName(v), value: v })
            v = DigishowEnvironment.ControlMediaYOffset;  items.push({ text: digishow.getMediaControlName(v), value: v })

            //if (digishow.appExperimental()) {
            items.push({ text: "-", value: -1 })
            v = DigishowEnvironment.ControlMediaVolume;   items.push({ text: digishow.getMediaControlName(v), value: v })
            v = DigishowEnvironment.ControlMediaSpeed;    items.push({ text: digishow.getMediaControlName(v), value: v })
            v = DigishowEnvironment.ControlMediaPosition; items.push({ text: digishow.getMediaControlName(v), value: v })
            //}

            menuMediaControl.optionItems = items
            menuMediaControl.selectedIndex = 1
        }

        // init more options
        refreshMoreOptions()
    }

    function refreshMoreOptions() {

        var endpointType = menuType.selectedItemValue
        var lightControl = menuLightControl.selectedItemValue
        var mediaControl = menuMediaControl.selectedItemValue
        var enables = {}

        if (endpointType === DigishowEnvironment.EndpointScreenLight ||
            endpointType === DigishowEnvironment.EndpointScreenCanvas) {

            enables["optInitialA"] = true

        } else if (endpointType === DigishowEnvironment.EndpointScreenMedia) {

            if (mediaControl === DigishowEnvironment.ControlMediaStart ||
                mediaControl === DigishowEnvironment.ControlMediaRestart ||
                mediaControl === DigishowEnvironment.ControlMediaStop ||
                mediaControl === DigishowEnvironment.ControlMediaStopAll) {

                enables["optInitialB"] = true

            } else if (mediaControl === DigishowEnvironment.ControlMediaPosition) {

                enables["optInitialA"] = true
                enables["optRangeMSec"] = true

            } else {

                enables["optInitialA"] = true
            }
        }

        popupMoreOptions.resetOptions()
        popupMoreOptions.enableOptions(enables)
        buttonMoreOptions.visible = (Object.keys(enables).length > 0)
    }

    function setEndpointMediaOptions(options) {

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

        v = options["mediaPositionA"];     spinVideoPositionA.value = (v === undefined ? 0    : v )
        v = options["mediaPositionB"];     spinVideoPositionB.value = (v === undefined ? 0    : v )

        checkVideoABRepeat.checked = (spinVideoPositionA.value>0 || spinVideoPositionB.value>0)
    }

    function getEndpointMediaOptions() {

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

        if (checkVideoABRepeat.checked) {
            options["mediaPositionA"] = spinVideoPositionA.value
            options["mediaPositionB"] = spinVideoPositionB.value
        } else {
            options["mediaPositionA"] = 0
            options["mediaPositionB"] = 0
        }

        return options
    }    

    function setEndpointOptions(endpointInfo, endpointOptions) {

        menuType.selectOption(endpointInfo["type"])

        switch (endpointInfo["type"]) {
        case DigishowEnvironment.EndpointScreenLight:
            menuLightControl.selectOption(endpointInfo["control"])
            setEndpointMediaOptions({}) // clear media options
            break
        case DigishowEnvironment.EndpointScreenCanvas:
            menuCanvasControl.selectOption(endpointInfo["control"])
            setEndpointMediaOptions({}) // clear media options
            break
        case DigishowEnvironment.EndpointScreenMedia:
            menuMediaControl.selectOption(endpointInfo["control"])

            var mediaName = endpointOptions["media"]
            var mediaIndex = digishow.findMediaWithName(interfaceIndex, mediaName)
            var mediaOptions = digishow.getMediaOptions(interfaceIndex, mediaIndex)
            var mediaUrl = "file://"
            if (mediaOptions["url"] !== undefined) mediaUrl = mediaOptions["url"]
            textMediaUrl.text = mediaUrl

            setEndpointMediaOptions(endpointOptions)

            break
        }
    }

    function getEndpointOptions() {

        var options = {}
        options["type"] = menuType.selectedItemTag

        switch (menuType.selectedItemValue) {
        case DigishowEnvironment.EndpointScreenLight:
            options["control"] = menuLightControl.selectedItemValue
            break
        case DigishowEnvironment.EndpointScreenCanvas:
            options["control"] = menuCanvasControl.selectedItemValue
            break
        case DigishowEnvironment.EndpointScreenMedia:
            options["control"] = menuMediaControl.selectedItemValue

            if (textMediaUrl.visible) {
                var interfaceIndex = menuInterface.selectedItemValue
                var mediaUrl = textMediaUrl.text
                var mediaType = digishow.getMediaType(mediaUrl)
                var mediaIndex = digishow.makeMedia(interfaceIndex, mediaUrl, mediaType)

                if (mediaIndex !== -1) {
                    options["media"] = digishow.getMediaName(interfaceIndex, mediaIndex)
                    if (menuMediaControl.selectedItemValue === DigishowEnvironment.ControlMediaStart ||
                        menuMediaControl.selectedItemValue === DigishowEnvironment.ControlMediaRestart)
                        options = utilities.merge(options, getEndpointMediaOptions())
                } else {
                    messageBox.show(qsTr("Please select a media clip file exists on your computer disks or enter a valid url of the media clip."), qsTr("OK"))
                }
            }
            break
        }

        return options
    }
}


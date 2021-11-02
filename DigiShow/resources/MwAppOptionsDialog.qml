import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

Dialog {
    id: dialog

    width: 360
    height: 280
    anchors.centerIn: parent
    modal: true
    focus: true

    background: Rectangle {
        anchors.fill: parent
        color: "#333333"
        radius: 5
        border.width: 2
        border.color: "#cccccc"
    }

    // @disable-check M16
    Overlay.modal: Rectangle {
        color: "#cc000000"
    }

    COptionButton {
        id: buttonOptionLanguage

        width: 185
        height: 28
        anchors.left: parent.left
        anchors.leftMargin: 100
        anchors.top: parent.top
        anchors.topMargin: 30
        text: menuOptionLanguage.selectedItemText
        onClicked: menuOptionLanguage.showOptions()

        Text {
            anchors.right: parent.left
            anchors.rightMargin: 15
            anchors.verticalCenter: parent.verticalCenter
            color: "#cccccc"
            font.pixelSize: 12
            text: qsTr("Language")
        }

        COptionMenu {
            id: menuOptionLanguage
            optionItems: [
                { value: 0, tag:"en",    text: "English"  },
                { value: 1, tag:"es",    text: "Español"  },
                { value: 2, tag:"jp",    text: "日本語" },
                { value: 3, tag:"zh_CN", text: "中文" }]
        }
    }

    COptionButton {
        id: buttonOptionScale

        width: 85
        height: 28
        anchors.left: parent.left
        anchors.leftMargin: 100
        anchors.top: buttonOptionLanguage.bottom
        anchors.topMargin: 20
        text: spinOptionScale.value + " %"
        onClicked: spinOptionScale.visible = true

        Text {
            anchors.right: parent.left
            anchors.rightMargin: 15
            anchors.verticalCenter: parent.verticalCenter
            color: "#cccccc"
            font.pixelSize: 12
            text: qsTr("Display Scale")
        }

        COptionSpinBox {
            id: spinOptionScale
            from: screen.devicePixelRatio >= 2 ? 50 : 100
            to: 200
            value: 100
            stepSize: 5
            visible: false
        }
    }

    CheckBox {
        id: checkOptionHiDPI
        height: 28
        anchors.left: buttonOptionScale.right
        anchors.leftMargin: 3
        anchors.verticalCenter: buttonOptionScale.verticalCenter
        checked: true

        Text {
            anchors.left: parent.right
            anchors.verticalCenter: parent.verticalCenter
            color: "#cccccc"
            font.pixelSize: 12
            text: qsTr("HiDPI")
        }

    }

    Text {
        id: textOptionHiDPI
        anchors.left: buttonOptionSave.left
        anchors.right: buttonOptionCancel.right
        anchors.top: buttonOptionScale.bottom
        anchors.topMargin: 15
        color: Material.accent
        wrapMode: Text.WordWrap
        font.pixelSize: 11
        text: qsTr("It is recommended to disable HiDPI mode for more display space.")
        visible:
            checkOptionHiDPI.checked &&
            !utilities.isMac() &&
            screen.devicePixelRatio >= 2 &&
            (screen.width < window.minimumWidth || screen.height < window.minimumHeight)
    }

    Image {
        width: 20
        height: 20
        anchors.right: textOptionHiDPI.right
        anchors.verticalCenter: checkOptionHiDPI.verticalCenter
        source: "qrc:///images/icon_attention.png"
        visible: textOptionHiDPI.visible
    }


    CButton {
        id: buttonOptionSave
        width: 85
        height: 32
        anchors.left: parent.left
        anchors.leftMargin: 100
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        label.font.bold: true
        label.font.pixelSize: 12
        label.text: qsTr("Save")
        box.radius: 3
        onClicked: {

            saveAppOptions()
            dialog.close()
            common.setTimeout(function() {
                messageBox.show(qsTr("The changes will take effect after restart."), qsTr("OK"))
            }, 200)
        }
    }

    CButton {
        id: buttonOptionCancel
        width: 85
        height: 32
        anchors.left: buttonOptionSave.right
        anchors.leftMargin: 15
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        label.font.bold: true
        label.font.pixelSize: 12
        label.text: qsTr("Cancel")
        box.radius: 3
        onClicked: {
            dialog.close()
        }
    }

    function show() {

        loadAppOptions()
        dialog.open()
    }

    function loadAppOptions() {

        var appOptions = digishow.getAppOptions()

        var language = appOptions["language"]
        if (language !== undefined) menuOptionLanguage.selectOptionWithTag(language)

        var scale = appOptions["scale"]
        if (scale !== undefined) spinOptionScale.value = scale * 100

        var hidpi = appOptions["hidpi"]
        if (hidpi !== undefined) checkOptionHiDPI.checked = hidpi
    }

    function saveAppOptions() {

        var appOptions = digishow.getAppOptions()
        appOptions["language"] = menuOptionLanguage.selectedItemTag
        appOptions["scale"] = spinOptionScale.value * 0.01
        appOptions["hidpi"] = checkOptionHiDPI.checked

        digishow.setAppOptions(appOptions)
    }

}

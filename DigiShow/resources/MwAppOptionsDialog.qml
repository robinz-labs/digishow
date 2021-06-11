import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import DigiShow 1.0

import "components"

Dialog {
    id: dialog

    width: 400
    height: 220
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
        color: "#55000000"
    }

    COptionButton {
        id: buttonOptionLanguage

        width: 150
        height: 28
        anchors.left: parent.left
        anchors.leftMargin: 120
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

    CButton {
        id: buttonOptionSave
        width: 65
        height: 32
        anchors.left: parent.left
        anchors.leftMargin: 120
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
                messageBox.show(qsTr("The language change will take effect after restart."), qsTr("OK"))
            }, 200)
        }
    }

    CButton {
        id: buttonOptionCancel
        width: 65
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
    }

    function saveAppOptions() {

        var appOptions = digishow.getAppOptions()
        appOptions["language"] = menuOptionLanguage.selectedItemTag

        digishow.setAppOptions(appOptions)
    }

}

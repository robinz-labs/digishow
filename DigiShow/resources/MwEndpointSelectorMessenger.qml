import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemMessenger

    COptionButton {
        id: buttonType
        width: 80
        height: 28
        anchors.left: parent.left
        anchors.top: parent.top
        text: menuType.selectedItemText
        onClicked: menuType.showOptions()

        COptionMenu {
            id: menuType
            onOptionSelected: refreshMoreOptions()
            onOptionClicked: {
                if (menuType.selectedItemValue == DigishowEnvironment.EndpointMessengerHexCode)
                    textMessage.text = utilities.txt2hex(textMessage.text)
                else
                    textMessage.text = utilities.hex2txt(textMessage.text)
            }
        }
    }

    CTextInputBox {
        id: textMessage
        anchors.left: buttonType.right
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: buttonSave.visible ? 70 : 0
        anchors.top: parent.top
        text: ""
        onTextEdited: isModified = true
    }

    CButton {
        id: buttonTip
        width: 20
        height: 20
        anchors.right: textMessage.right
        anchors.rightMargin: 4
        anchors.verticalCenter: textMessage.verticalCenter
        label.font.bold: false
        label.font.pixelSize: 11
        label.text: qsTr("?")
        box.radius: 3

        onClicked: messageBox.show(
                       qsTr("For selecting Text mode:\r\nThe entered text can contain escape sequences like: ") + "\\r \\n \\t \\x" + "\r\n\r\n" +
                       qsTr("For selecting Hex Code mode:\r\nYou can enter any hex code, for example: ") + "0A 1B 2C F3 E4",
                       qsTr("OK"))

    }

    function refresh() {

        var items
        var n

        // init message mode option menu
        if (menuType.count === 0) {
            items = []
            items.push({ text: qsTr("Text"    ), value: DigishowEnvironment.EndpointMessengerText   , tag:"text"})
            items.push({ text: qsTr("Hex Code"), value: DigishowEnvironment.EndpointMessengerHexCode, tag:"hex" })
            menuType.optionItems = items
            menuType.selectedIndex = 0
        }

        // init more options
        refreshMoreOptions()
    }

    function refreshMoreOptions() {

        var enables = {}
        enables["optInitialB"] = true

        popupMoreOptions.resetOptions()
        popupMoreOptions.enableOptions(enables)
        buttonMoreOptions.visible = (Object.keys(enables).length > 0)
    }

    function setEndpointOptions(endpointInfo, endpointOptions) {

        menuType.selectOption(endpointInfo.type)
        textMessage.text = endpointOptions["message"]
    }

    function getEndpointOptions() {

        if (menuType.selectedItemValue == DigishowEnvironment.EndpointMessengerHexCode)
            textMessage.text = validateAndFormatHex(textMessage.text)

        var options = {}
        options["type"] = menuType.selectedItemTag
        options["message"] = textMessage.text
        options["subscribed"] = forInput
        return options
    }

    function validateAndFormatHex(hexString) {
        // Remove all non-hexadecimal characters
        let cleanedHex = hexString.replace(/[^0-9a-fA-F]/g, "");

        // Ensure even length by padding with a zero if necessary
        if (cleanedHex.length % 2 !== 0) {
            cleanedHex = "0" + cleanedHex;
        }

        // Add space after every two characters
        let formattedHex = cleanedHex.match(/.{1,2}/g).join(" ");

        return formattedHex.toUpperCase(); // Convert to uppercase for uniformity
    }

    function learn(rawData) {

        textMessage.text = rawData["message"]
        menuType.selectOptionWithTag(rawData["type"])
        return true
    }

}

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12

Menu {

    id: menu

    property var optionItems: [] //[ { text: "", value: 0, tag:"" } ]
    property int selectedIndex: -1
    readonly property int    selectedItemValue: (selectedIndex === -1 || optionItems[selectedIndex].value === undefined) ? -1 : optionItems[selectedIndex].value
    readonly property string selectedItemTag:   (selectedIndex === -1 || optionItems[selectedIndex].tag   === undefined) ? "" : optionItems[selectedIndex].tag
    readonly property string selectedItemText:  (selectedIndex === -1 || optionItems[selectedIndex].text  === undefined) ? "" : optionItems[selectedIndex].text

    signal optionSelected(int value)

    width: parent.width
    height: Math.min(28*count + 2, 28*15+2)

    topPadding: 1
    bottomPadding: 1
    leftPadding: 1
    rightPadding: 1

    background: Rectangle {
        color: "#484848"
        radius: 3
        //border.color: "#666666"
        //border.width: 1
    }

    contentItem: ListView {

        model: menu.contentModel
        clip: true
        focus: true

        snapMode: ListView.SnapToItem

        highlightMoveVelocity: -1
        highlightMoveDuration: 0

        currentIndex: selectedIndex

        ScrollBar.vertical: ScrollBar {
            width: 8
            policy: ScrollBar.AsNeeded
        }
    }

    onOptionItemsChanged: {

        selectedIndex = -1

        // clear menu
        while(menu.count > 0)
            menu.removeItem(menu.itemAt(0));

        // add menu items
        for (var n=0 ; n< optionItems.length ; n++) {

            if (optionItems[n].text === "-") {

                var menuSeparator = Qt.createQmlObject("import QtQuick.Controls 2.12; MenuSeparator {}", this)
                addItem(menuSeparator)

            } else {

                var menuItem = Qt.createQmlObject("COptionMenuItem {}", this)
                menuItem.index = n
                menuItem.text = optionItems[n].text
                menuItem.itemSelected.connect(function(index) {
                    selectedIndex = index
                    optionSelected(optionItems[selectedIndex].value) // emit signal

                    common.setAncestorProperty(menu, "isModified", true)
                })
                addItem(menuItem)

                if (optionItems[n].selected) selectedIndex = n
            }
        }
    }

    function showOptions() {

        if (count === 0) return

        if (visible) {
            close()
        } else {
            currentIndex = selectedIndex

            var window = parent.Window
            var bottomInWindow = window.height - parent.mapToItem(window.contentItem, 0, parent.height).y
            if (bottomInWindow > menu.height + 10) {
                menu.transformOrigin = Menu.TopLeft
                popup(0, parent.height + 1)
            } else {
                menu.transformOrigin = Menu.BottomLeft
                popup(0, -menu.height - 1)
            }

            //if (selectedIndex === -1)
            //    popup(0, 0)
            //else
            //    popup(0, 0, itemAt(selectedIndex))
        }

    }

    function selectOption(value) {

        if (value !== -1) {
            for (var n=0 ; n< optionItems.length ; n++) {
                if (optionItems[n].value === value) {
                    selectedIndex = n
                    optionSelected(value) // emit signal
                    return
                }
            }
        }

        selectedIndex = -1
        optionSelected(-1) // emit signal
        return
    }

    function selectOptionWithTag(tag) {
        for (var n=0 ; n< optionItems.length ; n++) {
            if (optionItems[n].tag === tag) {
                selectedIndex = n
                optionSelected(optionItems[n].value) // emit signal
                return
            }
        }
    }

    function findOptionTextByValue(value) {
        for (var n=0 ; n< optionItems.length ; n++) {
            if (optionItems[n].value === value) {
                return optionItems[n].text
            }
        }
        return ""
    }

    function findOptionTextByTag(tag) {
        for (var n=0 ; n< optionItems.length ; n++) {
            if (optionItems[n].tag === tag) {
                return optionItems[n].text
            }
        }
        return ""
    }

}

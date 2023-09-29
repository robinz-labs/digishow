import QtQuick 2.12
import QtQuick.Controls 2.12
 
import DigiShow 1.0

import "components"

Item {
    id: itemHotkey

    COptionButton {
        id: buttonModifier1
        width: 100
        height: 28
        anchors.left: parent.right
        anchors.top: parent.top
        text: menuModifier1.selectedItemText
        onClicked: menuModifier1.showOptions()

        COptionMenu {
            id: menuModifier1

            onOptionSelected: if (menuModifier2.selectedItemValue === value && value !== 0)
                                  menuModifier2.selectOption(0)
        }
    }

    COptionButton {
        id: buttonModifier2
        width: 100
        height: 28
        anchors.left: buttonModifier1.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuModifier2.selectedItemText
        onClicked: menuModifier2.showOptions()

        COptionMenu {
            id: menuModifier2

            onOptionSelected: if (menuModifier1.selectedItemValue === value && value !== 0)
                                  menuModifier1.selectOption(0)
        }
    }

    COptionButton {
        id: buttonKey
        width: 100
        height: 28
        anchors.left: buttonModifier2.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        text: menuKey.selectedItemText
        onClicked: menuKey.showOptions()

        COptionMenu {
            id: menuKey

            onOptionSelected: {}
        }
    }

    function refresh() {

        var items
        var n

        // init modifier keys option menu
        if (menuModifier1.count === 0 ||
            menuModifier2.count === 0) {

            items = []
            items.push({ text: qsTr("(none)")                         , value: 0             , tag:"" })
            items.push({ text: utilities.isMac() ? "Shift"   : "Shift", value: Qt.Key_Shift  , tag:"Shift" })
            items.push({ text: utilities.isMac() ? "Command" : "Ctrl" , value: Qt.Key_Control, tag:"Ctrl"  })
            items.push({ text: utilities.isMac() ? "Option"  : "Alt"  , value: Qt.Key_Alt    , tag:"Alt"   })
            items.push({ text: utilities.isMac() ? "Control" : "Meta" , value: Qt.Key_Meta   , tag:"Meta"  })

            menuModifier1.optionItems = items
            menuModifier2.optionItems = items

            menuModifier1.selectedIndex = 1
            menuModifier2.selectedIndex = 2
        }

        if (menuKey.count === 0) {

            items = []
            items.push({ text: "Space"    , value: Qt.Key_Space    , tag:"Space"    })
            items.push({ text: "Enter"    , value: Qt.Key_Enter    , tag:"Enter"    })
            items.push({ text: "Return"   , value: Qt.Key_Return   , tag:"Return"   })
            items.push({ text: "Esc"      , value: Qt.Key_Escape   , tag:"Esc"      })
            items.push({ text: "Backspace", value: Qt.Key_Backspace, tag:"Backspace"})
            items.push({ text: "Del"      , value: Qt.Key_Delete   , tag:"Del"      })
            items.push({ text: "Home"     , value: Qt.Key_Home     , tag:"Home"     })
            items.push({ text: "End"      , value: Qt.Key_End      , tag:"End"      })
            items.push({ text: "Page Up"  , value: Qt.Key_PageUp   , tag:"PgUp"     })
            items.push({ text: "Page Down", value: Qt.Key_PageDown , tag:"PgDown"   })
            items.push({ text: "Up"       , value: Qt.Key_Up       , tag:"Up"       })
            items.push({ text: "Down"     , value: Qt.Key_Down     , tag:"Down"     })
            items.push({ text: "Left"     , value: Qt.Key_Left     , tag:"Left"     })
            items.push({ text: "Right"    , value: Qt.Key_Right    , tag:"Right"    })

            items.push({ text: "F1"       , value: Qt.Key_F1       , tag:"F1"       })
            items.push({ text: "F2"       , value: Qt.Key_F2       , tag:"F2"       })
            items.push({ text: "F3"       , value: Qt.Key_F3       , tag:"F3"       })
            items.push({ text: "F4"       , value: Qt.Key_F4       , tag:"F4"       })
            items.push({ text: "F5"       , value: Qt.Key_F5       , tag:"F5"       })
            items.push({ text: "F6"       , value: Qt.Key_F6       , tag:"F6"       })
            items.push({ text: "F7"       , value: Qt.Key_F7       , tag:"F7"       })
            items.push({ text: "F8"       , value: Qt.Key_F8       , tag:"F8"       })
            items.push({ text: "F9"       , value: Qt.Key_F9       , tag:"F9"       })
            items.push({ text: "F10"      , value: Qt.Key_F10      , tag:"F10"      })
            items.push({ text: "F11"      , value: Qt.Key_F11      , tag:"F11"      })
            items.push({ text: "F12"      , value: Qt.Key_F12      , tag:"F12"      })

            items.push({ text: "A"        , value: Qt.Key_A        , tag:"A"        })
            items.push({ text: "B"        , value: Qt.Key_B        , tag:"B"        })
            items.push({ text: "C"        , value: Qt.Key_C        , tag:"C"        })
            items.push({ text: "D"        , value: Qt.Key_D        , tag:"D"        })
            items.push({ text: "E"        , value: Qt.Key_E        , tag:"E"        })
            items.push({ text: "F"        , value: Qt.Key_F        , tag:"F"        })
            items.push({ text: "G"        , value: Qt.Key_G        , tag:"G"        })
            items.push({ text: "H"        , value: Qt.Key_H        , tag:"H"        })
            items.push({ text: "I"        , value: Qt.Key_I        , tag:"I"        })
            items.push({ text: "J"        , value: Qt.Key_J        , tag:"J"        })
            items.push({ text: "K"        , value: Qt.Key_K        , tag:"K"        })
            items.push({ text: "L"        , value: Qt.Key_L        , tag:"L"        })
            items.push({ text: "M"        , value: Qt.Key_M        , tag:"M"        })
            items.push({ text: "N"        , value: Qt.Key_N        , tag:"N"        })
            items.push({ text: "O"        , value: Qt.Key_O        , tag:"O"        })
            items.push({ text: "P"        , value: Qt.Key_P        , tag:"P"        })
            items.push({ text: "Q"        , value: Qt.Key_Q        , tag:"Q"        })
            items.push({ text: "R"        , value: Qt.Key_R        , tag:"R"        })
            items.push({ text: "S"        , value: Qt.Key_S        , tag:"S"        })
            items.push({ text: "T"        , value: Qt.Key_T        , tag:"T"        })
            items.push({ text: "U"        , value: Qt.Key_U        , tag:"U"        })
            items.push({ text: "V"        , value: Qt.Key_V        , tag:"V"        })
            items.push({ text: "W"        , value: Qt.Key_W        , tag:"W"        })
            items.push({ text: "X"        , value: Qt.Key_X        , tag:"X"        })
            items.push({ text: "Y"        , value: Qt.Key_Y        , tag:"Y"        })
            items.push({ text: "Z"        , value: Qt.Key_Z        , tag:"Z"        })

            items.push({ text: "1"        , value: Qt.Key_1        , tag:"1"        })
            items.push({ text: "2"        , value: Qt.Key_2        , tag:"2"        })
            items.push({ text: "3"        , value: Qt.Key_3        , tag:"3"        })
            items.push({ text: "4"        , value: Qt.Key_4        , tag:"4"        })
            items.push({ text: "5"        , value: Qt.Key_5        , tag:"5"        })
            items.push({ text: "6"        , value: Qt.Key_6        , tag:"6"        })
            items.push({ text: "7"        , value: Qt.Key_7        , tag:"7"        })
            items.push({ text: "8"        , value: Qt.Key_8        , tag:"8"        })
            items.push({ text: "9"        , value: Qt.Key_9        , tag:"9"        })
            items.push({ text: "0"        , value: Qt.Key_0        , tag:"0"        })

            menuKey.optionItems = items
            menuKey.selectOption(Qt.Key_A)
        }

        // init more options
        refreshMoreOptions()
    }

    function refreshMoreOptions() {

        var enables = {}
        enables["optInitialB"] = true
        moreOptions.resetOptions()
        moreOptions.enableOptions(enables)
        buttonMoreOptions.visible = (Object.keys(enables).length > 0)
    }

    function setEndpointOptions(endpointInfo, endpointOptions) {

        var hotkey = endpointInfo["address"].split("+")
        menuKey.selectedIndex = 0
        menuModifier1.selectedIndex = 0
        menuModifier2.selectedIndex = 0
        if (hotkey.length > 0) menuKey.selectOptionWithTag(hotkey.pop())
        if (hotkey.length > 0) menuModifier1.selectOptionWithTag(hotkey.shift())
        if (hotkey.length > 0) menuModifier2.selectOptionWithTag(hotkey.shift())
    }

    function getEndpointOptions() {

        var options = {}
        options["type"] = "press"
        var hotkey = [];
        if (menuModifier1.selectedIndex > 0) hotkey.push(menuModifier1.selectedItemTag)
        if (menuModifier2.selectedIndex > 0) hotkey.push(menuModifier2.selectedItemTag)
        hotkey.push(menuKey.selectedItemTag)
        options["address"] = hotkey.join("+")

        return options
    }

}

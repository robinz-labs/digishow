import QtQuick 2.12
import QtQuick.Controls 2.12

Item {

    id: button

    property bool hasBox: true
    property bool checkable: false
    property bool checked: false

    property string colorNormal: "#484848"
    property string colorActivated: "#666666"
    property string colorChecked: "#006699"

    property alias box: buttonBox
    property alias icon: buttonIcon
    property alias iconChecked: buttonIconChecked
    property alias label: buttonLabel
    property alias labelChecked: buttonLabelChecked

    property alias toolTip: toolTip
    property alias toolTipText: toolTip.text
    property bool  toolTipVisible: true

    property bool mouseOver: false
    property bool mouseDown: false
    property alias mouseX: mouseArea.mouseX
    property alias mouseY: mouseArea.mouseY

    property bool supportLongPress: false

    signal clicked()
    signal pressed()
    signal released()
    signal longPressed()
    signal rightClicked()

    focus: true

    CCommon { id: common }

    Rectangle {
        id: buttonBox
        color: {
            if (button.checkable)
                return button.checked ? colorChecked : colorNormal
            else
                return button.mouseDown ? colorActivated : colorNormal
        }
        radius: 6
        border.color: colorActivated
        border.width: button.mouseOver ? 1 : 0
        anchors.fill: parent
        anchors.margins: 0
        visible: button.hasBox

        //Behavior on color { ColorAnimation { duration: 200 } }
    }

    Image {
        id: buttonIcon
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        visible: buttonIconChecked.source=="" ? true : !button.checked
    }

    Image {
        id: buttonIconChecked
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        opacity: button.checked
    }

    Text {
        id: buttonLabel
        width: button.width
        height: button.height
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        color: "#eeeeee"
        text: ""
        font.pixelSize: 16
        font.bold: true
        visible: buttonLabelChecked.text=="" ? true : !button.checked
    }

    Text {
        id: buttonLabelChecked
        width: button.width
        height: button.height
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        color: "#eeeeee"
        text: ""
        font.pixelSize: 16
        font.bold: true
        visible: button.checked
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        pressAndHoldInterval: 600
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onEntered: {
            button.mouseOver = true
        }
        onExited: {
            button.mouseOver = false
        }
        onPressed: {
            button.mouseDown = true
            common.runLater(function(){
                button.pressed() // emit signal
            });
        }
        onReleased: {
            button.mouseDown = false
            common.runLater(function(){
                button.released() // emit signal
            });
        }
        onClicked: {

            forceActiveFocus()

            if (button.checkable) {
                button.checked = !button.checked
            }

            if (mouse.button === Qt.RightButton)
                common.runLater(function(){ button.rightClicked() }) // emit signal
            else
                common.runLater(function(){ button.clicked() }) // emit signal
        }
        onPressAndHold: {
            if (supportLongPress) {
                button.mouseDown = false
                button.longPressed() // emit signal
            }
        }
    }

    ToolTip {
        id: toolTip
        delay: 1000
        visible: text !=="" && button.mouseOver && button.toolTipVisible
        text: ""
        enter: Transition {}
        exit: Transition {}
    }
}

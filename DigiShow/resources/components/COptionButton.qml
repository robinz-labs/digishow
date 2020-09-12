import QtQuick 2.12

Item {

    id: button

    property bool hasBox: true

    property string colorNormal: "#484848"
    property string colorActivated: "#666666"

    property alias text: buttonLabel.text
    property alias box: buttonBox
    property alias icon: buttonIcon
    property alias label: buttonLabel

    property bool mouseOver: false
    property bool mouseDown: false

    signal clicked()
    signal pressed()
    signal released()

    implicitWidth: 150
    implicitHeight: 28

    CCommon { id: common }

    Rectangle {
        id: buttonBox
        color: button.mouseDown ? colorActivated : colorNormal
        radius: 3
        border.color: colorActivated
        border.width: button.mouseOver ? 1 : 0
        anchors.fill: parent
        anchors.margins: 0
        visible: button.hasBox
    }

    Image {
        id: buttonIcon
        width: 16
        height: 16
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 0
        opacity: 0.3
        source: "qrc:///images/icon_arrow_updown_white.png"
    }

    Text {
        id: buttonLabel
        anchors.fill: parent
        anchors.leftMargin: 8
        anchors.rightMargin: 16
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        color: "#eeeeee"
        text: ""
        font.pixelSize: 11
        font.bold: false
        clip: true
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
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
            common.runLater(function(){
                button.clicked() // emit signal
            })
        }
    }
}

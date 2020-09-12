import QtQuick 2.12

Item {

    id: item

    NumberAnimation {
        id: animationShowing
        target: item
        properties: "opacity"
        from: 0.0
        to: 1.0
        duration: 250
        onStarted: {
            item.visible = true
        }
    }

    NumberAnimation {
        id: animationHidding
        target: item
        properties: "opacity"
        from: 1.0
        to: 0.0
        duration: 250
        onStopped: {
            item.visible = false
        }
    }

    function open() {
        if (!item.visible) animationShowing.running = true
    }

    function close() {
        if (item.visible) animationHidding.running = true
    }

}

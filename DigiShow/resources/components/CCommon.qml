import QtQuick 2.12

QtObject {

    function setTimeout(callback, timeout) {
        var timer = Qt.createQmlObject("import QtQuick 2.0; Timer {}", this)
        timer.interval = timeout
        timer.repeat = false
        timer.triggered.connect(function() {
            callback()
            timer.destroy()
        })
        timer.start()
    }

    function runLater(callback) {
        setTimeout(callback, 1)
    }

    function setAncestorProperty(item, key, value) {
        var itemObject = item
        while (itemObject !== null && itemObject !== undefined) {
            if (itemObject.hasOwnProperty(key)) {
                itemObject[key] = value
                break
            }
            itemObject = itemObject.parent
        }
    }

    function isDescendantOf(item, parentItem) {
        if (!item || !parentItem) {
            return false;
        }

        var current = item.parent;
        while (current) {
            if (current === parentItem) {
                return true;
            }
            current = current.parent;
        }
        return false;
    }
}

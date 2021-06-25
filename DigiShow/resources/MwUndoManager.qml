import QtQml 2.12
import DigiShow 1.0

QtObject {

    property var history: ([])
    property int historyLength: 0
    property int currentIndex: -1
    readonly property bool canUndo: currentIndex > 0 && historyLength > 1
    readonly property bool canRedo: currentIndex >=0 && currentIndex < historyLength - 1

    function clear() {

        history = []
        historyLength = 0
        currentIndex = -1
    }

    function archive() {

        if (canRedo) {
            for (var n=historyLength-1 ; n>currentIndex ; n--) history.pop()
        }

        if (history.length > 50) history.shift()

        var data = app.exportData(slotListView.getVisualItemsIndexList())
        history.push(data)

        historyLength = history.length
        currentIndex = historyLength - 1
    }

    function undo() {

        if (!canUndo) return;

        if (app.isRunning) app.stop()

        currentIndex--
        var data = history[currentIndex]
        app.importData(data)

        app.interfaceListChanged()
        app.slotListChanged()
        app.launchListChanged()
    }

    function redo() {

        if (!canRedo) return;

        if (app.isRunning) app.stop()

        currentIndex++
        var data = history[currentIndex]
        app.importData(data)

        app.interfaceListChanged()
        app.slotListChanged()
        app.launchListChanged()
    }
}

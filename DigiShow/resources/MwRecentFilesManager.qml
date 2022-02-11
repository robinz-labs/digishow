import QtQml 2.12
import DigiShow 1.0

QtObject {

    property var recentFilesList: ([])

    signal recentFilesListUpdated()

    function add(filepath) {

        if (!utilities.fileExists(filepath)) return

        // remove item exists
        var indexExists = recentFilesList.indexOf(filepath)
        if (indexExists !== -1) recentFilesList.splice(indexExists, 1)

        // add new item
        recentFilesList.unshift(filepath)

        // the list is full
        if (recentFilesList.length > 10) recentFilesList.pop()

        recentFilesListUpdated() // emit signal
    }

    function clear() {

        recentFilesList = []
        recentFilesListUpdated() // emit signal
    }

    function load() {

        var appOptions = digishow.getAppOptions()
        var recentFiles = appOptions["recentFiles"]
        if (recentFiles !== undefined && recentFiles instanceof Array) {
            recentFilesList = recentFiles
            recentFilesListUpdated() // emit signal
        }
    }

    function save() {

        var appOptions = digishow.getAppOptions()
        appOptions["recentFiles"] = recentFilesList
        digishow.setAppOptions(appOptions)
    }

}

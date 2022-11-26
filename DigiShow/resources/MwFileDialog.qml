import QtQuick 2.12
import QtQuick.Dialogs 1.0

FileDialog {

    id: dialog

    function setDefaultFilePath(path) {
        var filepath = (path === undefined || !utilities.fileExists(path) ? app.filepath : path)
        var dirpath = utilities.getFileDir(filepath)
        if (dirpath !== "" && utilities.dirExists(dirpath)) {
            dialog.folder = utilities.filePathUrl(dirpath)
        }
    }

    function setDefaultFileUrl(url) {
        var path = utilities.fileUrlPath(url)
        setDefaultFilePath(path)
    }
}

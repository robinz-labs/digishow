import QtQml 2.15
import DigiShow 1.0

DigishowEnvironment {

    property Utilities utilities: Utilities { }

    function onStart() { }
    function onStop() { }

    function alert(message) { app.messageNotify(message, DigishowApp.MsgAlert) }
    function toast(message) { app.messageNotify(message, DigishowApp.MsgToast) }

    function execute(script, value) { return eval(script) }
}

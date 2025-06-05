import QtQml 2.12
import DigiShow 1.0

DigishowEnvironment {

    property Utilities utilities: Utilities { }

    function onStart() { }
    function onStop() { }

    function alert(message) { app.messageNotify(message, DigishowApp.MsgAlert) }
    function toast(message) { app.messageNotify(message, DigishowApp.MsgToast) }
    function popup(message) { app.messageNotify(message, DigishowApp.MsgPopup) }

    // @disable-check M23
    function execute(script) { return eval(script) }

    // @disable-check M23
    function executeExpression(expression, value, range, lastValue, index, end) { var r = eval(expression); return (r === null ? -1 : r) }
}

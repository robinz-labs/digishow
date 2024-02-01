import QtQml 2.12
import DigiShow 1.0

DigishowEnvironment {

    property Utilities utilities: Utilities { }

    function onStart() { }
    function onStop() { }

    function execute(script, inputValue) {
        return eval(script)
    }
}

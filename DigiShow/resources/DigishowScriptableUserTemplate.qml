DigishowScriptable {

    // this is a qml-based scriptable module for your digishow project.
    // you can declare variables and make functions in Qml/JavaScript language.
    // the variables and functions here can be referenced in any signal expressions.

    // for example:

    /*
        property int  v1: 100
        property real v2: 0.5
        property bool v3: true

        function myFunction(a, b) {
            v3 = (a > b)
            return Math.abs(a - b) * v2 + v1
        }
    */

    // event callback functions

    function onStart() {

        // toast('The scriptable module is started.')
    }

    function onStop() {

        // alert('The scriptable module is stopped.')
    }
}

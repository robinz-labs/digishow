.pragma library

function clone(obj) {

    // Handle the 3 simple types, and null or undefined
    if (null === obj || "object" != typeof obj) return obj;

    if (obj instanceof Date) {

        // Handle Date

        var copy = new Date();
        copy.setTime(obj.getTime());
        return copy;

    } else if (obj instanceof Array) {

        // Handle Array

        var copy = [];
        var len = obj.length;
        for (var i = 0 ; i < len; ++i) {
            copy[i] = clone(obj[i]);
        }
        return copy;

    } else if (obj instanceof Object) {

        // Handle Object

        var copy = {};
        for (var attr in obj) {
            //if (obj.hasOwnProperty(attr))
            copy[attr] = clone(obj[attr]);
        }
        return copy;
    }

    throw new Error("Unable to copy obj! Its type isn't supported.");
}

function replaceAll(str, search, replacement) {
    return str.split(search).join(replacement);
}
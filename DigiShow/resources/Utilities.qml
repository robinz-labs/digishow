import QtQml 2.12
import DigiShow 1.0

AppUtilities {

    function urlToPath(url) {
        var s = url.toString();
        if (s.startsWith("file:///")) {
            var k = (s.charAt(9) === ':' ? 8 : 7);
            s = s.substring(k);
        } else if (s.startsWith("file://")) {
            s = s.substring(5);
        }
        return decodeURIComponent(s);
    }

    function getFileName(filepath) {

        var parts = replaceAll(filepath, '\\', '/').split('/');
        return parts[parts.length-1];
    }

    function replaceAll(str, search, replacement) {
        return str.split(search).join(replacement);
    }

    function clone(obj) {

        // Handle the 3 simple types, and null or undefined
        if (null === obj || "object" != typeof obj) return obj;

        var copy;

        if (obj instanceof Date) {

            // Handle Date

            copy = new Date();
            copy.setTime(obj.getTime());
            return copy;

        } else if (obj instanceof Array) {

            // Handle Array

            copy = [];
            var len = obj.length;
            for (var i = 0 ; i < len; ++i) {
                copy[i] = clone(obj[i]);
            }
            return copy;

        } else if (obj instanceof Object) {

            // Handle Object

            copy = {};
            for (var attr in obj) {
                //if (obj.hasOwnProperty(attr))
                copy[attr] = clone(obj[attr]);
            }
            return copy;
        }

        //throw new Error("Unable to copy obj! Its type isn't supported.");
        return null;
    }

    function merge(obj1, obj2) {

        var combined = {};
        var attr;

        for (attr in obj1) {
          combined[attr] = obj1[attr];
        }
        for (attr in obj2) {
          combined[attr] = obj2[attr];
        }

        return combined;
    }

}

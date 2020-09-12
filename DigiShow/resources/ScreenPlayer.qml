import QtQuick 2.12
import QtQuick.Window 2.12
import QtMultimedia 5.12
import DigiShow 1.0

import "components"

QtObject {

    id: screenPlayer

    // screen light controls
    property real lightR: 0.0
    property real lightG: 0.0
    property real lightB: 0.0
    property real lightW: 0.0

    // media clip controls
    property var  mediaList:      ([]) // a list of all media metadata
    property var  mediaViewList:  ([]) // a list of all media views that created on the screen
    property bool mediaShowAlone: true // can only show one media clip on the screen at the same time
    property var  frontMediaView: null // the media view shown on the top level on the screen
    property real topZ:           100  // z order number of the top-level media view

    property Window playerWindow: Window {

        width: 800
        height: 450
        flags: Qt.WindowFullscreenButtonHint //| Qt.MaximizeUsingFullscreenGeometryHint //| Qt.FramelessWindowHint
        color: "white"

        Utilities { id: utilities }
        CCommon { id: common }

        Rectangle {
            id: light
            anchors.fill: parent
            color: Qt.rgba(lightR, lightG, lightB, 1.0 - lightW)
        }

        Rectangle {
            id: screenboard
            anchors.fill: parent
            color: "transparent"
        }

    }

    function showInScreen(screenIndex) {

        var screens = Qt.application.screens
        if (screenIndex>=0 && screenIndex<screens.length) {

            playerWindow.screen = screens[screenIndex]
            playerWindow.title = screens[screenIndex].name

            common.runLater(function() {
                playerWindow.showMaximized()
                playerWindow.showFullScreen()
            })
            return true
        }

        return false
    }

    function goodbye() {

        if (playerWindow.visibility === Window.FullScreen ||
            playerWindow.visibility === Window.Minimized) {
            playerWindow.showMaximized()
            utilities.delay(1000);
        }
        playerWindow.close()
    }

    function loadMedia(media) {

        var name = media["name"]
        var type = media["type"]
        var url  = media["url" ]

        var view = null
        if (type === "video") {

            view = Qt.createQmlObject("SpVideoView {}", screenboard)
            view.player.source = url

        } else if (type === "image") {

            view = Qt.createQmlObject("SpImageView {}", screenboard)
            view.source = url

        } else if (type === "web") {

            view = Qt.createQmlObject("SpWebView {}", screenboard)
            view.url = url
        }

        if (view !== null) {
            topZ += 1;
            view.z = topZ
            view.visible = false
            view.objectName = name

            mediaList.push(media)
            mediaViewList.push(view)
        }
    }

    function loadMediaList(list) {

        // release all existing media views
        for (; mediaViewList.length > 0 ;) mediaViewList.pop().destroy()

        mediaList = []
        mediaViewList = []
        frontMediaView = null

        // create all new media views
        for (var n=0 ; n<list.length ; n++) loadMedia(list[n])
    }

    function setMediaProperty(mediaName, propertyName, propertyValue) {

        var view = getMediaViewByName(mediaName)

        if (view !== null) {


            if (propertyName === "opacity") {

                console.log(mediaName, propertyName, propertyValue, new Date().getMilliseconds())

                view.stopFadeIn()
            }
            view[propertyName] = propertyValue
        }
    }

    function showMedia(mediaName, options) {

        // take media to front

        var view = getMediaViewByName(mediaName)
        if (view !== null) {

            var v = options["mediaShowAlone"]; mediaShowAlone = (v === undefined ? true : v )
            if (mediaShowAlone) pauseAllVideoViewsExcept(null)

            // set media display options
            v = options["mediaOpacity"];   var mediaOpacity   = (v === undefined ? 1.0  : v / 10000)
            v = options["mediaScale"];     var mediaScale     = (v === undefined ? 1.0  : v / 10000)
            v = options["mediaRotation"];  var mediaRotation  = (v === undefined ? 0    : v / 10 )
            v = options["mediaXOffset"];   var mediaXOffset   = (v === undefined ? 0.5  : v / 10000)
            v = options["mediaYOffset"];   var mediaYOffset   = (v === undefined ? 0.5  : v / 10000)

            view.opacity  = mediaOpacity
            view.scale    = mediaScale
            view.rotation = mediaRotation
            view.xOffset  = mediaXOffset
            view.yOffset  = mediaYOffset

            // for video media clip only
            if (view.mediaType === "video") {

                v = options["mediaVideoRepeat"];   var mediaVideoRepeat   = (v === undefined ? true : v )
                v = options["mediaVideoVolume"];   var mediaVideoVolume   = (v === undefined ? 1.0  : v / 10000)
                v = options["mediaVideoPosition"]; var mediaVideoPosition = (v === undefined ? 0    : v * 1000)

                view.player.loops = (mediaVideoRepeat ? MediaPlayer.Infinite : 1)
                view.player.volume = mediaVideoVolume
                view.player.seek(mediaVideoPosition)
                view.player.play()
            }

            // for web media clip only
            if (view.mediaType === "web") {

                v = options["mediaWebJavascript"]; var mediaWebJavascript = (v === undefined ? "" : v )

                if (mediaWebJavascript !== "") {
                    view.runJavaScript(mediaWebJavascript, function(result) {})
                }
            }

            // show media with fading effects
            v = options["mediaFadeIn"]; var mediaFadeIn = (v === undefined ? 300 : v )
            view.startFadeIn(mediaFadeIn)
        }
    }

    function hideMedia(mediaName) {

        // hide media

        var view = getMediaViewByName(mediaName)
        if (view !== null) {

            // stop video if necessary
            if (view.mediaType === "video") {
                view.player.stop()
            }

            view.visible = false
            if (frontMediaView === view) frontMediaView = null
        }
    }

    function clearAllMedia() {

        pauseAllVideoViewsExcept(null)
        hideAllMediaViewsExcept(null)
    }

    function setFrontMediaView(view) {

        frontMediaView = view
        if (view !== null) {
            topZ += 1
            view.z = topZ
            view.visible = true
        }
    }

    function getMediaViewByName(mediaName) {

        var view = null
        if (mediaName===undefined || mediaName==="") {
            view = frontMediaView
        } else {
            for (var n=0 ; n<mediaList.length ; n++) {
                if (mediaList[n]["name"] === mediaName) {
                    view = mediaViewList[n]
                    break
                }
            }
        }

        return view
    }

    function hideAllMediaViewsExcept(view) {

        for (var n=0 ; n<mediaViewList.length ; n++) {
            var view1 = mediaViewList[n]
            if (view1 !== view) view1.visible = false
        }
    }

    function pauseAllVideoViewsExcept(view) {

        for (var n=0 ; n<mediaViewList.length ; n++) {
            var view1 = mediaViewList[n]
            if (view1.mediaType === "video" &&
                view1 !== view) view1.player.pause()
        }
    }
}

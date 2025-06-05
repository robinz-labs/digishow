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

        id: window

        width: 800
        height: 450
        flags: Qt.Window | Qt.WindowResize | Qt.WindowTitleHint | Qt.WindowMinimizeButtonHint | Qt.WindowMaximizeButtonHint | Qt.WindowFullscreenButtonHint
        color: "white"

        Utilities { id: utilities }
        CCommon { id: common }

        MouseArea {
            id: mouseArea

            property var lastMouseMovingTime: 0
            property bool mouseIsMoving: lastMouseMovingTime > 0

            anchors.fill: parent
            hoverEnabled: true

            cursorShape: mouseArea.containsMouse && mouseArea.mouseIsMoving ? Qt.ArrowCursor : Qt.BlankCursor

            onMouseXChanged: {
                lastMouseMovingTime = new Date().getTime()
            }

            Timer {
                interval: 1000
                repeat: true
                running: true
                onTriggered: {
                    if (mouseArea.lastMouseMovingTime > 0) {
                        var now = new Date().getTime()
                        if (now - mouseArea.lastMouseMovingTime > 3000) {
                            mouseArea.lastMouseMovingTime = 0
                        }
                    }
                }
            }

            Rectangle {
                id: light
                anchors.fill: parent
                color: Qt.rgba(lightR, lightG, lightB, 1.0 - lightW)
            }

            Rectangle {
                id: canvas

                property real xOffset: 0.5  // 0 ~ 1.0
                property real yOffset: 0.5  // 0 ~ 1.0

                width: parent.width
                height: parent.height
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenterOffset: width * ( xOffset - 0.5 ) * 2
                anchors.verticalCenterOffset: height * ( -yOffset + 0.5 ) * 2
                color: "transparent"
            }

            CButton {
                id: buttonFullscreen
                height: 40
                width: 40
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.topMargin: 40
                anchors.leftMargin: 40
                colorNormal: "black"
                icon.width: 24
                icon.height: 24
                icon.source: ( window.visibility==Window.FullScreen ?
                    "qrc:///images/icon_fullscreen_exit_white.png" :
                    "qrc:///images/icon_fullscreen_enter_white.png" )
                visible: mouseArea.containsMouse && mouseArea.mouseIsMoving
                onClicked: {
                    if (window.visibility==Window.FullScreen)
                        window.showNormal()
                    else
                        window.showFullScreen()
                }
            }
        }
    }

    function showInScreen(screen) {

        var screens = Qt.application.screens

        var screenIndex = 0;
        if (screen === 0) {
            // default screen
            if (screens.length >= 2) screenIndex = 1;
        } else if (screen >= 1) {
            // specified screen
            screenIndex = screen - 1;
        }

        if (screenIndex >= screens.length) return false
        playerWindow.title = qsTr("Screen") + (screen >=1 ? " " + screen : "")

        if (screen === -1) {

            // show in a preview window
            common.runLater(function() {
                playerWindow.show()
            })

        } else {

            // show in the full screen
            playerWindow.screen = screens[screenIndex]
            common.runLater(function() {
                playerWindow.showMaximized()
                playerWindow.showFullScreen()
            })
        }

        return true
    }

    function goodbye() {

        if (playerWindow.visibility === Window.FullScreen ||
            playerWindow.visibility === Window.Minimized) {
            playerWindow.showMaximized()
            utilities.delay(1000);
        }
        playerWindow.close()
    }

    function setCanvasProperty(propertyName, propertyValue) {

        canvas[propertyName] = propertyValue
    }

    function loadMedia(media) {

        var name = media["name"]
        var type = media["type"]
        var url  = media["url" ]

        var view = null
        if (type === "video") {

            view = Qt.createQmlObject("SpVideoView {}", canvas)
            view.player.source = url

        } else if (type === "image") {

            view = Qt.createQmlObject("SpImageView {}", canvas)
            view.source = url

        } else if (type === "web") {

            view = Qt.createQmlObject("SpWebView {}", canvas)
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

            if (propertyName === "volume" ||
                propertyName === "speed" ||
                propertyName === "position")
                if (view.mediaType !== "video") return

            if (propertyName === "opacity") view.stopFadeIn()
            view[propertyName] = propertyValue
        }
    }

    function showMedia(mediaName, options) {

        // take media to front

        var view = getMediaViewByName(mediaName)
        if (view !== null) {

            mediaShowAlone = true
            var v
            v = options["mediaShowAlone"]; if (v !== undefined) mediaShowAlone = v // deprecated !!!
            v = options["mediaAlone"];     if (v !== undefined) mediaShowAlone = v
            if (mediaShowAlone) pauseAllVideoViewsExcept(null)

            // set media display options
            v = options["mediaOpacity"];   var mediaOpacity   = (v === undefined ? 1.0  : v / 10000)
            v = options["mediaScale"];     var mediaScale     = (v === undefined ? 0.5  : v / 10000)
            v = options["mediaRotation"];  var mediaRotation  = (v === undefined ? 0    : v / 10 )
            v = options["mediaXOffset"];   var mediaXOffset   = (v === undefined ? 0.5  : v / 10000)
            v = options["mediaYOffset"];   var mediaYOffset   = (v === undefined ? 0.5  : v / 10000)

            view.opacity  = mediaOpacity
            view.scale    = mediaScale*2
            view.rotation = mediaRotation
            view.xOffset  = mediaXOffset
            view.yOffset  = mediaYOffset

            // for video media clip only
            if (view.mediaType === "video") {

                // deprecated !!!
                v = options["mediaVideoRepeat"];   var mediaRepeat   = (v === undefined ? true : v )
                v = options["mediaVideoVolume"];   var mediaVolume   = (v === undefined ? 1.0  : v / 10000)
                v = options["mediaVideoSpeed"];    var mediaSpeed    = (v === undefined ? 1.0  : v / 10000)
                v = options["mediaVideoPosition"]; var mediaPosition = (v === undefined ? 0    : v * 1000)

                // replaced with
                v = options["mediaRepeat"];        if (v !== undefined) mediaRepeat   = v
                v = options["mediaVolume"];        if (v !== undefined) mediaVolume   = v / 10000
                v = options["mediaSpeed"];         if (v !== undefined) mediaSpeed    = v / 10000
                v = options["mediaPosition"];      if (v !== undefined) mediaPosition = v

                // a-b loop options
                v = options["mediaPositionA"];     var mediaPositionA = (v === undefined ? 0 : v)
                v = options["mediaPositionB"];     var mediaPositionB = (v === undefined ? 0 : v)

                view.repeat = mediaRepeat
                view.volume = mediaVolume
                view.speed  = mediaSpeed
                view.positionA = mediaPositionA
                view.positionB = mediaPositionB
                view.player.seek(mediaPosition)
                view.player.play()
            }

            // for web media clip only
            if (view.mediaType === "web") {

                var mediaScript = ""
                v = options["mediaWebJavascript"]; if (v !== undefined) mediaScript = v // deprecated !!!
                v = options["mediaScript"];        if (v !== undefined) mediaScript = v

                if (mediaScript !== "") {
                    view.runJavaScript(mediaScript, function(result) {})
                }

                var mediaWidth = 0;
                v = options["mediaWidth"]; if (v !== undefined) mediaWidth = v
                if (mediaWidth > 0) {
                    view.contentsWidth = mediaWidth
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

import QtQuick 2.12
import QtWebEngine 1.8

WebEngineView {

    id: webView

    property string mediaType: "web"

    property real xOffset: 0.5  // 0 ~ 1.0
    property real yOffset: 0.5  // 0 ~ 1.0

    width: parent.width
    height: parent.height
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenterOffset: width * ( xOffset - 0.5 ) * 2
    anchors.verticalCenterOffset: height * ( -yOffset + 0.5 ) * 2

    settings.autoLoadImages: true
    settings.javascriptEnabled: true
    settings.errorPageEnabled: true
    settings.pluginsEnabled: true
    settings.fullScreenSupportEnabled: true
    settings.autoLoadIconsForPage: true
    settings.touchIconsEnabled: false
    settings.webRTCPublicInterfacesOnly: false

    SpAnimationFadeIn {
        id: animationFadeIn
        target: webView
    }

    function startFadeIn(duration) { animationFadeIn.fadeIn(duration) }
    function stopFadeIn() { animationFadeIn.stop() }
}

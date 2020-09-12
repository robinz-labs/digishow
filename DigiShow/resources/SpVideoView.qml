import QtQuick 2.12
import QtMultimedia 5.12

Item {

    id: videoView

    property string mediaType: "video"
    property alias player: videoPlayer

    property real xOffset: 0.5  // 0 ~ 1.0
    property real yOffset: 0.5  // 0 ~ 1.0

    width: parent.width
    height: parent.height
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenterOffset: width * ( xOffset - 0.5 ) * 2
    anchors.verticalCenterOffset: height * ( -yOffset + 0.5 ) * 2

    //Behavior on anchors.horizontalCenterOffset { NumberAnimation { duration: 50 } }
    //Behavior on anchors.verticalCenterOffset   { NumberAnimation { duration: 50 } }

    MediaPlayer {
        id: videoPlayer
        autoPlay: false
        autoLoad: true
    }

    VideoOutput {
        id: videoOutput
        source: videoPlayer
        anchors.fill: parent
    }

    SpAnimationFadeIn {
        id: animationFadeIn
        target: videoView
    }

    function startFadeIn(duration) { animationFadeIn.fadeIn(duration) }
    function stopFadeIn() { animationFadeIn.stop() }
}

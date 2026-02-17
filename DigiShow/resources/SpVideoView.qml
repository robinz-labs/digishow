import QtQuick 2.12
import QtMultimedia 5.12

Item {

    id: videoView

    property string mediaType: "video"

    property real xOffset: 0.5  // 0 ~ 1.0
    property real yOffset: 0.5  // 0 ~ 1.0

    property alias player: videoPlayer
    property bool playing: videoPlayer.playbackState === MediaPlayer.PlayingState

    property bool repeat: false
    property real volume: 1.0
    property real speed: 1.0
    property int  position : 0 // millisecond
    property int  positionA: 0 // A-B loop
    property int  positionB: 0

    onPositionChanged: videoPlayer.seek(videoView.position)

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
        loops: (videoView.repeat ? MediaPlayer.Infinite : 1)
        volume: videoView.volume
        playbackRate: videoView.speed

        //onPositionChanged: {}
    }

    Timer {
        interval: 50
        running: videoPlayer.playbackState === MediaPlayer.PlayingState
        repeat: true

        onTriggered: {

            // A-B loop
            if (videoView.positionB > 0 && videoPlayer.position >= videoView.positionB) {
                videoPlayer.seek(videoView.positionA)
                return
            }

            // frozen at the last second
            if (!videoView.repeat && videoPlayer.duration > 0 && videoPlayer.position >= videoPlayer.duration-500) {
                videoPlayer.pause()
            }
        }
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

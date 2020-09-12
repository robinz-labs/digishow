import QtQuick 2.12

Image {

    id: imageView

    property string mediaType: "image"

    property real xOffset: 0.5  // 0 ~ 1.0
    property real yOffset: 0.5  // 0 ~ 1.0

    width: parent.width
    height: parent.height
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenterOffset: width * ( xOffset - 0.5 ) * 2
    anchors.verticalCenterOffset: height * ( -yOffset + 0.5 ) * 2
    fillMode: Image.PreserveAspectFit

    SpAnimationFadeIn {
        id: animationFadeIn
        target: imageView
    }

    function startFadeIn(duration) { animationFadeIn.fadeIn(duration) }
    function stopFadeIn() { animationFadeIn.stop() }
}

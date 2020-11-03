import QtQuick 2.12

NumberAnimation {

    id: animationFadeIn
    properties: "opacity"
    from: 0.0
    to: target.opacity
    duration: 300
    alwaysRunToEnd: false
    onStarted: {
        screenPlayer.setFrontMediaView(target)
    }
    onStopped: {
        if (mediaShowAlone) screenPlayer.hideAllMediaViewsExcept(target)
    }

    function fadeIn(duration) {

        if (target.visible || duration===undefined || duration===0) {

            // show media view shortly
            screenPlayer.setFrontMediaView(target)
            if (mediaShowAlone) screenPlayer.hideAllMediaViewsExcept(target)

        } else {

            // show media view with fade-in animation
            animationFadeIn.duration = duration
            animationFadeIn.start()
        }
    }
}

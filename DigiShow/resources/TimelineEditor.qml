import QtQuick 2.12
import QtQuick.Controls 2.12

// Timeline Editor Component
Item {
    id: root
    width: 800
    height: 400

    // Enum for point types
    readonly property var pointType: ({
        LINEAR: 0, // Linear interpolation point (straight line between points)
        BEZIER: 1  // Bezier curve point (smooth curve with control points)
    })

    // Properties for timeline configuration
    property real actualValueRangeLower: 0 // Lower bound of actual value range
    property real actualValueRangeUpper: 100 // Upper bound of actual value range
    property real totalTimeRange: 1800  // Total time range in seconds
    property real viewportTimeRange: 30 // Viewport time range in seconds
    property real viewportPosition: 0   // Current viewport position in seconds
    property real timeScale: (width - 50) / viewportTimeRange // Pixels per second
    property color lineColor: "#4CAF50" // Color for the curve and control points
    property color backgroundColor: "#2D2D2D" // Background color
    property color playheadColor: "#FFFFFF" // Color for the playhead
    property color markerColor: "#8B0000" // Dark red color for markers
    
    // Playback properties
    property real playheadTime: 0 // Current playhead position in seconds
    property bool isPlaying: false // Whether playback is active
    property real playbackSpeed: 1.0 // Playback speed multiplier
    
    // Points array for curve control points
    property var points: [
        { time: 0, value: 0, type: pointType.LINEAR },
        { time: 30, value: 0, type: pointType.LINEAR }
    ]
    
    // Markers array for time markers
    property var markers: [] // Array of time positions for markers
    property bool isMarkersModified: false

    // History stack for undo operations
    property var historyStack: []
    property int maxHistorySize: 50 // Maximum number of history states to keep
    signal historyChanged()

    // Helper function to calculate value at specific time
    function calculateValueAt(time) {
        // If time is before first point, return first point's value
        if (time <= points[0].time) {
            return points[0].value;
        }

        // If time is after last point, return last point's value
        if (time >= points[points.length - 1].time) {
            return points[points.length - 1].value;
        }

        var currentValue = 0;
        for (var i = 0; i < points.length - 1; i++) {
            var p1 = points[i];
            var p2 = points[i + 1];

            if (time >= p1.time && time <= p2.time) {
                // Calculate progress between points
                var progress = (time - p1.time) / (p2.time - p1.time);

                if (p1.type === pointType.BEZIER || p2.type === pointType.BEZIER) {
                    // Bezier interpolation
                    var cp1y = p1.value;
                    var cp2y = p2.value;

                    if (p1.type === pointType.BEZIER) {
                        cp1y = p1.value;
                    }
                    if (p2.type === pointType.BEZIER) {
                        cp2y = p2.value;
                    }

                    // Cubic bezier interpolation
                    var mt = 1 - progress;
                    currentValue = mt * mt * mt * p1.value +
                                 3 * mt * mt * progress * cp1y +
                                 3 * mt * progress * progress * cp2y +
                                 progress * progress * progress * p2.value;
                } else {
                    // Linear interpolation
                    currentValue = p1.value + (p2.value - p1.value) * progress;
                }
                break;
            }
        }
        return currentValue;
    }

    // Helper function to calculate actual value from percentage
    function calculateActualValue(percentage) {
        return actualValueRangeLower + (percentage / 100) * (actualValueRangeUpper - actualValueRangeLower)
    }

    // Function to import the data of curve points
    function importData(data) {
        // Validate data format
        if (!data || !data.points || !Array.isArray(data.points)) return false;

        // Create a deep copy of imported points
        var importedPoints = JSON.parse(JSON.stringify(data.points));

        // Replace current points with imported points
        if (importedPoints.length<2) return false;
        saveToHistory();
        points = importedPoints;

        // Update the grid canvas
        gridCanvas.requestPaint();

        return true;
    }

    // Function to export the data of curve points
    function exportData() {
        // Create a deep copy of current points
        var currentPoints = JSON.parse(JSON.stringify(points))
        // Create data object
        var data = {
            points: currentPoints
        };
        return data;
    }

    // Function to reset the timeline to default state
    function reset() {
        saveToHistory();
        points = [
            { time: 0, value: 0, type: pointType.LINEAR },
            { time: 30, value: 0, type: pointType.LINEAR }
        ];
        viewportPosition = 0;
        viewportTimeRange = 30;
        gridCanvas.requestPaint();
    }

    // Function to save current state to history
    function saveToHistory() {
        // Create a deep copy of current points
        var currentState = JSON.parse(JSON.stringify(points));

        // Add to history stack
        historyStack.push(currentState);

        // Keep history size within limit
        if (historyStack.length > maxHistorySize) {
            historyStack.shift();
        }

        // Emit signal to update history UI
        historyChanged();
    }

    // Function to undo last operation
    function undo() {
        if (historyStack.length > 0) {
            // Restore last state
            points = historyStack.pop();
            gridCanvas.requestPaint();
        }
    }

    // Function to play
    function play() {
        if (!isPlaying) {
            if (playheadTime >= points[points.length - 1].time) playheadTime = 0;
            isPlaying = true;
            playbackTimer.start();
        }
    }

    // Function to stop
    function stop() {
        if (isPlaying) {
            isPlaying = false;
            playbackTimer.stop();
        }
    }

    // Function to seek to a specific time
    function seek(time) {
        playheadTime = Math.max(0, Math.min(totalTimeRange, time));
        gridCanvas.requestPaint();
    }

    // Function to toggle playback (play/pause)
    function togglePlayback() {
        if (isPlaying) {
            stop();
        } else {
            play();
        }
    }

    // Function to import the data of markers
    function importMarkers(data) {
        console.log("Importing markers");
        if (!data || !data.markers || !Array.isArray(data.markers)) return false;
        
        // Create a deep copy of imported markers
        var importedMarkers = JSON.parse(JSON.stringify(data.markers));
        
        // Replace current markers with imported markers
        markers = importedMarkers;
        isMarkersModified = false;
        
        // Request repaint to show the imported markers
        gridCanvas.requestPaint();
        return true;
    }
    
    // Function to export the data of markers
    function exportMarkers() {
        // Create a deep copy of current markers
        var currentMarkers = JSON.parse(JSON.stringify(markers));
        
        return {
            markers: currentMarkers
        };
    }
    
    // Function to add a marker at specified time
    function addMarker(time) {
        console.log("Adding marker at time:", time);
        
        // Check if marker already exists at this time
        for (var i = 0; i < markers.length; i++) {
            if (Math.abs(markers[i] - time) < 0.01) return false; // Avoid duplicates
        }
        
        // Add marker to array
        markers.push(time);
        markers.sort(function(a, b) { return a - b; }); // Sort markers by time
        isMarkersModified = true;

        // Request repaint to show the new marker
        gridCanvas.requestPaint();
        return true;
    }
    
    // Function to clear all markers
    function clearMarkers() {
        console.log("Clearing all markers");
        markers = [];
        isMarkersModified = true;
        gridCanvas.requestPaint();
    }

    // Function to force repaint
    function paint() {
        gridCanvas.requestPaint();
    }

    // Timer for playback
    Timer {
        id: playbackTimer
        interval: 25 // 40fps
        repeat: true
        running: false
        onTriggered: {
            if (playheadTime >= totalTimeRange ||
                playheadTime >= points[points.length - 1].time) {
                parent.stop();
                return;
            }

            // Update playhead position
            playheadTime += (interval / 1000.0) * playbackSpeed;

            // Auto-scroll viewport if playhead reaches the edge
            var playheadX = gridCanvas.padding + (playheadTime - viewportPosition) * (width - 2 * gridCanvas.padding) / viewportTimeRange;
            if (playheadX > width - gridCanvas.padding * 3) {
                viewportPosition = Math.min(totalTimeRange - viewportTimeRange,
                                         playheadTime - viewportTimeRange * 0.5);
            }

            // Request canvas repaint to update playhead position
            gridCanvas.requestPaint();
        }
    }

    // Floating label for point information
    Item {
        id: pointInfoLabel
        width: timeText.width + percentText.width + actualValueText.width + 40
        height: Math.max(timeText.height, percentText.height, actualValueText.height) + 10
        visible: false
        z: 1

        Rectangle {
            anchors.fill: parent
            color: "#000000"
            opacity: 0.8
            radius: 5
        }

        Row {
            anchors.centerIn: parent
            spacing: 10
            Text {
                id: timeText
                color: "white"
                font.family: "Arial"
                font.pixelSize: 12
                anchors.verticalCenter: parent.verticalCenter
            }
            Text {
                id: percentText
                color: "white"
                font.family: "Arial"
                font.pixelSize: 12
                anchors.verticalCenter: parent.verticalCenter
            }
            Text {
                id: actualValueText
                color: lineColor
                font.family: "Arial"
                font.pixelSize: 12
                font.weight: Font.Bold
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    // Dark gray background
    Rectangle {
        anchors.fill: parent
        color: backgroundColor

        // Timeline area
        Rectangle {
            id: timelineArea
            anchors {
                left: valueAxis.right
                right: parent.right
                top: parent.top
                bottom: timeAxis.top
            }
            color: backgroundColor

            // Grid lines will be drawn here
            Canvas {
                id: gridCanvas
                anchors.fill: parent
                property real gridOpacity: 0.3
                property real padding: 30

                onPaint: {
                    var ctx = getContext("2d")
                    ctx.clearRect(0, 0, width, height)

                    drawHorizontalGrid(ctx)
                    drawVerticalGrid(ctx)
                    drawMarkers(ctx) // Draw markers behind the curve
                    drawCurve(ctx)
                    drawControlPoints(ctx)
                    drawPlayhead(ctx)
                }
                
                // Helper function to draw markers
                function drawMarkers(ctx) {
                    // Set marker style
                    ctx.strokeStyle = markerColor
                    ctx.lineWidth = 1
                    
                    // Draw each marker as a vertical line
                    for (var i = 0; i < markers.length; i++) {
                        var markerTime = markers[i];
                        var x = gridCanvas.padding + (markerTime - viewportPosition) * (width - 2 * gridCanvas.padding) / viewportTimeRange;
                        
                        // Only draw if marker is within viewport
                        if (x >= gridCanvas.padding && x <= width - gridCanvas.padding) {
                            // Draw solid vertical line
                            ctx.beginPath();
                            ctx.setLineDash([]); // Solid line
                            ctx.moveTo(x, gridCanvas.padding - 10);
                            ctx.lineTo(x, height - gridCanvas.padding);
                            ctx.stroke();
                            
                            // Draw label with dark red background and white text
                            var timeText = markerTime.toFixed(1);
                            var textWidth = ctx.measureText(timeText).width;
                            var labelWidth = textWidth + 8; // Add some padding
                            var labelHeight = 16;
                            
                            // Draw background rectangle
                            ctx.fillStyle = markerColor;
                            ctx.fillRect(x - labelWidth/2, gridCanvas.padding - labelHeight - 10, labelWidth, labelHeight);
                            
                            // Draw text
                            ctx.fillStyle = "white";
                            ctx.font = "10px Arial";
                            ctx.fillText(timeText, x - textWidth/2, gridCanvas.padding - 14);
                        }
                    }
                }

                // Helper function to draw curve segments
                function drawCurve(ctx) {
                    ctx.strokeStyle = lineColor
                    ctx.lineWidth = 2

                    for (var i = 0; i < points.length - 1; i++) {
                        var p1 = points[i]
                        var p2 = points[i + 1]

                        var x1 = padding + (p1.time - viewportPosition) * (width - 2 * padding) / viewportTimeRange
                    var y1 = padding + (height - 2 * padding) * (1 - p1.value / 100)
                    var x2 = padding + (p2.time - viewportPosition) * (width - 2 * padding) / viewportTimeRange
                    var y2 = padding + (height - 2 * padding) * (1 - p2.value / 100)

                        ctx.beginPath()
                        ctx.moveTo(x1, y1)

                        if (p1.type === pointType.BEZIER || p2.type === pointType.BEZIER) {
                            // Calculate control points for bezier curve
                            var cp1x = x1 + (x2 - x1) * 0.5
                            var cp1y = y1
                            var cp2x = x2 - (x2 - x1) * 0.5
                            var cp2y = y2

                            // Adjust control points based on point types
                            if (p1.type === pointType.BEZIER) {
                                cp1x = x1 + (x2 - x1) * 0.25
                                cp1y = y1
                            }
                            if (p2.type === pointType.BEZIER) {
                                cp2x = x2 - (x2 - x1) * 0.25
                                cp2y = y2
                            }

                            ctx.bezierCurveTo(cp1x, cp1y, cp2x, cp2y, x2, y2)
                        } else {
                            ctx.lineTo(x2, y2)
                        }

                        ctx.stroke()
                    }
                }

                // Helper function to draw control points
                function drawControlPoints(ctx) {
                    ctx.fillStyle = lineColor
                    const pointSize = 8

                    for (var i = 0; i < points.length; i++) {
                        var p = points[i]
                        var x = gridCanvas.padding + (p.time - viewportPosition) * (width - 2 * gridCanvas.padding) / viewportTimeRange
                        var y = gridCanvas.padding + (height - 2 * gridCanvas.padding) * (1 - p.value / 100)

                        ctx.beginPath()
                        if (p.type === pointType.BEZIER) {
                            // Draw a circle for bezier points
                            ctx.arc(x, y, pointSize/2 + 1, 0, Math.PI * 2)
                        } else {
                            // Draw a square for linear points
                            ctx.rect(x - pointSize/2, y - pointSize/2, pointSize, pointSize)
                        }
                        ctx.fill()
                    }
                }

                // Helper function to draw horizontal grid
                function drawHorizontalGrid(ctx) {
                    // Major grid lines (20%)
                    ctx.strokeStyle = Qt.rgba(1, 1, 1, gridOpacity)
                    ctx.lineWidth = 1
                    for (var i = 0; i <= 100; i += 20) {
                        var y = padding + (height - 2 * padding) * (1 - i / 100)
                        ctx.beginPath()
                        ctx.moveTo(padding, y)
                        ctx.lineTo(width - padding, y)
                        ctx.stroke()
                    }

                    // Minor grid lines (10%)
                    ctx.strokeStyle = Qt.rgba(1, 1, 1, gridOpacity * 0.2)
                    for (var i = 0; i <= 100; i += 10) {
                        if (i % 20 !== 0) {
                            var y = padding + (height - 2 * padding) * (1 - i / 100)
                            ctx.beginPath()
                            ctx.moveTo(padding, y)
                            ctx.lineTo(width - padding, y)
                            ctx.stroke()
                        }
                    }
                }

                // Helper function to draw vertical grid
                function drawVerticalGrid(ctx) {
                    // Major grid lines (5s)
                    ctx.strokeStyle = Qt.rgba(1, 1, 1, gridOpacity)
                    var startTime = Math.floor(viewportPosition / 5) * 5
                    var endTime = startTime + viewportTimeRange + 5

                    for (var t = startTime; t <= endTime; t += 5) {
                        var x = padding + (t - viewportPosition) * (width - 2 * padding) / viewportTimeRange
                        if (x >= padding && x <= width - padding) {
                            ctx.beginPath()
                            ctx.moveTo(x, padding)
                            ctx.lineTo(x, height - padding)
                            ctx.stroke()
                        }
                    }

                    // Minor grid lines (1s)
                    ctx.strokeStyle = Qt.rgba(1, 1, 1, gridOpacity * 0.2)
                    startTime = Math.floor(viewportPosition)
                    endTime = startTime + viewportTimeRange + 1

                    for (var t = startTime; t <= endTime; t += 1) {
                        if (t % 5 !== 0) {
                            var x = padding + (t - viewportPosition) * (width - 2 * padding) / viewportTimeRange
                            if (x >= padding && x <= width - padding) {
                                ctx.beginPath()
                                ctx.moveTo(x, padding)
                                ctx.lineTo(x, height - padding)
                                ctx.stroke()
                            }
                        }
                    }
                }

                // Helper function to draw playhead
                function drawPlayhead(ctx) {
                    var x = padding + (playheadTime - viewportPosition) * (width - 2 * padding) / viewportTimeRange

                    // Only draw if playhead is within viewport
                    if (x >= padding && x <= width - padding) {
                        ctx.strokeStyle = playheadColor
                        ctx.fillStyle = playheadColor
                        ctx.lineWidth = 1

                        // Draw vertical line
                        ctx.beginPath()
                        ctx.moveTo(x, padding)
                        ctx.lineTo(x, height - padding)
                        ctx.stroke()

                        // Draw triangle at top
                        var triangleSize = 8
                        ctx.beginPath()
                        ctx.moveTo(x - triangleSize, padding - triangleSize)
                        ctx.lineTo(x + triangleSize, padding - triangleSize)
                        ctx.lineTo(x, padding)
                        ctx.closePath()
                        ctx.fill()
                    }
                }
            }
        }

        // Value axis (vertical)
        Rectangle {
            id: valueAxis
            width: 50
            anchors {
                left: parent.left
                top: parent.top
                bottom: timeAxis.top
            }
            color: backgroundColor

            // Value labels
            Repeater {
                model: 6 // 0%, 20%, 40%, 60%, 80%, 100%
                delegate: Text {
                    x: 20
                    y: gridCanvas.padding + (valueAxis.height - 2 * gridCanvas.padding) * (1 - index / 5) - height / 2
                    text: (index * 20) + "%"
                    color: "white"
                    font.pixelSize: 12
                }
            }
        }

        // Time axis (horizontal)
        Rectangle {
            id: timeAxis
            height: 30
            anchors {
                left: valueAxis.right
                right: parent.right
                bottom: parent.bottom
            }
            color: backgroundColor

            // Time labels (5s intervals)
            Repeater {
                model: Math.ceil(viewportTimeRange / 5) + 1
                delegate: Text {
                    property real timeValue: Math.floor(viewportPosition / 5) * 5 + index * 5
                    x: gridCanvas.padding + (timeValue - viewportPosition) * (timelineArea.width - 2 * gridCanvas.padding) / viewportTimeRange - width / 2
                    y: -2
                    text: timeValue // + "s"
                    color: "white"
                    font.pixelSize: 12
                    visible: x >= -width && x <= timeAxis.width
                }
            }
        }
    }

    // Mouse area for interaction
    MouseArea {
        id: timelineMouseArea
        parent: timelineArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        property point lastPos
        property real dragThreshold: 1  // Minimum pixels to trigger drag
        property int draggingPointIndex: -1  // Index of currently dragged point (-1 means none)
        property int clickedPointIndex: -1 // Index of clicked point for double click detection
        property int clickedMarkerIndex: -1 // Index of clicked marker for double click detection
        property bool isDoubleClick: false // Flag to indicate double click
        property real curveClickThreshold: 5 // Maximum distance from curve to register a click
        property bool isDraggingPlayhead: false // Flag to indicate if playhead is being dragged
        property int draggingMarkerIndex: -1 // Index of currently dragged marker (-1 means none)

        // Helper function to check if click is on curve
        function isClickOnCurve(mouseX, mouseY) {
            for (var i = 0; i < points.length - 1; i++) {
                var p1 = points[i];
                var p2 = points[i + 1];

                var x1 = gridCanvas.padding + (p1.time - viewportPosition) * (width - 2 * gridCanvas.padding) / viewportTimeRange;
                var y1 = gridCanvas.padding + (height - 2 * gridCanvas.padding) * (1 - p1.value / 100);
                var x2 = gridCanvas.padding + (p2.time - viewportPosition) * (width - 2 * gridCanvas.padding) / viewportTimeRange;
                var y2 = gridCanvas.padding + (height - 2 * gridCanvas.padding) * (1 - p2.value / 100);

                // For linear segments, check distance to line segment
                if (p1.type === pointType.LINEAR && p2.type === pointType.LINEAR) {
                    if (distanceToLineSegment(mouseX, mouseY, x1, y1, x2, y2) < curveClickThreshold) {
                        return true;
                    }
                }
                // For bezier segments, approximate with multiple line segments
                else {
                    var steps = 20;
                    var lastX = x1, lastY = y1;

                    for (var t = 1; t <= steps; t++) {
                        var progress = t / steps;
                        var cp1x = x1 + (x2 - x1) * 0.5;
                        var cp1y = y1;
                        var cp2x = x2 - (x2 - x1) * 0.5;
                        var cp2y = y2;

                        if (p1.type === pointType.BEZIER) {
                            cp1x = x1 + (x2 - x1) * 0.25;
                        }
                        if (p2.type === pointType.BEZIER) {
                            cp2x = x2 - (x2 - x1) * 0.25;
                        }

                        var currentX = bezierPoint(x1, cp1x, cp2x, x2, progress);
                        var currentY = bezierPoint(y1, cp1y, cp2y, y2, progress);

                        if (distanceToLineSegment(mouseX, mouseY, lastX, lastY, currentX, currentY) < curveClickThreshold) {
                            return true;
                        }

                        lastX = currentX;
                        lastY = currentY;
                    }
                }
            }
            return false;
        }

        // Helper function to calculate distance from point to line segment
        function distanceToLineSegment(px, py, x1, y1, x2, y2) {
            var A = px - x1;
            var B = py - y1;
            var C = x2 - x1;
            var D = y2 - y1;

            var dot = A * C + B * D;
            var len_sq = C * C + D * D;
            var param = -1;

            if (len_sq !== 0) {
                param = dot / len_sq;
            }

            var xx, yy;

            if (param < 0) {
                xx = x1;
                yy = y1;
            } else if (param > 1) {
                xx = x2;
                yy = y2;
            } else {
                xx = x1 + param * C;
                yy = y1 + param * D;
            }

            var dx = px - xx;
            var dy = py - yy;

            return Math.sqrt(dx * dx + dy * dy);
        }

        // Helper function to calculate point on bezier curve
        function bezierPoint(p0, p1, p2, p3, t) {
            var mt = 1 - t;
            return mt * mt * mt * p0 + 3 * mt * mt * t * p1 + 3 * mt * t * t * p2 + t * t * t * p3;
        }

        // Handle mouse press event
        onPressed: {
            // Reset double click flag
            isDoubleClick = false;
            clickedPointIndex = -1;
            clickedMarkerIndex = -1;

            // Check if clicked on playhead triangle
            var playheadX = gridCanvas.padding + (playheadTime - viewportPosition) * (width - 2 * gridCanvas.padding) / viewportTimeRange;
            var triangleSize = 7.5;
            if (Math.abs(mouse.x - playheadX) < triangleSize && mouse.y < gridCanvas.padding && mouse.y > 20) {
                isDraggingPlayhead = true;
                lastPos = Qt.point(mouse.x, mouse.y);
                return;
            }

            // Check if clicked on a marker label
            for (var i = 0; i < markers.length; i++) {
                var markerTime = markers[i];
                var x = gridCanvas.padding + (markerTime - viewportPosition) * (width - 2 * gridCanvas.padding) / viewportTimeRange;
                
                // Check if click is on the marker label (at the top)
                var timeText = markerTime.toFixed(1);
                var textWidth = timeText.length * 6; // Estimate text width
                var labelWidth = textWidth + 8; // Same as in drawMarkers
                var labelHeight = 16; // Same as in drawMarkers
                
                if (mouse.x >= x - labelWidth/2 && mouse.x <= x + labelWidth/2 && 
                    mouse.y >= 0 && mouse.y <= gridCanvas.padding) {
                    console.log("Clicked on marker label", i);
                    draggingMarkerIndex = i;
                    clickedMarkerIndex = i;
                    lastPos = Qt.point(mouse.x, mouse.y);
                    return;
                }
            }

            // Check if clicked on a control point
            var pointSize = 10; // Larger hit area for easier selection
            for (var i = 0; i < points.length; i++) {
                var p = points[i];
                var x = gridCanvas.padding + (p.time - viewportPosition) * (width - 2 * gridCanvas.padding) / viewportTimeRange;
                var y = gridCanvas.padding + (height - 2 * gridCanvas.padding) * (1 - p.value / 100);

                if (Math.abs(mouse.x - x) < pointSize && Math.abs(mouse.y - y) < pointSize) {

                    console.log("Clicked on point", i);

                    saveToHistory(); // Save state before dragging point
                    timelineMouseArea.draggingPointIndex = i;
                    clickedPointIndex = i;
                    lastPos = Qt.point(mouse.x, mouse.y);
                    return;
                }
            }

            // Create new point only if clicking on curve and not on existing point
            if ((mouse.button === Qt.LeftButton || mouse.button === Qt.RightButton) && isClickOnCurve(mouse.x, mouse.y)) {

                console.log("Clicked on curve");

                saveToHistory(); // Save state before adding new point
                var newTime = viewportPosition + (mouse.x - gridCanvas.padding) * viewportTimeRange / (width - 2 * gridCanvas.padding);
                var newValue = 100 * (1 - (mouse.y - gridCanvas.padding) / (height - 2 * gridCanvas.padding));

                // Create new point with appropriate type
                var newPoint = {
                    time: Math.max(0, Math.min(totalTimeRange, newTime)),
                    value: Math.max(0, Math.min(100, newValue)),
                    type: mouse.button === Qt.RightButton ? pointType.BEZIER : pointType.LINEAR
                };

                // Find the correct insertion position based on time
                var insertIndex = points.findIndex(p => p.time > newPoint.time);
                if (insertIndex === -1) insertIndex = points.length;

                // Insert the new point at the correct position
                points.splice(insertIndex, 0, newPoint);
                points = points; // Trigger property change

                // Set dragging index to the insertion position
                timelineMouseArea.draggingPointIndex = insertIndex;
                lastPos = Qt.point(mouse.x, mouse.y);
                gridCanvas.requestPaint();
            } else {
                // Default behavior for viewport panning
                lastPos = Qt.point(mouse.x, mouse.y);
                timelineMouseArea.draggingPointIndex = -1;
            isDraggingPlayhead = false;
            }
        }

        // Handle double click to delete point or set playhead position
        onDoubleClicked: {
            // Check if double clicked on a marker label
            if (clickedMarkerIndex >= 0 && clickedMarkerIndex < markers.length) {
                console.log("Double clicked on marker", clickedMarkerIndex);
                
                // Remove the marker at the clicked index
                markers.splice(clickedMarkerIndex, 1);
                isMarkersModified = true;
                
                // Reset the clicked marker index
                clickedMarkerIndex = -1;
                draggingMarkerIndex = -1;
                
                // Request repaint to update the canvas
                gridCanvas.requestPaint();
                return;
            }
            
            // Check if double clicked on a control point
            if (clickedPointIndex >= 0 && clickedPointIndex < points.length) {
                // Don't allow deleting the first point or last point
                if (clickedPointIndex > 0 && clickedPointIndex < points.length - 1) {
                    console.log("Double clicked on point", clickedPointIndex);

                    // Update history stack:
                    // Two mouse pressed events were triggered before this double click
                    // So already add the state to the history stack two times
                    // So pop the last state from the history stack here and never add it again
                    if (historyStack.length > 1) historyStack.pop();

                    points.splice(clickedPointIndex, 1);
                    points = points; // Trigger property change
                    gridCanvas.requestPaint();
                }
            } else {

                console.log("Double clicked on empty area");

                // Double click on empty area - update playhead position
                var newPlayheadTime = viewportPosition + (mouse.x - gridCanvas.padding) * viewportTimeRange / (width - 2 * gridCanvas.padding);
                playheadTime = Math.max(0, Math.min(totalTimeRange, newPlayheadTime));
                gridCanvas.requestPaint();
            }
        }

        // Handle mouse drag event for viewport panning and point dragging
        onReleased: {
            isDraggingPlayhead = false;
            timelineMouseArea.draggingPointIndex = -1;
            draggingMarkerIndex = -1;
            pointInfoLabel.visible = false;
        }

        onPressAndHold: {
            positionChanged(mouse);
        }

        onPositionChanged: {
            if (pressed) {
                if (isDraggingPlayhead) {
                    var dx = mouseX - lastPos.x;
                    var timeDelta = dx * viewportTimeRange / (width - 2 * gridCanvas.padding);
                    var newPosition = Math.max(0, Math.min(totalTimeRange, playheadTime + timeDelta));
                    playheadTime = newPosition;
                    lastPos = Qt.point(mouseX, mouseY);
                    gridCanvas.requestPaint();
                } else if (draggingMarkerIndex >= 0) {
                    // Dragging a marker
                    var timeDelta = (mouse.x - lastPos.x) * viewportTimeRange / (width - 2 * gridCanvas.padding);
                    var newTime = Math.max(0, Math.min(totalTimeRange, markers[draggingMarkerIndex] + timeDelta));
                    
                    // Update the marker position
                    markers[draggingMarkerIndex] = newTime;                    
                    markers.sort(function(a, b) { return a - b; }); // Sort markers by time
                    isMarkersModified = true;
                    
                    // Update draggingMarkerIndex to match the new position in the sorted array
                    draggingMarkerIndex = markers.indexOf(newTime);
                    
                    lastPos = Qt.point(mouseX, mouseY);
                    gridCanvas.requestPaint();
                } else if (timelineMouseArea.draggingPointIndex >= 0) {
                    // Dragging a control point
                    var point = points[timelineMouseArea.draggingPointIndex];
                    var dx = mouseX - lastPos.x;
                    var dy = mouseY - lastPos.y;

                    // Calculate new time and value
                    var newTime = point.time;
                    var newValue = point.value - dy * 100 / (height - 2 * gridCanvas.padding);

                    // For first point, lock time at 0
                    if (timelineMouseArea.draggingPointIndex === 0) {
                        newTime = 0;
                    } else {
                        newTime = point.time + dx * viewportTimeRange / (width - 2 * gridCanvas.padding);

                        // Get previous and next point times for constraints
                        var prevTime = points[timelineMouseArea.draggingPointIndex - 1].time;
                        var nextTime = timelineMouseArea.draggingPointIndex < points.length - 1 ?
                            points[timelineMouseArea.draggingPointIndex + 1].time : totalTimeRange;

                        // Constrain time between previous and next points
                        newTime = Math.max(prevTime, Math.min(nextTime, newTime));
                    }

                    // Update point position with constraints
                    point.time = Math.max(0, Math.min(totalTimeRange, newTime));
                    point.value = Math.max(0, Math.min(100, newValue));

                    // Update points array to trigger property change
                    points = points;

                    // Update and show point info label
                    var actualValue = calculateActualValue(point.value);
                    // Three lines:
                    timeText.text = point.time.toFixed(1) + "s";
                    percentText.text = point.value.toFixed(1) + "%";
                    actualValueText.text = actualValue.toFixed(0);

                    // Calculate control point's actual position
                    var pointX = gridCanvas.padding + (point.time - viewportPosition) * (width - 2 * gridCanvas.padding) / viewportTimeRange;
                    var pointY = gridCanvas.padding + (height - 2 * gridCanvas.padding) * (1 - point.value / 100);

                    // Calculate label position with boundary checks
                    var labelX = Math.max(gridCanvas.padding, Math.min(width - pointInfoLabel.width - gridCanvas.padding, pointX - pointInfoLabel.width / 2 + 50));

                    // Determine if label should be below or above the point
                    var topSpace = pointY - gridCanvas.padding;
                    var labelY;
                    if (topSpace < pointInfoLabel.height + 30) {
                        // Show below point if close to top
                        labelY = pointY + 20;
                    } else {
                        // Show above point
                        labelY = pointY - pointInfoLabel.height - 20;
                    }

                    pointInfoLabel.x = labelX;
                    pointInfoLabel.y = labelY;
                    pointInfoLabel.visible = true;

                    gridCanvas.requestPaint();
                    lastPos = Qt.point(mouseX, mouseY);
                } else {
                    // Default viewport panning behavior
                    var dx = mouseX - lastPos.x;

                    // Only update if movement exceeds threshold
                    if (Math.abs(dx) >= dragThreshold) {
                        var timeDelta = dx / timeScale;
                        var newPosition = Math.max(0, Math.min(totalTimeRange - viewportTimeRange,
                                                              viewportPosition - timeDelta));

                        viewportPosition = newPosition;
                        lastPos = Qt.point(mouseX, mouseY);
                        gridCanvas.requestPaint();
                    }
                }
            }
        }

        // Handle mouse wheel event for zooming
        onWheel: {
            wheel.accepted = true

            // Calculate zoom direction and step size (5 seconds)
            var zoomStep = 5
            var zoomIn = wheel.angleDelta.y > 0
            var newRange = viewportTimeRange + (zoomIn ? -zoomStep : zoomStep)

            // Constrain zoom range between 5 seconds and 90 seconds
            newRange = Math.max(5, Math.min(90, newRange))


            if (newRange !== viewportTimeRange) {
                // Calculate time position under mouse cursor before zoom
                var mouseTimePos = viewportPosition + (mouseX / timeScale)

                // Update viewport range and scale
                viewportTimeRange = newRange
                timeScale = (timelineArea.width) / viewportTimeRange

                // Calculate new viewport position to keep mouse point at same time
                var targetMouseX = mouseX
                var newViewportPosition = mouseTimePos - (targetMouseX / timeScale)

                // Constrain viewport position within valid range
                viewportPosition = Math.max(0, Math.min(totalTimeRange - viewportTimeRange, newViewportPosition))
                gridCanvas.requestPaint()
            }
        }
    }

    // Update grid when viewport changes
    onViewportPositionChanged: gridCanvas.requestPaint()
    onViewportTimeRangeChanged: gridCanvas.requestPaint()
    onWidthChanged: {
        timeScale = timelineArea.width / viewportTimeRange
        gridCanvas.requestPaint()
    }
    onHeightChanged: gridCanvas.requestPaint()
    
}

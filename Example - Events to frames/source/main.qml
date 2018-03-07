import QtQuick 2.3
import QtQuick.Layouts 1.1
import BackgroundCleaner 1.0
//import ChangeDetectionDisplay 1.0
import LogarithmicDisplay 1.0
import FrameGenerator 1.0

Item {
    id: item
    visible: true
    width: 304
    height: 240

    Timer {
        interval: 20
        running: true
        repeat: true
        onTriggered: {
            backgroundCleaner.triggerDraw();
            //changeDetectionDisplay.triggerDraw();
            logarithmicDisplay.triggerDraw();
            frameGenerator.triggerDraw();
        }
    }

    FrameGenerator {
        objectName: "frameGenerator"
        id: frameGenerator
        width: item.width
        height: item.height
    }

    BackgroundCleaner {
        id: backgroundCleaner
        width: item.width
        height: item.height
    }

    RowLayout {
        anchors.rightMargin: 0
        anchors.leftMargin: 0
        anchors.bottomMargin: 0
        anchors.topMargin: 0
        anchors.fill: parent
        spacing: 0

        /*
        ChangeDetectionDisplay {
            objectName: "changeDetectionDisplay"
            id: changeDetectionDisplay
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            canvasSize: "304x240"
        }
        */

        LogarithmicDisplay {
            objectName: "logarithmicDisplay"
            id: logarithmicDisplay
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            canvasSize: "304x240"
        }
    }
}

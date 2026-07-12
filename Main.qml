import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    width: 780
    height: 500
    visible: true
    title: qsTr("Manager / Presenter")

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Label {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTr("Her panel: Manager (ayrı thread) → Presenter (main thread, for ile QML'e yazar).")
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 16

            Frame {
                Layout.fillWidth: true
                Layout.fillHeight: true

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 8

                    Label {
                        text: presenter1.title
                        font.bold: true
                        font.pixelSize: 16
                    }

                    Label {
                        text: qsTr("Sayaç: %1").arg(presenter1.count)
                        font.pixelSize: 22
                    }

                    ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true

                        TextArea {
                            readOnly: true
                            wrapMode: TextEdit.Wrap
                            text: presenter1.displayText
                        }
                    }

                    Button {
                        Layout.fillWidth: true
                        text: presenter1.running ? qsTr("Durdur") : qsTr("Başlat")
                        onClicked: presenter1.running ? presenter1.stop() : presenter1.start()
                    }
                }
            }

            Frame {
                Layout.fillWidth: true
                Layout.fillHeight: true

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 8

                    Label {
                        text: presenter2.title
                        font.bold: true
                        font.pixelSize: 16
                    }

                    Label {
                        text: qsTr("Sayaç: %1").arg(presenter2.count)
                        font.pixelSize: 22
                    }

                    ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true

                        TextArea {
                            readOnly: true
                            wrapMode: TextEdit.Wrap
                            text: presenter2.displayText
                        }
                    }

                    Button {
                        Layout.fillWidth: true
                        text: presenter2.running ? qsTr("Durdur") : qsTr("Başlat")
                        onClicked: presenter2.running ? presenter2.stop() : presenter2.start()
                    }
                }
            }
        }
    }
}

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    width: 820
    height: 520
    visible: true
    title: qsTr("Host — IPC")

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Label {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTr("Host UI. Sub-apps: subapp-a / subapp-b (separate exe).\nIPC: %1")
                  .arg(host.serverName)
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
                        text: qsTr("subapp-a")
                        font.bold: true
                        font.pixelSize: 16
                    }

                    Label {
                        text: qsTr("Sayaç: %1").arg(host.count1)
                        font.pixelSize: 22
                    }

                    ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true

                        TextArea {
                            readOnly: true
                            wrapMode: TextEdit.Wrap
                            text: host.text1
                        }
                    }

                    Button {
                        Layout.fillWidth: true
                        text: host.running1 ? qsTr("Durdur") : qsTr("Başlat")
                        onClicked: host.running1 ? host.stopWorker1() : host.startWorker1()
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
                        text: qsTr("subapp-b")
                        font.bold: true
                        font.pixelSize: 16
                    }

                    Label {
                        text: qsTr("Sayaç: %1").arg(host.count2)
                        font.pixelSize: 22
                    }

                    ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true

                        TextArea {
                            readOnly: true
                            wrapMode: TextEdit.Wrap
                            text: host.text2
                        }
                    }

                    Button {
                        Layout.fillWidth: true
                        text: host.running2 ? qsTr("Durdur") : qsTr("Başlat")
                        onClicked: host.running2 ? host.stopWorker2() : host.startWorker2()
                    }
                }
            }
        }
    }
}

import QtQuick
import QtQuick.Window
import QtQuick.Controls
import  "./HostStatusView.qml"
import  "./InfoView.qml"
Window {
    id: mainWindow
    width: 800
    height: 600
    visible: true
    title: qsTr("Client")
    Rectangle {
        id: root
        anchors.fill: parent
        color: "white"
         Row {
            anchors.centerIn: parent
            spacing: 20
          HostStatusView {
              id: hostStatusView
            }
           InfoView {
              id: infoView
              onActionPerformed: (id) => {
                     client.operationResult = id
               }
            }
        }
    }
}

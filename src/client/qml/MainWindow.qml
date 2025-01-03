import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: "Qt Client"

    Column {
        anchors.centerIn: parent

        Button {
            text: "Connect to Server"
            onClicked: {
                client.connectToServer();
            }
        }

        Button {
            text: "Send Message"
            onClicked: {
                client.sendMessage("Hello, Server!");
            }
        }
    }
}

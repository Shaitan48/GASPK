import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    width: 400
    height: 300
    color: "lightgray"

     ListView {
        id: hostListView
        anchors.fill: parent
        model: hostModel
        delegate: Rectangle {
            width: parent.width
            height: 40
            color: "white"
            Row{
                anchors.centerIn: parent
                spacing: 10
            Rectangle {
                width: 20
                height: 20
                radius: 10
                color: status === "online" ? "green" : "red"
             }
            Text {
               text: host
            }
        }
        }
    }
     ListModel {
         id: hostModel
     }

  function updateHostStatus(host, status){
       for(var i = 0; i < hostModel.count; i++){
           if(hostModel.get(i).host === host){
                 hostModel.setProperty(i, "status", status);
             return;
           }
        }
         hostModel.append({"host": host, "status": status})
   }
}
import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    width: 400
    height: 300
    color: "lightgray"

   ListView {
     id: messageListView
      anchors.fill: parent
       model: messageModel
       delegate: Rectangle {
            width: parent.width
            height: 60
            color:  "white"
           Text {
             anchors.centerIn: parent
              text: message
             }
            MouseArea {
                anchors.fill: parent
               onClicked: {
                     if(user_action_required){
                       root.actionPerformed(id)
                        }
                    }
                }
       }
 }
     ListModel {
        id: messageModel
     }
   function addMessage(id, message, user_action_required){
    messageModel.append({"id": id, "message": message, "user_action_required": user_action_required})
   }
   function  clearMessages(){
         messageModel.clear();
    }
   signal actionPerformed(string id);
}
import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.3
import QtWebSockets 1.1
import Qt.labs.settings 1.1
import myy.helpers.websocket 1.0

Window {
    id: window
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Item {
        id: config
        property string pixabay_key: ""
    }

    Babar {
        id: toolbar
        x: 0
        anchors.bottom: parent.bottom

        searchButton.onClicked: {
            var http = new XMLHttpRequest()
            var url = "https://pixabay.com/api/?image_type=photo&q=" + query.text + "&key=" + config.pixabay_key;
            http.open("GET", url, true);
            http.onreadystatechange = function() {
              if (http.readyState === 4) {
                  if (http.status === 200) {
                      results.addPixabayResults(JSON.parse(http.responseText))
                      console.debug(http.responseText);
                  }
                  else {
                      console.debug("XC")
                  }
              }
            }
            http.send();

        }
    }

    MyyWebSocketServer {
        id: server
        bindPort: 8080
        bindAddress: "localhost"

        Component.onCompleted: {
            server.listen();
        }
    }

    /*WebSocketServer {
        id: server
        port: 8080
        accept: true
        listen: true
        name: "Cochon D'inde King"
        property var socket
        onClientConnected: {
            webSocket.onTextMessageReceived.connect(function(message) {
                webSocket.sendTextMessage(qsTr("Bark bark bark"));
                console.log("Connected");

            });
            socket = webSocket
        }

        onErrorStringChanged: {
            console.log(qStr("Piou"));
        }

    }*/

    ScrollView {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: toolbar.top
        contentHeight: results.height
        contentWidth: results.width

        Grid {
            id: results
            function pixabayOnClick() {

            }

            function addPixabayResult(json_data) {
                var component = Qt.createComponent("PixabayResult.qml")
                var pix_result = component.createObject(results)
                pix_result.ws = server.socket
                pix_result.picture.data = json_data;

            }
            function addPixabayResults(json_data) {
                const hits = json_data["hits"];
                for (var i = 0; i < hits.length; i++)
                    results.addPixabayResult(hits[i]);
            }

        }
    }


    Settings {
        id: settings
        Component.onCompleted: {
            config.pixabay_key = settings.value("pixabay_key", "")
            //console.log(config.pixabay_key)
        }
        Component.onDestruction: {
            settings.setValue("pixabay_key", config.pixabay_key)
            //console.log("Saving key : " + config.pixabay_key)
        }
    }

}

/*##^##
Designer {
    D{i:1;anchors_width:640;anchors_x:0}
}
##^##*/

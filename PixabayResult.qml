import QtQuick 2.0
import QtQuick.Layouts 1.12

/* Why a Rectangle ?
 *
 * Because it works !
 * I tried putting Image as the root object but if I put
 * a MouseArea in it, onClick never works for "reasons".
 * Well, the reason being that the MouseArea ends up with
 * a 0x0 size.
 *
 * Seriously, the thing JUST DOESN'T WORK with MouseArea
 * as a child, in this specific configuration.
 *
 * So I gave up and use a Rectangle.
 * Now it only works when the width and height are set
 * back during a callback.
 * Anyway MouseArea syncs very well with Rectangle width
 * and height modifications, but NOT with Image width and
 * height modifications.
 *
 * Go figure !
 */
Rectangle {
    id: mainarea
    x: 0
    y: 0


    property alias picture: image
    Image {
        id: image
        visible: true
        sourceSize:"250x250"
        anchors.fill: parent

        property var data
        property string sourceID
        property string thumbnailURL
        property int thumbnailWidth
        property int thumbnailHeight

        onDataChanged: function() {
            sourceID = data["id"];
            thumbnailURL = data["previewURL"];
            thumbnailWidth = data["previewWidth"];
            thumbnailHeight = data["previewHeight"];
            source = thumbnailURL;
            parent.width = thumbnailWidth;
            parent.height = thumbnailHeight;
            console.log("New content : " + source + " - " + width + " x " + height);
        }
    }
    MouseArea {
        id: area
        anchors.fill: parent
        onClicked: { console.log(picture.data["largeImageURL"]); }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

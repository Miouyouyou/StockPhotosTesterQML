import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.3

ToolBar {
    id: babar
    anchors.left: parent.left
    anchors.right: parent.right

    property alias query: query
    property alias searchButton: button
    TextField {
        id: query
        width: 377
        text: qsTr("")
        anchors.left: parent.left
        anchors.leftMargin: 0
        transformOrigin: Item.Left
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 0
    }
    ComboBox {
        id: providers
        x: 384
        y: 0
        anchors.left: query.right
        model: ["Pixabay"]
    }

    Button {
        id: button
        x: 583
        y: 0
        text: qsTr("🔍")
        topPadding: 0
        anchors.left: providers.right
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: parent.top
    }


}

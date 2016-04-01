import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import com.bgreeves.aside 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    minimumWidth: 400
    minimumHeight: 300

    title: document.documentTitle + " - asIDE"

    MessageDialog {
        id: aboutBox
        title: "About Text"
        text: "This is a basic text editor \nwritten with Qt Quick Controls"
        icon: StandardIcon.Information
    }

    Action {
        id: cutAction
        text: "Cut"
        shortcut: StandardKey.Cut
        iconSource: "/images/editcut.png"
        iconName: "edit-cut"
        onTriggered: textArea.cut()
    }

    Action {
        id: copyAction
        text: "Copy"
        shortcut: StandardKey.Copy
        iconSource: "/images/editcopy.png"
        iconName: "edit-copy"
        onTriggered: textArea.copy()
    }

    Action {
        id: pasteAction
        text: "Paste"
        shortcut: StandardKey.Paste
        iconSource: "/images/editpaste.png"
        iconName: "edit-paste"
        onTriggered: textArea.paste()
    }

    // TODO
    Action {
        id: undoAction
        text: "Undo"
        shortcut: StandardKey.Undo
        iconSource: "/images/editundo.png"
        iconName: "edit-undo"
    }

    // TODO
    Action {
        id: redoAction
        text: "Redo"
        shortcut: StandardKey.Redo
        iconSource: "/images/editredo.png"
        iconName: "edit-redo"
    }

    FileDialog {
        id: fileDialog
        nameFilters: ["All files (*.*)"]
        onAccepted: {
            if (fileDialog.selectExisting)
                document.fileUrl = fileUrl
            else
                document.saveAs(fileUrl, selectedNameFilter)
        }
    }

    // TODO
    Action {
        id: fileNewAction
        shortcut: StandardKey.New
        iconSource: "/images/filenew.png"
        iconName: "document-new"
        text: "New"
    }

    Action {
        id: fileOpenAction
        shortcut: StandardKey.Open
        iconSource: "/images/fileopen.png"
        iconName: "document-open"
        text: "Open"
        onTriggered: {
            fileDialog.selectExisting = true
            fileDialog.open()
        }
    }

    Action {
        id: fileSaveAsAction
        shortcut: StandardKey.SaveAs
        iconSource: "/images/filesave.png"
        iconName: "document-save"
        text: "Save As…"
        onTriggered: {
            fileDialog.selectExisting = false
            fileDialog.open()
        }
    }

    menuBar: MenuBar {
        Menu {
            title: "&File"
            MenuItem { action: fileOpenAction }
            MenuItem { action: fileSaveAsAction }
            MenuItem { text: "Quit"; onTriggered: Qt.quit() }
        }
        Menu {
            title: "&Edit"
            MenuItem { action: copyAction }
            MenuItem { action: cutAction }
            MenuItem { action: pasteAction }
        }
        Menu {
            title: "F&ormat"
            MenuItem {
                text: "&Color ..."
                onTriggered: {
                    colorDialog.color = document.textColor
                    colorDialog.open()
                }
            }
        }
        Menu {
            title: "&Help"
            MenuItem { text: "About..." ; onTriggered: aboutBox.open() }
        }
    }

    toolBar: ToolBar {
        id: mainToolBar
        width: parent.width
        RowLayout {
            anchors.fill: parent
            spacing: 0

            ToolButton { action: fileNewAction }
            ToolButton { action: fileOpenAction }
            ToolButton { action: fileSaveAsAction }

            ToolBarSeparator {}

            ToolButton { action: undoAction }
            ToolButton { action: redoAction }

            ToolBarSeparator {}

            ToolButton { action: copyAction }
            ToolButton { action: cutAction }
            ToolButton { action: pasteAction }

            Item { Layout.fillWidth: true }
        }
    }

    TextArea {
        Accessible.name: "document"
        id: textArea
        frameVisible: false
        width: parent.width
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        baseUrl: "qrc:/"
        text: document.text
        textFormat: Qt.PlainText
        Component.onCompleted: forceActiveFocus()
        style: TextAreaStyle {
            font.family: "Courier"
            font.pointSize: 14
        }
    }

    MessageDialog {
        id: errorDialog
    }

    DocumentHandler {
        id: document
        target: textArea
        cursorPosition: textArea.cursorPosition
        selectionStart: textArea.selectionStart
        selectionEnd: textArea.selectionEnd
        Component.onCompleted: document.fileUrl = "qrc:/example.html"
        onError: {
            errorDialog.text = message
            errorDialog.visible = true
        }
    }
}

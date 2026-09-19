import QtQuick 2.6
import Sailfish.Silica 1.0

Page {
    id: page
    objectName: "aboutPage"

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: contentColumn.height + Theme.paddingLarge

        Column {
            id: contentColumn
            width: page.width
            spacing: Theme.paddingMedium

            PageHeader {
                title: qsTr("About")
            }

            Item {
                width: parent.width
                height: Theme.paddingMedium
            }

            Image {
                anchors.horizontalCenter: parent.horizontalCenter
                source: Qt.resolvedUrl("../icons/harbour-thakir.png")
                width: Theme.iconSizeExtraLarge
                height: Theme.iconSizeExtraLarge
                fillMode: Image.PreserveAspectFit
                smooth: true
            }

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Thakir")
                font.pixelSize: Theme.fontSizeLarge
                font.bold: true
                color: Theme.highlightColor
            }

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Islamic prayer times and athan reminders for Sailfish OS")
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
            }

            Item {
                width: parent.width
                height: Theme.paddingMedium
            }

            SectionHeader {
                text: qsTr("Application information")
            }

            DetailItem {
                label: qsTr("Developer")
                value: qsTr("Mohamed HAFIANE")
            }

            ValueButton {
                width: parent.width
                label: qsTr("E-mail for feedback")
                value: "thakir.dz@gmail.com"
                description: qsTr("Tap to send an email")
                onClicked: {
                    var subject = "Thakir (" + prayerManager.appVersion + ") on Sailfish OS"
                    if (prayerManager.osVersion.length > 0) {
                        subject += " " + prayerManager.osVersion
                    }
                    Qt.openUrlExternally("mailto:thakir.dz@gmail.com?subject=" + encodeURIComponent(subject))
                }
            }

            DetailItem {
                label: qsTr("Version")
                value: prayerManager.formatDigits(prayerManager.appVersion)
            }

            DetailItem {
                visible: prayerManager.osVersion.length > 0
                label: qsTr("Sailfish OS")
                value: prayerManager.formatDigits(prayerManager.osVersion)
            }

            DetailItem {
                label: qsTr("Date")
                value: prayerManager.buildDate
            }
        }
    }
}

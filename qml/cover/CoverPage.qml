import QtQuick 2.6
import Sailfish.Silica 1.0

CoverBackground {
    function prayerDisplayName(prayerKey) {
        var _ = prayerManager.appLanguage
        if (prayerKey === "dhuhr") {
            var d = new Date()
            if (prayerManager.isNextPrayerTomorrow && prayerKey === prayerManager.nextPrayerName) {
                d.setDate(d.getDate() + 1)
            }
            if (d.getDay() === 5) {
                return qsTr("Friday prayer")
            }
        }
        switch (prayerKey) {
        case "fajr": return qsTr("Fajr")
        case "sunrise": return qsTr("Chourouq")
        case "dhuhr": return qsTr("Dhouhr")
        case "asr": return qsTr("Assar")
        case "maghrib": return qsTr("Maghreb")
        case "isha": return qsTr("Ishaa")
        default: return prayerKey ? (prayerKey.charAt(0).toUpperCase() + prayerKey.slice(1)) : ""
        }
    }

    Column {
        anchors.centerIn: parent
        spacing: Theme.paddingSmall
        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: prayerManager.hasCity ? prayerManager.cityName : (prayerManager.appLanguage, qsTr("Athan"))
            font.pixelSize: Theme.fontSizeMedium
        }
        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: prayerManager.hasCity ? prayerDisplayName(prayerManager.nextPrayerName) : ""
            color: Theme.highlightColor
            font.pixelSize: Theme.fontSizeLarge
        }
        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: prayerManager.hasCity ? prayerManager.nextPrayerTime : ""
            font.pixelSize: Theme.fontSizeExtraLarge
        }
    }
}

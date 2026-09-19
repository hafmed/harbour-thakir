import QtQuick 2.6
import Sailfish.Silica 1.0

Page {
    id: page
    objectName: "timeAdjustmentsPage"

    function prayerDisplayName(prayerKey) {
        var _ = prayerManager.appLanguage
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

    function formatDigits(str) {
        var _ = prayerManager.useHindiNumerals
        return prayerManager.formatDigits(str)
    }

    Connections {
        target: prayerManager
        onUse24HourFormatChanged: timeFormatCombo.currentIndex = prayerManager.use24HourFormat ? 1 : 0
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: page.width

            PageHeader { title: qsTr("Time Adjustments") }

            SectionHeader { text: qsTr("Time format") }

            ComboBox {
                id: timeFormatCombo
                label: qsTr("Time format")
                width: parent.width
                currentIndex: prayerManager.use24HourFormat ? 1 : 0
                menu: ContextMenu {
                    MenuItem { text: formatDigits(qsTr("12-hour (am / pm)")) }
                    MenuItem { text: formatDigits(qsTr("24-hour")) }
                }
                onCurrentIndexChanged: prayerManager.use24HourFormat = (currentIndex === 1)
            }

            SectionHeader { text: qsTr("Prayer time adjustments") }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                text: formatDigits(qsTr("Fine-tune each prayer time (±30 min) to match your local mosque."))
            }

            Repeater {
                model: ["fajr", "sunrise", "dhuhr", "asr", "maghrib", "isha"]
                delegate: Slider {
                    width: parent.width
                    minimumValue: -30
                    maximumValue: 30
                    stepSize: 1
                    value: {
                        var _ = prayerManager.activeFavoriteId
                        return prayerManager.prayerAdjustment(modelData)
                    }
                    label: qsTr("Adjust %1").arg(prayerDisplayName(modelData))
                    valueText: (value > 0 ? "+" : "") + formatDigits(Math.round(value).toString()) + " " + qsTr("min")
                    onSliderValueChanged: {
                        prayerManager.setPrayerAdjustment(modelData, Math.round(value))
                    }
                }
            }

            SectionHeader { text: qsTr("Hijri calendar") }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                text: formatDigits(qsTr("Adjust the Islamic date by ±3 days to match your local moon-sighting."))
            }

            ComboBox {
                id: dateAdjCombo
                label: qsTr("Date adjustment")
                width: parent.width
                property var adjustmentValues: [-3, -2, -1, 0, 1, 2, 3]
                currentIndex: {
                    var adj = prayerManager.hijriAdjustment
                    var idx = adjustmentValues.indexOf(adj)
                    return idx >= 0 ? idx : 3
                }
                menu: ContextMenu {
                    MenuItem { text: formatDigits(qsTr("-3 days")) }
                    MenuItem { text: formatDigits(qsTr("-2 days")) }
                    MenuItem { text: formatDigits(qsTr("-1 day")) }
                    MenuItem { text: formatDigits(qsTr("0 days (default)")) }
                    MenuItem { text: formatDigits(qsTr("+1 day")) }
                    MenuItem { text: formatDigits(qsTr("+2 days")) }
                    MenuItem { text: formatDigits(qsTr("+3 days")) }
                }
                onCurrentIndexChanged: {
                    if (currentIndex >= 0 && currentIndex < adjustmentValues.length) {
                        prayerManager.hijriAdjustment = adjustmentValues[currentIndex]
                    }
                }
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.highlightColor
                text: formatDigits(qsTr("Resulting date: %1").arg(prayerManager.hijriDate))
            }
        }
    }
}

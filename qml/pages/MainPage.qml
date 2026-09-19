import QtQuick 2.6
import Sailfish.Silica 1.0

Page {
    id: page
    objectName: "mainPage"

    property var currentTime: new Date()
    property string countdownText: ""
    property real progressValue: 0.0
    property int remainingPercent: 0
    property bool inPreAlert: false
    property string dailyWisdomText: prayerManager.dailyWisdom
    readonly property bool isRtl: Qt.application.layoutDirection === Qt.RightToLeft || prayerManager.isArabicLanguage

    Connections {
        target: prayerManager
        onUseHindiNumeralsChanged: updateCountdown()
        onAppLanguageChanged: updateCountdown()
        onTimesChanged: updateCountdown()
    }

    // Vertical spacing and scaling calculations:
    // Page height equals device height minus bottomBar.height so content
    // fills the entire height of the device perfectly without scrolling overflow.
    readonly property real availableContentHeight: page.height - bottomBar.height

    // Dynamic item heights scaling with available screen height
    readonly property real buttonHeight0: Math.max(Theme.itemSizeMedium * 1.35, Math.min(Theme.itemSizeLarge * 1.25, availableContentHeight * 0.115))
    readonly property real buttonHeight1: Math.max(Theme.itemSizeMedium * 1.2, Math.min(Theme.itemSizeLarge * 1.15, availableContentHeight * 0.095))
    readonly property real rowHeight1: buttonHeight1 + Theme.fontSizeSmall + Theme.paddingSmall
    readonly property real buttonHeight2: Math.max(Theme.itemSizeMedium * 1.2, Math.min(Theme.itemSizeLarge * 1.15, availableContentHeight * 0.095))
    readonly property real heroHeight3: Math.max(165, Math.min(240, availableContentHeight * 0.19))
    readonly property real listItemHeight3: Math.max(46, Math.min(58, availableContentHeight * 0.052))
    readonly property real listSpacing3: prayerManager.homeLayout === 3 ? Math.max(4, totalFreeSpace * 0.015) : 0

    readonly property real baseItemsHeight: {
        var h = headerCol.height > 0 ? headerCol.height : (Theme.fontSizeLarge + 2 * Theme.fontSizeMedium + Theme.paddingSmall)
        if (!prayerManager.hasCity) return h

        switch (prayerManager.homeLayout) {
        case 0:
            h += 3 * buttonHeight0
            break
        case 1:
            h += 3 * rowHeight1
            break
        case 2:
            h += 4 * buttonHeight2
            break
        case 3:
            h += heroHeight3 + 8 * listItemHeight3
            break
        default:
            h += 3 * buttonHeight0
            break
        }

        // Progress bar and countdown for layouts 0, 1, 2
        if (prayerManager.homeLayout !== 3 && prayerManager.nextPrayerName.length > 0) {
            h += 6 // progress bar
            h += countdownCol.height > 0 ? countdownCol.height : (Theme.fontSizeMedium + Theme.itemSizeMedium + Theme.paddingMedium)
        }

        // Wisdom container
        if (dailyWisdomText.length > 0) {
            h += wisdomContainer.height > 0 ? wisdomContainer.height : (Theme.fontSizeExtraSmall * 2 + Theme.paddingSmall * 2)
        }

        return h
    }

    readonly property real totalFreeSpace: Math.max(0, availableContentHeight - baseItemsHeight)

    readonly property real spacerTop: {
        switch (prayerManager.homeLayout) {
        case 2: return totalFreeSpace * 0.07
        case 3: return totalFreeSpace * 0.06
        default: return totalFreeSpace * 0.08
        }
    }
    readonly property real spacerHeader: {
        switch (prayerManager.homeLayout) {
        case 3: return totalFreeSpace * 0.12
        default: return totalFreeSpace * 0.14
        }
    }
    readonly property real layoutRowSpacing: {
        switch (prayerManager.homeLayout) {
        case 2: return totalFreeSpace * 0.07
        case 3: return totalFreeSpace * 0.15
        default: return totalFreeSpace * 0.09
        }
    }
    readonly property real spacerProgressTop: {
        switch (prayerManager.homeLayout) {
        case 2: return totalFreeSpace * 0.11
        default: return totalFreeSpace * 0.12
        }
    }
    readonly property real spacerProgressBottom: {
        switch (prayerManager.homeLayout) {
        case 2: return totalFreeSpace * 0.11
        default: return totalFreeSpace * 0.12
        }
    }
    readonly property real spacerWisdom: {
        if (dailyWisdomText.length === 0) return 0
        switch (prayerManager.homeLayout) {
        case 3: return totalFreeSpace * 0.24
        default: return totalFreeSpace * 0.16
        }
    }
    readonly property real spacerBottom: {
        var hasWisdom = dailyWisdomText.length > 0
        switch (prayerManager.homeLayout) {
        case 3: return hasWisdom ? totalFreeSpace * 0.325 : totalFreeSpace * 0.565
        default: return hasWisdom ? totalFreeSpace * 0.20 : totalFreeSpace * 0.36
        }
    }

    function layoutName(idx) {
        var _ = prayerManager.appLanguage
        switch (idx) {
        case 0: return qsTr("All-in-One 3-Column")
        case 1: return qsTr("Classic 3-Column")
        case 2: return qsTr("2-Column Grid")
        case 3: return qsTr("Hero & List")
        default: return qsTr("All-in-One 3-Column")
        }
    }

    function updateCountdown() {
        currentTime = new Date()
        countdownText = prayerManager.nextPrayerCountdown()
        progressValue = prayerManager.nextPrayerProgress()
        remainingPercent = prayerManager.nextPrayerRemainingPercentage()
        inPreAlert = prayerManager.isPreAlertWindow()
        dailyWisdomText = prayerManager.dailyWisdom
    }

    function formatPrayerTime(timeStr) {
        var _ = prayerManager.appLanguage
        if (!timeStr || timeStr.indexOf(":") === -1) return "--:--"
        if (prayerManager.use24HourFormat) {
            return prayerManager.formatDigits(timeStr)
        }
        var parts = timeStr.split(":")
        var h = parseInt(parts[0], 10)
        var m = parts[1]
        var ampm = h >= 12 ? (isRtl ? "م" : "pm") : (isRtl ? "ص" : "am")
        var h12 = h % 12
        if (h12 === 0) h12 = 12
        var hStr = (h12 < 10 ? "0" : "") + h12
        return prayerManager.formatDigits(hStr + ":" + m) + " " + ampm
    }

    function formatTime12(timeStr) {
        return formatPrayerTime(timeStr)
    }

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
        case "midnight": return qsTr("Midnight")
        case "lastThird": return qsTr("Last 1/3 Night Begins")
        default:
            if (!prayerKey || prayerKey.length === 0) return "--"
            return prayerKey.charAt(0).toUpperCase() + prayerKey.slice(1)
        }
    }

    function getCountdownBadge() {
        var _ = prayerManager.useHindiNumerals
        var _lang = prayerManager.appLanguage
        var _t = currentTime
        var secs = prayerManager.nextPrayerRemainingSeconds()
        if (secs < 0) return "--:--"
        var h = Math.floor(secs / 3600)
        var m = Math.floor((secs % 3600) / 60)
        var hStr = (h < 10 ? "0" : "") + h
        var mStr = (m < 10 ? "0" : "") + m
        return prayerManager.formatDigits(hStr + ":" + mStr)
    }

    function formatCountdownText(str) {
        var _ = prayerManager.useHindiNumerals
        var _lang = prayerManager.appLanguage
        if (!str) return ""
        var hMatch = str.match(/(\d+)h/)
        var mMatch = str.match(/(\d+)m/)
        var h = hMatch ? parseInt(hMatch[1], 10) : 0
        var m = mMatch ? parseInt(mMatch[1], 10) : 0
        if (h > 0) {
            return qsTr("%1h %2m").arg(prayerManager.formatDigits(h.toString())).arg(prayerManager.formatDigits(m.toString()))
        }
        return qsTr("%1m").arg(prayerManager.formatDigits(m.toString()))
    }

    Timer {
        interval: 1000
        running: true
        repeat: true
        triggeredOnStart: true
        onTriggered: updateCountdown()
    }

    Connections {
        target: prayerManager
        onTimesChanged: updateCountdown()
        onUse24HourFormatChanged: updateCountdown()
        onHomeLayoutChanged: updateCountdown()
        onUseHindiNumeralsChanged: updateCountdown()
        onAppLanguageChanged: updateCountdown()
    }

    SilicaFlickable {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: bottomBar.top
        }
        contentHeight: Math.max(availableContentHeight, contentColumn.height)

        PullDownMenu {
            MenuItem {
                text: qsTr("About")
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
            }
            MenuItem {
                text: qsTr("Layout: %1").arg(layoutName(prayerManager.homeLayout))
                onClicked: {
                    prayerManager.homeLayout = (prayerManager.homeLayout + 1) % 4
                }
            }
            MenuItem {
                text: qsTr("Qibla direction")
                enabled: prayerManager.hasCity
                onClicked: pageStack.push(Qt.resolvedUrl("QiblaPage.qml"))
            }
            MenuItem {
                text: qsTr("Favorites")
                onClicked: pageStack.push(Qt.resolvedUrl("FavoritesPage.qml"))
            }
            MenuItem {
                text: qsTr("Change city")
                onClicked: pageStack.push(Qt.resolvedUrl("CitySearchPage.qml"))
            }
            MenuItem {
                text: qsTr("Settings")
                onClicked: pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
            }
        }

        Column {
            id: contentColumn
            width: page.width

            Item {
                width: parent.width
                height: spacerTop
            }

            // Top Header: Prayer Times for City, Date/Time, Hijri Date
            Column {
                id: headerCol
                width: parent.width - 2 * Theme.horizontalPageMargin
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: Theme.paddingSmall / 2

                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: prayerManager.hasCity ? qsTr("Prayer Times for: %1").arg(prayerManager.cityName) : qsTr("Prayer Times")
                    font.pixelSize: Theme.fontSizeLarge
                    color: Theme.highlightColor
                    font.bold: true
                    truncationMode: TruncationMode.Fade
                }

                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: prayerManager.formatCurrentDateTime(currentTime)
                    font.pixelSize: Theme.fontSizeMedium
                    color: Theme.highlightColor
                }

                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: prayerManager.hijriDate ? (prayerManager.hijriDate + (isRtl ? " هـ" : " H.Y")) : ""
                    font.pixelSize: Theme.fontSizeMedium
                    color: Theme.highlightColor
                    visible: text.length > 0
                }

                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: !prayerManager.hasCity
                    wrapMode: Text.Wrap
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width
                    text: qsTr("No city selected yet. Pull down and choose \u201cChange city\u201d to search any city in the world.")
                    color: Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeSmall
                }
            }

            Item {
                width: parent.width
                height: spacerHeader
                visible: prayerManager.hasCity
            }

            // =========================================================
            // LAYOUT 0: All-in-One 3-Column Buttons (Name + Time inside Button)
            // =========================================================
            Column {
                id: layout0Container
                visible: prayerManager.hasCity && prayerManager.homeLayout === 0
                width: parent.width
                spacing: layoutRowSpacing

                // Row 1: Fajr, Chourouq, Dhouhr
                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: Theme.paddingSmall
                    property real itemWidth: (page.width - 2 * Theme.horizontalPageMargin - 2 * spacing) / 3

                    Repeater {
                        model: isRtl ? ["dhuhr", "sunrise", "fajr"] : ["fajr", "sunrise", "dhuhr"]
                        delegate: MouseArea {
                            property bool isNext: modelData === prayerManager.nextPrayerName
                            width: parent.itemWidth
                            height: buttonHeight0

                            Rectangle {
                                anchors.fill: parent
                                radius: Theme.paddingSmall
                                color: isNext ? Theme.rgba(Theme.highlightBackgroundColor, 0.15) : Theme.rgba(Theme.primaryColor, 0.06)
                                border.color: isNext ? Theme.highlightColor : Theme.rgba(Theme.secondaryColor, 0.3)
                                border.width: isNext ? 2 : 1

                                Column {
                                    anchors.centerIn: parent
                                    width: parent.width - Theme.paddingSmall * 2
                                    spacing: 4

                                    Label {
                                        width: parent.width
                                        horizontalAlignment: Text.AlignHCenter
                                        text: prayerDisplayName(modelData)
                                        font.pixelSize: Theme.fontSizeExtraSmall
                                        color: isNext ? Theme.highlightColor : Theme.secondaryColor
                                        font.bold: isNext
                                        truncationMode: TruncationMode.Fade
                                    }

                                    Label {
                                        anchors.horizontalCenter: parent.horizontalCenter
                                        text: formatTime12(prayerManager.todayTimes[modelData])
                                        font.pixelSize: Theme.fontSizeMedium
                                        color: isNext ? Theme.highlightColor : Theme.primaryColor
                                        font.bold: isNext
                                    }
                                }
                            }
                        }
                    }
                }

                // Row 2: Assar, Maghreb, Ishaa
                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: Theme.paddingSmall
                    property real itemWidth: (page.width - 2 * Theme.horizontalPageMargin - 2 * spacing) / 3

                    Repeater {
                        model: isRtl ? ["isha", "maghrib", "asr"] : ["asr", "maghrib", "isha"]
                        delegate: MouseArea {
                            property bool isNext: modelData === prayerManager.nextPrayerName
                            width: parent.itemWidth
                            height: buttonHeight0

                            Rectangle {
                                anchors.fill: parent
                                radius: Theme.paddingSmall
                                color: isNext ? Theme.rgba(Theme.highlightBackgroundColor, 0.15) : Theme.rgba(Theme.primaryColor, 0.06)
                                border.color: isNext ? Theme.highlightColor : Theme.rgba(Theme.secondaryColor, 0.3)
                                border.width: isNext ? 2 : 1

                                Column {
                                    anchors.centerIn: parent
                                    width: parent.width - Theme.paddingSmall * 2
                                    spacing: 4

                                    Label {
                                        width: parent.width
                                        horizontalAlignment: Text.AlignHCenter
                                        text: prayerDisplayName(modelData)
                                        font.pixelSize: Theme.fontSizeExtraSmall
                                        color: isNext ? Theme.highlightColor : Theme.secondaryColor
                                        font.bold: isNext
                                        truncationMode: TruncationMode.Fade
                                    }

                                    Label {
                                        anchors.horizontalCenter: parent.horizontalCenter
                                        text: formatTime12(prayerManager.todayTimes[modelData])
                                        font.pixelSize: Theme.fontSizeMedium
                                        color: isNext ? Theme.highlightColor : Theme.primaryColor
                                        font.bold: isNext
                                    }
                                }
                            }
                        }
                    }
                }

                // Row 3: Midnight, Last 1/3 Night Begins
                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: Theme.paddingSmall
                    property real itemWidth: (page.width - 2 * Theme.horizontalPageMargin - spacing) / 2

                    Repeater {
                        model: isRtl ? ["lastThird", "midnight"] : ["midnight", "lastThird"]
                        delegate: MouseArea {
                            width: parent.itemWidth
                            height: buttonHeight0

                            Rectangle {
                                anchors.fill: parent
                                radius: Theme.paddingSmall
                                color: Theme.rgba(Theme.primaryColor, 0.06)
                                border.color: Theme.rgba(Theme.secondaryColor, 0.3)
                                border.width: 1

                                Column {
                                    anchors.centerIn: parent
                                    width: parent.width - Theme.paddingSmall * 2
                                    spacing: 4

                                    Label {
                                        width: parent.width
                                        horizontalAlignment: Text.AlignHCenter
                                        text: modelData === "midnight" ? qsTr("Midnight") : qsTr("Last 1/3 Night Begins")
                                        font.pixelSize: Theme.fontSizeExtraSmall
                                        color: Theme.secondaryColor
                                        wrapMode: Text.Wrap
                                        maximumLineCount: 2
                                        fontSizeMode: Text.Fit
                                        minimumPixelSize: Theme.fontSizeTiny
                                    }

                                    Label {
                                        anchors.horizontalCenter: parent.horizontalCenter
                                        text: formatTime12(prayerManager.nightTimes[modelData])
                                        font.pixelSize: Theme.fontSizeMedium
                                        color: Theme.primaryColor
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // =========================================================
            // LAYOUT 1: Classic 3-Column with Labels
            // =========================================================
            Column {
                id: layout1Container
                visible: prayerManager.hasCity && prayerManager.homeLayout === 1
                width: parent.width
                spacing: layoutRowSpacing

                // Row 1: Fajr, Chourouq, Dhouhr
                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: Theme.paddingSmall
                    property real itemWidth: (page.width - 2 * Theme.horizontalPageMargin - 2 * spacing) / 3

                    Repeater {
                        model: isRtl ? ["dhuhr", "sunrise", "fajr"] : ["fajr", "sunrise", "dhuhr"]
                        delegate: Column {
                            width: parent.itemWidth
                            spacing: Theme.paddingSmall
                            property bool isNext: modelData === prayerManager.nextPrayerName

                            Label {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: prayerDisplayName(modelData)
                                font.pixelSize: Theme.fontSizeSmall
                                color: isNext ? Theme.highlightColor : Theme.primaryColor
                                font.bold: isNext
                            }

                            Button {
                                anchors.horizontalCenter: parent.horizontalCenter
                                preferredWidth: parent.width
                                height: buttonHeight1
                                text: formatTime12(prayerManager.todayTimes[modelData])
                                color: isNext ? Theme.highlightColor : Theme.primaryColor
                                border.color: isNext ? Theme.highlightColor : Theme.rgba(Theme.secondaryColor, 0.25)
                            }
                        }
                    }
                }

                // Row 2: Assar, Maghreb, Ishaa
                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: Theme.paddingSmall
                    property real itemWidth: (page.width - 2 * Theme.horizontalPageMargin - 2 * spacing) / 3

                    Repeater {
                        model: isRtl ? ["isha", "maghrib", "asr"] : ["asr", "maghrib", "isha"]
                        delegate: Column {
                            width: parent.itemWidth
                            spacing: Theme.paddingSmall
                            property bool isNext: modelData === prayerManager.nextPrayerName

                            Label {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: prayerDisplayName(modelData)
                                font.pixelSize: Theme.fontSizeSmall
                                color: isNext ? Theme.highlightColor : Theme.primaryColor
                                font.bold: isNext
                            }

                            Button {
                                anchors.horizontalCenter: parent.horizontalCenter
                                preferredWidth: parent.width
                                height: buttonHeight1
                                text: formatTime12(prayerManager.todayTimes[modelData])
                                color: isNext ? Theme.highlightColor : Theme.primaryColor
                                border.color: isNext ? Theme.highlightColor : Theme.rgba(Theme.secondaryColor, 0.25)
                            }
                        }
                    }
                }

                // Row 3: Midnight, Last 1/3 Night Begins
                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: Theme.paddingSmall
                    property real itemWidth: (page.width - 2 * Theme.horizontalPageMargin - spacing) / 2

                    Repeater {
                        model: isRtl ? ["lastThird", "midnight"] : ["midnight", "lastThird"]
                        delegate: Column {
                            width: parent.itemWidth
                            spacing: Theme.paddingSmall

                            Label {
                                width: parent.width
                                horizontalAlignment: Text.AlignHCenter
                                text: modelData === "midnight" ? qsTr("Midnight") : qsTr("Last 1/3 Night Begins")
                                font.pixelSize: Theme.fontSizeSmall
                                color: Theme.primaryColor
                                wrapMode: Text.Wrap
                                maximumLineCount: 2
                                fontSizeMode: Text.Fit
                                minimumPixelSize: Theme.fontSizeTiny
                            }

                            Button {
                                anchors.horizontalCenter: parent.horizontalCenter
                                preferredWidth: parent.width
                                height: buttonHeight1
                                text: formatTime12(prayerManager.nightTimes[modelData])
                                color: Theme.primaryColor
                                border.color: Theme.rgba(Theme.secondaryColor, 0.25)
                            }
                        }
                    }
                }
            }

            // =========================================================
            // LAYOUT 2: 2-Column Grid (4 Paired Rows)
            // =========================================================
            Column {
                id: layout2Container
                visible: prayerManager.hasCity && prayerManager.homeLayout === 2
                width: parent.width
                spacing: layoutRowSpacing
                property real itemWidth: (page.width - 2 * Theme.horizontalPageMargin - Theme.paddingSmall) / 2

                Repeater {
                    model: isRtl ? [
                        { p1: "sunrise", p2: "fajr", n1: false, n2: false },
                        { p1: "asr", p2: "dhuhr", n1: false, n2: false },
                        { p1: "isha", p2: "maghrib", n1: false, n2: false },
                        { p1: "lastThird", p2: "midnight", n1: true, n2: true }
                    ] : [
                        { p1: "fajr", p2: "sunrise", n1: false, n2: false },
                        { p1: "dhuhr", p2: "asr", n1: false, n2: false },
                        { p1: "maghrib", p2: "isha", n1: false, n2: false },
                        { p1: "midnight", p2: "lastThird", n1: true, n2: true }
                    ]
                    delegate: Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: Theme.paddingSmall

                        // Item 1
                        MouseArea {
                            property bool isNext: !modelData.n1 && modelData.p1 === prayerManager.nextPrayerName
                            width: layout2Container.itemWidth
                            height: buttonHeight2

                            Rectangle {
                                anchors.fill: parent
                                radius: Theme.paddingSmall
                                color: isNext ? Theme.rgba(Theme.highlightBackgroundColor, 0.15) : Theme.rgba(Theme.primaryColor, 0.06)
                                border.color: isNext ? Theme.highlightColor : Theme.rgba(Theme.secondaryColor, 0.3)
                                border.width: isNext ? 2 : 1

                                Column {
                                    visible: modelData.p1 === "lastThird"
                                    anchors {
                                        left: isRtl ? p1TimeLabel.right : parent.left
                                        leftMargin: isRtl ? Theme.paddingSmall : Theme.paddingMedium
                                        right: isRtl ? parent.right : p1TimeLabel.left
                                        rightMargin: isRtl ? Theme.paddingMedium : Theme.paddingSmall
                                        verticalCenter: parent.verticalCenter
                                    }
                                    spacing: 1

                                    Label {
                                        width: parent.width
                                        text: qsTr("Last 1/3")
                                        font.pixelSize: Theme.fontSizeExtraSmall
                                        color: isNext ? Theme.highlightColor : Theme.primaryColor
                                        font.bold: isNext
                                        truncationMode: TruncationMode.Fade
                                        horizontalAlignment: isRtl ? Text.AlignRight : Text.AlignLeft
                                    }

                                    Label {
                                        width: parent.width
                                        text: qsTr("Night Begins")
                                        font.pixelSize: Theme.fontSizeExtraSmall
                                        color: isNext ? Theme.highlightColor : Theme.secondaryColor
                                        truncationMode: TruncationMode.Fade
                                        horizontalAlignment: isRtl ? Text.AlignRight : Text.AlignLeft
                                    }
                                }

                                Label {
                                    id: p1NameLabel
                                    visible: modelData.p1 !== "lastThird"
                                    anchors {
                                        left: isRtl ? p1TimeLabel.right : parent.left
                                        leftMargin: isRtl ? Theme.paddingSmall : Theme.paddingMedium
                                        right: isRtl ? parent.right : p1TimeLabel.left
                                        rightMargin: isRtl ? Theme.paddingMedium : Theme.paddingSmall
                                        verticalCenter: parent.verticalCenter
                                    }
                                    text: prayerDisplayName(modelData.p1)
                                    font.pixelSize: Theme.fontSizeSmall
                                    color: isNext ? Theme.highlightColor : Theme.primaryColor
                                    font.bold: isNext
                                    truncationMode: TruncationMode.Fade
                                    horizontalAlignment: isRtl ? Text.AlignRight : Text.AlignLeft
                                }

                                Label {
                                    id: p1TimeLabel
                                    anchors {
                                        left: isRtl ? parent.left : undefined
                                        right: isRtl ? undefined : parent.right
                                        leftMargin: isRtl ? Theme.paddingMedium : 0
                                        rightMargin: isRtl ? 0 : Theme.paddingMedium
                                        verticalCenter: parent.verticalCenter
                                    }
                                    text: modelData.n1 ? formatTime12(prayerManager.nightTimes[modelData.p1])
                                                       : formatTime12(prayerManager.todayTimes[modelData.p1])
                                    font.pixelSize: Theme.fontSizeMedium
                                    color: isNext ? Theme.highlightColor : Theme.primaryColor
                                    font.bold: isNext
                                }
                            }
                        }

                        // Item 2
                        MouseArea {
                            property bool isNext: !modelData.n2 && modelData.p2 === prayerManager.nextPrayerName
                            width: layout2Container.itemWidth
                            height: buttonHeight2

                            Rectangle {
                                anchors.fill: parent
                                radius: Theme.paddingSmall
                                color: isNext ? Theme.rgba(Theme.highlightBackgroundColor, 0.15) : Theme.rgba(Theme.primaryColor, 0.06)
                                border.color: isNext ? Theme.highlightColor : Theme.rgba(Theme.secondaryColor, 0.3)
                                border.width: isNext ? 2 : 1

                                Column {
                                    visible: modelData.p2 === "lastThird"
                                    anchors {
                                        left: isRtl ? p2TimeLabel.right : parent.left
                                        leftMargin: isRtl ? Theme.paddingSmall : Theme.paddingMedium
                                        right: isRtl ? parent.right : p2TimeLabel.left
                                        rightMargin: isRtl ? Theme.paddingMedium : Theme.paddingSmall
                                        verticalCenter: parent.verticalCenter
                                    }
                                    spacing: 1

                                    Label {
                                        width: parent.width
                                        text: qsTr("Last 1/3")
                                        font.pixelSize: Theme.fontSizeExtraSmall
                                        color: isNext ? Theme.highlightColor : Theme.primaryColor
                                        font.bold: isNext
                                        truncationMode: TruncationMode.Fade
                                        horizontalAlignment: isRtl ? Text.AlignRight : Text.AlignLeft
                                    }

                                    Label {
                                        width: parent.width
                                        text: qsTr("Night Begins")
                                        font.pixelSize: Theme.fontSizeExtraSmall
                                        color: isNext ? Theme.highlightColor : Theme.secondaryColor
                                        truncationMode: TruncationMode.Fade
                                        horizontalAlignment: isRtl ? Text.AlignRight : Text.AlignLeft
                                    }
                                }

                                Label {
                                    id: p2NameLabel
                                    visible: modelData.p2 !== "lastThird"
                                    anchors {
                                        left: isRtl ? p2TimeLabel.right : parent.left
                                        leftMargin: isRtl ? Theme.paddingSmall : Theme.paddingMedium
                                        right: isRtl ? parent.right : p2TimeLabel.left
                                        rightMargin: isRtl ? Theme.paddingMedium : Theme.paddingSmall
                                        verticalCenter: parent.verticalCenter
                                    }
                                    text: prayerDisplayName(modelData.p2)
                                    font.pixelSize: Theme.fontSizeSmall
                                    color: isNext ? Theme.highlightColor : Theme.primaryColor
                                    font.bold: isNext
                                    truncationMode: TruncationMode.Fade
                                    horizontalAlignment: isRtl ? Text.AlignRight : Text.AlignLeft
                                }

                                Label {
                                    id: p2TimeLabel
                                    anchors {
                                        left: isRtl ? parent.left : undefined
                                        right: isRtl ? undefined : parent.right
                                        leftMargin: isRtl ? Theme.paddingMedium : 0
                                        rightMargin: isRtl ? 0 : Theme.paddingMedium
                                        verticalCenter: parent.verticalCenter
                                    }
                                    text: modelData.n2 ? formatTime12(prayerManager.nightTimes[modelData.p2])
                                                       : formatTime12(prayerManager.todayTimes[modelData.p2])
                                    font.pixelSize: Theme.fontSizeMedium
                                    color: isNext ? Theme.highlightColor : Theme.primaryColor
                                    font.bold: isNext
                                }
                            }
                        }
                    }
                }
            }

            // =========================================================
            // LAYOUT 3: Hero & List (Sailfish Native Glanceable)
            // =========================================================
            Column {
                id: layout3Container
                visible: prayerManager.hasCity && prayerManager.homeLayout === 3
                width: parent.width
                spacing: layoutRowSpacing

                // Hero Card
                Rectangle {
                    id: heroCard
                    width: parent.width - 2 * Theme.horizontalPageMargin
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: Math.max(heroCol.implicitHeight + Theme.paddingMedium * 2, heroHeight3)
                    radius: Theme.paddingMedium
                    color: inPreAlert ? Theme.rgba("#f44336", 0.18) : Theme.rgba(Theme.highlightColor, 0.12)
                    border.color: inPreAlert ? "#f44336" : Theme.rgba(Theme.highlightColor, 0.4)
                    border.width: 1

                    Column {
                        id: heroCol
                        anchors.centerIn: parent
                        spacing: Theme.paddingSmall / 2
                        width: parent.width - 2 * Theme.paddingMedium

                        Label {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: (prayerManager.isNextPrayerTomorrow && prayerManager.nextPrayerName === "fajr")
                                  ? qsTr("UPCOMING: TOMORROW'S FAJR")
                                  : qsTr("UPCOMING: %1").arg((prayerManager.isNextPrayerTomorrow
                                      ? qsTr("Tomorrow's %1").arg(prayerDisplayName(prayerManager.nextPrayerName))
                                      : prayerDisplayName(prayerManager.nextPrayerName)).toUpperCase())
                            font.pixelSize: Theme.fontSizeSmall
                            color: inPreAlert ? "#f44336" : Theme.highlightColor
                            font.bold: true
                        }

                        Label {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: prayerManager.nextPrayerTime
                            font.pixelSize: Theme.fontSizeHuge
                            color: Theme.primaryColor
                            font.bold: true
                        }

                        Label {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: qsTr("Remaining: %1").arg(formatCountdownText(countdownText))
                            font.pixelSize: Theme.fontSizeMedium
                            color: inPreAlert ? "#f44336" : Theme.secondaryColor
                        }

                        // Mini progress bar in Hero card
                        Item {
                            width: parent.width * 0.8
                            height: 4
                            anchors.horizontalCenter: parent.horizontalCenter

                            Rectangle {
                                anchors.fill: parent
                                radius: 2
                                color: Theme.rgba(Theme.primaryColor, 0.15)
                            }
                            Rectangle {
                                anchors.left: isRtl ? undefined : parent.left
                                anchors.right: isRtl ? parent.right : undefined
                                width: parent.width * Math.max(0.0, Math.min(1.0, progressValue))
                                height: parent.height
                                radius: 2
                                color: inPreAlert ? "#f44336" : "#29b6f6"
                                Behavior on color { ColorAnimation { duration: 300 } }
                            }
                        }
                    }
                }

                // Vertical List of Times
                Column {
                    width: parent.width - 2 * Theme.horizontalPageMargin
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: listSpacing3

                    Repeater {
                        model: [
                            { key: "fajr", isNight: false },
                            { key: "sunrise", isNight: false },
                            { key: "dhuhr", isNight: false },
                            { key: "asr", isNight: false },
                            { key: "maghrib", isNight: false },
                            { key: "isha", isNight: false },
                            { key: "midnight", isNight: true },
                            { key: "lastThird", isNight: true }
                        ]
                        delegate: Rectangle {
                            property bool isNext: !modelData.isNight && modelData.key === prayerManager.nextPrayerName
                            width: parent.width
                            height: listItemHeight3
                            radius: Theme.paddingSmall
                            color: isNext ? Theme.rgba(Theme.highlightColor, 0.2) : "transparent"
                            border.color: isNext ? Theme.highlightColor : Theme.rgba(Theme.secondaryColor, 0.15)
                            border.width: isNext ? 1 : 0

                            Label {
                                anchors {
                                    left: isRtl ? l3TimeLabel.right : parent.left
                                    leftMargin: isRtl ? Theme.paddingSmall : Theme.paddingMedium
                                    right: isRtl ? parent.right : l3TimeLabel.left
                                    rightMargin: isRtl ? Theme.paddingMedium : Theme.paddingSmall
                                    verticalCenter: parent.verticalCenter
                                }
                                text: prayerDisplayName(modelData.key)
                                font.pixelSize: Theme.fontSizeSmall
                                color: isNext ? Theme.highlightColor : Theme.primaryColor
                                font.bold: isNext
                                truncationMode: TruncationMode.Fade
                                horizontalAlignment: isRtl ? Text.AlignRight : Text.AlignLeft
                            }

                            Label {
                                id: l3TimeLabel
                                anchors {
                                    left: isRtl ? parent.left : undefined
                                    right: isRtl ? undefined : parent.right
                                    leftMargin: isRtl ? Theme.paddingMedium : 0
                                    rightMargin: isRtl ? 0 : Theme.paddingMedium
                                    verticalCenter: parent.verticalCenter
                                }
                                text: modelData.isNight ? formatTime12(prayerManager.nightTimes[modelData.key])
                                                        : formatTime12(prayerManager.todayTimes[modelData.key])
                                font.pixelSize: Theme.fontSizeMedium
                                color: isNext ? Theme.highlightColor : Theme.primaryColor
                                font.bold: isNext
                            }
                        }
                    }
                }
            }

            Item {
                width: parent.width
                height: spacerProgressTop
                visible: prayerManager.hasCity && prayerManager.homeLayout !== 3
            }

            // Progress bar (for Layouts 0, 1, 2)
            Item {
                visible: prayerManager.hasCity && prayerManager.homeLayout !== 3 && prayerManager.nextPrayerName.length > 0
                width: parent.width - 2 * Theme.horizontalPageMargin
                height: 6
                anchors.horizontalCenter: parent.horizontalCenter

                Rectangle {
                    anchors.fill: parent
                    radius: 3
                    color: "#16222f"
                }

                Rectangle {
                    anchors.left: isRtl ? undefined : parent.left
                    anchors.right: isRtl ? parent.right : undefined
                    width: parent.width * Math.max(0.0, Math.min(1.0, progressValue))
                    height: parent.height
                    radius: 3
                    color: inPreAlert ? "#f44336" : "#29b6f6"
                    Behavior on color {
                        ColorAnimation { duration: 300 }
                    }
                }
            }

            Item {
                width: parent.width
                height: spacerProgressBottom
                visible: prayerManager.hasCity && prayerManager.homeLayout !== 3
            }

            // Remaining time until: <PrayerName> (for Layouts 0, 1, 2)
            Column {
                id: countdownCol
                visible: prayerManager.hasCity && prayerManager.homeLayout !== 3 && prayerManager.nextPrayerName.length > 0
                width: parent.width - 2 * Theme.horizontalPageMargin
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: Theme.paddingMedium

                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: (prayerManager.isNextPrayerTomorrow && prayerManager.nextPrayerName === "fajr")
                          ? qsTr("Remaining time until: Tomorrow's Fajr")
                          : (prayerManager.isNextPrayerTomorrow
                             ? qsTr("Remaining time until: Tomorrow's %1").arg(prayerDisplayName(prayerManager.nextPrayerName))
                             : qsTr("Remaining time until: %1").arg(prayerDisplayName(prayerManager.nextPrayerName)))
                    font.pixelSize: Theme.fontSizeMedium
                    color: Theme.primaryColor
                }

                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: Math.max(160, page.width * 0.42)
                    height: Theme.itemSizeSmall
                    radius: Theme.paddingSmall
                    color: inPreAlert ? Theme.rgba("#f44336", 0.15) : Theme.rgba(Theme.highlightBackgroundColor, 0.12)
                    border.color: inPreAlert ? "#f44336" : Theme.rgba(Theme.highlightColor, 0.4)
                    border.width: 1

                    Label {
                        anchors.centerIn: parent
                        text: getCountdownBadge()
                        font.pixelSize: Theme.fontSizeLarge
                        font.bold: true
                        color: inPreAlert ? "#f44336" : Theme.primaryColor
                    }
                }
            }

            Item {
                width: parent.width
                height: spacerWisdom
                visible: prayerManager.hasCity && dailyWisdomText.length > 0
            }

            // Daily Wisdom (حكمة اليوم)
            Item {
                id: wisdomContainer
                visible: prayerManager.hasCity && dailyWisdomText.length > 0
                width: parent.width - 2 * Theme.horizontalPageMargin
                anchors.horizontalCenter: parent.horizontalCenter
                height: wisdomCard.height

                Rectangle {
                    id: wisdomCard
                    width: parent.width
                    height: Math.max(Theme.itemSizeSmall, wisdomLabel.implicitHeight + Theme.paddingMedium * 2)
                    anchors.centerIn: parent
                    radius: Theme.paddingSmall
                    color: Theme.rgba(Theme.highlightBackgroundColor, 0.08)
                    border.color: Theme.rgba(Theme.highlightColor, 0.2)
                    border.width: 1

                    Label {
                        id: wisdomLabel
                        anchors {
                            left: parent.left
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                            margins: Theme.paddingMedium
                        }
                        text: dailyWisdomText
                        font.pixelSize: Theme.fontSizeExtraSmall
                        color: Theme.highlightColor
                        wrapMode: Text.Wrap
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }

            Item {
                width: parent.width
                height: spacerBottom
            }
        }
    }

    // Bottom Navigation Bar
    Rectangle {
        id: bottomBar
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: Theme.itemSizeMedium
        color: "#08131d"

        Rectangle {
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
            height: 1
            color: Theme.rgba(Theme.highlightColor, 0.15)
        }

        Row {
            anchors.fill: parent

            // 1. Favorites Button
            BackgroundItem {
                width: parent.width / 3
                height: parent.height
                onClicked: pageStack.push(Qt.resolvedUrl("FavoritesPage.qml"))

                Icon {
                    anchors.centerIn: parent
                    source: "image://theme/icon-m-favorite"
                    highlighted: parent.highlighted
                }
            }

            // 2. Qibla / Location Button
            BackgroundItem {
                width: parent.width / 3
                height: parent.height
                onClicked: pageStack.push(Qt.resolvedUrl("QiblaPage.qml"))

                Icon {
                    anchors.centerIn: parent
                    source: "image://theme/icon-m-location"
                    highlighted: parent.highlighted
                }
            }

            // 3. Settings Button
            BackgroundItem {
                width: parent.width / 3
                height: parent.height
                onClicked: pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))

                Icon {
                    id: settingsIcon
                    anchors.centerIn: parent
                    source: "image://theme/icon-m-setting"
                    highlighted: parent.highlighted
                    onStatusChanged: {
                        if (status === Image.Error && source == "image://theme/icon-m-setting") {
                            source = "image://theme/icon-m-settings"
                        }
                    }
                }
            }
        }
    }
}

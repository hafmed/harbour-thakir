import QtQuick 2.6
import Sailfish.Silica 1.0

Page {
    id: downloadPage
    allowedOrientations: Orientation.All

    readonly property bool isRtl: prayerManager.isArabicLanguage
    readonly property bool isOnline: (typeof quranManager.isOnline !== "undefined" ? quranManager.isOnline : true) && prayerManager.geocoder.isOnline

    property int pagesCount: quranManager.getDownloadedPagesCount(quranManager.riwayah)
    property var pagesSize: quranManager.getPagesCacheSize(quranManager.riwayah)

    property int audioCount: quranManager.getDownloadedAudioCount(quranManager.selectedReciterId)
    property var audioSize: quranManager.getAudioCacheSize(quranManager.selectedReciterId)

    property var recitersList: quranManager.getAvailableReciters()
    property var surahsList: []
    property int initialSurah: 1
    property int selectedSurahToDownload: (initialSurah > 0 && initialSurah <= 114) ? initialSurah : 1

    function refreshStats() {
        recitersList = quranManager.getAvailableReciters()
        pagesCount = quranManager.getDownloadedPagesCount(quranManager.riwayah)
        pagesSize = quranManager.getPagesCacheSize(quranManager.riwayah)
        audioCount = quranManager.getDownloadedAudioCount(quranManager.selectedReciterId)
        audioSize = quranManager.getAudioCacheSize(quranManager.selectedReciterId)
    }

    Component.onCompleted: {
        surahsList = quranManager.getSurahs()
        if (initialSurah > 0 && initialSurah <= 114) {
            selectedSurahToDownload = initialSurah
        }
        refreshStats()
    }

    Connections {
        target: quranManager
        onRiwayahChanged: {
            surahsList = quranManager.getSurahs()
            refreshStats()
        }
        onReciterChanged: refreshStats()
        onBulkPagesProgressChanged: refreshStats()
        onAudioDownloadProgressChanged: refreshStats()
        onPageCached: refreshStats()
        onOnlineStateChanged: refreshStats()
    }

    RemorsePopup { id: remorse }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: contentCol.height + Theme.paddingLarge * 2

        PullDownMenu {
            MenuItem {
                text: isRtl ? "تحديث الإحصائيات" : qsTr("Refresh Stats")
                onClicked: refreshStats()
            }
        }

        Column {
            id: contentCol
            width: parent.width - 2 * Theme.horizontalPageMargin
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: Theme.paddingMedium

            PageHeader {
                title: isRtl ? "إدارة التحميل والأوفلاين" : qsTr("Downloads & Offline")
                description: isRtl ? "تحميل المصحف والتلاوات للاستخدام بدون إنترنت" : qsTr("Download Quran and recitations for offline use")
            }

            // Network Offline Warning Banner
            Rectangle {
                id: offlineBanner
                visible: !downloadPage.isOnline
                width: parent.width
                height: visible ? (offlineContent.height + Theme.paddingMedium * 2) : 0
                radius: Theme.paddingSmall
                color: Qt.rgba(1.0, 0.2, 0.2, 0.15)
                border.color: "#cc3333"
                border.width: 1

                Row {
                    id: offlineContent
                    width: parent.width - 2 * Theme.paddingMedium
                    anchors.centerIn: parent
                    spacing: Theme.paddingMedium
                    layoutDirection: isRtl ? Qt.RightToLeft : Qt.LeftToRight

                    Icon {
                        source: "image://theme/icon-m-cloud-download"
                        color: "#ff5555"
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Label {
                        width: parent.width - Theme.iconSizeMedium - Theme.paddingMedium
                        anchors.verticalCenter: parent.verticalCenter
                        wrapMode: Text.Wrap
                        font.pixelSize: Theme.fontSizeSmall
                        font.bold: true
                        color: "#ff5555"
                        text: isRtl
                              ? "الاتصال بالإنترنت معطل حالياً. تم تعطيل أزرار التحميل حتى يتم تفعيل الاتصال بالإنترنت."
                              : qsTr("Internet connection is disabled. All download buttons are disabled until connection is restored.")
                    }
                }
            }

            // Info note
            Rectangle {
                width: parent.width
                height: infoText.paintedHeight + Theme.paddingMedium * 2
                radius: Theme.paddingSmall
                color: Theme.rgba(Theme.highlightBackgroundColor, 0.15)
                border.color: Theme.rgba(Theme.highlightColor, 0.3)
                border.width: 1

                Label {
                    id: infoText
                    anchors {
                        left: parent.left
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                        margins: Theme.paddingMedium
                    }
                    wrapMode: Text.Wrap
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.secondaryColor
                    text: isRtl
                          ? "• يمكنك استخدام التطبيق مباشرة عبر الإنترنت (أونلاين) حيث يتم حفظ الصفحات والتلاوات المستمع إليها تلقائياً في جهازك.\n• أو يمكنك هنا تحميل الصفحات والتلاوات كاملة بضغطة واحدة لاستخدام التطبيق دون اتصال (أوفلاين) في أي وقت."
                          : qsTr("• You can stream online on-demand (pages and played recitations are cached automatically).\n• Or you can download full Quran pages and recitations here for 100% offline use.")
                }
            }

            // ========================================================
            // SECTION 1: RIWAYAH SELECTION
            // ========================================================
            SectionHeader {
                text: isRtl ? "رواية المصحف الشريف" : qsTr("Holy Quran Riwayah")
            }

            ComboBox {
                width: parent.width
                label: isRtl ? "الرواية الحالية:" : qsTr("Current Riwayah:")
                currentIndex: quranManager.riwayah === 2 ? 1 : 0
                menu: ContextMenu {
                    MenuItem {
                        text: isRtl ? "حفص عن عاصم" : "Hafs 'an 'Asim"
                        onClicked: quranManager.riwayah = 1
                    }
                    MenuItem {
                        text: isRtl ? "ورش عن نافع (طريق الأزرق)" : "Warsh 'an Nafi'"
                        onClicked: quranManager.riwayah = 2
                    }
                }
            }

            // ========================================================
            // SECTION 2: MUSHAF PAGES (المصحف المصور)
            // ========================================================
            SectionHeader {
                text: isRtl ? "صفحات المصحف (المصحف المصور)" : qsTr("Quran Pages (Mushaf View)")
            }

            Rectangle {
                width: parent.width
                height: pagesCol.height + Theme.paddingMedium * 2
                radius: Theme.paddingSmall
                color: Theme.rgba(Theme.highlightBackgroundColor, 0.1)
                border.color: Theme.rgba(Theme.highlightColor, 0.25)
                border.width: 1

                Column {
                    id: pagesCol
                    width: parent.width - 2 * Theme.paddingMedium
                    anchors.centerIn: parent
                    spacing: Theme.paddingMedium

                    Row {
                        width: parent.width
                        spacing: Theme.paddingSmall

                        Label {
                            text: isRtl ? "الحالة:" : qsTr("Status:")
                            font.bold: true
                            font.pixelSize: Theme.fontSizeSmall
                            color: Theme.highlightColor
                        }

                        Label {
                            width: parent.width - 80
                            wrapMode: Text.Wrap
                            font.pixelSize: Theme.fontSizeSmall
                            color: pagesCount >= 604 ? "#4cd964" : (!downloadPage.isOnline ? Theme.secondaryColor : Theme.primaryColor)
                            text: {
                                var base = isRtl
                                      ? (prayerManager.formatDigits(pagesCount.toString()) + " / " + prayerManager.formatDigits("604") + " صفحة محملة (" + quranManager.formatFileSize(pagesSize) + ")" + (pagesCount >= 604 ? " • مكتمل وجاهز أوفلاين" : ""))
                                      : (pagesCount + " / 604 pages downloaded (" + quranManager.formatFileSize(pagesSize) + ")" + (pagesCount >= 604 ? " • Ready Offline" : ""))
                                if (!downloadPage.isOnline && pagesCount < 604) {
                                    base += isRtl ? " (الإنترنت معطل)" : " " + qsTr("(Internet disabled)")
                                }
                                return base
                            }
                        }
                    }

                    // Progress bar when downloading
                    Column {
                        width: parent.width
                        visible: quranManager.isDownloadingPages
                        spacing: Theme.paddingSmall

                        ProgressBar {
                            width: parent.width
                            minimumValue: 0
                            maximumValue: 1.0
                            value: quranManager.pagesDownloadProgress
                            label: isRtl ? "جاري تحميل صفحات المصحف..." : qsTr("Downloading Quran pages...")
                        }

                        Button {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: isRtl ? "إيقاف التحميل" : qsTr("Stop Download")
                            onClicked: quranManager.cancelBulkPagesDownload()
                        }
                    }

                    // Download and Delete buttons
                    Row {
                        width: parent.width
                        spacing: Theme.paddingMedium
                        visible: !quranManager.isDownloadingPages

                        Button {
                            preferredWidth: (parent.width - Theme.paddingMedium) / (pagesCount > 0 ? 2 : 1)
                            text: isRtl
                                  ? (pagesCount >= 604 ? "إعادة فحص وتحديث" : "تحميل كل الصفحات (604)")
                                  : (pagesCount >= 604 ? qsTr("Verify & Update") : qsTr("Download All Pages (604)"))
                            enabled: downloadPage.isOnline && !quranManager.isDownloadingPages
                            onClicked: quranManager.startBulkPagesDownload(quranManager.riwayah)
                        }

                        Button {
                            visible: pagesCount > 0
                            preferredWidth: (parent.width - Theme.paddingMedium) / 2
                            text: isRtl ? "حذف الصفحات" : qsTr("Clear Pages")
                            onClicked: {
                                remorse.execute(
                                    isRtl ? "حذف صفحات المصحف المخزنة" : qsTr("Deleting cached pages"),
                                    function() {
                                        quranManager.clearPagesCache(quranManager.riwayah)
                                        refreshStats()
                                    }
                                )
                            }
                        }
                    }
                }
            }

            // ========================================================
            // SECTION 3: RECITATION AUDIO (المصحف الصوتي)
            // ========================================================
            SectionHeader {
                text: isRtl ? "التلاوة الصوتية (المصحف الصوتي)" : qsTr("Audio Recitation")
            }

            ComboBox {
                id: reciterCombo
                width: parent.width
                label: isRtl ? "القارئ:" : qsTr("Reciter:")
                currentIndex: {
                    for (var i = 0; i < recitersList.length; ++i) {
                        if (recitersList[i].id === quranManager.selectedReciterId) {
                            return i
                        }
                    }
                    return 0
                }
                menu: ContextMenu {
                    Repeater {
                        model: recitersList
                        MenuItem {
                            text: isRtl ? modelData.name_ar : (modelData.name_en ? modelData.name_en : modelData.name_ar)
                            onClicked: {
                                quranManager.setReciter(modelData.id)
                                refreshStats()
                            }
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: audioCol.height + Theme.paddingMedium * 2
                radius: Theme.paddingSmall
                color: Theme.rgba(Theme.highlightBackgroundColor, 0.1)
                border.color: Theme.rgba(Theme.highlightColor, 0.25)
                border.width: 1

                Column {
                    id: audioCol
                    width: parent.width - 2 * Theme.paddingMedium
                    anchors.centerIn: parent
                    spacing: Theme.paddingMedium

                    Row {
                        width: parent.width
                        spacing: Theme.paddingSmall

                        Label {
                            text: isRtl ? "الحالة:" : qsTr("Status:")
                            font.bold: true
                            font.pixelSize: Theme.fontSizeSmall
                            color: Theme.highlightColor
                        }

                        Label {
                            width: parent.width - 80
                            wrapMode: Text.Wrap
                            font.pixelSize: Theme.fontSizeSmall
                            color: audioCount >= (quranManager.riwayah === 2 ? 6214 : 6236) ? "#4cd964" : (!downloadPage.isOnline ? Theme.secondaryColor : Theme.primaryColor)
                            text: {
                                var base = isRtl
                                      ? (prayerManager.formatDigits(audioCount.toString()) + " آية محملة (" + quranManager.formatFileSize(audioSize) + ")" + (audioCount >= (quranManager.riwayah === 2 ? 6214 : 6236) ? " • المصحف كامل جاهز أوفلاين" : ""))
                                      : (audioCount + " ayahs downloaded (" + quranManager.formatFileSize(audioSize) + ")")
                                if (!downloadPage.isOnline && audioCount < (quranManager.riwayah === 2 ? 6214 : 6236)) {
                                    base += isRtl ? " (الإنترنت معطل)" : " " + qsTr("(Internet disabled)")
                                }
                                return base
                            }
                        }
                    }

                    // Progress bar when downloading audio
                    Column {
                        width: parent.width
                        visible: quranManager.isDownloadingAudio
                        spacing: Theme.paddingSmall

                        ProgressBar {
                            width: parent.width
                            minimumValue: 0
                            maximumValue: 1.0
                            value: quranManager.audioDownloadProgress
                            label: quranManager.audioDownloadStatus
                        }

                        Button {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: isRtl ? "إيقاف التحميل" : qsTr("Stop Download")
                            onClicked: quranManager.cancelAudioDownload()
                        }
                    }

                    // Full recitation download button
                    Button {
                        width: parent.width
                        visible: !quranManager.isDownloadingAudio
                        text: isRtl ? "تحميل المصحف الصوتي كاملاً" : qsTr("Download Full Quran Audio")
                        enabled: downloadPage.isOnline && !quranManager.isDownloadingAudio
                        onClicked: quranManager.startFullAudioDownload(quranManager.selectedReciterId)
                    }

                    // Download by Surah section
                    Column {
                        width: parent.width
                        visible: !quranManager.isDownloadingAudio
                        spacing: Theme.paddingSmall

                        Label {
                            text: isRtl ? "أو تحميل سورة محددة للاستماع أوفلاين:" : qsTr("Or download specific Surah:")
                            font.pixelSize: Theme.fontSizeExtraSmall
                            color: Theme.secondaryColor
                        }

                        Row {
                            width: parent.width
                            spacing: Theme.paddingSmall

                            ComboBox {
                                id: surahCombo
                                width: parent.width - downloadSurahBtn.width - Theme.paddingSmall
                                label: isRtl ? "السورة:" : qsTr("Surah:")
                                currentIndex: Math.max(0, selectedSurahToDownload - 1)
                                menu: ContextMenu {
                                    Repeater {
                                        model: surahsList
                                        MenuItem {
                                            text: modelData.number + ". " + (isRtl ? modelData.name_ar : modelData.name_en)
                                            onClicked: selectedSurahToDownload = modelData.number
                                        }
                                    }
                                }
                            }

                            Button {
                                id: downloadSurahBtn
                                preferredWidth: Theme.buttonWidthSmall
                                anchors.verticalCenter: surahCombo.verticalCenter
                                text: isRtl ? "تحميل" : qsTr("Download")
                                enabled: downloadPage.isOnline && !quranManager.isDownloadingAudio
                                onClicked: quranManager.startSurahAudioDownload(selectedSurahToDownload, quranManager.selectedReciterId)
                            }
                        }
                    }

                    // Clear audio cache button
                    Button {
                        visible: !quranManager.isDownloadingAudio && audioCount > 0
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: isRtl ? "حذف التلاوات المخزنة للقارئ" : qsTr("Clear Cached Audio")
                        onClicked: {
                            remorse.execute(
                                isRtl ? "حذف التلاوات المخزنة" : qsTr("Deleting cached audio"),
                                function() {
                                    quranManager.clearAudioCache(quranManager.selectedReciterId)
                                    refreshStats()
                                }
                            )
                        }
                    }
                }
            }

            Item { width: 1; height: Theme.paddingLarge }
        }
    }
}

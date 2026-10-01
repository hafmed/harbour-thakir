/****************************************************************************
** Meta object code from reading C++ file 'quranmanager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/quranmanager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'quranmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QuranManager_t {
    QByteArrayData data[160];
    char stringdata0[2403];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QuranManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QuranManager_t qt_meta_stringdata_QuranManager = {
    {
QT_MOC_LITERAL(0, 0, 12), // "QuranManager"
QT_MOC_LITERAL(1, 13, 14), // "riwayahChanged"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 15), // "fontSizeChanged"
QT_MOC_LITERAL(4, 45, 21), // "tafsirFontSizeChanged"
QT_MOC_LITERAL(5, 67, 15), // "viewModeChanged"
QT_MOC_LITERAL(6, 83, 15), // "darkModeChanged"
QT_MOC_LITERAL(7, 99, 18), // "tajweedModeChanged"
QT_MOC_LITERAL(8, 118, 19), // "lastPositionChanged"
QT_MOC_LITERAL(9, 138, 28), // "selectedTafsirEditionChanged"
QT_MOC_LITERAL(10, 167, 17), // "audioStateChanged"
QT_MOC_LITERAL(11, 185, 18), // "playingAyahChanged"
QT_MOC_LITERAL(12, 204, 14), // "reciterChanged"
QT_MOC_LITERAL(13, 219, 20), // "audioProgressChanged"
QT_MOC_LITERAL(14, 240, 10), // "pageCached"
QT_MOC_LITERAL(15, 251, 10), // "pageNumber"
QT_MOC_LITERAL(16, 262, 18), // "pageDownloadFailed"
QT_MOC_LITERAL(17, 281, 16), // "bookmarksChanged"
QT_MOC_LITERAL(18, 298, 24), // "bulkPagesProgressChanged"
QT_MOC_LITERAL(19, 323, 28), // "audioDownloadProgressChanged"
QT_MOC_LITERAL(20, 352, 20), // "audioStreamingNotice"
QT_MOC_LITERAL(21, 373, 5), // "surah"
QT_MOC_LITERAL(22, 379, 4), // "ayah"
QT_MOC_LITERAL(23, 384, 19), // "audioPlaybackFailed"
QT_MOC_LITERAL(24, 404, 14), // "isNetworkError"
QT_MOC_LITERAL(25, 419, 23), // "listeningHistoryChanged"
QT_MOC_LITERAL(26, 443, 18), // "onlineStateChanged"
QT_MOC_LITERAL(27, 462, 8), // "isOnline"
QT_MOC_LITERAL(28, 471, 20), // "onMediaStatusChanged"
QT_MOC_LITERAL(29, 492, 25), // "QMediaPlayer::MediaStatus"
QT_MOC_LITERAL(30, 518, 6), // "status"
QT_MOC_LITERAL(31, 525, 17), // "onPositionChanged"
QT_MOC_LITERAL(32, 543, 8), // "position"
QT_MOC_LITERAL(33, 552, 17), // "onDurationChanged"
QT_MOC_LITERAL(34, 570, 8), // "duration"
QT_MOC_LITERAL(35, 579, 13), // "onPlayerError"
QT_MOC_LITERAL(36, 593, 19), // "QMediaPlayer::Error"
QT_MOC_LITERAL(37, 613, 5), // "error"
QT_MOC_LITERAL(38, 619, 16), // "onPageDownloaded"
QT_MOC_LITERAL(39, 636, 17), // "formatTajweedHtml"
QT_MOC_LITERAL(40, 654, 11), // "tajweedText"
QT_MOC_LITERAL(41, 666, 6), // "isDark"
QT_MOC_LITERAL(42, 673, 10), // "getRiwayat"
QT_MOC_LITERAL(43, 684, 9), // "getSurahs"
QT_MOC_LITERAL(44, 694, 8), // "getSurah"
QT_MOC_LITERAL(45, 703, 11), // "surahNumber"
QT_MOC_LITERAL(46, 715, 7), // "getJuzs"
QT_MOC_LITERAL(47, 723, 8), // "getHizbs"
QT_MOC_LITERAL(48, 732, 16), // "getAyahsForSurah"
QT_MOC_LITERAL(49, 749, 15), // "getAyahsForPage"
QT_MOC_LITERAL(50, 765, 14), // "getPageForAyah"
QT_MOC_LITERAL(51, 780, 9), // "riwayahId"
QT_MOC_LITERAL(52, 790, 7), // "getAyah"
QT_MOC_LITERAL(53, 798, 10), // "ayahNumber"
QT_MOC_LITERAL(54, 809, 6), // "search"
QT_MOC_LITERAL(55, 816, 5), // "query"
QT_MOC_LITERAL(56, 822, 11), // "surahFilter"
QT_MOC_LITERAL(57, 834, 14), // "searchAdvanced"
QT_MOC_LITERAL(58, 849, 10), // "searchMode"
QT_MOC_LITERAL(59, 860, 9), // "scopeType"
QT_MOC_LITERAL(60, 870, 10), // "scopeValue"
QT_MOC_LITERAL(61, 881, 18), // "translationEdition"
QT_MOC_LITERAL(62, 900, 15), // "respectTashkeel"
QT_MOC_LITERAL(63, 916, 14), // "getSearchStats"
QT_MOC_LITERAL(64, 931, 17), // "cleanTashkeelText"
QT_MOC_LITERAL(65, 949, 4), // "text"
QT_MOC_LITERAL(66, 954, 15), // "getPageImageUrl"
QT_MOC_LITERAL(67, 970, 18), // "getRenderedPageUrl"
QT_MOC_LITERAL(68, 989, 14), // "highlightSurah"
QT_MOC_LITERAL(69, 1004, 13), // "highlightAyah"
QT_MOC_LITERAL(70, 1018, 8), // "darkMode"
QT_MOC_LITERAL(71, 1027, 12), // "isPageCached"
QT_MOC_LITERAL(72, 1040, 17), // "isPageDownloading"
QT_MOC_LITERAL(73, 1058, 11), // "preloadPage"
QT_MOC_LITERAL(74, 1070, 19), // "getAyahAtCoordinate"
QT_MOC_LITERAL(75, 1090, 11), // "normalizedX"
QT_MOC_LITERAL(76, 1102, 11), // "normalizedY"
QT_MOC_LITERAL(77, 1114, 22), // "startBulkPagesDownload"
QT_MOC_LITERAL(78, 1137, 23), // "cancelBulkPagesDownload"
QT_MOC_LITERAL(79, 1161, 8), // "playAyah"
QT_MOC_LITERAL(80, 1170, 9), // "playSurah"
QT_MOC_LITERAL(81, 1180, 9), // "startAyah"
QT_MOC_LITERAL(82, 1190, 10), // "pauseAudio"
QT_MOC_LITERAL(83, 1201, 11), // "resumeAudio"
QT_MOC_LITERAL(84, 1213, 9), // "stopAudio"
QT_MOC_LITERAL(85, 1223, 20), // "getAvailableReciters"
QT_MOC_LITERAL(86, 1244, 10), // "setReciter"
QT_MOC_LITERAL(87, 1255, 9), // "reciterId"
QT_MOC_LITERAL(88, 1265, 23), // "startSurahAudioDownload"
QT_MOC_LITERAL(89, 1289, 22), // "startFullAudioDownload"
QT_MOC_LITERAL(90, 1312, 19), // "cancelAudioDownload"
QT_MOC_LITERAL(91, 1332, 17), // "isAyahAudioCached"
QT_MOC_LITERAL(92, 1350, 18), // "isSurahAudioCached"
QT_MOC_LITERAL(93, 1369, 15), // "getAyahAudioUrl"
QT_MOC_LITERAL(94, 1385, 23), // "getDownloadedPagesCount"
QT_MOC_LITERAL(95, 1409, 17), // "getPagesCacheSize"
QT_MOC_LITERAL(96, 1427, 15), // "clearPagesCache"
QT_MOC_LITERAL(97, 1443, 23), // "getDownloadedAudioCount"
QT_MOC_LITERAL(98, 1467, 17), // "getAudioCacheSize"
QT_MOC_LITERAL(99, 1485, 15), // "clearAudioCache"
QT_MOC_LITERAL(100, 1501, 14), // "formatFileSize"
QT_MOC_LITERAL(101, 1516, 5), // "bytes"
QT_MOC_LITERAL(102, 1522, 14), // "toggleBookmark"
QT_MOC_LITERAL(103, 1537, 14), // "removeBookmark"
QT_MOC_LITERAL(104, 1552, 17), // "clearAllBookmarks"
QT_MOC_LITERAL(105, 1570, 12), // "isBookmarked"
QT_MOC_LITERAL(106, 1583, 12), // "getBookmarks"
QT_MOC_LITERAL(107, 1596, 16), // "saveLastPosition"
QT_MOC_LITERAL(108, 1613, 4), // "page"
QT_MOC_LITERAL(109, 1618, 26), // "getAvailableTafsirEditions"
QT_MOC_LITERAL(110, 1645, 13), // "getAyahTafsir"
QT_MOC_LITERAL(111, 1659, 9), // "editionId"
QT_MOC_LITERAL(112, 1669, 24), // "getTafsirAndTranslations"
QT_MOC_LITERAL(113, 1694, 19), // "createTextShareFile"
QT_MOC_LITERAL(114, 1714, 5), // "title"
QT_MOC_LITERAL(115, 1720, 7), // "content"
QT_MOC_LITERAL(116, 1728, 18), // "recordAyahListened"
QT_MOC_LITERAL(117, 1747, 22), // "getDailyListeningStats"
QT_MOC_LITERAL(118, 1770, 21), // "getTodayListenedCount"
QT_MOC_LITERAL(119, 1792, 21), // "getTotalListenedCount"
QT_MOC_LITERAL(120, 1814, 21), // "clearListeningHistory"
QT_MOC_LITERAL(121, 1836, 7), // "riwayah"
QT_MOC_LITERAL(122, 1844, 11), // "riwayahCode"
QT_MOC_LITERAL(123, 1856, 11), // "riwayahName"
QT_MOC_LITERAL(124, 1868, 18), // "riwayahDescription"
QT_MOC_LITERAL(125, 1887, 8), // "fontSize"
QT_MOC_LITERAL(126, 1896, 14), // "tafsirFontSize"
QT_MOC_LITERAL(127, 1911, 8), // "viewMode"
QT_MOC_LITERAL(128, 1920, 11), // "tajweedMode"
QT_MOC_LITERAL(129, 1932, 9), // "lastSurah"
QT_MOC_LITERAL(130, 1942, 8), // "lastAyah"
QT_MOC_LITERAL(131, 1951, 8), // "lastPage"
QT_MOC_LITERAL(132, 1960, 23), // "selectedTafsirEditionId"
QT_MOC_LITERAL(133, 1984, 9), // "isPlaying"
QT_MOC_LITERAL(134, 1994, 12), // "playingSurah"
QT_MOC_LITERAL(135, 2007, 11), // "playingAyah"
QT_MOC_LITERAL(136, 2019, 17), // "selectedReciterId"
QT_MOC_LITERAL(137, 2037, 19), // "selectedReciterName"
QT_MOC_LITERAL(138, 2057, 13), // "audioProgress"
QT_MOC_LITERAL(139, 2071, 18), // "isDownloadingPages"
QT_MOC_LITERAL(140, 2090, 20), // "downloadedPagesCount"
QT_MOC_LITERAL(141, 2111, 20), // "totalPagesToDownload"
QT_MOC_LITERAL(142, 2132, 21), // "pagesDownloadProgress"
QT_MOC_LITERAL(143, 2154, 18), // "isDownloadingAudio"
QT_MOC_LITERAL(144, 2173, 20), // "downloadedAudioCount"
QT_MOC_LITERAL(145, 2194, 20), // "totalAudioToDownload"
QT_MOC_LITERAL(146, 2215, 21), // "audioDownloadProgress"
QT_MOC_LITERAL(147, 2237, 19), // "audioDownloadStatus"
QT_MOC_LITERAL(148, 2257, 10), // "SearchMode"
QT_MOC_LITERAL(149, 2268, 15), // "WordWithAffixes"
QT_MOC_LITERAL(150, 2284, 12), // "ExactLiteral"
QT_MOC_LITERAL(151, 2297, 12), // "PartialMatch"
QT_MOC_LITERAL(152, 2310, 10), // "RootSearch"
QT_MOC_LITERAL(153, 2321, 12), // "PhraseSearch"
QT_MOC_LITERAL(154, 2334, 17), // "TranslationSearch"
QT_MOC_LITERAL(155, 2352, 11), // "SearchScope"
QT_MOC_LITERAL(156, 2364, 8), // "ScopeAll"
QT_MOC_LITERAL(157, 2373, 10), // "ScopeSurah"
QT_MOC_LITERAL(158, 2384, 8), // "ScopeJuz"
QT_MOC_LITERAL(159, 2393, 9) // "ScopePage"

    },
    "QuranManager\0riwayahChanged\0\0"
    "fontSizeChanged\0tafsirFontSizeChanged\0"
    "viewModeChanged\0darkModeChanged\0"
    "tajweedModeChanged\0lastPositionChanged\0"
    "selectedTafsirEditionChanged\0"
    "audioStateChanged\0playingAyahChanged\0"
    "reciterChanged\0audioProgressChanged\0"
    "pageCached\0pageNumber\0pageDownloadFailed\0"
    "bookmarksChanged\0bulkPagesProgressChanged\0"
    "audioDownloadProgressChanged\0"
    "audioStreamingNotice\0surah\0ayah\0"
    "audioPlaybackFailed\0isNetworkError\0"
    "listeningHistoryChanged\0onlineStateChanged\0"
    "isOnline\0onMediaStatusChanged\0"
    "QMediaPlayer::MediaStatus\0status\0"
    "onPositionChanged\0position\0onDurationChanged\0"
    "duration\0onPlayerError\0QMediaPlayer::Error\0"
    "error\0onPageDownloaded\0formatTajweedHtml\0"
    "tajweedText\0isDark\0getRiwayat\0getSurahs\0"
    "getSurah\0surahNumber\0getJuzs\0getHizbs\0"
    "getAyahsForSurah\0getAyahsForPage\0"
    "getPageForAyah\0riwayahId\0getAyah\0"
    "ayahNumber\0search\0query\0surahFilter\0"
    "searchAdvanced\0searchMode\0scopeType\0"
    "scopeValue\0translationEdition\0"
    "respectTashkeel\0getSearchStats\0"
    "cleanTashkeelText\0text\0getPageImageUrl\0"
    "getRenderedPageUrl\0highlightSurah\0"
    "highlightAyah\0darkMode\0isPageCached\0"
    "isPageDownloading\0preloadPage\0"
    "getAyahAtCoordinate\0normalizedX\0"
    "normalizedY\0startBulkPagesDownload\0"
    "cancelBulkPagesDownload\0playAyah\0"
    "playSurah\0startAyah\0pauseAudio\0"
    "resumeAudio\0stopAudio\0getAvailableReciters\0"
    "setReciter\0reciterId\0startSurahAudioDownload\0"
    "startFullAudioDownload\0cancelAudioDownload\0"
    "isAyahAudioCached\0isSurahAudioCached\0"
    "getAyahAudioUrl\0getDownloadedPagesCount\0"
    "getPagesCacheSize\0clearPagesCache\0"
    "getDownloadedAudioCount\0getAudioCacheSize\0"
    "clearAudioCache\0formatFileSize\0bytes\0"
    "toggleBookmark\0removeBookmark\0"
    "clearAllBookmarks\0isBookmarked\0"
    "getBookmarks\0saveLastPosition\0page\0"
    "getAvailableTafsirEditions\0getAyahTafsir\0"
    "editionId\0getTafsirAndTranslations\0"
    "createTextShareFile\0title\0content\0"
    "recordAyahListened\0getDailyListeningStats\0"
    "getTodayListenedCount\0getTotalListenedCount\0"
    "clearListeningHistory\0riwayah\0riwayahCode\0"
    "riwayahName\0riwayahDescription\0fontSize\0"
    "tafsirFontSize\0viewMode\0tajweedMode\0"
    "lastSurah\0lastAyah\0lastPage\0"
    "selectedTafsirEditionId\0isPlaying\0"
    "playingSurah\0playingAyah\0selectedReciterId\0"
    "selectedReciterName\0audioProgress\0"
    "isDownloadingPages\0downloadedPagesCount\0"
    "totalPagesToDownload\0pagesDownloadProgress\0"
    "isDownloadingAudio\0downloadedAudioCount\0"
    "totalAudioToDownload\0audioDownloadProgress\0"
    "audioDownloadStatus\0SearchMode\0"
    "WordWithAffixes\0ExactLiteral\0PartialMatch\0"
    "RootSearch\0PhraseSearch\0TranslationSearch\0"
    "SearchScope\0ScopeAll\0ScopeSurah\0"
    "ScopeJuz\0ScopePage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QuranManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
     112,   14, // methods
      29,  954, // properties
       2, 1070, // enums/sets
       0,    0, // constructors
       0,       // flags
      21,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  574,    2, 0x06 /* Public */,
       3,    0,  575,    2, 0x06 /* Public */,
       4,    0,  576,    2, 0x06 /* Public */,
       5,    0,  577,    2, 0x06 /* Public */,
       6,    0,  578,    2, 0x06 /* Public */,
       7,    0,  579,    2, 0x06 /* Public */,
       8,    0,  580,    2, 0x06 /* Public */,
       9,    0,  581,    2, 0x06 /* Public */,
      10,    0,  582,    2, 0x06 /* Public */,
      11,    0,  583,    2, 0x06 /* Public */,
      12,    0,  584,    2, 0x06 /* Public */,
      13,    0,  585,    2, 0x06 /* Public */,
      14,    1,  586,    2, 0x06 /* Public */,
      16,    1,  589,    2, 0x06 /* Public */,
      17,    0,  592,    2, 0x06 /* Public */,
      18,    0,  593,    2, 0x06 /* Public */,
      19,    0,  594,    2, 0x06 /* Public */,
      20,    2,  595,    2, 0x06 /* Public */,
      23,    3,  600,    2, 0x06 /* Public */,
      25,    0,  607,    2, 0x06 /* Public */,
      26,    1,  608,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      28,    1,  611,    2, 0x08 /* Private */,
      31,    1,  614,    2, 0x08 /* Private */,
      33,    1,  617,    2, 0x08 /* Private */,
      35,    1,  620,    2, 0x08 /* Private */,
      38,    0,  623,    2, 0x08 /* Private */,

 // methods: name, argc, parameters, tag, flags
      39,    2,  624,    2, 0x02 /* Public */,
      39,    1,  629,    2, 0x22 /* Public | MethodCloned */,
      42,    0,  632,    2, 0x02 /* Public */,
      43,    0,  633,    2, 0x02 /* Public */,
      44,    1,  634,    2, 0x02 /* Public */,
      46,    0,  637,    2, 0x02 /* Public */,
      47,    0,  638,    2, 0x02 /* Public */,
      48,    1,  639,    2, 0x02 /* Public */,
      49,    1,  642,    2, 0x02 /* Public */,
      50,    3,  645,    2, 0x02 /* Public */,
      50,    2,  652,    2, 0x22 /* Public | MethodCloned */,
      52,    3,  657,    2, 0x02 /* Public */,
      52,    2,  664,    2, 0x22 /* Public | MethodCloned */,
      54,    2,  669,    2, 0x02 /* Public */,
      54,    1,  674,    2, 0x22 /* Public | MethodCloned */,
      57,    6,  677,    2, 0x02 /* Public */,
      57,    5,  690,    2, 0x22 /* Public | MethodCloned */,
      57,    4,  701,    2, 0x22 /* Public | MethodCloned */,
      57,    3,  710,    2, 0x22 /* Public | MethodCloned */,
      57,    2,  717,    2, 0x22 /* Public | MethodCloned */,
      57,    1,  722,    2, 0x22 /* Public | MethodCloned */,
      63,    4,  725,    2, 0x02 /* Public */,
      63,    3,  734,    2, 0x22 /* Public | MethodCloned */,
      63,    2,  741,    2, 0x22 /* Public | MethodCloned */,
      63,    1,  746,    2, 0x22 /* Public | MethodCloned */,
      64,    1,  749,    2, 0x02 /* Public */,
      66,    1,  752,    2, 0x02 /* Public */,
      67,    4,  755,    2, 0x02 /* Public */,
      71,    1,  764,    2, 0x02 /* Public */,
      72,    1,  767,    2, 0x02 /* Public */,
      73,    1,  770,    2, 0x02 /* Public */,
      74,    4,  773,    2, 0x02 /* Public */,
      74,    3,  782,    2, 0x22 /* Public | MethodCloned */,
      77,    1,  789,    2, 0x02 /* Public */,
      77,    0,  792,    2, 0x22 /* Public | MethodCloned */,
      78,    0,  793,    2, 0x02 /* Public */,
      79,    2,  794,    2, 0x02 /* Public */,
      80,    2,  799,    2, 0x02 /* Public */,
      80,    1,  804,    2, 0x22 /* Public | MethodCloned */,
      82,    0,  807,    2, 0x02 /* Public */,
      83,    0,  808,    2, 0x02 /* Public */,
      84,    0,  809,    2, 0x02 /* Public */,
      85,    0,  810,    2, 0x02 /* Public */,
      86,    1,  811,    2, 0x02 /* Public */,
      88,    2,  814,    2, 0x02 /* Public */,
      88,    1,  819,    2, 0x22 /* Public | MethodCloned */,
      89,    1,  822,    2, 0x02 /* Public */,
      89,    0,  825,    2, 0x22 /* Public | MethodCloned */,
      90,    0,  826,    2, 0x02 /* Public */,
      91,    3,  827,    2, 0x02 /* Public */,
      91,    2,  834,    2, 0x22 /* Public | MethodCloned */,
      92,    2,  839,    2, 0x02 /* Public */,
      92,    1,  844,    2, 0x22 /* Public | MethodCloned */,
      93,    3,  847,    2, 0x02 /* Public */,
      27,    0,  854,    2, 0x02 /* Public */,
      94,    1,  855,    2, 0x02 /* Public */,
      94,    0,  858,    2, 0x22 /* Public | MethodCloned */,
      95,    1,  859,    2, 0x02 /* Public */,
      95,    0,  862,    2, 0x22 /* Public | MethodCloned */,
      96,    1,  863,    2, 0x02 /* Public */,
      96,    0,  866,    2, 0x22 /* Public | MethodCloned */,
      97,    1,  867,    2, 0x02 /* Public */,
      97,    0,  870,    2, 0x22 /* Public | MethodCloned */,
      98,    1,  871,    2, 0x02 /* Public */,
      98,    0,  874,    2, 0x22 /* Public | MethodCloned */,
      99,    1,  875,    2, 0x02 /* Public */,
      99,    0,  878,    2, 0x22 /* Public | MethodCloned */,
     100,    1,  879,    2, 0x02 /* Public */,
     102,    2,  882,    2, 0x02 /* Public */,
     103,    2,  887,    2, 0x02 /* Public */,
     104,    0,  892,    2, 0x02 /* Public */,
     105,    2,  893,    2, 0x02 /* Public */,
     106,    0,  898,    2, 0x02 /* Public */,
     107,    3,  899,    2, 0x02 /* Public */,
     109,    0,  906,    2, 0x02 /* Public */,
     110,    4,  907,    2, 0x02 /* Public */,
     110,    3,  916,    2, 0x22 /* Public | MethodCloned */,
     110,    2,  923,    2, 0x22 /* Public | MethodCloned */,
     112,    3,  928,    2, 0x02 /* Public */,
     112,    2,  935,    2, 0x22 /* Public | MethodCloned */,
     113,    2,  940,    2, 0x02 /* Public */,
     116,    2,  945,    2, 0x02 /* Public */,
     117,    0,  950,    2, 0x02 /* Public */,
     118,    0,  951,    2, 0x02 /* Public */,
     119,    0,  952,    2, 0x02 /* Public */,
     120,    0,  953,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   21,   22,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Bool,   21,   22,   24,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   27,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 29,   30,
    QMetaType::Void, QMetaType::LongLong,   32,
    QMetaType::Void, QMetaType::LongLong,   34,
    QMetaType::Void, 0x80000000 | 36,   37,
    QMetaType::Void,

 // methods: parameters
    QMetaType::QString, QMetaType::QString, QMetaType::Bool,   40,   41,
    QMetaType::QString, QMetaType::QString,   40,
    QMetaType::QVariantList,
    QMetaType::QVariantList,
    QMetaType::QVariantMap, QMetaType::Int,   45,
    QMetaType::QVariantList,
    QMetaType::QVariantList,
    QMetaType::QVariantList, QMetaType::Int,   45,
    QMetaType::QVariantList, QMetaType::Int,   15,
    QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,   21,   22,   51,
    QMetaType::Int, QMetaType::Int, QMetaType::Int,   21,   22,
    QMetaType::QVariantMap, QMetaType::Int, QMetaType::Int, QMetaType::Int,   45,   53,   51,
    QMetaType::QVariantMap, QMetaType::Int, QMetaType::Int,   45,   53,
    QMetaType::QVariantList, QMetaType::QString, QMetaType::Int,   55,   56,
    QMetaType::QVariantList, QMetaType::QString,   55,
    QMetaType::QVariantList, QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::Bool,   55,   58,   59,   60,   61,   62,
    QMetaType::QVariantList, QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::QString,   55,   58,   59,   60,   61,
    QMetaType::QVariantList, QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::Int,   55,   58,   59,   60,
    QMetaType::QVariantList, QMetaType::QString, QMetaType::Int, QMetaType::Int,   55,   58,   59,
    QMetaType::QVariantList, QMetaType::QString, QMetaType::Int,   55,   58,
    QMetaType::QVariantList, QMetaType::QString,   55,
    QMetaType::QVariantMap, QMetaType::QString, QMetaType::Int, QMetaType::Bool, QMetaType::QString,   55,   58,   62,   61,
    QMetaType::QVariantMap, QMetaType::QString, QMetaType::Int, QMetaType::Bool,   55,   58,   62,
    QMetaType::QVariantMap, QMetaType::QString, QMetaType::Int,   55,   58,
    QMetaType::QVariantMap, QMetaType::QString,   55,
    QMetaType::QString, QMetaType::QString,   65,
    QMetaType::QString, QMetaType::Int,   15,
    QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Bool,   15,   68,   69,   70,
    QMetaType::Bool, QMetaType::Int,   15,
    QMetaType::Bool, QMetaType::Int,   15,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::QVariantMap, QMetaType::Int, QMetaType::Double, QMetaType::Double, QMetaType::Int,   15,   75,   76,   51,
    QMetaType::QVariantMap, QMetaType::Int, QMetaType::Double, QMetaType::Double,   15,   75,   76,
    QMetaType::Void, QMetaType::Int,   51,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   21,   22,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   21,   81,
    QMetaType::Void, QMetaType::Int,   21,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::QVariantList,
    QMetaType::Void, QMetaType::QString,   87,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   45,   87,
    QMetaType::Void, QMetaType::Int,   45,
    QMetaType::Void, QMetaType::QString,   87,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool, QMetaType::Int, QMetaType::Int, QMetaType::QString,   21,   22,   87,
    QMetaType::Bool, QMetaType::Int, QMetaType::Int,   21,   22,
    QMetaType::Bool, QMetaType::Int, QMetaType::QString,   21,   87,
    QMetaType::Bool, QMetaType::Int,   21,
    QMetaType::QString, QMetaType::QString, QMetaType::Int, QMetaType::Int,   87,   21,   22,
    QMetaType::Bool,
    QMetaType::Int, QMetaType::Int,   51,
    QMetaType::Int,
    QMetaType::LongLong, QMetaType::Int,   51,
    QMetaType::LongLong,
    QMetaType::Void, QMetaType::Int,   51,
    QMetaType::Void,
    QMetaType::Int, QMetaType::QString,   87,
    QMetaType::Int,
    QMetaType::LongLong, QMetaType::QString,   87,
    QMetaType::LongLong,
    QMetaType::Void, QMetaType::QString,   87,
    QMetaType::Void,
    QMetaType::QString, QMetaType::LongLong,  101,
    QMetaType::Bool, QMetaType::Int, QMetaType::Int,   21,   22,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   21,   22,
    QMetaType::Void,
    QMetaType::Bool, QMetaType::Int, QMetaType::Int,   21,   22,
    QMetaType::QVariantList,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,   21,   22,  108,
    QMetaType::QVariantList,
    QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::Int,   21,   22,  111,   51,
    QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::QString,   21,   22,  111,
    QMetaType::QString, QMetaType::Int, QMetaType::Int,   21,   22,
    QMetaType::QVariantList, QMetaType::Int, QMetaType::Int, QMetaType::Int,   21,   22,   51,
    QMetaType::QVariantList, QMetaType::Int, QMetaType::Int,   21,   22,
    QMetaType::QString, QMetaType::QString, QMetaType::QString,  114,  115,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   21,   22,
    QMetaType::QVariantList,
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::Void,

 // properties: name, type, flags
     121, QMetaType::Int, 0x00495103,
     122, QMetaType::QString, 0x00495001,
     123, QMetaType::QString, 0x00495001,
     124, QMetaType::QString, 0x00495001,
     125, QMetaType::Int, 0x00495103,
     126, QMetaType::Int, 0x00495103,
     127, QMetaType::Int, 0x00495103,
      70, QMetaType::Bool, 0x00495103,
     128, QMetaType::Bool, 0x00495103,
     129, QMetaType::Int, 0x00495001,
     130, QMetaType::Int, 0x00495001,
     131, QMetaType::Int, 0x00495001,
     132, QMetaType::QString, 0x00495103,
     133, QMetaType::Bool, 0x00495001,
     134, QMetaType::Int, 0x00495001,
     135, QMetaType::Int, 0x00495001,
     136, QMetaType::QString, 0x00495001,
     137, QMetaType::QString, 0x00495001,
     138, QMetaType::Double, 0x00495001,
      27, QMetaType::Bool, 0x00495001,
     139, QMetaType::Bool, 0x00495001,
     140, QMetaType::Int, 0x00495001,
     141, QMetaType::Int, 0x00495001,
     142, QMetaType::Double, 0x00495001,
     143, QMetaType::Bool, 0x00495001,
     144, QMetaType::Int, 0x00495001,
     145, QMetaType::Int, 0x00495001,
     146, QMetaType::Double, 0x00495001,
     147, QMetaType::QString, 0x00495001,

 // properties: notify_signal_id
       0,
       0,
       0,
       0,
       1,
       2,
       3,
       4,
       5,
       6,
       6,
       6,
       7,
       8,
       9,
       9,
      10,
      10,
      11,
      20,
      15,
      15,
      15,
      15,
      16,
      16,
      16,
      16,
      16,

 // enums: name, flags, count, data
     148, 0x0,    6, 1078,
     155, 0x0,    4, 1090,

 // enum data: key, value
     149, uint(QuranManager::WordWithAffixes),
     150, uint(QuranManager::ExactLiteral),
     151, uint(QuranManager::PartialMatch),
     152, uint(QuranManager::RootSearch),
     153, uint(QuranManager::PhraseSearch),
     154, uint(QuranManager::TranslationSearch),
     156, uint(QuranManager::ScopeAll),
     157, uint(QuranManager::ScopeSurah),
     158, uint(QuranManager::ScopeJuz),
     159, uint(QuranManager::ScopePage),

       0        // eod
};

void QuranManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QuranManager *_t = static_cast<QuranManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->riwayahChanged(); break;
        case 1: _t->fontSizeChanged(); break;
        case 2: _t->tafsirFontSizeChanged(); break;
        case 3: _t->viewModeChanged(); break;
        case 4: _t->darkModeChanged(); break;
        case 5: _t->tajweedModeChanged(); break;
        case 6: _t->lastPositionChanged(); break;
        case 7: _t->selectedTafsirEditionChanged(); break;
        case 8: _t->audioStateChanged(); break;
        case 9: _t->playingAyahChanged(); break;
        case 10: _t->reciterChanged(); break;
        case 11: _t->audioProgressChanged(); break;
        case 12: _t->pageCached((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->pageDownloadFailed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->bookmarksChanged(); break;
        case 15: _t->bulkPagesProgressChanged(); break;
        case 16: _t->audioDownloadProgressChanged(); break;
        case 17: _t->audioStreamingNotice((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 18: _t->audioPlaybackFailed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 19: _t->listeningHistoryChanged(); break;
        case 20: _t->onlineStateChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 21: _t->onMediaStatusChanged((*reinterpret_cast< QMediaPlayer::MediaStatus(*)>(_a[1]))); break;
        case 22: _t->onPositionChanged((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 23: _t->onDurationChanged((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 24: _t->onPlayerError((*reinterpret_cast< QMediaPlayer::Error(*)>(_a[1]))); break;
        case 25: _t->onPageDownloaded(); break;
        case 26: { QString _r = _t->formatTajweedHtml((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 27: { QString _r = _t->formatTajweedHtml((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 28: { QVariantList _r = _t->getRiwayat();
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 29: { QVariantList _r = _t->getSurahs();
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 30: { QVariantMap _r = _t->getSurah((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = _r; }  break;
        case 31: { QVariantList _r = _t->getJuzs();
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 32: { QVariantList _r = _t->getHizbs();
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 33: { QVariantList _r = _t->getAyahsForSurah((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 34: { QVariantList _r = _t->getAyahsForPage((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 35: { int _r = _t->getPageForAyah((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 36: { int _r = _t->getPageForAyah((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 37: { QVariantMap _r = _t->getAyah((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = _r; }  break;
        case 38: { QVariantMap _r = _t->getAyah((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = _r; }  break;
        case 39: { QVariantList _r = _t->search((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 40: { QVariantList _r = _t->search((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 41: { QVariantList _r = _t->searchAdvanced((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< const QString(*)>(_a[5])),(*reinterpret_cast< bool(*)>(_a[6])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 42: { QVariantList _r = _t->searchAdvanced((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< const QString(*)>(_a[5])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 43: { QVariantList _r = _t->searchAdvanced((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 44: { QVariantList _r = _t->searchAdvanced((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 45: { QVariantList _r = _t->searchAdvanced((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 46: { QVariantList _r = _t->searchAdvanced((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 47: { QVariantMap _r = _t->getSearchStats((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = _r; }  break;
        case 48: { QVariantMap _r = _t->getSearchStats((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = _r; }  break;
        case 49: { QVariantMap _r = _t->getSearchStats((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = _r; }  break;
        case 50: { QVariantMap _r = _t->getSearchStats((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = _r; }  break;
        case 51: { QString _r = _t->cleanTashkeelText((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 52: { QString _r = _t->getPageImageUrl((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 53: { QString _r = _t->getRenderedPageUrl((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 54: { bool _r = _t->isPageCached((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 55: { bool _r = _t->isPageDownloading((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 56: _t->preloadPage((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 57: { QVariantMap _r = _t->getAyahAtCoordinate((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = _r; }  break;
        case 58: { QVariantMap _r = _t->getAyahAtCoordinate((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = _r; }  break;
        case 59: _t->startBulkPagesDownload((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 60: _t->startBulkPagesDownload(); break;
        case 61: _t->cancelBulkPagesDownload(); break;
        case 62: _t->playAyah((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 63: _t->playSurah((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 64: _t->playSurah((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 65: _t->pauseAudio(); break;
        case 66: _t->resumeAudio(); break;
        case 67: _t->stopAudio(); break;
        case 68: { QVariantList _r = _t->getAvailableReciters();
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 69: _t->setReciter((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 70: _t->startSurahAudioDownload((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 71: _t->startSurahAudioDownload((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 72: _t->startFullAudioDownload((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 73: _t->startFullAudioDownload(); break;
        case 74: _t->cancelAudioDownload(); break;
        case 75: { bool _r = _t->isAyahAudioCached((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 76: { bool _r = _t->isAyahAudioCached((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 77: { bool _r = _t->isSurahAudioCached((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 78: { bool _r = _t->isSurahAudioCached((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 79: { QString _r = _t->getAyahAudioUrl((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 80: { bool _r = _t->isOnline();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 81: { int _r = _t->getDownloadedPagesCount((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 82: { int _r = _t->getDownloadedPagesCount();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 83: { qint64 _r = _t->getPagesCacheSize((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< qint64*>(_a[0]) = _r; }  break;
        case 84: { qint64 _r = _t->getPagesCacheSize();
            if (_a[0]) *reinterpret_cast< qint64*>(_a[0]) = _r; }  break;
        case 85: _t->clearPagesCache((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 86: _t->clearPagesCache(); break;
        case 87: { int _r = _t->getDownloadedAudioCount((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 88: { int _r = _t->getDownloadedAudioCount();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 89: { qint64 _r = _t->getAudioCacheSize((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< qint64*>(_a[0]) = _r; }  break;
        case 90: { qint64 _r = _t->getAudioCacheSize();
            if (_a[0]) *reinterpret_cast< qint64*>(_a[0]) = _r; }  break;
        case 91: _t->clearAudioCache((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 92: _t->clearAudioCache(); break;
        case 93: { QString _r = _t->formatFileSize((*reinterpret_cast< qint64(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 94: { bool _r = _t->toggleBookmark((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 95: _t->removeBookmark((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 96: _t->clearAllBookmarks(); break;
        case 97: { bool _r = _t->isBookmarked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 98: { QVariantList _r = _t->getBookmarks();
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 99: _t->saveLastPosition((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 100: { QVariantList _r = _t->getAvailableTafsirEditions();
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 101: { QString _r = _t->getAyahTafsir((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 102: { QString _r = _t->getAyahTafsir((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 103: { QString _r = _t->getAyahTafsir((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 104: { QVariantList _r = _t->getTafsirAndTranslations((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 105: { QVariantList _r = _t->getTafsirAndTranslations((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 106: { QString _r = _t->createTextShareFile((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 107: _t->recordAyahListened((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 108: { QVariantList _r = _t->getDailyListeningStats();
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 109: { int _r = _t->getTodayListenedCount();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 110: { int _r = _t->getTotalListenedCount();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 111: _t->clearListeningHistory(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 21:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QMediaPlayer::MediaStatus >(); break;
            }
            break;
        case 24:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QMediaPlayer::Error >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QuranManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuranManager::riwayahChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (QuranManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuranManager::fontSizeChanged)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (QuranManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuranManager::tafsirFontSizeChanged)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (QuranManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuranManager::viewModeChanged)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (QuranManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuranManager::darkModeChanged)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (QuranManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuranManager::tajweedModeChanged)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (QuranManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuranManager::lastPositionChanged)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (QuranManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuranManager::selectedTafsirEditionChanged)) {
                *result = 7;
                return;
            }
        }
        {
            typedef void (QuranManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuranManager::audioStateChanged)) {
                *result = 8;
                return;
            }
        }
        {
            typedef void (QuranManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuranManager::playingAyahChanged)) {
                *result = 9;
                return;
            }
        }
        {
            typedef void (QuranManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuranManager::reciterChanged)) {
                *result = 10;
                return;
            }
        }
        {
            typedef void (QuranManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuranManager::audioProgressChanged)) {
                *result = 11;
                return;
            }
        }
        {
            typedef void (QuranManager::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuranManager::pageCached)) {
                *result = 12;
                return;
            }
        }
        {
            typedef void (QuranManager::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuranManager::pageDownloadFailed)) {
                *result = 13;
                return;
            }
        }
        {
            typedef void (QuranManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuranManager::bookmarksChanged)) {
                *result = 14;
                return;
            }
        }
        {
            typedef void (QuranManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuranManager::bulkPagesProgressChanged)) {
                *result = 15;
                return;
            }
        }
        {
            typedef void (QuranManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuranManager::audioDownloadProgressChanged)) {
                *result = 16;
                return;
            }
        }
        {
            typedef void (QuranManager::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuranManager::audioStreamingNotice)) {
                *result = 17;
                return;
            }
        }
        {
            typedef void (QuranManager::*_t)(int , int , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuranManager::audioPlaybackFailed)) {
                *result = 18;
                return;
            }
        }
        {
            typedef void (QuranManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuranManager::listeningHistoryChanged)) {
                *result = 19;
                return;
            }
        }
        {
            typedef void (QuranManager::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuranManager::onlineStateChanged)) {
                *result = 20;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        QuranManager *_t = static_cast<QuranManager *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->riwayah(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->riwayahCode(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->riwayahName(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->riwayahDescription(); break;
        case 4: *reinterpret_cast< int*>(_v) = _t->fontSize(); break;
        case 5: *reinterpret_cast< int*>(_v) = _t->tafsirFontSize(); break;
        case 6: *reinterpret_cast< int*>(_v) = _t->viewMode(); break;
        case 7: *reinterpret_cast< bool*>(_v) = _t->darkMode(); break;
        case 8: *reinterpret_cast< bool*>(_v) = _t->tajweedMode(); break;
        case 9: *reinterpret_cast< int*>(_v) = _t->lastSurah(); break;
        case 10: *reinterpret_cast< int*>(_v) = _t->lastAyah(); break;
        case 11: *reinterpret_cast< int*>(_v) = _t->lastPage(); break;
        case 12: *reinterpret_cast< QString*>(_v) = _t->selectedTafsirEditionId(); break;
        case 13: *reinterpret_cast< bool*>(_v) = _t->isPlaying(); break;
        case 14: *reinterpret_cast< int*>(_v) = _t->playingSurah(); break;
        case 15: *reinterpret_cast< int*>(_v) = _t->playingAyah(); break;
        case 16: *reinterpret_cast< QString*>(_v) = _t->selectedReciterId(); break;
        case 17: *reinterpret_cast< QString*>(_v) = _t->selectedReciterName(); break;
        case 18: *reinterpret_cast< double*>(_v) = _t->audioProgress(); break;
        case 19: *reinterpret_cast< bool*>(_v) = _t->isOnline(); break;
        case 20: *reinterpret_cast< bool*>(_v) = _t->isDownloadingPages(); break;
        case 21: *reinterpret_cast< int*>(_v) = _t->downloadedPagesCount(); break;
        case 22: *reinterpret_cast< int*>(_v) = _t->totalPagesToDownload(); break;
        case 23: *reinterpret_cast< double*>(_v) = _t->pagesDownloadProgress(); break;
        case 24: *reinterpret_cast< bool*>(_v) = _t->isDownloadingAudio(); break;
        case 25: *reinterpret_cast< int*>(_v) = _t->downloadedAudioCount(); break;
        case 26: *reinterpret_cast< int*>(_v) = _t->totalAudioToDownload(); break;
        case 27: *reinterpret_cast< double*>(_v) = _t->audioDownloadProgress(); break;
        case 28: *reinterpret_cast< QString*>(_v) = _t->audioDownloadStatus(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        QuranManager *_t = static_cast<QuranManager *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setRiwayah(*reinterpret_cast< int*>(_v)); break;
        case 4: _t->setFontSize(*reinterpret_cast< int*>(_v)); break;
        case 5: _t->setTafsirFontSize(*reinterpret_cast< int*>(_v)); break;
        case 6: _t->setViewMode(*reinterpret_cast< int*>(_v)); break;
        case 7: _t->setDarkMode(*reinterpret_cast< bool*>(_v)); break;
        case 8: _t->setTajweedMode(*reinterpret_cast< bool*>(_v)); break;
        case 12: _t->setSelectedTafsirEditionId(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject QuranManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QuranManager.data,
      qt_meta_data_QuranManager,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QuranManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QuranManager::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QuranManager.stringdata0))
        return static_cast<void*>(const_cast< QuranManager*>(this));
    return QObject::qt_metacast(_clname);
}

int QuranManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 112)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 112;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 112)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 112;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 29;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 29;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 29;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 29;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 29;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QuranManager::riwayahChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void QuranManager::fontSizeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void QuranManager::tafsirFontSizeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void QuranManager::viewModeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void QuranManager::darkModeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}

// SIGNAL 5
void QuranManager::tajweedModeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}

// SIGNAL 6
void QuranManager::lastPositionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, Q_NULLPTR);
}

// SIGNAL 7
void QuranManager::selectedTafsirEditionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, Q_NULLPTR);
}

// SIGNAL 8
void QuranManager::audioStateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, Q_NULLPTR);
}

// SIGNAL 9
void QuranManager::playingAyahChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, Q_NULLPTR);
}

// SIGNAL 10
void QuranManager::reciterChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, Q_NULLPTR);
}

// SIGNAL 11
void QuranManager::audioProgressChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 11, Q_NULLPTR);
}

// SIGNAL 12
void QuranManager::pageCached(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void QuranManager::pageDownloadFailed(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void QuranManager::bookmarksChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 14, Q_NULLPTR);
}

// SIGNAL 15
void QuranManager::bulkPagesProgressChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 15, Q_NULLPTR);
}

// SIGNAL 16
void QuranManager::audioDownloadProgressChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 16, Q_NULLPTR);
}

// SIGNAL 17
void QuranManager::audioStreamingNotice(int _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}

// SIGNAL 18
void QuranManager::audioPlaybackFailed(int _t1, int _t2, bool _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}

// SIGNAL 19
void QuranManager::listeningHistoryChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 19, Q_NULLPTR);
}

// SIGNAL 20
void QuranManager::onlineStateChanged(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 20, _a);
}
QT_END_MOC_NAMESPACE

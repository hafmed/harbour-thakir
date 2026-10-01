import sqlite3, re, io, sys

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
conn = sqlite3.connect('data/quran.db')

def norm(t):
    t = re.sub(r'[\u0610-\u061A\u064B-\u065F\u0670\u06D6-\u06ED\u08D4-\u08E1\u08E3-\u08FF]', '', t)
    t = re.sub(r'[\u06D6-\u06DC\u06DF-\u06E4\u06E7\u06E8\u06EA-\u06ED۞۩ۣۚۖۗۘۜ۟۠ۢۥۦ۪ۭۧۨ۫۬]', '', t)
    t = re.sub(r'[إأآاٱء]', 'ا', t)
    t = re.sub(r'[يىئ]', 'ي', t)
    t = re.sub(r'[ة]', 'ه', t)
    return re.sub(r'\s+', ' ', t).strip()

# Let's inspect surahs with verse count differences
diff_surahs = []
for s in range(1, 115):
    h_c = conn.execute("SELECT COUNT(*) FROM ayahs WHERE riwayah_id=1 AND surah_number=?", (s,)).fetchone()[0]
    w_c = conn.execute("SELECT COUNT(*) FROM ayahs WHERE riwayah_id=2 AND surah_number=?", (s,)).fetchone()[0]
    if h_c != w_c:
        s_name = conn.execute("SELECT name_ar FROM surahs WHERE number=?", (s,)).fetchone()[0]
        diff_surahs.append((s, s_name, h_c, w_c))

print(f"Total surahs with different verse counts: {len(diff_surahs)}")
for s, name, hc, wc in diff_surahs:
    print(f"Surah {s:3d} ({name:12s}): Hafs={hc:3d}, Warsh={wc:3d}, Diff={wc - hc:+2d}")

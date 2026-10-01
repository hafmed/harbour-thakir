import sqlite3, re, sys, io

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')

conn = sqlite3.connect('data/quran.db')

def normalize(t):
    t = re.sub(r'[\u0610-\u061A\u064B-\u065F\u0670\u06D6-\u06ED\u08D4-\u08E1\u08E3-\u08FF]', '', t)
    t = re.sub(r'[\u06D6-\u06DC\u06DF-\u06E4\u06E7\u06E8\u06EA-\u06ED]', '', t)
    t = re.sub(r'[۞۩ۣۚۖۗۘۜ۟۠ۢۥۦ۪ۭۧۨ۫۬]', '', t)
    t = re.sub(r'[إأآاٱء]', 'ا', t)
    t = re.sub(r'[يىئ]', 'ي', t)
    t = re.sub(r'[ة]', 'ه', t)
    t = re.sub(r'\s+', ' ', t).strip()
    return t

# For each surah, find which Warsh verses cover multiple Hafs verses!
multi_hafs_warsh_ayahs = []

for surah in range(1, 115):
    h_rows = conn.execute('SELECT ayah_number, text_uthmani FROM ayahs WHERE riwayah_id=1 AND surah_number=? ORDER BY ayah_number', (surah,)).fetchall()
    w_rows = conn.execute('SELECT ayah_number, text_uthmani FROM ayahs WHERE riwayah_id=2 AND surah_number=? ORDER BY ayah_number', (surah,)).fetchall()
    
    if surah == 1:
        # Fatiha: Warsh 1 should include Basmalah (1) + Alhamd (2)
        multi_hafs_warsh_ayahs.append((1, 1, [1, 2]))
        continue

    h_words = []
    for h_num, h_text in h_rows:
        for w in normalize(h_text).split():
            if w:
                h_words.append((w, h_num))

    w_words = []
    for w_num, w_text in w_rows:
        for w in normalize(w_text).split():
            if w:
                w_words.append((w, w_num))

    # For each Warsh ayah, find the set of Hafs ayahs that overlap with it!
    h_idx = 0
    w_to_h_set = {w: set() for w in range(1, len(w_rows) + 1)}
    
    for w_word, w_ayah in w_words:
        for step in range(0, 25):
            if h_idx + step < len(h_words):
                hw, ha = h_words[h_idx + step]
                if hw == w_word or hw[:3] == w_word[:3] or (len(hw)>3 and len(w_word)>3 and hw[1:4] == w_word[1:4]):
                    h_idx += step
                    w_to_h_set[w_ayah].add(ha)
                    break
        h_idx += 1

    for w_num, h_set in w_to_h_set.items():
        if len(h_set) > 1:
            multi_hafs_warsh_ayahs.append((surah, w_num, sorted(list(h_set))))

print(f'Total Warsh ayahs spanning multiple Hafs ayahs: {len(multi_hafs_warsh_ayahs)}')
for s, w, h_list in multi_hafs_warsh_ayahs[:20]:
    s_name = conn.execute('SELECT name_ar FROM surahs WHERE number=?', (s,)).fetchone()[0]
    print(f'Surah {s} ({s_name}) Warsh Ayah {w} covers Hafs ayahs: {h_list}')

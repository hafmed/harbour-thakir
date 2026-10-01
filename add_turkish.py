import urllib.request, sqlite3, io, sys

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')

conn = sqlite3.connect('data/quran.db')

# Check if tr.diyanet already exists
existing = conn.execute("SELECT COUNT(*) FROM tafsir_translations WHERE edition_id='tr.diyanet'").fetchone()[0]
if existing == 0:
    print("Downloading tr.diyanet (Turkish)...")
    req = urllib.request.Request('https://tanzil.net/trans/tr.diyanet', headers={'User-Agent': 'Mozilla/5.0'})
    content = urllib.request.urlopen(req, timeout=30).read().decode('utf-8', errors='ignore')
    
    rows = []
    for line in content.splitlines():
        line = line.strip()
        if not line or line.startswith('#'): continue
        parts = line.split('|', 2)
        if len(parts) == 3:
            s_num = int(parts[0])
            a_num = int(parts[1])
            t_text = parts[2].strip()
            rows.append(('tr.diyanet', 'ديانت (التركية)', 'Diyanet Isleri (Turkish)', 'translation', 'tr', s_num, a_num, t_text))
            
    print(f"Parsed {len(rows)} verses. Inserting into database...")
    conn.executemany("INSERT INTO tafsir_translations (edition_id, edition_name_ar, edition_name_en, edition_type, language, surah_number, ayah_number, text) VALUES (?, ?, ?, ?, ?, ?, ?, ?)", rows)
    conn.commit()

conn.execute("VACUUM")
conn.commit()

total = conn.execute("SELECT COUNT(*) FROM tafsir_translations").fetchone()[0]
print(f"Total rows in tafsir_translations: {total}")
for ed, count in conn.execute("SELECT edition_id, COUNT(*) FROM tafsir_translations GROUP BY edition_id").fetchall():
    print(f"  {ed}: {count}")

conn.close()

import urllib.request, sqlite3, io, sys, time

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')

EDITIONS = [
    {
        'id': 'ar.muyassar',
        'name_ar': 'التفسير الميسر',
        'name_en': 'Tafsir Al-Muyassar',
        'type': 'tafsir',
        'lang': 'ar',
        'url': 'https://tanzil.net/trans/ar.muyassar'
    },
    {
        'id': 'en.sahih',
        'name_ar': 'صحيح إنترناشونال (الإنجليزية)',
        'name_en': 'Saheeh International (English)',
        'type': 'translation',
        'lang': 'en',
        'url': 'https://tanzil.net/trans/en.sahih'
    },
    {
        'id': 'fr.hamidullah',
        'name_ar': 'حميد الله (الفرنسية)',
        'name_en': 'Muhammad Hamidullah (French)',
        'type': 'translation',
        'lang': 'fr',
        'url': 'https://tanzil.net/trans/fr.hamidullah'
    }
]

conn = sqlite3.connect('data/quran.db')

conn.execute("DROP TABLE IF EXISTS tafsir_translations")
conn.execute("""
CREATE TABLE tafsir_translations (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    edition_id TEXT NOT NULL,
    edition_name_ar TEXT NOT NULL,
    edition_name_en TEXT NOT NULL,
    edition_type TEXT NOT NULL,
    language TEXT NOT NULL,
    surah_number INTEGER NOT NULL,
    ayah_number INTEGER NOT NULL,
    text TEXT NOT NULL
)
""")

for ed in EDITIONS:
    print(f"Downloading {ed['id']} ({ed['name_en']})...")
    req = urllib.request.Request(ed['url'], headers={'User-Agent': 'Mozilla/5.0'})
    with urllib.request.urlopen(req, timeout=30) as resp:
        content = resp.read().decode('utf-8', errors='ignore')
    
    rows = []
    for line in content.splitlines():
        line = line.strip()
        if not line or line.startswith('#'):
            continue
        parts = line.split('|', 2)
        if len(parts) == 3:
            s_num = int(parts[0])
            a_num = int(parts[1])
            t_text = parts[2].strip()
            rows.append((ed['id'], ed['name_ar'], ed['name_en'], ed['type'], ed['lang'], s_num, a_num, t_text))
            
    print(f"Parsed {len(rows)} verses for {ed['id']}. Inserting into database...")
    conn.executemany("INSERT INTO tafsir_translations (edition_id, edition_name_ar, edition_name_en, edition_type, language, surah_number, ayah_number, text) VALUES (?, ?, ?, ?, ?, ?, ?, ?)", rows)
    conn.commit()

print("Creating database index on tafsir_translations...")
conn.execute("CREATE INDEX idx_tafsir_trans_lookup ON tafsir_translations(edition_id, surah_number, ayah_number)")
conn.execute("CREATE INDEX idx_tafsir_trans_type ON tafsir_translations(edition_type, language)")
conn.commit()

count = conn.execute("SELECT COUNT(*) FROM tafsir_translations").fetchone()[0]
print(f"Done! Total entries inserted: {count}")

# Verify
sample = conn.execute("SELECT edition_id, text FROM tafsir_translations WHERE surah_number=1 AND ayah_number=1").fetchall()
for s in sample:
    print(s)

conn.close()

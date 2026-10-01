import re

with open('test_106.svg', 'r', encoding='utf-8') as f:
    content = f.read()

highlightSurah = 5
highlightAyah = 2

rx = re.compile(r'(<path[^>]*class=["\']ayahPolygon["\'][^>]*>)')
found = False
for tag in rx.findall(content):
    hasSurah = (f'surah="{highlightSurah}"' in tag) or (f"surah='{highlightSurah}'" in tag)
    hasAyah = (f'ayah="{highlightAyah}"' in tag) or (f"ayah='{highlightAyah}'" in tag)
    if hasSurah and hasAyah:
        print("MATCHED TAG:")
        print(tag)
        found = True
        break

if not found:
    print("NOT FOUND!")

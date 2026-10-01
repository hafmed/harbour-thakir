import re

with open('test_106.svg', 'r', encoding='utf-8') as f:
    c = f.read()

polys = re.findall(r'<path[^>]*class=["\']ayahPolygon["\'][^>]*>', c)
print(f"Total ayah polygons on page 106: {len(polys)}")
for p in polys:
    s = re.search(r'surah=["\'](\d+)["\']', p)
    a = re.search(r'ayah=["\'](\d+)["\']', p)
    print(f"surah={s.group(1) if s else 'None'} ayah={a.group(1) if a else 'None'}")

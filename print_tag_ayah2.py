import re

with open('test_106.svg', 'r', encoding='utf-8') as f:
    c = f.read()

polys = re.findall(r'<path[^>]*class=["\']ayahPolygon["\'][^>]*>', c)
for p in polys:
    if 'ayah="2"' in p or "ayah='2'" in p:
        print("EXACT TAG:")
        print(p)

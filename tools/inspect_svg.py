with open('scratch/page_359.svg', 'r', encoding='utf-8') as f:
    s = f.read()

import re
print("Root tag:", s[:200])
paths = re.findall(r'<path[^>]+>', s)
print("Total paths:", len(paths))
non_poly_paths = [p for p in paths if 'ayahPolygon' not in p]
print("Total non-poly paths:", len(non_poly_paths))
if non_poly_paths:
    print("Sample non-poly path:", non_poly_paths[0][:200])

texts = re.findall(r'<text[^>]+>', s)
print("Total text tags:", len(texts))
if texts:
    print("Sample text tag:", texts[0][:200])

polys = [p for p in paths if 'ayahPolygon' in p]
print("Ayah polygons found:")
for p in polys:
    print(p)
for line in s.split('\n')[:50]:
    if 'ayah' in line:
        print(line[:200])

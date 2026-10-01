import urllib.request, re

url = 'https://quranpedia.net/api/page/warsh/106'
req = urllib.request.Request(url, headers={'User-Agent': 'Mozilla/5.0'})
with urllib.request.urlopen(req) as resp:
    c = resp.read().decode('utf-8')

with open('test_106.svg', 'w', encoding='utf-8') as f:
    f.write(c)

print('Length:', len(c))
m = re.search(r'viewBox=["\']([^"\']+)["\']', c)
if m:
    print('viewBox:', m.group(1))

# Check bounds of d in ayahPolygons
polys = re.findall(r'<path[^>]*class=["\']ayahPolygon["\'][^>]*>', c)
min_y = 9999
max_y = -9999
for p in polys:
    m_d = re.search(r'd=["\']([^"\']+)["\']', p)
    if m_d:
        coords = [float(x) for x in re.findall(r'[-+]?\d*\.?\d+', m_d.group(1))]
        ys = coords[1::2]
        if ys:
            min_y = min(min_y, min(ys))
            max_y = max(max_y, max(ys))

print(f"AyahPolygons Y range: min_y={min_y}, max_y={max_y}")

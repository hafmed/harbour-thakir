import re

with open('test_106.svg', 'r', encoding='utf-8') as f:
    s = f.read()

m_vb = re.search(r'viewBox=["\']([^"\']+)["\']', s)
print('viewBox:', m_vb.group(1))

polys = re.findall(r'<path[^>]*class=["\']ayahPolygon["\'][^>]*>', s)
for p in polys:
    m_d = re.search(r'd=["\']([^"\']+)["\']', p)
    print(p[:60], m_d.group(1)[:80])

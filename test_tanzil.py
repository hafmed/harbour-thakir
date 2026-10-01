import urllib.request, io, sys

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')

for code in ['ar.muyassar', 'en.sahih', 'fr.hamidullah', 'ar.jalalayn']:
    url = f'https://tanzil.net/trans/{code}'
    try:
        req = urllib.request.Request(url, headers={'User-Agent': 'Mozilla/5.0'})
        with urllib.request.urlopen(req, timeout=10) as resp:
            data = resp.read(300)
            print(f'{code}: OK, preview:')
            print(data.decode('utf-8', errors='ignore')[:150])
            print('-' * 40)
    except Exception as e:
        print(f'{code}: Error {e}')

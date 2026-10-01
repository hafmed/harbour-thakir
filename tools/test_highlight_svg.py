import re

with open('scratch/page_359.svg', 'r', encoding='utf-8') as f:
    svg = f.read()

def highlight_svg(svg_text, surah, ayah, dark=False):
    # Highlight specific ayahPolygon
    # Pattern: <path class="ayahPolygon" ... ayah="X" surah="Y" ... />
    def repl(m):
        tag = m.group(0)
        # Check if matches surah and ayah
        if f'surah="{surah}"' in tag and f'ayah="{ayah}"' in tag:
            # Replace fill-opacity="0" with highlight fill
            fill_color = "#ffd700" if dark else "#007c89"
            fill_op = "0.35" if dark else "0.22"
            stroke_color = "#ffe066" if dark else "#007c89"
            tag = re.sub(r'fill-opacity="0"', f'fill="{fill_color}" fill-opacity="{fill_op}" stroke="{stroke_color}" stroke-width="1.5" stroke-opacity="0.8"', tag)
        return tag

    svg_text = re.sub(r'<path[^>]*class="ayahPolygon"[^>]*>', repl, svg_text)

    if dark:
        # Inject dark background and color styling
        # Quranpedia dark mode: invert text colors, dark background
        dark_style = """<style>
        svg { background-color: #12151b; }
        path:not(.ayahPolygon) { fill: #ede4ce !important; }
        .ayahPolygon { mix-blend-mode: screen; }
        </style>"""
        svg_text = svg_text.replace('</svg>', f'{dark_style}</svg>')
    else:
        light_style = """<style>
        svg { background-color: #fffdf5; }
        path:not(.ayahPolygon) { fill: #1a1a1a; }
        </style>"""
        svg_text = svg_text.replace('</svg>', f'{light_style}</svg>')

    return svg_text

highlighted_light = highlight_svg(svg, 24, 63, dark=False)
highlighted_dark = highlight_svg(svg, 24, 63, dark=True)

with open('scratch/page_359_hl_light.svg', 'w', encoding='utf-8') as f:
    f.write(highlighted_light)

with open('scratch/page_359_hl_dark.svg', 'w', encoding='utf-8') as f:
    f.write(highlighted_dark)

print("Generated highlighted SVGs successfully!")
print("Light has highlight:", 'fill="#007c89"' in highlighted_light)
print("Dark has highlight:", 'fill="#ffd700"' in highlighted_dark)

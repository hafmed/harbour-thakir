import re

def render_quranpedia_svg(svg_content, highlight_surah=0, highlight_ayah=0, dark=False):
    # Ensure background rect
    bg_color = "#12151b" if dark else "#fffdf5"
    
    # 1. Background
    # Insert background rect right after <svg ...>
    svg_tag_end = svg_content.find('>')
    if svg_tag_end != -1:
        bg_rect = f'\n<rect width="100%" height="100%" fill="{bg_color}" />\n'
        svg_content = svg_content[:svg_tag_end+1] + bg_rect + svg_content[svg_tag_end+1:]

    # 2. Text colors
    if dark:
        # Replace ink colors with warm ivory
        svg_content = re.sub(r'fill="#231f20"', 'fill="#ede4ce"', svg_content)
        svg_content = re.sub(r'fill="#000000"', 'fill="#ede4ce"', svg_content)
        svg_content = re.sub(r'fill="#1a1a1a"', 'fill="#ede4ce"', svg_content)
    else:
        # In light mode, ink is crisp dark
        pass

    # 3. Highlight Ayah Polygon
    if highlight_surah > 0 and highlight_ayah > 0:
        fill_col = "#ffd700" if dark else "#007c89"
        fill_op = "0.38" if dark else "0.22"
        stroke_col = "#ffe066" if dark else "#007c89"
        stroke_w = "2" if dark else "1.5"

        # Regex match ayahPolygon with surah and ayah
        pattern = rf'(<path[^>]*class="ayahPolygon"[^>]*surah="{highlight_surah}"[^>]*ayah="{highlight_ayah}"[^>]*>)'
        def add_highlight(m):
            tag = m.group(1)
            # Remove existing fill-opacity
            tag = re.sub(r'fill-opacity="[^"]*"', '', tag)
            tag = re.sub(r'fill="[^"]*"', '', tag)
            tag = tag[:-1].strip() + f' fill="{fill_col}" fill-opacity="{fill_op}" stroke="{stroke_col}" stroke-width="{stroke_w}" stroke-opacity="0.8" />'
            return tag
        
        svg_content = re.sub(pattern, add_highlight, svg_content)
        # Also try reverse attribute order (ayah before surah)
        pattern_rev = rf'(<path[^>]*class="ayahPolygon"[^>]*ayah="{highlight_ayah}"[^>]*surah="{highlight_surah}"[^>]*>)'
        svg_content = re.sub(pattern_rev, add_highlight, svg_content)

    return svg_content

if __name__ == '__main__':
    with open('scratch/page_359.svg', 'r', encoding='utf-8') as f:
        raw = f.read()

    res_light = render_quranpedia_svg(raw, highlight_surah=25, highlight_ayah=1, dark=False)
    res_dark = render_quranpedia_svg(raw, highlight_surah=25, highlight_ayah=1, dark=True)

    with open('scratch/page_359_rendered_light.svg', 'w', encoding='utf-8') as f:
        f.write(res_light)
    with open('scratch/page_359_rendered_dark.svg', 'w', encoding='utf-8') as f:
        f.write(res_dark)

    print("Rendered light and dark SVGs successfully!")
    print("Light highlight present:", 'fill="#007c89"' in res_light)
    print("Dark highlight present:", 'fill="#ffd700"' in res_dark)
    print("Dark ink present:", 'fill="#ede4ce"' in res_dark)

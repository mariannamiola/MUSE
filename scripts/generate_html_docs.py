#!/usr/bin/env python3
"""
HTML Documentation Generator for MUSE Applications

Converts Markdown documentation files to HTML with basic styling.
"""

import os
import sys
import re
from pathlib import Path

def markdown_to_html(md_content: str) -> str:
    """Convert basic Markdown to HTML"""
    html = md_content
    
    # Horizontal rules to visual separators
    html = re.sub(r'^---$', '<div class="flag-separator"></div>', html, flags=re.MULTILINE)
    
    # Headers with anchor support
    html = re.sub(r'^# (.*$)', r'<h1>\1</h1>', html, flags=re.MULTILINE)
    html = re.sub(r'^## (.*$)', r'<h2>\1</h2>', html, flags=re.MULTILINE)
    html = re.sub(r'^### (.*$)', r'<h3>\1</h3>', html, flags=re.MULTILINE)
    # Handle h4 headers with anchor IDs like: #### `--flag` {#anchor-id}
    html = re.sub(r'^#### (.+?)\s*\{#([^}]+)\}\s*$', r'<h4 id="\2">\1</h4>', html, flags=re.MULTILINE)
    html = re.sub(r'^#### (.*$)', r'<h4>\1</h4>', html, flags=re.MULTILINE)
    
    # Bold and italic
    html = re.sub(r'\*\*(.*?)\*\*', r'<strong>\1</strong>', html)
    html = re.sub(r'\*(.*?)\*', r'<em>\1</em>', html)
    
    # Handle Type and Description metadata lines specifically
    html = re.sub(r'^<strong>Type:</strong>(.+)$', r'<p><strong>Type:</strong>\1</p>', html, flags=re.MULTILINE)
    html = re.sub(r'^<strong>Description:</strong>(.+)$', r'<p><strong>Description:</strong>\1</p>', html, flags=re.MULTILINE)
    
    # Special formatting for Dependencies and Examples
    html = re.sub(r'<strong>Dependencies:</strong>\s*(.+?)(?=\n\n|\n<strong>|\n$)', 
                  r'<div class="dependencies"><strong>Dependencies:</strong> \1</div>', html, flags=re.DOTALL)
    html = re.sub(r'<strong>Example:</strong>\s*`(.+?)`', 
                  r'<div class="example"><strong>Example:</strong> <code>\1</code></div>', html)
    
    # Code blocks
    html = re.sub(r'```(\w*)\n(.*?)```', r'<pre><code class="\1">\2</code></pre>', html, flags=re.DOTALL)
    
    # Inline code
    html = re.sub(r'`([^`]+)`', r'<code>\1</code>', html)
    
    # Links - convert .md to .html
    html = re.sub(r'\[([^\]]+)\]\(([^)]+)\.md\)', r'<a href="\2.html">\1</a>', html)
    html = re.sub(r'\[([^\]]+)\]\(([^)]+)\)', r'<a href="\2">\1</a>', html)
    
    # Lists
    lines = html.split('\n')
    in_list = False
    result_lines = []
    
    for line in lines:
        if re.match(r'^\s*-\s+', line):
            if not in_list:
                result_lines.append('<ul>')
                in_list = True
            item_content = re.sub(r'^\s*-\s+', '', line)
            result_lines.append(f'  <li>{item_content}</li>')
        else:
            if in_list:
                result_lines.append('</ul>')
                in_list = False
            result_lines.append(line)
    
    if in_list:
        result_lines.append('</ul>')
    
    html = '\n'.join(result_lines)
    
    # Paragraphs
    paragraphs = html.split('\n\n')
    html_paragraphs = []
    
    for para in paragraphs:
        para = para.strip()
        if para and not para.startswith('<'):
            para = f'<p>{para}</p>'
        html_paragraphs.append(para)
    
    return '\n\n'.join(html_paragraphs)

def create_html_template(title: str, content: str, is_index: bool = False) -> str:
    """Create complete HTML document"""
    nav_links = ""
    if not is_index:
        nav_links = '<a href="index.html">← Back to Index</a>'
    
    return f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{title} - MUSE Documentation</title>
    <style>
        :root {{
            /* Light theme variables */
            --bg-primary: #fafafa;
            --bg-secondary: white;
            --bg-tertiary: #f8f9fa;
            --bg-quaternary: #f1f3f4;
            --text-primary: #333;
            --text-secondary: #555;
            --text-tertiary: #666;
            --border-color: #e1e4e8;
            --border-light: #eee;
            --shadow-color: rgba(0,0,0,0.1);
            --shadow-light: rgba(0,0,0,0.05);
            --code-bg: #f1f3f4;
            --pre-bg: #f8f8f8;
            --link-color: #0366d6;
            --accent-color: #3498db;
            --success-color: #28a745;
            --danger-color: #d73a49;
            --gradient-start: #f8f9fa;
            --gradient-end: #e9ecef;
        }}
        
        [data-theme="dark"] {{
            /* Dark theme variables */
            --bg-primary: #0d1117;
            --bg-secondary: #161b22;
            --bg-tertiary: #21262d;
            --bg-quaternary: #30363d;
            --text-primary: #f0f6fc;
            --text-secondary: #c9d1d9;
            --text-tertiary: #8b949e;
            --border-color: #30363d;
            --border-light: #21262d;
            --shadow-color: rgba(0,0,0,0.3);
            --shadow-light: rgba(0,0,0,0.2);
            --code-bg: #161b22;
            --pre-bg: #0d1117;
            --link-color: #58a6ff;
            --accent-color: #58a6ff;
            --success-color: #3fb950;
            --danger-color: #f85149;
            --gradient-start: #21262d;
            --gradient-end: #30363d;
        }}
        
        body {{
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            line-height: 1.6;
            color: var(--text-primary);
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
            background-color: var(--bg-primary);
            transition: background-color 0.3s ease, color 0.3s ease;
        }}
        .container {{
            background: var(--bg-secondary);
            padding: 40px;
            border-radius: 8px;
            box-shadow: 0 2px 10px var(--shadow-color);
            transition: background-color 0.3s ease, box-shadow 0.3s ease;
        }}
        .nav {{
            margin-bottom: 20px;
            padding-bottom: 20px;
            border-bottom: 1px solid var(--border-light);
            display: flex;
            justify-content: space-between;
            align-items: center;
        }}
        .nav a {{
            color: var(--link-color);
            text-decoration: none;
            font-weight: 500;
        }}
        .nav a:hover {{
            text-decoration: underline;
        }}
        .theme-toggle {{
            background: var(--bg-tertiary);
            border: 1px solid var(--border-color);
            border-radius: 20px;
            padding: 8px 16px;
            cursor: pointer;
            color: var(--text-primary);
            font-size: 0.9em;
            transition: all 0.3s ease;
            display: flex;
            align-items: center;
            gap: 6px;
        }}
        .theme-toggle:hover {{
            background: var(--bg-quaternary);
        }}
        .contact-button {{
            background: var(--bg-tertiary);
            border: 1px solid var(--border-color);
            border-radius: 20px;
            padding: 8px 16px;
            cursor: pointer;
            color: var(--text-primary);
            font-size: 0.9em;
            font-weight: 500;
            text-decoration: none;
            display: inline-flex;
            align-items: center;
            gap: 6px;
            transition: all 0.3s ease;
        }}
        .contact-button:hover {{
            background: var(--bg-quaternary);
            transform: translateY(-1px);
        }}
        .nav-buttons {{
            display: flex;
            align-items: center;
            gap: 12px;
        }}
        .muse-logo {{
            position: fixed;
            top: 20px;
            left: 20px;
            width: 200px;
            z-index: 1000;
        }}
        .muse-logo img {{
            width: 100%;
            height: auto;
            border-radius: 8px;
            box-shadow: 0 2px 8px var(--shadow-light);
            transition: transform 0.3s ease;
        }}
        .muse-logo img:hover {{
            transform: scale(1.05);
        }}        h1 {{
            color: var(--text-primary);
            border-bottom: 2px solid var(--accent-color);
            padding-bottom: 10px;
        }}
        h2 {{
            color: var(--text-primary);
            margin-top: 30px;
        }}
        h3, h4 {{
            color: var(--text-secondary);
        }}
        h4 {{
            background: linear-gradient(135deg, var(--gradient-start) 0%, var(--gradient-end) 100%);
            border-left: 4px solid var(--link-color);
            padding: 12px 16px;
            margin: 30px 0 20px 0;
            border-radius: 0 6px 6px 0;
            box-shadow: 0 2px 4px var(--shadow-light);
            border-top: 1px solid var(--border-color);
        }}
        h4 code {{
            background: transparent;
            color: var(--link-color);
            font-weight: 600;
            font-size: 1.1em;
        }}
        .flag-separator {{
            height: 2px;
            background: linear-gradient(90deg, var(--link-color) 0%, var(--accent-color) 50%, transparent 100%);
            margin: 25px 0;
            border-radius: 1px;
        }}
        code {{
            background: var(--code-bg);
            padding: 2px 6px;
            border-radius: 3px;
            font-family: 'Monaco', 'Menlo', monospace;
            font-size: 0.9em;
            color: var(--text-primary);
        }}
        pre {{
            background: var(--pre-bg);
            border: 1px solid var(--border-color);
            border-radius: 6px;
            padding: 16px;
            overflow-x: auto;
        }}
        pre code {{
            background: none;
            padding: 0;
        }}
        ul {{
            padding-left: 20px;
        }}
        li {{
            margin: 8px 0;
        }}
        a {{
            color: var(--link-color);
            text-decoration: none;
        }}
        a:hover {{
            text-decoration: underline;
        }}
        .dependencies a {{
            color: var(--link-color);
            font-weight: 500;
            background: var(--bg-tertiary);
            padding: 2px 4px;
            border-radius: 3px;
            text-decoration: none;
        }}
        .dependencies a:hover {{
            background: var(--bg-quaternary);
            text-decoration: none;
        }}
        .app-list {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
            margin: 20px 0;
        }}
        .app-card {{
            border: 1px solid var(--border-color);
            border-radius: 6px;
            padding: 16px;
            background: var(--bg-tertiary);
        }}
        .app-card h3 {{
            margin-top: 0;
            color: var(--link-color);
        }}
        hr {{
            border: none;
            height: 1px;
            background: var(--border-color);
            margin: 30px 0;
        }}
        .dependencies {{
            background: var(--bg-tertiary);
            border-left: 4px solid var(--link-color);
            padding: 12px 16px;
            margin: 12px 0;
            border-radius: 0 4px 4px 0;
        }}
        .dependencies strong {{
            color: var(--link-color);
        }}
        .example {{
            background: var(--bg-tertiary);
            border-left: 4px solid var(--success-color);
            padding: 12px 16px;
            margin: 12px 0;
            border-radius: 0 4px 4px 0;
        }}
        .example strong {{
            color: var(--success-color);
        }}
        .footer {{
            margin-top: 40px;
            text-align: center;
            color: var(--text-tertiary);
            font-size: 0.9em;
            border-top: 1px solid var(--border-light);
            padding-top: 20px;
        }}
    </style>
    <script>
        // Theme switching functionality
        function initTheme() {{
            const savedTheme = localStorage.getItem('theme') || 'light';
            document.documentElement.setAttribute('data-theme', savedTheme);
            updateThemeIcon(savedTheme);
        }}
        
        function toggleTheme() {{
            const currentTheme = document.documentElement.getAttribute('data-theme') || 'light';
            const newTheme = currentTheme === 'light' ? 'dark' : 'light';
            
            document.documentElement.setAttribute('data-theme', newTheme);
            localStorage.setItem('theme', newTheme);
            updateThemeIcon(newTheme);
        }}
        
        function updateThemeIcon(theme) {{
            const themeButton = document.getElementById('theme-toggle');
            if (themeButton) {{
                themeButton.innerHTML = theme === 'light' ? 
                    '🌙 Dark' : 
                    '☀️ Light';
            }}
        }}
        
        // Initialize theme when DOM loads
        document.addEventListener('DOMContentLoaded', initTheme);
    </script>
</head>
<body>
    <div class="muse-logo">
        <img src="img/muse_logo.svg" alt="MUSE - Modelling of Uncertainty as a Support of Environment" />
    </div>
    <div class="container">
        <div class="nav">
            <div>{nav_links}</div>
            <div class="nav-buttons">
                <a href="mailto:marianna.miola@cnr.it" class="contact-button" title="Contact support">
                    ✉️ Contact
                </a>
                <button class="theme-toggle" id="theme-toggle" onclick="toggleTheme()" title="Toggle theme">
                    🌙 Dark
                </button>
            </div>
        </div>
        {content}
        <div class="footer">
            <p>Generated from MUSE source code documentation</p>
        </div>
    </div>
</body>
</html>"""

def convert_md_to_html(input_dir: str, output_dir: str):
    """Convert all MD files in input_dir to HTML in output_dir"""
    import shutil
    
    os.makedirs(output_dir, exist_ok=True)
    
    # Copy images directory if it exists
    img_src = Path(input_dir).parent / 'img'
    img_dst = Path(output_dir) / 'img'
    
    if img_src.exists():
        if img_dst.exists():
            shutil.rmtree(img_dst)
        shutil.copytree(img_src, img_dst)
        print(f"✓ Copied images from {img_src} to {img_dst}")
    
    md_files = list(Path(input_dir).glob('*.md'))
    
    if not md_files:
        print(f"No .md files found in {input_dir}")
        return
    
    print(f"Converting {len(md_files)} Markdown files to HTML...")
    
    for md_file in md_files:
        with open(md_file, 'r', encoding='utf-8') as f:
            md_content = f.read()
        
        # Extract title from first header or filename
        title_match = re.search(r'^# (.+)', md_content, re.MULTILINE)
        title = title_match.group(1) if title_match else md_file.stem
        
        # Convert to HTML
        html_content = markdown_to_html(md_content)
        
        # Create full HTML document
        is_index = md_file.name == 'index.md'
        full_html = create_html_template(title, html_content, is_index)
        
        # Write HTML file
        html_file = Path(output_dir) / f"{md_file.stem}.html"
        with open(html_file, 'w', encoding='utf-8') as f:
            f.write(full_html)
        
        print(f"✓ Generated: {html_file}")
    
    print(f"\n✓ HTML documentation generated in: {output_dir}")
    print(f"  Open {output_dir}/index.html in your browser to view")

def main():
    if len(sys.argv) < 2:
        print("Usage: python generate_html_docs.py <input_dir> [output_dir]")
        print("  input_dir: Directory containing .md files")
        print("  output_dir: Directory to create HTML files (default: html_docs)")
        sys.exit(1)
    
    input_dir = sys.argv[1]
    output_dir = sys.argv[2] if len(sys.argv) > 2 else "html_docs"
    
    if not os.path.isdir(input_dir):
        print(f"Error: {input_dir} is not a directory", file=sys.stderr)
        sys.exit(1)
    
    convert_md_to_html(input_dir, output_dir)

if __name__ == '__main__':
    main()
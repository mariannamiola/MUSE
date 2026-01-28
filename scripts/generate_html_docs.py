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
        body {{
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            line-height: 1.6;
            color: #333;
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
            background-color: #fafafa;
        }}
        .container {{
            background: white;
            padding: 40px;
            border-radius: 8px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }}
        .nav {{
            margin-bottom: 20px;
            padding-bottom: 20px;
            border-bottom: 1px solid #eee;
        }}
        .nav a {{
            color: #0366d6;
            text-decoration: none;
            font-weight: 500;
        }}
        .nav a:hover {{
            text-decoration: underline;
        }}
        h1 {{
            color: #2c3e50;
            border-bottom: 2px solid #3498db;
            padding-bottom: 10px;
        }}
        h2 {{
            color: #34495e;
            margin-top: 30px;
        }}
        h3, h4 {{
            color: #555;
        }}
        code {{
            background: #f1f3f4;
            padding: 2px 6px;
            border-radius: 3px;
            font-family: 'Monaco', 'Menlo', monospace;
            font-size: 0.9em;
        }}
        pre {{
            background: #f8f8f8;
            border: 1px solid #e1e4e8;
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
            color: #0366d6;
            text-decoration: none;
        }}
        a:hover {{
            text-decoration: underline;
        }}
        .dependencies a {{
            color: #0366d6;
            font-weight: 500;
            background: rgba(3, 102, 214, 0.1);
            padding: 2px 4px;
            border-radius: 3px;
            text-decoration: none;
        }}
        .dependencies a:hover {{
            background: rgba(3, 102, 214, 0.2);
            text-decoration: none;
        }}
        .app-list {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
            margin: 20px 0;
        }}
        .app-card {{
            border: 1px solid #e1e4e8;
            border-radius: 6px;
            padding: 16px;
            background: #f8f9fa;
        }}
        .app-card h3 {{
            margin-top: 0;
            color: #0366d6;
        }}
        hr {{
            border: none;
            height: 1px;
            background: #e1e4e8;
            margin: 30px 0;
        }}
        .dependencies {{
            background: #f8f9fa;
            border-left: 4px solid #0366d6;
            padding: 12px 16px;
            margin: 12px 0;
            border-radius: 0 4px 4px 0;
        }}
        .dependencies strong {{
            color: #0366d6;
        }}
        .example {{
            background: #f1f3f4;
            border-left: 4px solid #28a745;
            padding: 12px 16px;
            margin: 12px 0;
            border-radius: 0 4px 4px 0;
            font-family: 'Monaco', 'Menlo', monospace;
            font-size: 0.9em;
        }}
        .example strong {{
            color: #28a745;
        }}
        .footer {{
            margin-top: 40px;
            text-align: center;
            color: #666;
            font-size: 0.9em;
            border-top: 1px solid #eee;
            padding-top: 20px;
        }}
    </style>
</head>
<body>
    <div class="container">
        <div class="nav">{nav_links}</div>
        {content}
        <div class="footer">
            <p>Generated from MUSE source code documentation</p>
        </div>
    </div>
</body>
</html>"""

def convert_md_to_html(input_dir: str, output_dir: str):
    """Convert all MD files in input_dir to HTML in output_dir"""
    os.makedirs(output_dir, exist_ok=True)
    
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
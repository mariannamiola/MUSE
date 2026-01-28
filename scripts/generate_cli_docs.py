#!/usr/bin/env python3
"""
CLI Documentation Generator for MUSE Applications

This script parses C++ source files that use TCLAP and extracts command-line
options along with their Doxygen documentation to generate Markdown documentation.

Usage:
    python generate_cli_docs.py <apps_dir> <output_dir> [header_file] [--single-file]
    
    By default, generates separate .md files for each app plus an index.md file.
    Use --single-file flag to generate one combined documentation file instead.
"""

import re
import os
import sys
from pathlib import Path
from typing import Dict, List, Tuple

class TCLAPParser:
    """Parser for TCLAP command-line arguments in C++ source files"""
    
    def __init__(self, source_file: str):
        self.source_file = source_file
        self.app_name = self._extract_app_name()
        self.content = self._read_file()
        self.description = self._extract_description()
        self.options = self._extract_options()
    
    def _read_file(self) -> str:
        """Read source file content"""
        try:
            with open(self.source_file, 'r', encoding='utf-8') as f:
                return f.read()
        except Exception as e:
            print(f"Error reading {self.source_file}: {e}", file=sys.stderr)
            return ""
    
    def _extract_app_name(self) -> str:
        """Extract application name from file path"""
        # e.g., /path/to/muse_project/main.cpp -> muse_project
        parts = Path(self.source_file).parts
        for i, part in enumerate(parts):
            if part.startswith('muse_'):
                return part
        return "unknown"
    
    def _extract_description(self) -> str:
        """Extract CmdLine description"""
        pattern = r'CmdLine\s+\w+\s*\(\s*"([^"]+)"'
        match = re.search(pattern, self.content)
        if match:
            return match.group(1)
        return "MUSE Application"
    
    def _extract_options(self) -> List[Dict]:
        """Extract all TCLAP options with their documentation"""
        options = []
        
        # Find all doxygen comments followed by Arg declarations
        # Pattern: /** ... */ followed by (Switch|Value)Arg
        pattern = r'(\/\*\*.*?\*\/\s*\n\s*)([SV]\w+Arg[^;]+;)'
        
        matches = re.finditer(pattern, self.content, re.DOTALL)
        
        for match in matches:
            doc = match.group(1)
            declaration = match.group(2)
            
            option_info = self._parse_option(doc, declaration)
            if option_info:
                options.append(option_info)
        
        return options
    
    def _parse_option(self, doc: str, declaration: str) -> Dict:
        """Parse a single option from its documentation and declaration"""
        option = {}
        
        # Extract from Doxygen comment
        brief_match = re.search(r'@brief\s+(.+?)(?:\n|@)', doc)
        if brief_match:
            option['brief'] = brief_match.group(1).strip()
        
        # Extract short flag, long flag, and type from declaration
        # Pattern: SwitchArg("X", "long_name", "desc", cmd, default)
        # Pattern: ValueArg<type>("X", "long_name", "desc", required, default, type_name, cmd)
        
        type_match = re.search(r'(Switch|Value)Arg', declaration)
        if type_match:
            option['type'] = 'Switch' if type_match.group(1) == 'Switch' else 'Value'
        
        # Extract flags
        flags_match = re.search(r'\("([^"]*)",\s*"([^"]+)"', declaration)
        if flags_match:
            option['short_flag'] = flags_match.group(1) if flags_match.group(1) else None
            option['long_flag'] = flags_match.group(2)
        
        # Extract description from declaration
        desc_match = re.search(r',\s*"([^"]+)"', declaration)
        if desc_match:
            option['desc'] = desc_match.group(1)
        
        # Extract detailed documentation from Doxygen comment
        # Look for @param and extract only clean lines
        param_match = re.search(r'@param\s+\w+\s+(.+?)(?:@\w+|\*\/)', doc, re.DOTALL)
        if param_match:
            details = param_match.group(1).strip()
            # Clean up comment markers and extra whitespace
            details = re.sub(r'\s*\*\s*', '\n', details)
            details = re.sub(r'\n\s*\n', '\n', details)
            option['details'] = details.strip()
        
        return option if 'long_flag' in option else None
    
    def __str__(self) -> str:
        """String representation of parsed data"""
        return f"App: {self.app_name}, Desc: {self.description}, Options: {len(self.options)}"


class MarkdownGenerator:
    """Generate Markdown documentation from parsed TCLAP options"""
    
    def __init__(self):
        self.apps = {}
    
    def _read_header_file(self, header_file: str = None) -> str:
        """Read header content from external file"""
        if header_file is None:
            # Default to muse_header.md in the docs directory
            script_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
            header_file = os.path.join(script_dir, 'docs', 'muse_header.md')
        
        try:
            with open(header_file, 'r', encoding='utf-8') as f:
                return f.read()
        except Exception as e:
            print(f"Warning: Could not read header file {header_file}: {e}", file=sys.stderr)
            # Fallback to minimal header
            return "# MUSE Command-Line Options Documentation\n\n**Auto-generated from source code**\n\n"
    
    def add_app(self, parser: TCLAPParser):
        """Add a parsed application to the generator"""
        self.apps[parser.app_name] = {
            'description': parser.description,
            'options': parser.options
        }
    
    def generate_single_app(self, app_name: str, header_file: str = None) -> str:
        """Generate Markdown documentation for a single app"""
        if app_name not in self.apps:
            return ""
        
        app_info = self.apps[app_name]
        md = self._generate_app_section(app_name, app_info)
        
        md += self._generate_common_section()
        return md
    
    def generate_index(self, header_file: str = None) -> str:
        """Generate an index file linking to all app documentation"""
        md = self._read_header_file(header_file)
        md += "\n\n" if not md.endswith('\n\n') else ""
        
        md += "## Available Applications\n\n"
        md += "The MUSE toolkit consists of the following applications:\n\n"
        
        for app_name in sorted(self.apps.keys()):
            app_info = self.apps[app_name]
            md += f"- **[{app_name}]({app_name}.md)**: {app_info['description']}\n"
        
        md += "\n"
        md += self._generate_common_section()
        return md
    
    def generate(self, header_file: str = None) -> str:
        """Generate complete Markdown documentation (legacy method for backward compatibility)"""
        md = self._read_header_file(header_file)
        md += "\n\n" if not md.endswith('\n\n') else ""
        
        # Table of contents
        md += "## Table of Contents\n\n"
        for app_name in sorted(self.apps.keys()):
            md += f"- [{app_name}](#{app_name})\n"
        md += "\n---\n\n"
        
        # Detailed documentation for each app
        for app_name in sorted(self.apps.keys()):
            app_info = self.apps[app_name]
            md += self._generate_app_section(app_name, app_info)
        
        md += self._generate_common_section()
        return md
    
    def _generate_app_section(self, app_name: str, app_info: Dict) -> str:
        """Generate documentation section for one application"""
        md = f"## {app_name}\n\n"
        md += f"**Description:** {app_info['description']}\n\n"
        
        if not app_info['options']:
            md += "### Usage\n\n"
            md += f"```bash\n{app_name} [OPTIONS]\n```\n\n"
            md += "**Note:** For detailed options, run:\n"
            md += f"```bash\n{app_name} --help\n```\n\n"
            md += "---\n\n"
            return md
        
        md += "### Usage\n\n"
        md += f"```bash\n{app_name} [OPTIONS]\n```\n\n"
        
        md += "### Options\n\n"
        
        for opt in app_info['options']:
            md += self._generate_option_doc(opt)
        
        md += "---\n\n"
        return md
    
    def _generate_option_doc(self, opt: Dict) -> str:
        """Generate documentation for a single option"""
        md = ""
        
        # Flag line
        flag_str = ""
        if opt.get('short_flag'):
            flag_str += f"`-{opt['short_flag']}`, "
        flag_str += f"`--{opt['long_flag']}`"
        
        md += f"#### {flag_str}\n"
        md += f"**Type:** {opt['type']} (flag)\n"
        
        if opt.get('brief'):
            md += f"**Description:** {opt['brief']}\n"
        elif opt.get('desc'):
            md += f"**Description:** {opt['desc']}\n"
        
        if opt.get('details'):
            md += f"\n{opt['details']}\n"
        
        md += "\n"
        return md
    
    def _generate_common_section(self) -> str:
        """Generate common section with tips and help info"""
        md = "## Getting Help\n\n"
        md += "All MUSE applications support the standard help flags:\n\n"
        md += "```bash\n"
        md += "# Display help for any tool\n"
        md += "<tool> --help\n"
        md += "<tool> -h\n"
        md += "<tool> /?\n"
        md += "```\n\n"
        
        md += "## Project Structure\n\n"
        md += "When creating a project with `muse_project`, the directory structure will be:\n\n"
        md += "```\n"
        md += "MyProject/\n"
        md += "├── in/           # Input data directory\n"
        md += "└── out/          # Output results directory\n"
        md += "    └── MyProject.json  # Project metadata and settings\n"
        md += "```\n\n"
        
        md += "## Tips and Best Practices\n\n"
        md += "1. Use `--help` flag to see all available options for any tool\n"
        md += "2. Use absolute paths to avoid confusion with relative paths\n"
        md += "3. Project names should not contain spaces; use underscores or hyphens instead\n"
        md += "4. Keep projects organized with consistent naming conventions\n"
        
        return md


def find_main_files(apps_dir: str) -> List[str]:
    """Find all main.cpp files in apps directory"""
    main_files = []
    apps_path = Path(apps_dir)
    
    for main_file in apps_path.glob('*/main*.cpp'):
        main_files.append(str(main_file))
    
    return sorted(main_files)


def main():
    if len(sys.argv) < 3:
        print("Usage: python generate_cli_docs.py <apps_dir> <output_dir> [header_file] [--single-file]")
        print("  By default, generates separate .md files for each app")
        print("  Use --single-file to generate one combined file")
        sys.exit(1)
    
    apps_dir = sys.argv[1]
    output_path = sys.argv[2]
    
    # Check for optional arguments
    header_file = None
    single_file_mode = False
    
    for i in range(3, len(sys.argv)):
        if sys.argv[i] == '--single-file':
            single_file_mode = True
        else:
            header_file = sys.argv[i]
    
    if not os.path.isdir(apps_dir):
        print(f"Error: {apps_dir} is not a directory", file=sys.stderr)
        sys.exit(1)
    
    # Find all main.cpp files
    main_files = find_main_files(apps_dir)
    
    if not main_files:
        print(f"Warning: No main*.cpp files found in {apps_dir}", file=sys.stderr)
    
    # Parse each file
    generator = MarkdownGenerator()
    
    for main_file in main_files:
        parser = TCLAPParser(main_file)
        if parser.options or parser.description:
            generator.add_app(parser)
            print(f"Parsed: {parser.app_name} ({len(parser.options)} options)")
    
    try:
        if single_file_mode:
            # Generate single combined file (legacy mode)
            markdown_content = generator.generate(header_file)
            os.makedirs(os.path.dirname(output_path), exist_ok=True)
            with open(output_path, 'w', encoding='utf-8') as f:
                f.write(markdown_content)
            print(f"✓ Combined documentation generated: {output_path}")
        else:
            # Generate separate files for each app
            os.makedirs(output_path, exist_ok=True)
            
            # Generate individual app files
            for app_name in sorted(generator.apps.keys()):
                app_content = generator.generate_single_app(app_name, header_file)
                app_file = os.path.join(output_path, f"{app_name}.md")
                with open(app_file, 'w', encoding='utf-8') as f:
                    f.write(app_content)
                print(f"✓ Generated: {app_file}")
            
            # Generate index file in docs directory
            project_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
            docs_dir = os.path.join(project_root, 'docs')
            
            index_content = generator.generate_index(header_file)
            index_file = os.path.join(docs_dir, "index.md")
            os.makedirs(docs_dir, exist_ok=True)
            with open(index_file, 'w', encoding='utf-8') as f:
                f.write(index_content)
            print(f"✓ Generated index: {index_file}")
            
            print(f"\n✓ Documentation generated in directory: {output_path}")
            print(f"  - Index file: index.md")
            print(f"  - {len(generator.apps)} application files")
    
    except Exception as e:
        print(f"Error generating documentation: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == '__main__':
    main()

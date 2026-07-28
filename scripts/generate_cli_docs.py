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
        """Extract application name from file path.

        The primary source (main.cpp) maps to the app folder name, e.g.
        /path/to/muse_project/main.cpp -> muse_project.

        Secondary executables living in the same folder (e.g. main_raster.cpp,
        main-tetmesh-generator.cpp) are separate CMake targets, so they get their
        own documentation page named <app>_<suffix>, e.g.
        muse_export/main_raster.cpp            -> muse_export_raster
        muse_geometry/main-tetmesh-generator.cpp -> muse_geometry_tetmesh-generator
        """
        path = Path(self.source_file)
        muse_dir = next((p for p in path.parts if p.startswith('muse_')), None)
        if muse_dir is None:
            return "unknown"

        stem = path.stem  # 'main', 'main_raster', 'main-tetmesh-generator'
        if stem == 'main':
            return muse_dir

        # Strip the leading 'main' + separator to obtain the executable suffix
        suffix = stem
        for pref in ('main-', 'main_', 'main'):
            if suffix.startswith(pref):
                suffix = suffix[len(pref):]
                break
        suffix = suffix.strip('-_')
        return f"{muse_dir}_{suffix}" if suffix else muse_dir
    
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
        # Pattern: /** ... */ followed by any *Arg declaration (Switch, Value, Multi, Unlabeled*)
        pattern = r'(\/\*\*.*?\*\/\s*\n\s*)(\w+Arg\s*(?:<[^>]+>)?\s*\w+[^;]+;)'
        
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
        # Supports: SwitchArg, ValueArg<T>, MultiArg<T>, MultiSwitchArg,
        #           UnlabeledValueArg<T>, UnlabeledMultiArg<T>
        type_match = re.search(r'(MultiSwitch|Multi|UnlabeledMulti|UnlabeledValue|Switch|Value)Arg', declaration)
        if type_match:
            raw = type_match.group(1)
            if raw in ('Switch', 'MultiSwitch'):
                option['type'] = 'Switch'
            else:
                option['type'] = 'Value'
            option['is_multi']   = raw in ('Multi', 'MultiSwitch', 'UnlabeledMulti')
            option['positional'] = raw in ('UnlabeledValue', 'UnlabeledMulti')

        # Extract C++ template type parameter, e.g. ValueArg<float> -> 'float'
        template_match = re.search(r'Arg\s*<\s*([^>]+?)\s*>', declaration)
        if template_match:
            option['value_type'] = template_match.group(1).strip()
        
        # Extract flags
        flags_match = re.search(r'\("([^"]*)",\s*"([^"]+)"', declaration)
        if flags_match:
            option['short_flag'] = flags_match.group(1) if flags_match.group(1) else None
            option['long_flag'] = flags_match.group(2)

        # Extract description: always the 3rd quoted string in the declaration
        # Signature: Arg("short", "long", "desc", ...)
        all_strings = re.findall(r'"([^"]*)"', declaration)
        if len(all_strings) >= 3:
            option['desc'] = all_strings[2]
        
        # Extract @required section — replaces @param, only for mandatory flags
        required_match = re.search(r'@required\s+(.+?)(?:@\w+|\*\/)', doc, re.DOTALL)
        if required_match:
            required_text = required_match.group(1).strip()
            required_text = re.sub(r'\s*\*\s*', ' ', required_text)
            option['required_note'] = required_text.strip()

        # Extract @note section for dependency information
        note_match = re.search(r'@note\s+(.+?)(?:@\w+|\*\/)', doc, re.DOTALL)
        if note_match:
            note_text = note_match.group(1).strip()
            # Clean up comment markers and extra whitespace
            note_text = re.sub(r'\s*\*\s*', '\n', note_text)
            note_text = re.sub(r'\n\s*\n', '\n', note_text)
            option['note'] = note_text.strip()
        
        # Extract @example section for usage examples
        example_match = re.search(r'@example\s+(.+?)(?:@\w+|\*\/)', doc, re.DOTALL)
        if example_match:
            example_text = example_match.group(1).strip()
            # Clean up comment markers and extra whitespace
            example_text = re.sub(r'\s*\*\s*', ' ', example_text)
            option['example'] = example_text.strip()

        # Extract @format section for value format description
        format_match = re.search(r'@format\s+(.+?)(?:@\w+|\*\/)', doc, re.DOTALL)
        if format_match:
            format_text = format_match.group(1).strip()
            format_text = re.sub(r'\s*\*\s*', ' ', format_text)
            option['format'] = format_text.strip()

        # Extract @default section for default value
        default_match = re.search(r'@default\s+(.+?)(?:@\w+|\*\/)', doc, re.DOTALL)
        if default_match:
            default_text = default_match.group(1).strip()
            default_text = re.sub(r'\s*\*\s*', ' ', default_text)
            option['default'] = default_text.strip()

        # Extract @values section for allowed/enumerated values
        values_match = re.search(r'@values\s+(.+?)(?:@\w+|\*\/)', doc, re.DOTALL)
        if values_match:
            values_text = values_match.group(1).strip()
            values_text = re.sub(r'\s*\*\s*', ' ', values_text)
            # Split on commas and clean each value
            option['allowed_values'] = [v.strip() for v in values_text.split(',') if v.strip()]
        
        return option if 'long_flag' in option else None
    
    def __str__(self) -> str:
        """String representation of parsed data"""
        return f"App: {self.app_name}, Desc: {self.description}, Options: {len(self.options)}"


class MarkdownGenerator:
    """Generate Markdown documentation from parsed TCLAP options"""

    TOC_THRESHOLD = 8  # Auto-generate TOC when an app has >= this many documented flags

    def __init__(self, toc_threshold: int = None):
        self.apps = {}
        self.toc_threshold = toc_threshold if toc_threshold is not None else self.TOC_THRESHOLD
    
    def _read_header_file(self, header_file: str = None) -> str:
        """Read header content from external file"""
        if header_file is None:
            # Default to muse_header.md in the docs/md directory
            script_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
            header_file = os.path.join(script_dir, 'docs', 'md', 'muse_header.md')
        
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
    
    def _generate_usage_string(self, app_name: str, options: List[Dict]) -> str:
        """Generate detailed usage string based on parsed options"""
        usage_parts = [app_name]
        
        # Separate required and optional arguments
        required_args = []
        optional_args = []
        switches = []
        
        for opt in options:
            flag_name = opt.get('long_flag', '')
            short_flag = opt.get('short_flag', '')
            opt_type = opt.get('type', '')
            note = opt.get('note', '')
            
            # More precise required flag detection
            is_required = (
                'MANDATORY when using' in note and flag_name in ['var', 'geom'] or  # Only var and geom are truly required
                note.startswith('MANDATORY:') or  # Explicit mandatory marker
                (flag_name in ['pdir', 'name'] and 'required' in note.lower())  # Project-specific requirements
            )
            
            # Create flag representation
            if short_flag:
                flag_repr = f"-{short_flag}"
            else:
                flag_repr = f"--{flag_name}"
                
            if opt_type == 'Switch':
                switches.append(flag_repr)
            elif opt_type == 'Value':
                value_name = flag_name.upper().replace('-', '_')
                suffix = '...' if opt.get('is_multi') else ''
                if is_required:
                    required_args.append(f"{flag_repr} <{value_name}{suffix}>")
                else:
                    optional_args.append(f"[{flag_repr} <{value_name}{suffix}>]")
        
        # Add required arguments first
        if required_args:
            usage_parts.extend(required_args)
        
        # For complex apps, limit the usage line length
        total_options = len(optional_args) + len(switches)
        if total_options > 15:
            usage_parts.append("[OPTIONS]")
        else:
            # Add most important optional arguments first (up to 8)
            important_flags = ['pdir', 'mode', 'sub', 'out', 'format', 'input']
            important_optional = []
            other_optional = []
            
            for opt_arg in optional_args:
                flag_found = False
                for imp_flag in important_flags:
                    if f"--{imp_flag}" in opt_arg or f"-{imp_flag[0].upper()}" in opt_arg:
                        important_optional.append(opt_arg)
                        flag_found = True
                        break
                if not flag_found:
                    other_optional.append(opt_arg)
            
            # Add important optional args (max 6)
            usage_parts.extend(important_optional[:6])
            
            # Add switches (max 3)
            if switches:
                if len(switches) <= 3:
                    usage_parts.extend([f"[{s}]" for s in switches[:3]])
                else:
                    usage_parts.append("[SWITCHES]")
            
            # Add [OPTIONS] if there are more
            if len(other_optional) > 0 or len(important_optional) > 6 or len(switches) > 3:
                usage_parts.append("[OPTIONS]")
        
        return " ".join(usage_parts)
    
    def _generate_toc(self, options: List[Dict]) -> str:
        """Generate a Table of Contents from the list of documented options"""
        toc = "### Options Index\n\n"
        for opt in options:
            long_flag = opt.get('long_flag', '')
            anchor_id = long_flag.replace('_', '-')
            short = f"`-{opt['short_flag']}`, " if opt.get('short_flag') else ''
            brief = opt.get('desc') or opt.get('brief') or ''
            label = f"{short}`--{long_flag}`"
            toc += f"- [{label}](#{anchor_id})"
            if brief:
                toc += f" — {brief}"
            toc += "\n"
        toc += "\n"
        return toc

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
        
        # Generate enhanced usage string
        usage_string = self._generate_usage_string(app_name, app_info['options'])
        
        md += "### Usage\n\n"
        md += f"```bash\n{usage_string}\n```\n\n"

        # Auto-generate TOC if the app has enough documented flags
        if len(app_info['options']) >= self.toc_threshold:
            md += self._generate_toc(app_info['options'])

        md += "### Options\n\n"
        
        for opt in app_info['options']:
            md += self._generate_option_doc(opt)
        
        md += "---\n\n"
        return md
    
    def _generate_option_doc(self, opt: Dict) -> str:
        """Generate documentation for a single option"""
        md = ""
        
        # Add visual separator before flag
        md += "---\n\n"
        
        # Flag line with anchor
        flag_str = ""
        if opt.get('short_flag'):
            flag_str += f"`-{opt['short_flag']}`, "
        flag_str += f"`--{opt['long_flag']}`"
        
        # Create anchor ID from long flag name
        anchor_id = opt['long_flag'].replace('_', '-')
        
        md += f"#### {flag_str} {{#{anchor_id}}}\n"

        # 1. Required — first thing the user needs to know
        if opt.get('required_note'):
            md += f'\n<div class="required"><strong>⚠ Required:</strong> {opt["required_note"]}</div>\n'

        # 2. Type
        base_type = opt.get('type', 'Value')
        value_type = opt.get('value_type', '')
        is_multi   = opt.get('is_multi', False)
        positional = opt.get('positional', False)

        if base_type == 'Switch':
            type_label = 'Switch | repeatable' if is_multi else 'Switch'
        else:
            parts = []
            if value_type:
                parts.append(f'`{value_type}`')
            if positional:
                parts.append('positional')
            if is_multi:
                parts.append('repeatable')
            type_label = 'Value'
            if parts:
                type_label += ' | ' + ', '.join(parts)

        md += f"\n**Type:** {type_label}\n"

        # 3. Format + Allowed values inline
        if opt.get('format') and opt.get('allowed_values'):
            values_str = ', '.join(f'`{v}`' for v in opt['allowed_values'])
            md += f"\n**Format:** `{opt['format']}` ({values_str})\n"
        elif opt.get('format'):
            md += f"\n**Format:** `{opt['format']}`\n"
        elif opt.get('allowed_values'):
            values_str = ', '.join(f'`{v}`' for v in opt['allowed_values'])
            md += f"\n**Allowed values:** {values_str}\n"

        # 4. Description
        md += "\n"
        if opt.get('brief'):
            md += f"**Description:** {opt['brief']}\n"
        elif opt.get('desc'):
            md += f"**Description:** {opt['desc']}\n"

        # 6. Default
        if opt.get('default'):
            md += f"\n**Default:** `{opt['default']}`\n"

        # 7. Dependencies
        if opt.get('note'):
            linked_dependencies = self._convert_flag_references_to_links(opt['note'])
            md += f"\n**Dependencies:** {linked_dependencies}\n"

        # 8. Example
        if opt.get('example'):
            md += f"\n**Example:** `{opt['example']}`\n"

        md += "\n"
        return md
    
    def _convert_flag_references_to_links(self, text: str) -> str:
        """Convert flag references like --flag to clickable links"""
        import re
        
        # Pattern to match flag references like --flag-name or -X
        pattern = r'(--[a-zA-Z0-9_-]+|(?<!-)\b-[a-zA-Z]\b)'
        
        def replace_flag(match):
            flag = match.group(1)
            # Convert --flag-name to anchor link
            if flag.startswith('--'):
                anchor_id = flag[2:].replace('_', '-')  # Remove -- and convert underscores
                return f'[{flag}](#{anchor_id})'
            else:
                # For short flags like -X, we need to find the corresponding long flag
                # For now, just return the original flag (could be enhanced later)
                return flag
        
        return re.sub(pattern, replace_flag, text)
    
    def _generate_common_section(self) -> str:
        """Generate common section with tips and help info"""
        return ""


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
    toc_threshold = None

    for i in range(3, len(sys.argv)):
        if sys.argv[i] == '--single-file':
            single_file_mode = True
        elif sys.argv[i].startswith('--toc-threshold='):
            try:
                toc_threshold = int(sys.argv[i].split('=')[1])
            except ValueError:
                print("Warning: invalid --toc-threshold value, using default", file=sys.stderr)
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
    generator = MarkdownGenerator(toc_threshold=toc_threshold)
    
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
            # Generate separate files for each app in the same directory
            os.makedirs(output_path, exist_ok=True)
            
            # Generate individual app files
            for app_name in sorted(generator.apps.keys()):
                app_content = generator.generate_single_app(app_name, header_file)
                app_file = os.path.join(output_path, f"{app_name}.md")
                with open(app_file, 'w', encoding='utf-8') as f:
                    f.write(app_content)
                print(f"✓ Generated: {app_file}")
            
            # Generate index file in the same directory
            index_content = generator.generate_index(header_file)
            index_file = os.path.join(output_path, "index.md")
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
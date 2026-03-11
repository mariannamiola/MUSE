#!/usr/bin/env python3
import csv
from collections import OrderedDict

import os
base = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'data'))
file_phi = os.path.join(base, 'amga_class_v_4000.csv')
file_phi_sgs = os.path.join(base, 'amga.csv')
out_file = os.path.join(base, 'amga_merged.csv')

keys = OrderedDict()


def read_header_and_data(path):
    header_lines = []
    data = []
    with open(path, 'r', encoding='utf-8') as f:
        for line in f:
            line_str = line.rstrip('\n')
            if line_str.strip() == '':
                # preserve blank header lines
                if not data:
                    header_lines.append([])
                continue
            parts = [p for p in line_str.split(';')]
            # try to detect first data row by converting first 3 fields
            if len(parts) >= 3:
                try:
                    float(parts[0]); float(parts[1]); float(parts[2])
                    # this is the first data row; add and read remaining lines as data
                    data.append(parts)
                    for l in f:
                        l = l.rstrip('\n')
                        if l.strip() == '':
                            continue
                        data.append([p for p in l.split(';')])
                    break
                except Exception:
                    header_lines.append(parts)
            else:
                header_lines.append(parts)
    return header_lines, data

# read headers and data from both files
header_phi, data_phi = read_header_and_data(file_phi)
header_phi_sgs, data_phi_sgs = read_header_and_data(file_phi_sgs)

# populate keys preserving order from the phi file first
for parts in data_phi:
    if len(parts) < 3:
        continue
    x, y, z = parts[0].strip(), parts[1].strip(), parts[2].strip()
    val = parts[3].strip() if len(parts) > 3 else ''
    key = (x, y, z)
    if key not in keys:
        keys[key] = {'phi': None, 'phi_sgs': None}
    if val != '':
        keys[key]['phi'] = val

for parts in data_phi_sgs:
    if len(parts) < 3:
        continue
    x, y, z = parts[0].strip(), parts[1].strip(), parts[2].strip()
    val = parts[3].strip() if len(parts) > 3 else ''
    key = (x, y, z)
    if key not in keys:
        keys[key] = {'phi': None, 'phi_sgs': None}
    if val != '':
        keys[key]['phi_sgs'] = val

# merge header lines: produce lines with 5 columns (X,Y,Z,phi,phi_sgs)
max_hdr = max(len(header_phi), len(header_phi_sgs))
merged_headers = []

def _clean_hdr_cell(x):
    # remove tabs and trim whitespace; empty or whitespace-only -> ''
    if x is None:
        return ''
    s = x.replace('\t', '').strip()
    return s if s != '' else ''

for i in range(max_hdr):
    A = header_phi[i] if i < len(header_phi) else []
    B = header_phi_sgs[i] if i < len(header_phi_sgs) else []
    # pad to at least 4 elements
    while len(A) < 4:
        A.append('')
    while len(B) < 4:
        B.append('')
    merged = [_clean_hdr_cell(A[0]), _clean_hdr_cell(A[1]), _clean_hdr_cell(A[2]), _clean_hdr_cell(A[3]), _clean_hdr_cell(B[3])]
    merged_headers.append(merged)
# ensure first header is the column names we want
if merged_headers:
    merged_headers[0] = ['X', 'Y', 'Z', 'phi', 'phi_sgs']
else:
    merged_headers.append(['X', 'Y', 'Z', 'phi', 'phi_sgs'])

# write merged file with headers preserved
with open(out_file, 'w', encoding='utf-8', newline='') as f:
    # write header block (preserve empty cells and number of rows)
    for hdr in merged_headers:
        f.write(';'.join(hdr) + '\n')
    writer = csv.writer(f, delimiter=';')
    for (x, y, z), vals in keys.items():
        phi = vals['phi'] if vals['phi'] is not None else ''
        phi_sgs = vals['phi_sgs'] if vals['phi_sgs'] is not None else ''
        writer.writerow([x, y, z, phi, phi_sgs])

print(f"Wrote {len(keys)} rows to {out_file}")

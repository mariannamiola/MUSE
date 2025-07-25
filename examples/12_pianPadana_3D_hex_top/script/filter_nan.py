import csv
import math

import sys
	                
input_file = sys.argv[1]
output_file = sys.argv[2]

with open(input_file, newline='', encoding='utf-8') as infile:
    reader = csv.reader(infile)
    header = next(reader)  # Read header
    cleaned_rows = []

    for row in reader:
        first_value = row[0].strip().lower()
        if first_value != "" and first_value != "nan":
            cleaned_rows.append(row)

# Write filtered data
with open(output_file, "w", newline='', encoding='utf-8') as outfile:
    writer = csv.writer(outfile)
    writer.writerow(header)  # Write header
    writer.writerows(cleaned_rows)

print(f"✅ Cleaned file saved to: {output_file}")


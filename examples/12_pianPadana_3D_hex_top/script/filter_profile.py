import csv
import math

import sys

def distanza(pozzo, punto):
    return math.sqrt((pozzo[0] - punto[0])**2 + (pozzo[1] - punto[1])**2)

def filtra_punti(input_csv, output_csv, pozzo):
    with open(input_csv, newline='', encoding='utf-8') as csvfile:
        reader = csv.reader(csvfile, delimiter=';')
        header = next(reader)  # Legge l'header
        righe_filtrate = [header]  # Mantiene l'header nel file di output

        for riga in reader:
            ##print(riga)
            try:
                punto = (float(riga[1]), float(riga[2]))
                if distanza(pozzo, punto) < 20:
                    righe_filtrate.append(riga)
            except ValueError:
                print(f"Conversion error - ignored line: {riga}")

    with open(output_csv, mode='w', newline='', encoding='utf-8') as csvfile:
        writer = csv.writer(csvfile, delimiter=';')
        writer.writerows(righe_filtrate)

if __name__ == "__main__":
    ##pozzo = (477550.0, 5014200.0)  # Definizione del pozzo (x, y, z)
    input_csv = sys.argv[1]
    output_csv = sys.argv[2]
    pozzo_x = float(sys.argv[3])
    pozzo_y = float(sys.argv[4])
    pozzo = (pozzo_x, pozzo_y)  # Definizione del pozzo (x, y)
    
    filtra_punti(input_csv, output_csv, pozzo)
    print("Saving file as ", output_csv)

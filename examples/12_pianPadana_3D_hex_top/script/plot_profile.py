import matplotlib.pyplot as plt
import pandas as pd

import sys
import os

delim=','
scale_factor=-1.0

# Lettura del file CSV
df = pd.read_csv(sys.argv[2], delimiter=delim)
#depth = df["Points:2"]
temperature = df["Field 0"]

# Conversione della colonna 'Points:2' in float
df["Points:2"] = pd.to_numeric(df["Points:2"], errors="coerce")

# Calcolo della temperatura trasformata: t' = temperatura * (-5)
df["p_transf"] = df["Points:2"] / (scale_factor)

# Ordinamento del DataFrame in base alla temperatura trasformata
#df_sorted = df.sort_values(by="p_transf", ascending=True)

# Estrazione dei dati ordinati
depth = df["p_transf"]
#temperature = df_sorted["Field 0"]



### Lettura DEV-MISUS
df_mdev = pd.read_csv(sys.argv[3], delimiter=delim)
#depth_mdev = df_mdev["Points:2"]
temp_mdev = df_mdev["Field 0"]

# Conversione della colonna 'Points:2' in float
#df_mdev["Points:2"] = pd.to_numeric(df_mdev["Points:2"], errors="coerce")

# Calcolo della temperatura trasformata: t' = temperatura * (-5)
#df_mdev["p_transf"] = df_mdev["Points:2"] / (scale_factor)

# Ordinamento del DataFrame in base alla temperatura trasformata
#df_mdev_sorted = df_mdev.sort_values(by="p_transf", ascending=True)

# Estrazione dei dati ordinati
#depth_mdev = df_mdev_sorted["p_transf"]
#temp_mdev = df_mdev_sorted["Field 0"]
#temp_mdev_nomean = df_sorted["Field 0"] - df_mdev_sorted["Field 0"]



### Lettura DEV-PLUS
df_pdev = pd.read_csv(sys.argv[4], delimiter=delim)
#depth_pdev = df_pdev["Points:2"]
temp_pdev = df_pdev["Field 0"]

# Conversione della colonna 'Points:2' in float
#df_pdev["Points:2"] = pd.to_numeric(df_pdev["Points:2"], errors="coerce")

# Calcolo della temperatura trasformata: t' = temperatura * (-5)
#df_pdev["p_transf"] = df_pdev["Points:2"] / (scale_factor)

# Ordinamento del DataFrame in base alla temperatura trasformata
#df_pdev_sorted = df_pdev.sort_values(by="p_transf", ascending=True)

# Estrazione dei dati ordinati
#depth_pdev = df_pdev_sorted["p_transf"]
#temp_pdev = df_pdev_sorted["Field 0"]
#temp_pdev_nomean = df_pdev_sorted["Field 0"] - df_sorted["Field 0"] 


# === Secondo profilo ===
df2 = pd.read_csv(sys.argv[1], delimiter=';')
temp2 = df2["T"]  # Sostituisci con il nome corretto!
depth2 = df2["depth"]


# Creazione del plot
plt.figure(figsize=(6, 8))
plt.plot(temperature, depth, marker='o', markersize=3, color='red', linestyle='-', label='MUSE model')

# Secondo profilo
plt.plot(temp2, depth2, marker='o', markersize=3, linestyle='-', color='blue', label='BS model')

# Secondo profilo
plt.plot(temp_mdev, depth, marker='o', markersize=1, linestyle='--', linewidth=1, color='gray', label='stdev_m')
plt.plot(temp_pdev, depth, marker='o', markersize=1, linestyle='--', linewidth=1, color='gray', label='stdev_p')

# Colorazione della banda tra le due curve grigie (m_stdev e p_stdev)
plt.fill_betweenx(depth, temp_mdev, temp_pdev, color='gray', alpha=0.2, label='Variability band')



# Gruppo 1: punti misurati a mano
bh_depths = [3142.60, 4380.10, 5102.60, 5321.67, 5731.57]
bh_temps = [85, 105, 136, 144, 158]

# Aggiunta al grafico
plt.scatter(bh_temps, bh_depths, color='yellow', s=60, marker='o', edgecolors='black', label='BHT')



# Inversione asse y
plt.gca().invert_yaxis()

# Etichette e titolo
plt.xlabel("Temperature [°C]")
plt.ylabel("Depth (m)")
plt.title(sys.argv[5] + " - Stochastic model vs Backstripping model")

# Griglia e layout
plt.legend()
plt.grid(True)
plt.tight_layout()

# Salvataggio in PNG
directory_profile = '_fig_1Dprofile'
if not os.path.exists(directory_profile): 
    os.mkdir(directory_profile)
plt.savefig(directory_profile+'/'+sys.argv[5]+"_MUSEvsBS.png", dpi=300)

# Mostra il grafico
#plt.show()
plt.close()

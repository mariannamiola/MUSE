#!/usr/bin/env python3

import os
import sys
import pandas as pd

script_dir=os.path.dirname(os.path.realpath(__file__))
folder=sys.argv[1]
filename=sys.argv[2]
extension=sys.argv[3]
folder_out=sys.argv[4]

###folder='data_convert'
###filename='ID_21044_UmiditàeTemperatura_2024-04-02_2024-04-2215_38_41.txt'
###extension='.csv'


# Load your data
df = pd.read_csv(script_dir+'/'+folder+'/'+filename+extension, sep=';')
print(df)

print('### DATA TRANSPOSE ...')
df_transposed = df.T
print(df_transposed)

df_transposed.to_csv(script_dir+'/'+folder_out+'/'+filename+extension, sep=';')
print('### DATA TRANSPOSE ... COMPLETED.')
print()


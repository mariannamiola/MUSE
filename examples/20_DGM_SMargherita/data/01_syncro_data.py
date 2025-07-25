#!/usr/bin/env python3

import os
import sys
import pandas as pd

script_dir=os.path.dirname(os.path.realpath(__file__))
folder=sys.argv[1]
filename=sys.argv[2]
extension=sys.argv[3]
time=sys.argv[4]


# Load your data
df = pd.read_csv(script_dir+'/'+folder+'/'+filename+extension, sep=';')
#print(df)

# Ensure your time series data is recognized as datetime
df['Timestamp'] = pd.to_datetime(df['Timestamp'])

print('### TIME SERIE SYNCRONIZATION ...')
# Resample the data to the specific timestamp you want, and interpolate
upsampled = pd.concat([df.set_index('Timestamp')['T (°C)'].resample(time).first().interpolate('linear'),
						df.set_index('Timestamp')['H (%)'].resample(time).first().interpolate('linear')], axis=1)
#print(upsampled)

upsampled.to_csv(script_dir+'/'+folder+'/'+filename+'_syncro'+extension, sep=';')
print(upsampled.head(5))
print('...')
print('### TIME SERIE SYNCRONIZATION ... COMPLETED.')
print()


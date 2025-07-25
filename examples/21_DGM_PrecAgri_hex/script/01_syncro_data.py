#!/usr/bin/env python3

import os
import sys
import pandas as pd

script_dir=os.path.dirname(os.path.realpath(__file__))
#folder=sys.argv[1]
filename=sys.argv[1]
#extension=sys.argv[2]
time=sys.argv[2]
folder=sys.argv[3]
label_h=sys.argv[4]
#label_T=sys.argv[5]

#folder=script_dir+'/../data/data_convert'
#filename='ID_21044_date_h1'
#extension='.csv'
#time='1h'

# Load your data
df = pd.read_csv(filename, sep=';')
#print(df)

# Ensure your time series data is recognized as datetime
df['Time'] = pd.to_datetime(df['Time'])

print('### TIME SERIE SYNCRONIZATION ...')
# Resample the data to the specific timestamp you want, and interpolate
upsampled = df.set_index('Time')[label_h].resample(time).first().interpolate('linear')
#upsampled = pd.concat([df.set_index('Time')[label_h].resample(time).first().interpolate('linear'),
						#df.set_index('Time')[label_T].resample(time).first().interpolate('linear')], axis=1)
#print(upsampled)

upsampled.to_csv(script_dir+'/../data/'+folder+'/'+filename, sep=';')
print(upsampled.head(5))
print('...')
print('### TIME SERIE SYNCRONIZATION ... COMPLETED.')
print()


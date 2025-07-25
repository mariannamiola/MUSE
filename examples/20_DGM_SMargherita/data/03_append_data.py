#!/usr/bin/env python3

import os
import sys
import pandas as pd
import numpy as np

script_dir=os.path.dirname(os.path.realpath(__file__))
folder=sys.argv[1]
filename=sys.argv[2]
extension=sys.argv[3]
folder_out=sys.argv[4]

# Load your data
df = pd.read_csv(script_dir+'/'+folder+'/'+filename+extension, sep=';', header=None)
df[7]=df[5]
df[8]=df[6]

node_name='NODE'

nodes = [['1','2'],['2','5'],['5','4'],['4','3'],['3','1'],['3','2'],['3','5'],['1','4'],['2','4'],['5','1']]
for i in range(len(nodes)):
	node_first=nodes[i][0]
	node_second=nodes[i][1]

	# Set id column as the index
	df = df.set_index(0)

	# Create new row and add to dataframe
	x12=(df[1][node_name+node_first] + df[1][node_name+node_second])*0.5
	y12=(df[2][node_name+node_first] + df[2][node_name+node_second])*0.5
	TS12=df[4][node_name+node_first]
	T12=(df[7][node_name+node_first] + df[7][node_name+node_second])*0.5
	H12=(df[8][node_name+node_first] + df[8][node_name+node_second])*0.5
	#print(df)
    
	# Reset id column as the index
	df = df.reset_index()

	#print('### ADD NODES BY LINEAR INTERPOLATION ...')

	# Create new row and add to dataframe
	new_row = {0: node_name+node_first+node_second, 1: x12, 2: y12, 3: 0.0, 4: TS12, 5: 'NA', 6: 'NA', 7: T12, 8: H12}
	df.loc[len(df)] = new_row

print(df)
print('### ADD NODES BY LINEAR INTERPOLATION ... COMPLETED.')
df.to_csv(script_dir+'/'+folder_out+'/'+filename+extension, sep=';', index=False, header=False)

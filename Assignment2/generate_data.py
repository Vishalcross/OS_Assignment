'''
	The objective of this program is to generate the test data for the simulation based on the exponential distribution.
	Here we make use of three hardcoded values:
		process_count: The number of processes to be run
		beta: The parameter to be used for the exponential distribution
		file_name: Name of the file being used to store the data
'''

import numpy as np
import json

# location of config file
hardcode={}
headerfile = "header.h"
with open("config.json") as file:
	hardcode=json.load(file)

# Array to store the data points
data_points = np.round( np.random.exponential(scale=hardcode['beta'],size=(hardcode['process_count'],2) ) )

# Write the data into the file

with open(hardcode['file_name'],"w") as file:
	for data_point in data_points:
		file.write("%d %d\n"% (data_point[0] , data_point[1] ) )

with open(headerfile,"w") as file:
	for key in hardcode.keys():
		if(type(hardcode[key]) is str):
			file.write('#define %s "%s"\n'%(key.upper(),hardcode[key]))
		else:
			file.write('#define %s %d\n'%(key.upper(),hardcode[key]))

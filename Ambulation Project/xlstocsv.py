import pandas
import time
import csv
import os

while True: #(for infinite looping)
	df = pandas.read_excel('Versus Excel Sheet.xlsx', usecols=[0, 1, 17, 6, 15, 16, 18, 20, 23], skiprows=[1])
	#df.reindex(columns=[*df.columns.tolist(), 'col1', 'col2'], fill_value=0)
	df['Unnamed: 8'] = "NULL"
	df['Unnamed: 9'] = "NULL"
	df = df.fillna("NULL")
	df['Versus Data'] = df['Versus Data'].replace(',', '', regex=True)
	df['Unnamed: 1'] = df['Unnamed: 1'].replace(',', '', regex=True)
	df['Unnamed: 2'] = df['Unnamed: 2'].replace(',', '', regex=True)
	df['Unnamed: 3'] = df['Unnamed: 3'].replace(',', '', regex=True)
	df['Unnamed: 4'] = df['Unnamed: 4'].replace(',', '', regex=True)
	df['Unnamed: 5'] = df['Unnamed: 5'].replace(',', '', regex=True)
	df['Unnamed: 6'] = df['Unnamed: 6'].replace(',', '', regex=True)
	df.to_csv('output.csv', encoding='utf-8')
	with open('output.csv') as f:
		with open("temp.csv",'w') as f1:
			reader = csv.reader(f, quotechar = '"', delimiter=',', quoting=csv.QUOTE_ALL, skipinitialspace=True)
			for row in reader:
				f1.write(",".join([row[8],row[1],row[5],row[2],row[3],row[4],row[6],row[7],row[9],row[10]]))
				f1.write('\n')
	with open("temp.csv",'r') as f:
		with open("output.csv",'w') as f1:
			f.readline() # skip header line
			for line in f:
				f1.write(line)
	os.remove("temp.csv")
	input('Loop finished')
	#print('Loop finished')
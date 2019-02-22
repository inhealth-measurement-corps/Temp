import csv
import pandas as pd
import math
import MySQLdb

host = '10.162.80.9'
username = 'jessie'
password = 'rJvMXazyFrZCw7O4'
db = 'mmambulation'

db = MySQLdb.connect(host=host, user=username, passwd=password, db=db)
cursor = db.cursor()
# with open('test.csv', "r+") as infile, open('newlive.csv', "w+") as outfile:
#     #Formatted as (Patient ID, Amb#, Date, Time, Dist, Duration, Speed)
#      fieldnames = ['PatientID', 'Ambulation', 'Start_Date', 'Start_Date', 'Distance', 'Time', 'Speed']
#      writer = csv.DictWriter(outfile, fieldnames=fieldnames, extrasaction='ignore') 
#      writer.writeheader()
#      for row in csv.DictReader(infile):
#         writer.writerow(row)
# infile.close()
# outfile.close()
columns = ['PatientID','BadgeID','Start_Dates','Duration','Distance','Speed','Ambulation','Placehold1','Placehold2','Placehold3','End_Date','Placehold4']
fieldnames = ['PatientID', 'Ambulation', 'Start_Dates', 'Distance', 'Duration', 'Speed']
df = pd.read_csv('live.csv', index_col=False, names = columns) ########################reads live.csv
#print (df.BadgeID)
df['Date'] = df.Start_Dates.str[:10]
df['Time of Day'] = df.Start_Dates.str[11:16]
df['Speed'] = round(df.Speed,4)
df = df[['PatientID', 'Ambulation', 'Date', 'Time of Day', 'Distance', 'Duration', 'Speed']]
#pd.to_numeric(df.Duration)
#df['MinSec'] = (math.floor(df.Duration/60)) #+ ":" + str(math.fmod(df.Duration,60))
#df['Durationz'] = math.floor((df.Duration)/60.0)




df.to_csv('newlive.csv', index_label=False, index=False, header=False) ######################writes to newlive.csv and pushes to db

csv_data = csv.reader(open('newlive.csv'))
for row in csv_data:
    cursor.execute("INSERT IGNORE INTO live_details VALUES (%s, %s, %s, %s, %s, %s, %s)", row[:7])
cursor.close() 

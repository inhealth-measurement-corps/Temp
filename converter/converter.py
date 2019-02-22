import csv
import MySQLdb
from conv_classes import Patient, Ambulation
import numpy as np
import csv


host = '10.162.80.9'
username = 'jessie'
password = 'rJvMXazyFrZCw7O4'
db = 'mmambulation'

db = MySQLdb.connect(host=host, user=username, passwd=password, db=db) #connecting to the sql database
cur = db.cursor() #make a cursor object that lets us navigate through the database
cur2 = db.cursor() #make a second cursor to go through the patient_info table
cur.execute("SELECT * FROM historical_details") #get the ambulation table

patient_map = {} # A map of patient numbers/MRNs to patient objects

#for row in cur.fetchmany(27):
for row in cur.fetchall():
    if patient_map.get(row[0]) != None: #the patient is already in our dict
        patient = patient_map[row[0]] #get the patient object corresponding to this MRN - stays the same
        ambulation = Ambulation(row[0], row[2], row[3], row[4], row[5], row[6])
        patient.add_ambulation(ambulation)

    else: #the patient is not in our dict yet
        cur2.execute("SELECT * FROM patient_info WHERE patient_id = %s", (row[0],))
        row2 = cur2.fetchone()
        patient =  Patient(row[0], row2[2], row2[1], row2[3], row2[4], row2[6], row2[7])
        ambulation = Ambulation(row[0], row[2], row[3], row[4], row[5], row[6])
        patient.add_ambulation(ambulation)
        patient_map[patient.mrn] = patient


    #print(type(str(ambulation.start_datetime)))


#Need to do regression on each patient to get deltas
for x, y in patient_map.items():
    print()
    print("Data for MRN", x)
    #y.print_days()
    y.regression_v1()
    #y.regression_v2()
    y.print_data()


'''
for x, y in patient_map.items():
    print()
    print("Data for MRN", x)
    #y.adjust_data()
    y.regression_v1()
    y.print_list()
    #y.show_regression("speed")
'''

'''
with open("Ambulation_Unit.csv", 'w') as csvfile:
    filewriter = csv.writer(csvfile, delimiter=',',
                            quotechar='|', quoting=csv.QUOTE_MINIMAL)
    filewriter.writerow(["Patient ID", "# of Ambulations (N)", "Ambulation Frequency (N/LoS)", "Days with ambulation (number)", "Compliance 1/day (%)", "Compliance 2/day (%)", "Compliance 3/day (%)", "Total Distance (ft)", "Distance CoT","Duration CoT","Speed CoT"])
    for x, y in patient_map.items():
        freq = y.num_ambulations/y.length_of_stay
        filewriter.writerow([y.mrn, y.num_ambulations, freq, len(y.ambulations_per_day), y.compliance_1, y.compliance_2, y.compliance_3, y.total_distance, y.delta_dist, y.delta_dur, y.delta_speed])

with open("Ambulation_Daily.csv", 'w') as csvfile:
    filewriter = csv.writer(csvfile, delimiter=',',
                            quotechar='|', quoting=csv.QUOTE_MINIMAL)
    filewriter.writerow(["Patient ID", "Day on Unit", "# of Ambulations", "Dist Max", "Dist Mean", "Dist Min", "Dur Max", "Dur Mean", "Dur Min","Speed Max","Speed Mean", "Speed Min"])
    for x, y in patient_map.items():
        for a, b in y.date_to_day.items():
            filewriter.writerow([y.mrn, b.day_number, b.num_ambulations, b.max_dist, b.mean_dist, b.min_dist, b.max_dur ,b.mean_dur, b.min_dur, b.max_speed, b.mean_speed, b.min_speed])
'''



'''
for x, y in patient_map.items():
    freq = y.num_ambulations/y.length_of_stay
    cur.execute("INSERT INTO ambulation_unit VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)", [y.mrn, y.num_ambulations, freq, len(y.ambulations_per_day), y.compliance_1, y.compliance_2, y.compliance_3, y.total_distance, y.delta_dist, y.delta_dur, y.delta_speed])
'''
'''
for x, y in patient_map.items(): #x is mrn, y is Patient object
        for a, b in y.date_to_day.items(): #a is the date, b is the Day object
            if b.num_ambulations == 0:
                cur.execute("INSERT INTO ambulation_daily VALUES (%s, %s, %s, null, null, null, null, null, null, null, null, null)", [y.mrn, b.day_number, b.num_ambulations])
            else:
                cur.execute("INSERT INTO ambulation_daily VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)", [y.mrn, b.day_number, b.num_ambulations, b.max_dist, b.mean_dist, b.min_dist, b.max_dur ,b.mean_dur, b.min_dur, b.max_speed, b.mean_speed, b.min_speed])
'''


cur.close()
cur2.close()















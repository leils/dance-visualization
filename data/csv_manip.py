import csv

with open('cut_data.csv', 'rb') as csvfile:
  r= csv.reader(csvfile, delimiter=",", quotechar="|")
  for row in r:
    x = float(row[0]) / 10
    y = float(row[1]) / 10
    z = float(row[2]) / 10
    print str(x) + "," + str(y) + "," + str(z) + ","

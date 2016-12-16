import csv

with open('data.csv', 'rb') as csvfile:
  r= csv.reader(csvfile, delimiter=",", quotechar="|")
  for row in r:
    print row[1] + "," + row[2] + "," + row[3]

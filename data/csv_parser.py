import sys
import csv


def main(argv):
   data_num = int(argv[0])
   base = (data_num - 1) * 3 #The data is indexed at 1, not 0 

   with open('data.csv', 'rb') as csvfile:
     r= csv.reader(csvfile, delimiter=",", quotechar="|")
     for row in r:
       print row[base + 1] + "," + row[base + 2] + "," + row[base + 3]

   return

if __name__ == "__main__":
   main(sys.argv[1:])
import csv
import sys

def main(argv):
   with open(argv[0], 'rb') as csvfile:
      r= csv.reader(csvfile, delimiter=",", quotechar="|")
      for row in r:
         x = float(row[0]) / 100
         y = float(row[1]) / 100
         z = float(row[2]) / 100 
         print str(x) + "," + str(y) + "," + str(z) + ","
      return 

if __name__ == "__main__":
   main(sys.argv[1:])
import csv
import sys

displacement = .5

def main(argv):
    with open(argv[0], 'rb') as csvfile:
        r= csv.reader(csvfile, delimiter=",", quotechar="|")
        a = r.next() #array of 3 floats
        b = r.next() #array of 3 floats
        while b:
            ax = a[0]
            ay = a[1]
            az = a[2]

            bx = b[0]
            by = b[1]
            bz = b[2]

            prime_ay = str(float(ay) - displacement);
            prime_by = str(float(by) - displacement);

            triangles = [ax, ay, az,
                        ax, prime_ay, az,
                        bx, by, bz,

                        bx, by, bz,
                        ax, prime_ay, az,
                        bx, prime_by, bz]

            for num in triangles:
                print num + ","

            a = b
            b = r.next()


    return

if __name__ == "__main__":
   main(sys.argv[1:])

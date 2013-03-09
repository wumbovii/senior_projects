#my super script to run apyc on ALL THE THINGS
import os
import re

output = "tests/correct2/melviniswin.txt"
os.system("rm " + output)
test_dir = "tests/correct2"
for subdirs, dirs, files, in os.walk(test_dir):
    for file in files:
        lol = file[len(file)-3 : len(file)]
        if (lol == ".py"):
            f = open(output, 'a')
            f.write("\nFILENAME: " + file + "\n\n CODE:\n")
            py_loc = "tests/correct2/" + file
            py = open(py_loc, 'r')
            for line in py:
                f.write(line)
            f.write("\n\nCOMMAND OUTPUT\n\n")
            f.close()
            command = './apyc --phase=2 tests/correct2/' + file 
            print command
            command = command + " >> tests/correct2/melviniswin.txt"
            os.system(command)
            
            #dast
            dast = file[0 : len(file)-3] + ".dast"
            if dast in files:
                f = open(output, 'a')
                f.write("\n\nDAST OF " + file + "\n\n")
                dast_loc = "tests/correct2/" + dast
                j = open(dast_loc, 'r')
                for line in j:
                    f.write(line)
                f.close()
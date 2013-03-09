#my super script to run apyc on ALL THE THINGS
import os
import re

output = "tests/error2/melviniserror.txt"
os.system("rm " + output)
test_dir = "tests/error2"
for subdirs, dirs, files, in os.walk(test_dir):
    for file in files:
        lol = file[len(file)-3 : len(file)]
        if (lol == ".py"):
            f = open(output, 'a')
            f.write("\nFILENAME: " + file + "\n\n CODE:\n")
            py_loc = "tests/error2/" + file
            py = open(py_loc, 'r')
            for line in py:
                f.write(line)
            f.write("\n\nCOMMAND OUTPUT\n\n")
            f.close()
            command = './apyc --phase=2 tests/error2/' + file 
            print command
            command = command + " >> tests/error2/melviniserror.txt"
            os.system(command)
            
            #dast
            dast = file[0 : len(file)-3] + ".dast"
            if dast in files:
                f = open(output, 'a')
                f.write("\n\nDAST OF " + file + "\n\n")
                dast_loc = "tests/error2/" + dast
                j = open(dast_loc, 'r')
                for line in j:
                    f.write(line)
                f.close()
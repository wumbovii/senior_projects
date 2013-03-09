#constants
padlen = 32
enc1 = "CCFDA8723218716E08AF482C6D0ABA25D6EFB9742A1C366F4CAC432773169AA8"
enc2 = "DAF2A974341A7A6E08AF482C6D0A9A64EE8C47C7EB138357185EE87947FD833D"
enc3 = "CDF9B969290B732B4EA554246107F327D19C394E3CBB5AAA6AF81B62C8E72CF3"
pt12 = (hex(int(enc1, 16) ^ int(enc2,16)))[2:]
pt23 = (hex(int(enc2, 16) ^ int(enc3,16)))[2:]
pt13 = (hex(int(enc1, 16) ^ int(enc3,16)))[2:]
print pt12
print pt23
print pt13
commands = []
commands.append('charge')
commands.append('prepare ambush')
commands.append('raid trenches')
commands.append('tank attack')
commands.append('bomber attack')
commands.append('battleship attack')
commands.append('submarine attack')
commands.append('fire missiles')
commands.append('sniper fire')
commands.append('encircle enemy')
commands.append('full frontal assault')
commands.append('retreat')
commands.append('feint retreat')
commands.append('skirmish')
commands.append('blockade')
commands.append('lay mines')
commands.append('fortify')
commands.append('create distraction')
commands.append('clear and hold')
commands.append('split up')
commands.append('set up a perimeter')
commands.append('destroy bridges')
commands.append('sabotage enemy missile defense')
commands.append('restore formation')
commands.append('launch the nukes')

def findPadding(cmd):
    cmd_len = len(cmd)
    if  cmd_len > padlen:
        print "Error, length > 32"
    else:
        numspaces = padlen - cmd_len
        return cmd + (numspaces* ' ')

def findMatch(cmd1, cmd2):
    len1 = len(cmd1)
    len2 = len(cmd2)
    lenhex = 2 * min(len1, len2)
    
    padded1 = findPadding(cmd1)
    padded2 = findPadding(cmd2)
    
    match12 = (hex(int(padded1.encode('hex'), 16) ^ 
               int(padded2.encode('hex'), 16))[2:])[0:lenhex]
    
    if match12 == pt12[0:lenhex]:
        print "pt12 = " + pt12 + ": " + cmd1 + ", " + cmd2
    elif match12 == pt23[0:lenhex]:
        print "pt23 = " + pt23 + ": " + cmd1 + ", " + cmd2
    elif match12 == pt13[0:lenhex]:
        print "pt13 = " + pt13 + ": " + cmd1 + ", " + cmd2
        
for cmd1 in commands:
    for cmd2 in commands:
        findMatch(cmd1, cmd2)
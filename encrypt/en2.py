enc1 = "CCFDA8723218716E08AF482C6D0ABA25D6EFB9742A1C366F4CAC432773169AA8"
enc2 = "DAF2A974341A7A6E08AF482C6D0A9A64EE8C47C7EB138357185EE87947FD833D"
enc3 = "CDF9B969290B732B4EA554246107F327D19C394E3CBB5AAA6AF81B62C8E72CF3"

# check conversions
# print hex(int(enc1, 16))[2:]

plain12 = hex(int(enc1, 16) ^ int(enc2, 16))[2:]
plain13 = hex(int(enc1, 16) ^ int(enc3, 16))[2:]
plain23 = hex(int(enc2, 16) ^ int(enc3, 16))[2:]

print "plain12: " + plain12
print "plain13: " + plain13
print "plain23: " + plain23

def padcom(com):
  if len(com) > 32:
    print "ERROR in padcom: command-length already > 32"
  else:
    numspaces = 32 - len(com)
    return com + numspaces * ' '

def check_match(com1, com2):
  len1 = len(com1)
  len2 = len(com2)
  num_hex_digits =  min(len1, len2)
  
  padcom1 = padcom(com1)
  padcom2 = padcom(com2)
  attempt = int(padcom1.encode('hex'), 16) ^ int(padcom2.encode('hex'), 16)
  attempt = hex(attempt)[2:]
  attempt = attempt[0:num_hex_digits]
  # check if the first num_hex_digits of attempt matches plain12/13/23
  if attempt == plain12[0:num_hex_digits]:
    print "Found a match!"
    print "Command 1 is either: " + com1 + " or " + com2
    print "Command 2 is either: " + com1 + " or " + com2
  if attempt == plain13[0:num_hex_digits]:
    print "Found a match!"
    print "Command 1 is either: " + com1 + " or " + com2
    print "Command 3 is either: " + com1 + " or " + com2
  if attempt == plain23[0:num_hex_digits]:
    print "Found a match!"
    print "Command 2 is either: " + com1 + " or " + com2
    print "Command 3 is either: " + com1 + " or " + com2
  

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

i = 0
commands_len = len(commands)
while i < commands_len:
  j = i+1
  while j < commands_len:
    com1 = commands[i]
    com2 = commands[j]
    #print "Checking '" + com1 + "' and '" + com2 + "'"
    check_match(com1, com2)
    j += 1
  i += 1

# ---- crap below
'''
command1 = padcom("charge")
hex1 = command1.encode("hex")

command2 = padcom("prepare ambush")
hex2 = command2.encode('hex')

print "--- HEX 1 ---"
print hex1
print int(hex1, 16) # int
print "%x" % int(hex1, 16) # hex

print "--- HEX 2 ---"
print hex2
print int(hex2, 16) # int
print "%x" % int(hex2, 16) # hex

print " HEX1 ^ HEX2 "
print "%x" % (int(hex1, 16) ^ int(hex2, 16))

'''
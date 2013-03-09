import md5
m = md5.new()
m2 = md5.new()

salt = 'Frvhlb'
handshake = '153aea8ac9e4c4907cdd1bcffe53653c'

#ascii 48 90
c1 = 48
c2 = 48
c3 = 48
c4 = 48
c5 = 48
c6 = 48
pw = chr(c1)

def incr(c)
	if c == 58:
		c = 65
	else:
		c += 1

	if c >= 90:
		c = 48
	return chr(c)

print "Starting fuzz"
while(True):
    m.update(pw+salt)
    m2.update(salt+pw)
    #print pw
    if m.hexdigest()== handshake or m2.hexdigest() == handshake:
        print pw
        break; 
    
    if currentChar >= 90:
        currentChar = 48
        pw = pw + chr(currentChar)
    else:
        currentChar += 1
        pw = pw[:len(pw)-1] 
        pw += chr(currentChar)
    

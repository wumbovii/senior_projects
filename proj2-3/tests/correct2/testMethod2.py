#multiple methods with mutations, parameters, and return values
print "hello! this is a friendly test"

class UnfriendlyTest(object):
    
    def __init__(self):
        self.friendly = False
        self.anger = 9001
        
    def setAnger(self, val):
        self.anger = val
        return self.anger
    
    def feed(self):
        self.friendly = True
        
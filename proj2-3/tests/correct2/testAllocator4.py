#simple allocator test with NO __init__, multiple allocations of multiple classes
class iPhone4(object):
    def hasSiri(self):
        return False
        
class iPhone4S(object):
    def hasSir(self):
        return True

myPhone1 = iPhone4()
myPhone2 = iPhone4S()
myPhone3 = iPhone4()
myPhone4 = iPhone4S()
myPhone5 = iPhone4()
myPhone6 = iPhone4S()

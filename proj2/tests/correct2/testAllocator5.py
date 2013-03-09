#test multiple allocations with method calls in between
class Marine:
    def __init__(self):
        self.dmg = 6;
        self.health = 45;
        self.op = True;
        
    def balance(self):
        patch = lambda x: x -1
        self.dmg = patch(self.dmg)
        self.op = False
        
    def attack(self, unit):
        unit.health -= self.dmg
        
class Zealot:
    def __init__(self):
        self.dmg = 16
        self.health = 80
        self.shield = 80
        self.op = False
    
    def charge(self, unit):
        unit.health -= self.dmg
        self.op = True

myMarine = Marine()
myMarine.balance()
myMarine.attack(myMarine)
myZealot = Zealot()
myZealot.charge(myMarine)
myMarine.attack(myZealot)
myMarine.attack(myZealot)
myZealot2 = Zealot()
myZealot = Zealot()
myMarine = Marine()

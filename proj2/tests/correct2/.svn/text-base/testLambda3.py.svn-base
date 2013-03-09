#tests lambdas in a class body
class Marine:
    def __init__(self):
        self.dmg = 6;
        self.health = 45;
        self.op = True;
        
    def balance(self):
        patch = lambda x: x -1
        self.dmg = patch(self.dmg)
        self.op = False

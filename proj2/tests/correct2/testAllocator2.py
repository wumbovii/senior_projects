#test multiple allocations of a single class with __init__
class EngineerClass:
    def __init__(self):
        self.weapon = "SCAR-H"
        self.gadget = "SMAW"

myEngineer1 = EngineerClass()
myEngineer2 = EngineerClass()
myEngineer3 = EngineerClass()
myEngineer4 = EngineerClass()
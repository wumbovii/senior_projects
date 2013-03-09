#simple allocation test with __init__
class AssaultClass(object):
    def __init__(self):
        self.weapon = "M16A3"
        self.gadget = "MedKit"
    
mySolder = AssaultClass()

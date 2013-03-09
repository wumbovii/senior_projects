#test multiple allocations of various classes with __init__, including reassignments and allocations
class SupportClass:
    def __init__(self, team):
        self.weapon = "M249 SAW"
        self.gadget = "AmmoKit"
        self.team = team

class ReconClass:
    def __init__(self, team):
        self.weapon = "MK11"
        self.gadget = "RadioBeacon"
        self.team = team
        
mySupport1 = SupportClass("USMC")
mySupport2 = SupportClass("OpFor")
myRecon1 = ReconClass("USMC")
myRecon2 = ReconClass("OpFor")

#reassignments
mySupport1 = SupportClass("OpFor")
myRecon2 = ReconClass("USMC")
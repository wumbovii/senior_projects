class foo(Object):
    x = [2, 3, 4]
    
    def f1(self):
        def f2():
            print self.x
        print self.x


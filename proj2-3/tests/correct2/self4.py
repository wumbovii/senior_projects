class foo(Object):
    x = 2
    
    def f1(self):
        self.x = 4
        def f2():
            print self.x
        print self.x


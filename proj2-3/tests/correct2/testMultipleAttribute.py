class A(object):
    y = 1
class B(object):
    def g():
        return x
    x = A()
f = B.g().y

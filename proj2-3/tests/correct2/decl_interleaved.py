
global_var = 1
global_typed::int = 1
class class_external(parent):
    
    instance_var1 = 1
    instanced_typed1::int = 1

    def func_def(self):
        local_var = 1
        local_typed::int = 1

    instance_var2 = 1
    instance_typed::int = 1

    def func_def_with_params(self):
        local_var = 1

    def func_def_with_multiparam(self, x, y, z):
        pass

    def func_def_with_typedparam(self, x::int):
        pass

    def func_def_nest1(self):
        local_nest1 = 1
        def func_def_nest2():
            pass
        local_nest2 = 1


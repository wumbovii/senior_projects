
global_var =1
global_typed::int = 1
class class_external(parent): 
    instance_var = 1
    instanced_typed::int = 1

    def func_def(self):
        local_var = 1
        local_typed::int = 1

    def func_def_with_params(self, x):
        pass

    def func_def_with_multiparam(self, x, y, z):
        pass

    def func_def_with_typedparam(self, x::int):
        pass

    def func_def_nest1(self):

        def func_def_nest2():
            pass

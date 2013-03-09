
global_var =1
global_typed::int = 1
class class_external(parent): 
    instance_var = 1
    instanced_typed::int = 1

    def func_def():
        local_var = 1
        local_typed::int = 1

    def func_def_with_params(x):
        pass

    def func_def_with_multiparam(x, y, z):
        pass

    def func_def_with_typedparam(x::int):
        pass

    def func_def_nest1():

        def func_def_nest2():
            pass

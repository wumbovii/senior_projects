set print object on
set print vtbl on
set print static-members off

define pstr
   p (char*) $arg0
end
define vget
   p $arg0._M_impl._M_start[$arg1]
end


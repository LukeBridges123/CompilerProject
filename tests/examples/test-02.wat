(module
  (func $Echo (param $var0 i32) (result i32)
    ;; Variables
    
    (block $fun_exit1 (result i32)
      (local.get $var0)    ;; Place var 'x' onto stack
    ) ;; end of function block.
  ) ;; END Echo function definition.
  
  (export "Echo" (func $Echo))
  
) ;; END program module

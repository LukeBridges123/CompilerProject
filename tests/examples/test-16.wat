(module
  (func $EchoC (param $var0 i32) (result i32)
    ;; Variables
    
    (block $fun_exit1 (result i32)
      (local.get $var0)    ;; Place var 'c' onto stack
    ) ;; end of function block.
  ) ;; END EchoC function definition.
  
  (export "EchoC" (func $EchoC))
  
) ;; END program module

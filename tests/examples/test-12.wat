(module
  (func $EchoD (param $var0 f64) (result f64)
    ;; Variables
    
    (block $fun_exit1 (result f64)
      (local.get $var0)    ;; Place var 'x' onto stack
    ) ;; end of function block.
  ) ;; END EchoD function definition.
  
  (export "EchoD" (func $EchoD))
  
) ;; END program module

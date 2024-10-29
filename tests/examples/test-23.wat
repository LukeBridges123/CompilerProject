(module
  (func $Plus1 (param $var0 i32) (result i32)
    ;; Variables
    
    (block $fun_exit1 (result i32)
      (local.get $var0)    ;; Place var 'in' onto stack
      (i32.const 1)    ;; Put a 1 on the stack
      (i32.add)   ;; Stack2 + Stack1
    ) ;; end of function block.
  ) ;; END Plus1 function definition.
  
  (export "Plus1" (func $Plus1))
  
  (func $PlusOneHalf (param $var1 f64) (result f64)
    ;; Variables
    
    (block $fun_exit2 (result f64)
      (local.get $var1)    ;; Place var 'in' onto stack
      (f64.const 0.5)    ;; Put a 0.5 on the stack
      (f64.add)   ;; Stack2 + Stack1
    ) ;; end of function block.
  ) ;; END PlusOneHalf function definition.
  
  (export "PlusOneHalf" (func $PlusOneHalf))
  
  (func $HalfAgain (param $var2 f64) (result f64)
    ;; Variables
    
    (block $fun_exit3 (result f64)
      (local.get $var2)    ;; Place var 'in' onto stack
      (f64.const 1.5)    ;; Put a 1.5 on the stack
      (f64.mul)   ;; Stack2 * Stack1
    ) ;; end of function block.
  ) ;; END HalfAgain function definition.
  
  (export "HalfAgain" (func $HalfAgain))
  
) ;; END program module

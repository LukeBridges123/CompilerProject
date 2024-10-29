(module
  (func $CalcHypotenuse (param $var0 f64) (param $var1 f64) (result f64)
    ;; Variables
    (local $var2 f64)  ;; Variable: c_sqr
    
    (block $fun_exit1 (result f64)
      ;; ASSIGNMENT
        (local.get $var0)    ;; Place var 'sideA' onto stack
        (local.get $var0)    ;; Place var 'sideA' onto stack
        (f64.mul)   ;; Stack2 * Stack1
        (local.get $var1)    ;; Place var 'sideB' onto stack
        (local.get $var1)    ;; Place var 'sideB' onto stack
        (f64.mul)   ;; Stack2 * Stack1
        (f64.add)   ;; Stack2 + Stack1
        (local.set $var2)    ;; Set var 'c_sqr' from stack
        (local.get $var2)    ;; Place var 'c_sqr' onto stack
      ;; END ASSIGNMENT (with result on stack)
      (drop)  ;; remove unused value on stack.
      (local.get $var2)    ;; Place var 'c_sqr' onto stack
      (f64.sqrt)     ;; Square Root
    ) ;; end of function block.
  ) ;; END CalcHypotenuse function definition.
  
  (export "CalcHypotenuse" (func $CalcHypotenuse))
  
) ;; END program module

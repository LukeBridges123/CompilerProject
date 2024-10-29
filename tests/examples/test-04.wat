(module
  (func $Multiply (param $var0 i32) (param $var1 i32) (result i32)
    ;; Variables
    
    (block $fun_exit1 (result i32)
      (local.get $var0)    ;; Place var 'val1' onto stack
      (local.get $var1)    ;; Place var 'val2' onto stack
      (i32.mul)   ;; Stack2 * Stack1
    ) ;; end of function block.
  ) ;; END Multiply function definition.
  
  (export "Multiply" (func $Multiply))
  
) ;; END program module

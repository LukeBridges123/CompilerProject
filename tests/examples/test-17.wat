(module
  (func $IsUpper (param $var0 i32) (result i32)
    ;; Variables
    
    (block $fun_exit1 (result i32)
      ;; Setup the && operation
      (local.get $var0)    ;; Place var 'c' onto stack
      (i32.const 65)    ;; Put a 65 on the stack
      (i32.ge_s)  ;; Stack2 >= Stack1
      (if (result i32) ;; Setup for && operator
        (then
          (local.get $var0)    ;; Place var 'c' onto stack
          (i32.const 90)    ;; Put a 90 on the stack
          (i32.le_s)  ;; Stack2 <= Stack1
          (i32.const 0) ;; Put a zero on the stack for comparison)
          (i32.ne)      ;; Set any non-zero value to one.)
        )
        (else 
          (i32.const 0) ;; First clause of && was false.
        )
      )
      ;; End of && operation
    ) ;; end of function block.
  ) ;; END IsUpper function definition.
  
  (export "IsUpper" (func $IsUpper))
  
) ;; END program module

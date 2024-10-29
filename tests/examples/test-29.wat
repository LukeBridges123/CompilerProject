(module
  (func $ExactlyTwo (param $var0 i32) (param $var1 i32) (param $var2 i32) (result i32)
    ;; Variables
    
    (block $fun_exit1 (result i32)
      ;; Setup the || operation
      ;; Setup the || operation
      ;; Setup the && operation
      ;; Setup the && operation
      (local.get $var0)    ;; Place var 'opt1' onto stack
      (if (result i32) ;; Setup for && operator
        (then
          (local.get $var1)    ;; Place var 'opt2' onto stack
          (i32.const 0) ;; Put a zero on the stack for comparison)
          (i32.ne)      ;; Set any non-zero value to one.)
        )
        (else 
          (i32.const 0) ;; First clause of && was false.
        )
      )
      ;; End of && operation
      (if (result i32) ;; Setup for && operator
        (then
          (local.get $var2)    ;; Place var 'opt3' onto stack
          i32.eqz ;; Boolean NOT.
          (i32.const 0) ;; Put a zero on the stack for comparison)
          (i32.ne)      ;; Set any non-zero value to one.)
        )
        (else 
          (i32.const 0) ;; First clause of && was false.
        )
      )
      ;; End of && operation
      (if (result i32)  ;; Setup for || operator
        (then
          (i32.const 1) ;; First clause of || was true.
        )
        (else 
          ;; Setup the && operation
          ;; Setup the && operation
          (local.get $var0)    ;; Place var 'opt1' onto stack
          (if (result i32) ;; Setup for && operator
            (then
              (local.get $var1)    ;; Place var 'opt2' onto stack
              i32.eqz ;; Boolean NOT.
              (i32.const 0) ;; Put a zero on the stack for comparison)
              (i32.ne)      ;; Set any non-zero value to one.)
            )
            (else 
              (i32.const 0) ;; First clause of && was false.
            )
          )
          ;; End of && operation
          (if (result i32) ;; Setup for && operator
            (then
              (local.get $var2)    ;; Place var 'opt3' onto stack
              (i32.const 0) ;; Put a zero on the stack for comparison)
              (i32.ne)      ;; Set any non-zero value to one.)
            )
            (else 
              (i32.const 0) ;; First clause of && was false.
            )
          )
          ;; End of && operation
          (i32.const 0) ;; Put a zero on the stack for comparison)
          (i32.ne)      ;; Set any non-zero value to one.)
        )
      )
      ;; End of || operation
      (if (result i32)  ;; Setup for || operator
        (then
          (i32.const 1) ;; First clause of || was true.
        )
        (else 
          ;; Setup the && operation
          ;; Setup the && operation
          (local.get $var0)    ;; Place var 'opt1' onto stack
          i32.eqz ;; Boolean NOT.
          (if (result i32) ;; Setup for && operator
            (then
              (local.get $var1)    ;; Place var 'opt2' onto stack
              (i32.const 0) ;; Put a zero on the stack for comparison)
              (i32.ne)      ;; Set any non-zero value to one.)
            )
            (else 
              (i32.const 0) ;; First clause of && was false.
            )
          )
          ;; End of && operation
          (if (result i32) ;; Setup for && operator
            (then
              (local.get $var2)    ;; Place var 'opt3' onto stack
              (i32.const 0) ;; Put a zero on the stack for comparison)
              (i32.ne)      ;; Set any non-zero value to one.)
            )
            (else 
              (i32.const 0) ;; First clause of && was false.
            )
          )
          ;; End of && operation
          (i32.const 0) ;; Put a zero on the stack for comparison)
          (i32.ne)      ;; Set any non-zero value to one.)
        )
      )
      ;; End of || operation
    ) ;; end of function block.
  ) ;; END ExactlyTwo function definition.
  
  (export "ExactlyTwo" (func $ExactlyTwo))
  
) ;; END program module

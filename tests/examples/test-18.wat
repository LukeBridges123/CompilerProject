(module
  (func $ToUpper (param $var0 i32) (result i32)
    ;; Variables
    (local $var1 i32)  ;; Variable: shift
    
    (block $fun_exit1 (result i32)
      ;; Test condition for if.
      ;; Setup the && operation
      (local.get $var0)    ;; Place var 'c' onto stack
      (i32.const 97)    ;; Put a 97 on the stack
      (i32.ge_s)  ;; Stack2 >= Stack1
      (if (result i32) ;; Setup for && operator
        (then
          (local.get $var0)    ;; Place var 'c' onto stack
          (i32.const 122)    ;; Put a 122 on the stack
          (i32.le_s)  ;; Stack2 <= Stack1
          (i32.const 0) ;; Put a zero on the stack for comparison)
          (i32.ne)      ;; Set any non-zero value to one.)
        )
        (else 
          (i32.const 0) ;; First clause of && was false.
        )
      )
      ;; End of && operation
      (if  ;; Execute code based on result of condition.
        (then ;; 'then' block
          ;; ASSIGNMENT
            (i32.const 97)    ;; Put a 97 on the stack
            (i32.const 65)    ;; Put a 65 on the stack
            (i32.sub)   ;; Stack2 - Stack1
            (local.set $var1)    ;; Set var 'shift' from stack
            (local.get $var1)    ;; Place var 'shift' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          (local.get $var0)    ;; Place var 'c' onto stack
          (local.get $var1)    ;; Place var 'shift' onto stack
          (i32.sub)   ;; Stack2 - Stack1
          (br $fun_exit1) ;; return value.
        ) ;; End 'then'
      )  ;; End 'if'
      (local.get $var0)    ;; Place var 'c' onto stack
    ) ;; end of function block.
  ) ;; END ToUpper function definition.
  
  (export "ToUpper" (func $ToUpper))
  
) ;; END program module

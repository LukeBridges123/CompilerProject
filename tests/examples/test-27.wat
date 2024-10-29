(module
  (func $Logish (param $var0 f64) (result i32)
    ;; Variables
    (local $var1 i32)  ;; Variable: count
    (local $var2 f64)  ;; Variable: count
    
    (block $fun_exit1 (result i32)
      ;; ASSIGNMENT
        (i32.const 0)    ;; Put a 0 on the stack
        (local.set $var1)    ;; Set var 'count' from stack
        (local.get $var1)    ;; Place var 'count' onto stack
      ;; END ASSIGNMENT (with result on stack)
      (drop)  ;; remove unused value on stack.
      (block $exit1   ;; Outer block for breaking while loop.
        (loop $loop1  ;; Inner loop for continuing while.
          ;; WHILE Test condition...
          (local.get $var0)    ;; Place var 'value' onto stack
          (f64.const 1.5)    ;; Put a 1.5 on the stack
          (f64.gt)  ;; Stack2 > Stack1
          (i32.eqz)  ;; Invert the result of the test condition.
          (br_if $exit1)  ;; If condition is false (0), exit the loop
          ;; WHILE Loop body...
          ;; ASSIGNMENT
            (local.get $var1)    ;; Place var 'count' onto stack
            (i32.const 1)    ;; Put a 1 on the stack
            (i32.add)   ;; Stack2 + Stack1
            (local.set $var1)    ;; Set var 'count' from stack
            (local.get $var1)    ;; Place var 'count' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          ;; ASSIGNMENT
            (local.get $var0)    ;; Place var 'value' onto stack
            (i32.const 2)    ;; Put a 2 on the stack
            (f64.convert_i32_s) ;; Convert to double.
            (f64.div) ;; Stack2 / Stack1
            (local.set $var0)    ;; Set var 'value' from stack
            (local.get $var0)    ;; Place var 'value' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          ;; ASSIGNMENT
            (i32.const 100)    ;; Put a 100 on the stack
            (f64.convert_i32_s) ;; Convert to double.
            (local.set $var2)    ;; Set var 'count' from stack
            (local.get $var2)    ;; Place var 'count' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          ;; ASSIGNMENT
            (local.get $var2)    ;; Place var 'count' onto stack
            (local.get $var0)    ;; Place var 'value' onto stack
            (i32.trunc_f64_s) ;; Convert to int.
            (f64.convert_i32_s) ;; Convert to double.
            (f64.mul)   ;; Stack2 * Stack1
            (local.set $var2)    ;; Set var 'count' from stack
            (local.get $var2)    ;; Place var 'count' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          ;; WHILE start next loop.
          (br $loop1)   ;; Jump back to the start of the loop
        ) ;; End loop
      )   ;; End block
      (local.get $var1)    ;; Place var 'count' onto stack
    ) ;; end of function block.
  ) ;; END Logish function definition.
  
  (export "Logish" (func $Logish))
  
) ;; END program module

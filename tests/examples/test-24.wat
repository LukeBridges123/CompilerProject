(module
  (func $CountDivSeven (param $var0 f64) (param $var1 f64) (result i32)
    ;; Variables
    (local $var2 i32)  ;; Variable: value
    (local $var3 i32)  ;; Variable: stop
    (local $var4 i32)  ;; Variable: count
    
    (block $fun_exit1 (result i32)
      ;; ASSIGNMENT
        (local.get $var0)    ;; Place var 'min' onto stack
        (i32.trunc_f64_s) ;; Convert to int.
        (local.set $var2)    ;; Set var 'value' from stack
        (local.get $var2)    ;; Place var 'value' onto stack
      ;; END ASSIGNMENT (with result on stack)
      (drop)  ;; remove unused value on stack.
      ;; ASSIGNMENT
        (local.get $var1)    ;; Place var 'max' onto stack
        (i32.trunc_f64_s) ;; Convert to int.
        (local.set $var3)    ;; Set var 'stop' from stack
        (local.get $var3)    ;; Place var 'stop' onto stack
      ;; END ASSIGNMENT (with result on stack)
      (drop)  ;; remove unused value on stack.
      ;; ASSIGNMENT
        (i32.const 0)    ;; Put a 0 on the stack
        (local.set $var4)    ;; Set var 'count' from stack
        (local.get $var4)    ;; Place var 'count' onto stack
      ;; END ASSIGNMENT (with result on stack)
      (drop)  ;; remove unused value on stack.
      ;; Test condition for if.
      (local.get $var2)    ;; Place var 'value' onto stack
      (f64.convert_i32_s) ;; Convert to double.
      (local.get $var0)    ;; Place var 'min' onto stack
      (f64.lt)  ;; Stack2 < Stack1
      (if  ;; Execute code based on result of condition.
        (then ;; 'then' block
          ;; ASSIGNMENT
            (local.get $var2)    ;; Place var 'value' onto stack
            (i32.const 1)    ;; Put a 1 on the stack
            (i32.add)   ;; Stack2 + Stack1
            (local.set $var2)    ;; Set var 'value' from stack
            (local.get $var2)    ;; Place var 'value' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)
        ) ;; End 'then'
      )  ;; End 'if'
      (block $exit1   ;; Outer block for breaking while loop.
        (loop $loop1  ;; Inner loop for continuing while.
          ;; WHILE Test condition...
          (local.get $var2)    ;; Place var 'value' onto stack
          (local.get $var3)    ;; Place var 'stop' onto stack
          (i32.le_s)  ;; Stack2 <= Stack1
          (i32.eqz)  ;; Invert the result of the test condition.
          (br_if $exit1)  ;; If condition is false (0), exit the loop
          ;; WHILE Loop body...
          ;; Test condition for if.
          (local.get $var2)    ;; Place var 'value' onto stack
          (i32.const 7)    ;; Put a 7 on the stack
          (i32.rem_s) ;; Stack2 % Stack1
          (i32.const 0)    ;; Put a 0 on the stack
          (i32.ne)    ;; Stack2 != Stack1
          (if  ;; Execute code based on result of condition.
            (then ;; 'then' block
              ;; ASSIGNMENT
                (local.get $var2)    ;; Place var 'value' onto stack
                (i32.const 1)    ;; Put a 1 on the stack
                (i32.add)   ;; Stack2 + Stack1
                (local.set $var2)    ;; Set var 'value' from stack
                (local.get $var2)    ;; Place var 'value' onto stack
              ;; END ASSIGNMENT (with result on stack)
              (drop)  ;; remove unused value on stack.
              (br $loop1) ;; 'continue' command.
            ) ;; End 'then'
          )  ;; End 'if'
          ;; ASSIGNMENT
            (local.get $var4)    ;; Place var 'count' onto stack
            (i32.const 1)    ;; Put a 1 on the stack
            (i32.add)   ;; Stack2 + Stack1
            (local.set $var4)    ;; Set var 'count' from stack
            (local.get $var4)    ;; Place var 'count' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          ;; ASSIGNMENT
            (local.get $var2)    ;; Place var 'value' onto stack
            (i32.const 7)    ;; Put a 7 on the stack
            (i32.add)   ;; Stack2 + Stack1
            (local.set $var2)    ;; Set var 'value' from stack
            (local.get $var2)    ;; Place var 'value' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (drop)  ;; remove unused value on stack.
          ;; WHILE start next loop.
          (br $loop1)   ;; Jump back to the start of the loop
        ) ;; End loop
      )   ;; End block
      (local.get $var4)    ;; Place var 'count' onto stack
    ) ;; end of function block.
  ) ;; END CountDivSeven function definition.
  
  (export "CountDivSeven" (func $CountDivSeven))
  
) ;; END program module

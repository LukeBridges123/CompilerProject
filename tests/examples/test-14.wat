(module
  (func $CompareD (param $var0 f64) (param $var1 f64) (result i32)
    ;; Variables
    
    (block $fun_exit1 (result i32)
      ;; Test condition for if.
      (local.get $var0)    ;; Place var 'x' onto stack
      (local.get $var1)    ;; Place var 'y' onto stack
      (f64.lt)  ;; Stack2 < Stack1
      (if  ;; Execute code based on result of condition.
        (then ;; 'then' block
          (i32.const 0) ;; Setup unary negation
          (i32.const 1)    ;; Put a 1 on the stack
          (i32.sub)     ;; Unary negation.
          (br $fun_exit1) ;; return value.
        ) ;; End 'then'
        (else ;; 'else' block
          ;; Test condition for if.
          (local.get $var0)    ;; Place var 'x' onto stack
          (local.get $var1)    ;; Place var 'y' onto stack
          (f64.gt)  ;; Stack2 > Stack1
          (if  ;; Execute code based on result of condition.
            (then ;; 'then' block
              (i32.const 1)    ;; Put a 1 on the stack
              (br $fun_exit1) ;; return value.
            ) ;; End 'then'
          )  ;; End 'if'
        ) ;; End 'else'
      )  ;; End 'if'
      (i32.const 0)    ;; Put a 0 on the stack
    ) ;; end of function block.
  ) ;; END CompareD function definition.
  
  (export "CompareD" (func $CompareD))
  
) ;; END program module

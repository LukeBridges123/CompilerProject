(module
  (func $Triple (param $var0 f64) (result f64)
    ;; Variables
    (local $var1 f64)  ;; Variable: x
    (local $var2 f64)  ;; Variable: y
    (local $var3 f64)  ;; Variable: z
    
    (block $fun_exit1 (result f64)
      ;; ASSIGNMENT
        (i32.const 0)    ;; Put a 0 on the stack
        (f64.convert_i32_s) ;; Convert to double.
        (local.set $var1)    ;; Set var 'x' from stack
        (local.get $var1)    ;; Place var 'x' onto stack
      ;; END ASSIGNMENT (with result on stack)
      (drop)  ;; remove unused value on stack.
      ;; ASSIGNMENT
        ;; ASSIGNMENT
          ;; ASSIGNMENT
            (local.get $var0)    ;; Place var 'val' onto stack
            (local.set $var2)    ;; Set var 'y' from stack
            (local.get $var2)    ;; Place var 'y' onto stack
          ;; END ASSIGNMENT (with result on stack)
          (local.set $var1)    ;; Set var 'x' from stack
          (local.get $var1)    ;; Place var 'x' onto stack
        ;; END ASSIGNMENT (with result on stack)
        (local.set $var3)    ;; Set var 'z' from stack
        (local.get $var3)    ;; Place var 'z' onto stack
      ;; END ASSIGNMENT (with result on stack)
      (drop)  ;; remove unused value on stack.
      (local.get $var1)    ;; Place var 'x' onto stack
      (local.get $var2)    ;; Place var 'y' onto stack
      (f64.add)   ;; Stack2 + Stack1
      (local.get $var3)    ;; Place var 'z' onto stack
      (f64.add)   ;; Stack2 + Stack1
    ) ;; end of function block.
  ) ;; END Triple function definition.
  
  (export "Triple" (func $Triple))
  
) ;; END program module

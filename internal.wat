;; Function to allocate space for a string; add one to size and place null terminator there.
(func $_str_alloc (param $size i32) (result i32)
  (local $null_pos i32)     ;; Local variable for where to place null terminator.
  (global.get $_free)       ;; Staring free pos is start of new string; put on stack to return.
  (i32.add                  ;; Add free pos and size to get position for null terminator.
    (global.get $_free)     ;; Load a second copy to adjust new free pos.
    (local.get $size))      ;; Load size of new string
  (local.set $null_pos)     ;; Save position for null terminator to use it again.
  (i32.store8               ;; Place null terminator.
    (local.get $null_pos)
    (i32.const 0))
  (i32.add
    (i32.const 1)
    (local.get $null_pos))  ;; Increment past null for new free pos.
  (global.set $_free))      ;; Update free memory start.

;;(func $getStringLength (export "getStringLength") (param $str_ptr i32) (result i32)
;;    (local $length i32) ;; Variable to store the string length
;;    ;; Initialize length to 0
;;    (i32.const 0)
;;    (set_local $length)
;;
;;    (block $exit1 ;; Outer block
;;      (loop $loop
;;        ;; Load a byte from the current address
;;        (local.get $str_ptr)
;;        (local.get $length)
;;        (i32.add)          ;; Current address = start + length
;;        (i32.load8_u)      ;; Load 1 byte from memory (unsigned)
;;
;;        ;; Check if it's the null terminator
;;        (i32.eqz)          ;; Is the byte 0?
;;        (br_if $exit1)
;;
;;        ;; Increment length
;;        (local.get $length)
;;        (i32.const 1)
;;        (i32.add)
;;        (set_local $length)
;;
;;        ;; Continue the loop
;;        (br $loop)
;;      )
;;    )
;;    ;; Return the calculated length
;;    local.get $length
;;  )
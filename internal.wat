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

(func $getStringLength (param $str_ptr i32) (result i32)
    (local $length i32) ;; Variable to store the string length
    ;; Initialize length to 0
    (i32.const 0)
    (local.set $length)

    (block $exit1 ;; Outer block
      (loop $loop
        ;; Load a byte from the current address
        (local.get $str_ptr)
        (local.get $length)
        (i32.add)          ;; Current address = start + length
        (i32.load8_u)      ;; Load 1 byte from memory (unsigned)

        ;; Check if it's the null terminator
        (i32.eqz)          ;; Is the byte 0?
        (br_if $exit1)

        ;; Increment length
        (local.get $length)
        (i32.const 1)
        (i32.add)
        (local.set $length)

        ;; Continue the loop
        (br $loop)
      )
    )
    ;; Return the calculated length
    local.get $length
  )

(func $copyStr (param $mem_ptr i32) (param $str i32) (result i32)
  (local $index i32) 
  (i32.const 0)
  (local.set $index)

  (block $exit1 ;; Outer block
    (loop $loop
      ;; Load a byte from the current address
      (local.get $str)
      (local.get $index)
      (i32.add) 
      (i32.load8_u)      ;; Load 1 byte from memory (unsigned)

      ;; Check if it's the null terminator
      (i32.eqz)          ;; Is the byte 0?
      (br_if $exit1)
  
      (local.get $mem_ptr)

      (local.get $str)
      (local.get $index)
      (i32.add)          ;; Current address = start + length
      (i32.load8_u)

      (i32.store8)

      (local.get $mem_ptr)
      (i32.const 1)
      (i32.add)
      (local.set $mem_ptr)

      (local.get $index)
      (i32.const 1)
      (i32.add)
      (local.set $index)

      ;; Continue the loop
      (br $loop)
    )
  )
  ;; Return the calculated length
  local.get $mem_ptr
)

(func $addTwo_str (param $str1 i32) (param $str2 i32) (result i32)
  (global.get $_free) ;; Save starting point to return

  (local.get $str1)
  (call $getStringLength)

  (local.get $str2)
  (call $getStringLength)

  (i32.add)
  (call $_str_alloc)
  
  (local.get $str1)
  (call $copyStr)

  (local.get $str2)
  (call $copyStr)

  (drop)
)

(func $charTo_str (param $char i32) (result i32)
  (global.get $_free) ;; Save starting point to return
  
  (i32.const 1)
  (call $_str_alloc)

  (local.get $char)
  ;;(i32.load8_u) this shouldn't be here--we need to store in the newly allocated memory, not access what's there

  (i32.store8)
)

(func $multply_str (param $str i32) (param $times i32) (result i32)
  (local $res_val i32)   ;; Return value
  (local $mem_ptr i32)

  (local.get $str)
  (call $getStringLength)

  (local.get $times)
  
  (i32.mul)
  (call $_str_alloc)

  (local.set $mem_ptr)
  (local.get $mem_ptr)
  (local.set $res_val)  ;;Assigning return value

  (block $exit1 ;; Outer block
    (loop $loop
      (local.get $times)
      (i32.eqz)        
      (br_if $exit1)

      (local.get $mem_ptr)
      (local.get $str)
      (call $copyStr)

      (local.set $mem_ptr)

      (local.get $times)
      (i32.const 1)
      (i32.sub)
      (local.set $times)
      
      ;; Continue the loop
      (br $loop)
    )
  )

  (local.get $res_val)
)

(func $index_str (param $str i32) (param $index i32) (result i32)
  (local.get $str)
  (local.get $index)
  (i32.add)

  (i32.load8_u)
)

test_lexer([[
0b
]], [[
(1,1-1,2): invalid binary constant
(2,1): <end of file>
]])

test_lexer([[
0b0
0b1
0b00 |
0b01
0b10 
0b11
]], [[
(1,1-1,3): binary constant (0x0)
(2,1-2,3): binary constant (0x1)
(3,1-3,4): binary constant (0x0)
(3,6-3,6): '|'
(4,1-4,4): binary constant (0x1)
(5,1-5,4): binary constant (0x2)
(6,1-6,4): binary constant (0x3)
(7,1): <end of file>
]])

-- 16
test_lexer([[
0b0000000000000000
0b0000000000000001 @
0b1000000000000001
]], [[
(1,1-1,18): binary constant (0x0)
(2,1-2,18): binary constant (0x1)
(2,20-2,20): '@'
(3,1-3,18): binary constant (0x8001)
(4,1): <end of file>
]])

-- 32
test_lexer([[
0b00000000000000000000000000000000
0b00000000000000000000000000000001
0b10000000000000000000000000000001 ||
0b11111111111111111111111111111111
0b100000000000000000000000000000000
]], [[
(1,1-1,34): binary constant (0x0)
(2,1-2,34): binary constant (0x1)
(3,1-3,34): binary constant (0x80000001)
(3,36-3,37): '||'
(4,1-4,34): binary constant (0xffffffff)
(5,1-5,35): binary constant (0x0) <overflow>
(6,1): <end of file>
]])

-- 64
test_lexer([[
0b0000000000000000000000000000000000000000000000000000000000000000
0b0000000000000000000000000000000000000000000000000000000000000001 }
0b1000000000000000000000000000000000000000000000000000000000000001
]], [[
(1,1-1,66): binary constant (0x0)
(2,1-2,66): binary constant (0x1)
(2,68-2,68): '}'
(3,1-3,66): binary constant (0x1) <overflow>
(4,1): <end of file>
]])

test_lexer([[
0b2
0b3
0b4
0b5
0b6
0b7
0b8
0b9
0ba
0bb 
0bc
0bd
0be
0bf
0bw
0b1f
]], [[
(1,1-1,3): invalid binary constant "0b2"
(2,1-2,3): invalid binary constant "0b3"
(3,1-3,3): invalid binary constant "0b4"
(4,1-4,3): invalid binary constant "0b5"
(5,1-5,3): invalid binary constant "0b6"
(6,1-6,3): invalid binary constant "0b7"
(7,1-7,3): invalid binary constant "0b8"
(8,1-8,3): invalid binary constant "0b9"
(9,1-9,3): invalid binary constant "0ba"
(10,1-10,3): invalid binary constant "0bb"
(11,1-11,3): invalid binary constant "0bc"
(12,1-12,3): invalid binary constant "0bd"
(13,1-13,3): invalid binary constant "0be"
(14,1-14,3): invalid binary constant "0bf"
(15,1-15,3): invalid binary constant "0bw"
(16,1-16,4): invalid binary constant "0b1f"
(17,1): <end of file>
]])

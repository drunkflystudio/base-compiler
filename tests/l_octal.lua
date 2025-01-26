
test_lexer([[
0b0
0b1
0b00
0b01
0b10
0b11
]], [[
(1,0-1,0): file start
(1,1-1,3): binary constant (0x0)
(2,1-2,3): binary constant (0x1)
(3,1-3,4): binary constant (0x0)
(4,1-4,4): binary constant (0x1)
(5,1-5,4): binary constant (0x2)
(6,1-6,4): binary constant (0x3)
(7,1-7,1): <end of file>
]])

-- 16
test_lexer([[
0b0000000000000000
0b0000000000000001
0b1000000000000001
]], [[
(1,0-1,0): file start
(1,1-1,18): binary constant (0x0)
(2,1-2,18): binary constant (0x1)
(3,1-3,18): binary constant (0x8001)
(4,1-4,1): <end of file>
]])

-- 32
test_lexer([[
0b00000000000000000000000000000000
0b00000000000000000000000000000001
0b10000000000000000000000000000001
0b11111111111111111111111111111111
0b100000000000000000000000000000000
]], [[
[warning] (5,1-5,35): overflow while parsing octal constant.
(1,0-1,0): file start
(1,1-1,34): binary constant (0x0)
(2,1-2,34): binary constant (0x1)
(3,1-3,34): binary constant (0x80000001)
(4,1-4,34): binary constant (0xffffffff)
(5,1-5,35): binary constant (0x0)
(6,1-6,1): <end of file>
]])

-- 64
test_lexer([[
0b0000000000000000000000000000000000000000000000000000000000000000
0b0000000000000000000000000000000000000000000000000000000000000001
0b1000000000000000000000000000000000000000000000000000000000000001
]], [[
[warning] (3,1-3,66): overflow while parsing octal constant.
(1,0-1,0): file start
(1,1-1,66): binary constant (0x0)
(2,1-2,66): binary constant (0x1)
(3,1-3,66): binary constant (0x1)
(4,1-4,1): <end of file>
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
[error] (1,1-1,3): syntax error in binary constant.
[error] (2,1-2,3): syntax error in binary constant.
[error] (3,1-3,3): syntax error in binary constant.
[error] (4,1-4,3): syntax error in binary constant.
[error] (5,1-5,3): syntax error in binary constant.
[error] (6,1-6,3): syntax error in binary constant.
[error] (7,1-7,3): syntax error in binary constant.
[error] (8,1-8,3): syntax error in binary constant.
[error] (9,1-9,3): syntax error in binary constant.
[error] (10,1-10,3): syntax error in binary constant.
[error] (11,1-11,3): syntax error in binary constant.
[error] (12,1-12,3): syntax error in binary constant.
[error] (13,1-13,3): syntax error in binary constant.
[error] (14,1-14,3): syntax error in binary constant.
[error] (15,1-15,3): syntax error in binary constant.
[error] (16,1-16,4): syntax error in binary constant.
(1,0-1,0): file start
(1,1-1,3): binary constant (0x2)
(2,1-2,3): binary constant (0x3)
(3,1-3,3): binary constant (0x4)
(4,1-4,3): binary constant (0x5)
(5,1-5,3): binary constant (0x6)
(6,1-6,3): binary constant (0x7)
(7,1-7,3): binary constant (0x8)
(8,1-8,3): binary constant (0x9)
(9,1-9,3): binary constant (0xa)
(10,1-10,3): binary constant (0xb)
(11,1-11,3): binary constant (0xc)
(12,1-12,3): binary constant (0xd)
(13,1-13,3): binary constant (0xe)
(14,1-14,3): binary constant (0xf)
(15,1-15,3): binary constant (0x0)
(16,1-16,4): binary constant (0xf)
(17,1-17,1): <end of file>
]])

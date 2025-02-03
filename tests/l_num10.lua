
test_lexer([[
0
1
9
09
90
99
]], [[
(1,1-1,1): decimal constant (0x0)
(2,1-2,1): decimal constant (0x1)
(3,1-3,1): decimal constant (0x9)
(4,1-4,2): decimal constant (0x9)
(5,1-5,2): decimal constant (0x5a)
(6,1-6,2): decimal constant (0x63)
(7,1): <end of file>
]])

-- 16
test_lexer([[
00000
00001
32769
]], [[
(1,1-1,5): decimal constant (0x0)
(2,1-2,5): decimal constant (0x1)
(3,1-3,5): decimal constant (0x8001)
(4,1): <end of file>
]])

-- 32
test_lexer([[
0000000000
0000000001
2147483649
4294967295
4294967296
]], [[
(1,1-1,10): decimal constant (0x0)
(2,1-2,10): decimal constant (0x1)
(3,1-3,10): decimal constant (0x80000001)
(4,1-4,10): decimal constant (0xffffffff)
(5,1-5,10): decimal constant (0x0) <overflow>
(6,1): <end of file>
]])

-- 64
test_lexer([[
0000000000000000000
0000000000000000001
9223372036854775807
9223372036854775808
]], [[
(1,1-1,19): decimal constant (0x0)
(2,1-2,19): decimal constant (0x1)
(3,1-3,19): decimal constant (0xffffffff) <overflow>
(4,1-4,19): decimal constant (0x0) <overflow>
(5,1): <end of file>
]])

test_lexer([[
1a
3z
4b
5o
6x
]], [[
(1,1-1,2): invalid decimal constant
(2,1-2,2): invalid decimal constant
(3,1-3,2): invalid decimal constant
(4,1-4,2): invalid decimal constant
(5,1-5,2): invalid decimal constant
(6,1): <end of file>
]])

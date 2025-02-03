
test_lexer([[
0o
]], [[
(1,1-1,2): invalid octal constant
(2,1): <end of file>
]])

test_lexer([[
0o0
0o1
0o00
0o07
0o70
0o77
]], [[
(1,1-1,3): octal constant (0x0)
(2,1-2,3): octal constant (0x1)
(3,1-3,4): octal constant (0x0)
(4,1-4,4): octal constant (0x7)
(5,1-5,4): octal constant (0x38)
(6,1-6,4): octal constant (0x3f)
(7,1): <end of file>
]])

-- 16
test_lexer([[
0o000000
0o000001
0o100001
]], [[
(1,1-1,8): octal constant (0x0)
(2,1-2,8): octal constant (0x1)
(3,1-3,8): octal constant (0x8001)
(4,1): <end of file>
]])

-- 32
test_lexer([[
0o00000000000
0o00000000001
0o20000000001
0o37777777777
0o200000000000
]], [[
(1,1-1,13): octal constant (0x0)
(2,1-2,13): octal constant (0x1)
(3,1-3,13): octal constant (0x80000001)
(4,1-4,13): octal constant (0xffffffff)
(5,1-5,14): octal constant (0x0) <overflow>
(6,1): <end of file>
]])

-- 64
test_lexer([[
0o0000000000000000000000
0o0000000000000000000001
0o1000000000000000000001
]], [[
(1,1-1,24): octal constant (0x0)
(2,1-2,24): octal constant (0x1)
(3,1-3,24): octal constant (0x1) <overflow>
(4,1): <end of file>
]])

test_lexer([[
0o8
0o9
0oa
0ob
0oc
0od
0oe
0of
0o319
0o3f3
]], [[
(1,1-1,3): invalid octal constant
(2,1-2,3): invalid octal constant
(3,1-3,3): invalid octal constant
(4,1-4,3): invalid octal constant
(5,1-5,3): invalid octal constant
(6,1-6,3): invalid octal constant
(7,1-7,3): invalid octal constant
(8,1-8,3): invalid octal constant
(9,1-9,5): invalid octal constant
(10,1-10,5): invalid octal constant
(11,1): <end of file>
]])

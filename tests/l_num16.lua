
test_lexer([[
0x
0x?
]], [[
(1,1-1,2): invalid hexadecimal constant
(2,1-2,2): invalid hexadecimal constant
(2,3-2,3): '?'
(3,1): <end of file>
]])

test_lexer([[
0x0
0x1
0x2
0x3
0x4?
0x5
0x6
0x7
0x8
0x9%
0xa
0xb
0xc
0xd
0xe
0xf
0x00
0x10-
0x01
]], [[
(1,1-1,3): hexadecimal constant (0x0)
(2,1-2,3): hexadecimal constant (0x1)
(3,1-3,3): hexadecimal constant (0x2)
(4,1-4,3): hexadecimal constant (0x3)
(5,1-5,3): hexadecimal constant (0x4)
(5,4-5,4): '?'
(6,1-6,3): hexadecimal constant (0x5)
(7,1-7,3): hexadecimal constant (0x6)
(8,1-8,3): hexadecimal constant (0x7)
(9,1-9,3): hexadecimal constant (0x8)
(10,1-10,3): hexadecimal constant (0x9)
(10,4-10,4): '%'
(11,1-11,3): hexadecimal constant (0xa)
(12,1-12,3): hexadecimal constant (0xb)
(13,1-13,3): hexadecimal constant (0xc)
(14,1-14,3): hexadecimal constant (0xd)
(15,1-15,3): hexadecimal constant (0xe)
(16,1-16,3): hexadecimal constant (0xf)
(17,1-17,4): hexadecimal constant (0x0)
(18,1-18,4): hexadecimal constant (0x10)
(18,5-18,5): '-'
(19,1-19,4): hexadecimal constant (0x1)
(20,1): <end of file>
]])

-- 16
test_lexer([[
0x0000
0x0001
0x8001+
0xffff
]], [[
(1,1-1,6): hexadecimal constant (0x0)
(2,1-2,6): hexadecimal constant (0x1)
(3,1-3,6): hexadecimal constant (0x8001)
(3,7-3,7): '+'
(4,1-4,6): hexadecimal constant (0xffff)
(5,1): <end of file>
]])

-- 32
test_lexer([[
0x00000000
0x00000001
0x80000001
0xffffffff
0x100000000
]], [[
(1,1-1,10): hexadecimal constant (0x0)
(2,1-2,10): hexadecimal constant (0x1)
(3,1-3,10): hexadecimal constant (0x80000001)
(4,1-4,10): hexadecimal constant (0xffffffff)
(5,1-5,11): hexadecimal constant (0x0) <overflow>
(6,1): <end of file>
]])

-- 64
test_lexer([[
0x0000000000000000
0x0000000000000001!
0x10000000000000001
]], [[
(1,1-1,18): hexadecimal constant (0x0)
(2,1-2,18): hexadecimal constant (0x1)
(2,19-2,19): '!'
(3,1-3,19): hexadecimal constant (0x1) <overflow>
(4,1): <end of file>
]])

test_lexer([[
0xg
0xh
0xi
0xj
0xk
0xl
0xm
0xn
0xo
0xp
0xq
0xr
0xs
0xt
0x4u4
0x1fg
0x8hh>
]], [[
(1,1-1,3): invalid hexadecimal constant
(2,1-2,3): invalid hexadecimal constant
(3,1-3,3): invalid hexadecimal constant
(4,1-4,3): invalid hexadecimal constant
(5,1-5,3): invalid hexadecimal constant
(6,1-6,3): invalid hexadecimal constant
(7,1-7,3): invalid hexadecimal constant
(8,1-8,3): invalid hexadecimal constant
(9,1-9,3): invalid hexadecimal constant
(10,1-10,3): invalid hexadecimal constant
(11,1-11,3): invalid hexadecimal constant
(12,1-12,3): invalid hexadecimal constant
(13,1-13,3): invalid hexadecimal constant
(14,1-14,3): invalid hexadecimal constant
(15,1-15,5): invalid hexadecimal constant
(16,1-16,5): invalid hexadecimal constant
(17,1-17,5): invalid hexadecimal constant
(17,6-17,6): '>'
(18,1): <end of file>
]])

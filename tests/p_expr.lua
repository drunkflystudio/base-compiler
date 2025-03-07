
test_parser_expr('null', [[
exprNull loc:(2,1-2,4)
result value:null
]])

test_parser_expr('false', [[
exprFalse loc:(2,1-2,5)
result value:false
]])

test_parser_expr('true', [[
exprTrue loc:(2,1-2,4)
result value:true
]])

test_parser_expr('Q', [[
exprIdentifier loc:(2,1-2,1) name:"Q"
result value:Q
]])

test_parser_expr('4', [[
exprInteger loc:(2,1-2,1) value:0x4
result value:0x4
]])

test_parser_expr('"Hello, world!"', [[
exprString loc:(2,1-2,15) text:"Hello, world!"
result value:"Hello, world!"
]])

test_parser_expr('(4)', [[
exprInteger loc:(2,2-2,2) value:0x4
exprParentheses loc:(2,1-2,3) operand:0x4
result value:(0x4)
]])

test_parser_expr('arr[index]', [[
exprIdentifier loc:(2,1-2,3) name:"arr"
exprIdentifier loc:(2,5-2,9) name:"index"
exprSubscript loc:(2,1-2,10) arr:arr idx:index
result value:arr[index]
]])

test_parser_expr('struc.member', [[
exprIdentifier loc:(2,1-2,5) name:"struc"
exprMember loc:(2,1-2,12) target:struc nameLoc:(2,7-2,12) name:"member"
result value:struc.member
]])

test_parser_expr('++i', [[
exprIdentifier loc:(2,3-2,3) name:"i"
exprPrefixIncr loc:(2,1-2,3) operand:i
result value:++i
]])

test_parser_expr('--i', [[
exprIdentifier loc:(2,3-2,3) name:"i"
exprPrefixDecr loc:(2,1-2,3) operand:i
result value:--i
]])

test_parser_expr('i++', [[
exprIdentifier loc:(2,1-2,1) name:"i"
exprPostfixIncr loc:(2,1-2,3) operand:i
result value:i++
]])

test_parser_expr('i--', [[
exprIdentifier loc:(2,1-2,1) name:"i"
exprPostfixDecr loc:(2,1-2,3) operand:i
result value:i--
]])

test_parser_expr('&i', [[
exprIdentifier loc:(2,2-2,2) name:"i"
exprTakeAddress loc:(2,1-2,2) operand:i
result value:&i
]])

test_parser_expr('*i', [[
exprIdentifier loc:(2,2-2,2) name:"i"
exprDeref loc:(2,1-2,2) operand:i
result value:*i
]])

test_parser_expr('+i', [[
exprIdentifier loc:(2,2-2,2) name:"i"
exprUnaryPlus loc:(2,1-2,2) operand:i
result value:+i
]])

test_parser_expr('-i', [[
exprIdentifier loc:(2,2-2,2) name:"i"
exprUnaryMinus loc:(2,1-2,2) operand:i
result value:-i
]])

test_parser_expr('~i', [[
exprIdentifier loc:(2,2-2,2) name:"i"
exprBitwiseNot loc:(2,1-2,2) operand:i
result value:~i
]])

test_parser_expr('!i', [[
exprIdentifier loc:(2,2-2,2) name:"i"
exprLogicNot loc:(2,1-2,2) operand:i
result value:!i
]])

test_parser_expr('sizeof(i)', [[
exprIdentifier loc:(2,8-2,8) name:"i"
exprSizeOf loc:(2,1-2,9) operand:i
result value:sizeof(i)
]])

test_parser_expr('a*b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,3-2,3) name:"b"
exprMultiply loc:(2,1-2,3) op1:a op2:b
result value:a*b
]])

test_parser_expr('a/b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,3-2,3) name:"b"
exprDivide loc:(2,1-2,3) op1:a op2:b
result value:a/b
]])

test_parser_expr('a%b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,3-2,3) name:"b"
exprModulo loc:(2,1-2,3) op1:a op2:b
result value:a%b
]])

test_parser_expr('a+b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,3-2,3) name:"b"
exprAdd loc:(2,1-2,3) op1:a op2:b
result value:a+b
]])

test_parser_expr('a-b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,3-2,3) name:"b"
exprSubtract loc:(2,1-2,3) op1:a op2:b
result value:a-b
]])

test_parser_expr('a<<b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,4-2,4) name:"b"
exprShiftLeft loc:(2,1-2,4) op1:a op2:b
result value:a<<b
]])

test_parser_expr('a>>b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,4-2,4) name:"b"
exprShiftRight loc:(2,1-2,4) op1:a op2:b
result value:a>>b
]])

test_parser_expr('a<b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,3-2,3) name:"b"
exprLess loc:(2,1-2,3) op1:a op2:b
result value:a<b
]])

test_parser_expr('a>b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,3-2,3) name:"b"
exprGreater loc:(2,1-2,3) op1:a op2:b
result value:a>b
]])

test_parser_expr('a<=b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,4-2,4) name:"b"
exprLessEq loc:(2,1-2,4) op1:a op2:b
result value:a<=b
]])

test_parser_expr('a>=b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,4-2,4) name:"b"
exprGreaterEq loc:(2,1-2,4) op1:a op2:b
result value:a>=b
]])

test_parser_expr('a==b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,4-2,4) name:"b"
exprEqual loc:(2,1-2,4) op1:a op2:b
result value:a==b
]])

test_parser_expr('a!=b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,4-2,4) name:"b"
exprNotEqual loc:(2,1-2,4) op1:a op2:b
result value:a!=b
]])

test_parser_expr('a&b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,3-2,3) name:"b"
exprBitwiseAnd loc:(2,1-2,3) op1:a op2:b
result value:a&b
]])

test_parser_expr('a^b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,3-2,3) name:"b"
exprBitwiseXor loc:(2,1-2,3) op1:a op2:b
result value:a^b
]])

test_parser_expr('a|b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,3-2,3) name:"b"
exprBitwiseOr loc:(2,1-2,3) op1:a op2:b
result value:a|b
]])

test_parser_expr('a&&b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,4-2,4) name:"b"
exprLogicAnd loc:(2,1-2,4) op1:a op2:b
result value:a&&b
]])

test_parser_expr('a||b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,4-2,4) name:"b"
exprLogicOr loc:(2,1-2,4) op1:a op2:b
result value:a||b
]])

test_parser_expr('a?b:c', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,3-2,3) name:"b"
exprIdentifier loc:(2,5-2,5) name:"c"
exprTernary loc:(2,1-2,5) cond:a trueValue:b falseValue:c
result value:a?b:c
]])

test_parser_expr('a=b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,3-2,3) name:"b"
exprAssign loc:(2,1-2,3) op1:a op2:b
result value:a=b
]])

test_parser_expr('a+=b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,4-2,4) name:"b"
exprAssignAdd loc:(2,1-2,4) op1:a op2:b
result value:a+=b
]])

test_parser_expr('a-=b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,4-2,4) name:"b"
exprAssignSub loc:(2,1-2,4) op1:a op2:b
result value:a-=b
]])

test_parser_expr('a*=b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,4-2,4) name:"b"
exprAssignMul loc:(2,1-2,4) op1:a op2:b
result value:a*=b
]])

test_parser_expr('a/=b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,4-2,4) name:"b"
exprAssignDiv loc:(2,1-2,4) op1:a op2:b
result value:a/=b
]])

test_parser_expr('a%=b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,4-2,4) name:"b"
exprAssignMod loc:(2,1-2,4) op1:a op2:b
result value:a%=b
]])

test_parser_expr('a&=b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,4-2,4) name:"b"
exprAssignAnd loc:(2,1-2,4) op1:a op2:b
result value:a&=b
]])

test_parser_expr('a|=b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,4-2,4) name:"b"
exprAssignOr loc:(2,1-2,4) op1:a op2:b
result value:a|=b
]])

test_parser_expr('a^=b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,4-2,4) name:"b"
exprAssignXor loc:(2,1-2,4) op1:a op2:b
result value:a^=b
]])

test_parser_expr('a<<=b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,5-2,5) name:"b"
exprAssignShl loc:(2,1-2,5) op1:a op2:b
result value:a<<=b
]])

test_parser_expr('a>>=b', [[
exprIdentifier loc:(2,1-2,1) name:"a"
exprIdentifier loc:(2,5-2,5) name:"b"
exprAssignShr loc:(2,1-2,5) op1:a op2:b
result value:a>>=b
]])

test_parser_expr('[obj main]', [[
exprIdentifier loc:(2,2-2,4) name:"obj"
exprMethodCallBegin callee:obj
 exprMethodSimple loc:(2,6-2,9) name:"main"
exprMethodCallEnd loc:(2,1-2,10)
result value:[obj main]
]])

test_parser_expr('[obj arg:x]', [[
exprIdentifier loc:(2,2-2,4) name:"obj"
exprMethodCallBegin callee:obj
 exprIdentifier loc:(2,10-2,10) name:"x"
 exprMethodArg loc:(2,6-2,9) name:"arg" value:x
exprMethodCallEnd loc:(2,1-2,11)
result value:[obj arg:x]
]])

test_parser_expr('[obj firstArg:4+8 secondArg:38-x*4]', [[
exprIdentifier loc:(2,2-2,4) name:"obj"
exprMethodCallBegin callee:obj
 exprInteger loc:(2,15-2,15) value:0x4
 exprInteger loc:(2,17-2,17) value:0x8
 exprAdd loc:(2,15-2,17) op1:0x4 op2:0x8
 exprMethodArg loc:(2,6-2,14) name:"firstArg" value:0x4+0x8
 exprInteger loc:(2,29-2,30) value:0x26
 exprIdentifier loc:(2,32-2,32) name:"x"
 exprInteger loc:(2,34-2,34) value:0x4
 exprMultiply loc:(2,32-2,34) op1:x op2:0x4
 exprSubtract loc:(2,29-2,34) op1:0x26 op2:x*0x4
 exprMethodArg loc:(2,19-2,28) name:"secondArg" value:0x26-x*0x4
exprMethodCallEnd loc:(2,1-2,35)
result value:[obj firstArg:0x4+0x8 secondArg:0x26-x*0x4]
]])

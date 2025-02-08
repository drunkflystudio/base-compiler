
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


test_parser_expr([[
null
]], [[
exprNull loc:(2,1-2,4)
result value:null
]])

test_parser_expr([[
false
]], [[
exprFalse loc:(2,1-2,5)
result value:false
]])

test_parser_expr([[
true
]], [[
exprTrue loc:(2,1-2,4)
result value:true
]])

test_parser_expr([[
Q
]], [[
exprIdentifier loc:(2,1-2,1) name:"Q"
result value:Q
]])

test_parser_expr([[
4
]], [[
exprInteger loc:(2,1-2,1) value:0x4
result value:0x4
]])

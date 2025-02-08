
test_parser_stmt(';', [[
stmtEmpty loc:(2,1-2,1)
]])

test_parser_stmt('4;', [[
stmtExpr loc:(2,1-2,2) expr:0x4
]])

test_parser_stmt('4', [[
(3,1-3,1): unexpected token: '}'
]])

test_parser_stmt('label:', [[
stmtLabel loc:(2,1-2,6) name:"label"
]])

test_parser_stmt('label:;', [[
stmtLabel loc:(2,1-2,6) name:"label"
stmtEmpty loc:(2,7-2,7)
]])

test_parser_stmt('goto label;', [[
stmtGoto loc:(2,1-2,11) nameLoc:(2,6-2,10) name:"label"
]])

test_parser_stmt('break;', [[
stmtBreak loc:(2,1-2,6)
]])

test_parser_stmt('break 4;', [[
(2,7-2,7): unexpected token: decimal constant
]])

test_parser_stmt('continue;', [[
stmtContinue loc:(2,1-2,9)
]])

test_parser_stmt('continue 4;', [[
(2,10-2,10): unexpected token: decimal constant
]])

test_parser_stmt('delete;', [[
(2,7-2,7): unexpected token: ';'
]])

test_parser_stmt('delete null;', [[
stmtDelete loc:(2,1-2,12) expr:null
]])

test_parser_stmt('throw;', [[
stmtThrow loc:(2,1-2,6) optionalExpr:<none>
]])

test_parser_stmt('throw null;', [[
stmtThrow loc:(2,1-2,11) optionalExpr:null
]])

test_parser_stmt('{}', [[
stmtCompoundBegin loc:(2,1-2,1)
stmtCompoundEnd loc:(2,2-2,2)
]])

test_parser_stmt('{;}', [[
stmtCompoundBegin loc:(2,1-2,1)
stmtEmpty loc:(2,2-2,2)
stmtCompoundEnd loc:(2,3-2,3)
]])

test_parser_stmt('{;;}', [[
stmtCompoundBegin loc:(2,1-2,1)
stmtEmpty loc:(2,2-2,2)
stmtEmpty loc:(2,3-2,3)
stmtCompoundEnd loc:(2,4-2,4)
]])

test_parser_stmt([[
{
    break;
    continue;
}
]], [[
stmtCompoundBegin loc:(2,1-2,1)
stmtBreak loc:(3,5-3,10)
stmtContinue loc:(4,5-4,13)
stmtCompoundEnd loc:(5,1-5,1)
stmtCompoundEnd loc:(7,1-7,1)
]])

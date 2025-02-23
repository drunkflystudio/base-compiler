
test_parser_stmt(';', [[
stmtEmpty loc:(2,1-2,1)
]])

test_parser_stmt('4;', [[
stmtExpr loc:(2,1-2,2) e:0x4
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
stmtDelete loc:(2,1-2,12) e:null
]])

test_parser_stmt('throw;', [[
stmtThrow loc:(2,1-2,6) optionalExpr:<none>
]])

test_parser_stmt('throw null;', [[
stmtThrow loc:(2,1-2,11) optionalExpr:null
]])

test_parser_stmt('return;', [[
stmtReturn loc:(2,1-2,7) optionalExpr:<none>
]])

test_parser_stmt('return null;', [[
stmtReturn loc:(2,1-2,12) optionalExpr:null
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
]])

test_parser_stmt([[
if (a)
    delete a;
]], [[
stmtIfBegin loc:(2,1-2,2)
stmtIfThen loc:(2,6-2,6) e:a
stmtDelete loc:(3,5-3,13) e:a
stmtIfEnd
]])

test_parser_stmt([[
if (a)
    delete a;
else
    goto x;
]], [[
stmtIfBegin loc:(2,1-2,2)
stmtIfThen loc:(2,6-2,6) e:a
stmtDelete loc:(3,5-3,13) e:a
stmtIfElse loc:(4,1-4,4)
stmtGoto loc:(5,5-5,11) nameLoc:(5,10-5,10) name:"x"
stmtIfEnd
]])

test_parser_stmt([[
if (a)
    break;
else if (b)
    goto y;
else
    goto w;
]], [[
stmtIfBegin loc:(2,1-2,2)
stmtIfThen loc:(2,6-2,6) e:a
stmtBreak loc:(3,5-3,10)
stmtIfElse loc:(4,1-4,4)
stmtIfBegin loc:(4,6-4,7)
stmtIfThen loc:(4,11-4,11) e:b
stmtGoto loc:(5,5-5,11) nameLoc:(5,10-5,10) name:"y"
stmtIfElse loc:(6,1-6,4)
stmtGoto loc:(7,5-7,11) nameLoc:(7,10-7,10) name:"w"
stmtIfEnd
stmtIfEnd
]])

test_parser_stmt([[
if (a) {}
if (b) {}
]], [[
stmtIfBegin loc:(2,1-2,2)
stmtIfThen loc:(2,6-2,6) e:a
stmtCompoundBegin loc:(2,8-2,8)
stmtCompoundEnd loc:(2,9-2,9)
stmtIfEnd
stmtIfBegin loc:(3,1-3,2)
stmtIfThen loc:(3,6-3,6) e:b
stmtCompoundBegin loc:(3,8-3,8)
stmtCompoundEnd loc:(3,9-3,9)
stmtIfEnd
]])

test_parser_stmt([[
if (a) if (b) break; else continue; else goto q;
]], [[
stmtIfBegin loc:(2,1-2,2)
stmtIfThen loc:(2,6-2,6) e:a
stmtIfBegin loc:(2,8-2,9)
stmtIfThen loc:(2,13-2,13) e:b
stmtBreak loc:(2,15-2,20)
stmtIfElse loc:(2,22-2,25)
stmtContinue loc:(2,27-2,35)
stmtIfEnd
stmtIfElse loc:(2,37-2,40)
stmtGoto loc:(2,42-2,48) nameLoc:(2,47-2,47) name:"q"
stmtIfEnd
]])

test_parser_stmt([[
if (a);
]], [[
stmtIfBegin loc:(2,1-2,2)
stmtIfThen loc:(2,6-2,6) e:a
stmtEmpty loc:(2,7-2,7)
stmtIfEnd
]])

test_parser_stmt([[
while (a);
]], [[
stmtWhileBegin loc:(2,1-2,5)
stmtWhileDo loc:(2,9-2,9) e:a
stmtEmpty loc:(2,10-2,10)
stmtWhileEnd
]])

test_parser_stmt([[
while (a) break;
]], [[
stmtWhileBegin loc:(2,1-2,5)
stmtWhileDo loc:(2,9-2,9) e:a
stmtBreak loc:(2,11-2,16)
stmtWhileEnd
]])

test_parser_stmt([[
while (a) {}
]], [[
stmtWhileBegin loc:(2,1-2,5)
stmtWhileDo loc:(2,9-2,9) e:a
stmtCompoundBegin loc:(2,11-2,11)
stmtCompoundEnd loc:(2,12-2,12)
stmtWhileEnd
]])

test_parser_stmt([[
do ; while(a);
]], [[
stmtDoBegin loc:(2,1-2,2)
stmtEmpty loc:(2,4-2,4)
stmtDoEnd loc:(2,6-2,14) e:a
]])

test_parser_stmt([[
do break; while(a);
]], [[
stmtDoBegin loc:(2,1-2,2)
stmtBreak loc:(2,4-2,9)
stmtDoEnd loc:(2,11-2,19) e:a
]])

test_parser_stmt([[
do {} while(a);
]], [[
stmtDoBegin loc:(2,1-2,2)
stmtDoEnd loc:(2,7-2,15) e:a
]])

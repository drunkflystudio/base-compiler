
test_lexer([[]], [[
(1,1): <end of file>
]])

test_lexer('\n', [[
(2,1): <end of file>
]])

test_lexer('\n\n', [[
(3,1): <end of file>
]])

test_lexer('\r', [[
(2,1): <end of file>
]])

test_lexer('\r\r', [[
(3,1): <end of file>
]])

test_lexer('\r\n', [[
(2,1): <end of file>
]])

test_lexer('\r\r\n', [[
(3,1): <end of file>
]])

test_lexer('\n\r', [[
(2,1): <end of file>
]])

test_lexer('\n\r\r', [[
(3,1): <end of file>
]])

test_lexer('/*', [[
(1,1-1,2): multi-line comment
(1,3): unterminated comment
(1,3): <end of file>
]])

test_lexer('/**/', [[
(1,1-1,4): multi-line comment
(1,5): <end of file>
]])

test_lexer('/***/', [[
(1,1-1,5): multi-line comment
(1,6): <end of file>
]])

test_lexer('/*/*', [[
(1,1-1,4): multi-line comment
(1,5): unterminated comment
(1,5): <end of file>
]])

test_lexer([[/*
*/ ]], [[
(1,1-1,2): multi-line comment
(2,1-2,2): multi-line comment
(2,4): <end of file>
]])

test_lexer('/**/*/', [[
(1,1-1,4): multi-line comment
(1,5-1,5): '*'
(1,6-1,6): '/'
(1,7): <end of file>
]])

test_lexer('//', [[
(1,1-1,2): single line comment
(1,3): <end of file>
]])

test_lexer('//\n', [[
(1,1-1,2): single line comment
(2,1): <end of file>
]])

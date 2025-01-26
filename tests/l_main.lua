
test_lexer([[]], [[
(1,0-1,0): file start
(1,1-1,1): <end of file>
]])

test_lexer('\n', [[
(1,0-1,0): file start
(2,1-2,1): <end of file>
]])

test_lexer('\r', [[
(1,0-1,0): file start
(2,1-2,1): <end of file>
]])

test_lexer('\r\n', [[
(1,0-1,0): file start
(2,1-2,1): <end of file>
]])

test_lexer('\n\r', [[
(1,0-1,0): file start
(3,1-3,1): <end of file>
]])

test_lexer('/*', [[
[error] (1,1-1,2): unterminated comment.
(1,0-1,0): file start
(1,1-1,2): multi-line comment
(1,3-1,3): <end of file>
]])

test_lexer('/**/', [[
(1,0-1,0): file start
(1,1-1,4): multi-line comment
(1,5-1,5): <end of file>
]])

test_lexer('/***/', [[
(1,0-1,0): file start
(1,1-1,5): multi-line comment
(1,6-1,6): <end of file>
]])

test_lexer('/*/*', [[
[error] (1,1-1,4): unterminated comment.
(1,0-1,0): file start
(1,1-1,4): multi-line comment
(1,5-1,5): <end of file>
]])

test_lexer([[/*
*/ ]], [[
(1,0-1,0): file start
(1,1-2,2): multi-line comment
(2,4-2,4): <end of file>
]])

test_lexer('/**/*/', [[
(1,0-1,0): file start
(1,1-1,4): multi-line comment
(1,5-1,5): '*'
(1,6-1,6): '/'
(1,7-1,7): <end of file>
]])

test_lexer('//', [[
(1,0-1,0): file start
(1,1-1,2): single line comment
(1,3-1,3): <end of file>
]])

test_lexer('//\n', [[
(1,0-1,0): file start
(1,1-1,2): single line comment
(2,1-2,1): <end of file>
]])

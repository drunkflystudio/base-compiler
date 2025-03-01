
test_lexer([[""]], [[
(1,1-1,2): string literal ("")
(1,3): <end of file>
]])

test_lexer([["\"]], [[
(1,1-1,3): unterminated string literal (""")
(1,4): <end of file>
]])

test_lexer([["""]], [[
(1,1-1,2): string literal ("")
(1,3-1,3): unterminated string literal ("")
(1,4): <end of file>
]])

test_lexer([[""""]], [[
(1,1-1,2): string literal ("")
(1,3-1,4): string literal ("")
(1,5): <end of file>
]])

test_lexer([["\\"]], [[
(1,1-1,4): string literal ("\")
(1,5): <end of file>
]])

test_lexer([["\\\"]], [[
(1,1-1,5): unterminated string literal ("\"")
(1,6): <end of file>
]])

test_lexer([["\\\\"]], [[
(1,1-1,6): string literal ("\\")
(1,7): <end of file>
]])

test_lexer([["\""]], [[
(1,1-1,4): string literal (""")
(1,5): <end of file>
]])

test_lexer([["
"]], [[
(1,1-1,1): unterminated string literal ("")
(2,1-2,1): unterminated string literal ("")
(2,2): <end of file>
]])

test_lexer([["a"
]], [[
(1,1-1,3): string literal ("a")
(2,1): <end of file>
]])

test_lexer([["\a"]], [[
(1,1-1,4): string literal ("#7")
(1,5): <end of file>
]])

test_lexer([["\b"]], [[
(1,1-1,4): string literal ("#8")
(1,5): <end of file>
]])

test_lexer([["\e"]], [[
(1,1-1,4): string literal ("#27")
(1,5): <end of file>
]])

test_lexer([["\f"]], [[
(1,1-1,4): string literal ("#12")
(1,5): <end of file>
]])

test_lexer([["\n"]], [[
(1,1-1,4): string literal ("#10")
(1,5): <end of file>
]])

test_lexer([["\r"]], [[
(1,1-1,4): string literal ("#13")
(1,5): <end of file>
]])

test_lexer([["\v"]], [[
(1,1-1,4): string literal ("#11")
(1,5): <end of file>
]])

test_lexer([["\?"]], [[
(1,1-1,4): string literal ("\?")
(1,5): <end of file>
]])

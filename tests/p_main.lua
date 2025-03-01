
test_parser_global([[
]], [[
translationUnitBegin
translationUnitEnd
]])

test_parser_global([[#]], [[
translationUnitBegin
(1,1-1,1): invalid symbol: '#'.
translationUnitEnd
]])

test_parser_global([[1a]], [[
translationUnitBegin
(1,1-1,2): invalid decimal constant: 1a.
translationUnitEnd
]])

test_parser_global([[0b12]], [[
translationUnitBegin
(1,1-1,4): invalid binary constant: 0b12.
translationUnitEnd
]])

test_parser_global([[0o9]], [[
translationUnitBegin
(1,1-1,3): invalid octal constant: 0o9.
translationUnitEnd
]])

test_parser_global([[0x1g]], [[
translationUnitBegin
(1,1-1,4): invalid hexadecimal constant: 0x1g.
translationUnitEnd
]])

test_parser_global([["]], [[
translationUnitBegin
(1,1-1,1): parse error: unterminated string constant.
translationUnitEnd
]])

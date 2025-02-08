
test_parser_global([[
enum A {
}
]], [[
translationUnitBegin
 enumBegin loc:(1,1-1,4) nameLoc:(1,6-1,6) name:"A" isFlags:false
  enumMembersBegin loc:(1,8-1,8)
  enumMembersEnd loc:(2,1-2,1)
 enumEnd loc:(2,1-2,1)
translationUnitEnd
]])

test_parser_global([[
enum A {
};
]], [[
translationUnitBegin
 enumBegin loc:(1,1-1,4) nameLoc:(1,6-1,6) name:"A" isFlags:false
  enumMembersBegin loc:(1,8-1,8)
  enumMembersEnd loc:(2,1-2,1)
 enumEnd loc:(2,1-2,1)
(2,2-2,2): unexpected token: ';'
]])

test_parser_global([[
enum A : int {
}
]], [[
translationUnitBegin
 enumBegin loc:(1,1-1,4) nameLoc:(1,6-1,6) name:"A" isFlags:false
  enumType loc:(1,10-1,12) type:i16
  enumMembersBegin loc:(1,14-1,14)
  enumMembersEnd loc:(2,1-2,1)
 enumEnd loc:(2,1-2,1)
translationUnitEnd
]])

test_parser_global([[
enum A {
    F
}
]], [[
translationUnitBegin
 enumBegin loc:(1,1-1,4) nameLoc:(1,6-1,6) name:"A" isFlags:false
  enumMembersBegin loc:(1,8-1,8)
   enumMember loc:(2,5-2,5) name:"F" optionalValue:(null)
  enumMembersEnd loc:(3,1-3,1)
 enumEnd loc:(3,1-3,1)
translationUnitEnd
]])

test_parser_global([[
enum A {
    F,
}
]], [[
translationUnitBegin
 enumBegin loc:(1,1-1,4) nameLoc:(1,6-1,6) name:"A" isFlags:false
  enumMembersBegin loc:(1,8-1,8)
   enumMember loc:(2,5-2,5) name:"F" optionalValue:(null)
  enumMembersEnd loc:(3,1-3,1)
 enumEnd loc:(3,1-3,1)
translationUnitEnd
]])

test_parser_global([[
enum A {
    F
    G
}
]], [[
translationUnitBegin
 enumBegin loc:(1,1-1,4) nameLoc:(1,6-1,6) name:"A" isFlags:false
  enumMembersBegin loc:(1,8-1,8)
(3,5-3,5): unexpected token: identifier
]])

test_parser_global([[
enum A {
    F,
    G
}
]], [[
translationUnitBegin
 enumBegin loc:(1,1-1,4) nameLoc:(1,6-1,6) name:"A" isFlags:false
  enumMembersBegin loc:(1,8-1,8)
   enumMember loc:(2,5-2,5) name:"F" optionalValue:(null)
   enumMember loc:(3,5-3,5) name:"G" optionalValue:(null)
  enumMembersEnd loc:(4,1-4,1)
 enumEnd loc:(4,1-4,1)
translationUnitEnd
]])

test_parser_global([[
enum A {
    F,
    G,
}
]], [[
translationUnitBegin
 enumBegin loc:(1,1-1,4) nameLoc:(1,6-1,6) name:"A" isFlags:false
  enumMembersBegin loc:(1,8-1,8)
   enumMember loc:(2,5-2,5) name:"F" optionalValue:(null)
   enumMember loc:(3,5-3,5) name:"G" optionalValue:(null)
  enumMembersEnd loc:(4,1-4,1)
 enumEnd loc:(4,1-4,1)
translationUnitEnd
]])

test_parser_global([[
enum A {
    F = 4
}
]], [[
translationUnitBegin
 enumBegin loc:(1,1-1,4) nameLoc:(1,6-1,6) name:"A" isFlags:false
  enumMembersBegin loc:(1,8-1,8)
   enumMember loc:(2,5-2,5) name:"F" optionalValue:0x4
  enumMembersEnd loc:(3,1-3,1)
 enumEnd loc:(3,1-3,1)
translationUnitEnd
]])

test_parser_global([[
enum A {
    F = 4,
}
]], [[
translationUnitBegin
 enumBegin loc:(1,1-1,4) nameLoc:(1,6-1,6) name:"A" isFlags:false
  enumMembersBegin loc:(1,8-1,8)
   enumMember loc:(2,5-2,5) name:"F" optionalValue:0x4
  enumMembersEnd loc:(3,1-3,1)
 enumEnd loc:(3,1-3,1)
translationUnitEnd
]])

test_parser_global([[
enum A {
    F =
}
]], [[
translationUnitBegin
 enumBegin loc:(1,1-1,4) nameLoc:(1,6-1,6) name:"A" isFlags:false
  enumMembersBegin loc:(1,8-1,8)
(3,1-3,1): unexpected token: '}'
]])

test_parser_global([[
enum A {
    F = 4
    G
}
]], [[
translationUnitBegin
 enumBegin loc:(1,1-1,4) nameLoc:(1,6-1,6) name:"A" isFlags:false
  enumMembersBegin loc:(1,8-1,8)
   enumMember loc:(2,5-2,5) name:"F" optionalValue:0x4
(3,5-3,5): unexpected token: identifier
]])

test_parser_global([[
enum A {
    F = 4,
    G = 4
}
]], [[
translationUnitBegin
 enumBegin loc:(1,1-1,4) nameLoc:(1,6-1,6) name:"A" isFlags:false
  enumMembersBegin loc:(1,8-1,8)
   enumMember loc:(2,5-2,5) name:"F" optionalValue:0x4
   enumMember loc:(3,5-3,5) name:"G" optionalValue:0x4
  enumMembersEnd loc:(4,1-4,1)
 enumEnd loc:(4,1-4,1)
translationUnitEnd
]])

test_parser_global([[
enum A {
    F = 4,
    G = 4,
}
]], [[
translationUnitBegin
 enumBegin loc:(1,1-1,4) nameLoc:(1,6-1,6) name:"A" isFlags:false
  enumMembersBegin loc:(1,8-1,8)
   enumMember loc:(2,5-2,5) name:"F" optionalValue:0x4
   enumMember loc:(3,5-3,5) name:"G" optionalValue:0x4
  enumMembersEnd loc:(4,1-4,1)
 enumEnd loc:(4,1-4,1)
translationUnitEnd
]])

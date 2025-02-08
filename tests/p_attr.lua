
test_parser_full([[
@attr
enum A {
}
]], [[
translationUnitBegin
 attrListBegin
  attrBegin loc:(1,1-1,5) name:"attr"
  attrEnd
 attrListEnd
translationUnitEnd
]])

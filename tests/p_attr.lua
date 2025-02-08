
test_parser_attr([[
@attr
enum A {}
]], [[
attrListBegin
 attrBegin loc:(1,1-1,5) name:"attr"
 attrEnd
attrListEnd
enum nameLoc:(2,6-2,6)
]])


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

test_parser_attr([[
@attr
class A {}
]], [[
attrListBegin
 attrBegin loc:(1,1-1,5) name:"attr"
 attrEnd
attrListEnd
classBegin loc:(2,1-2,5) nameLoc:(2,7-2,7) name:"A" isFinal:false
]])

test_parser_attr([[
@attr
interface A {}
]], [[
attrListBegin
 attrBegin loc:(1,1-1,5) name:"attr"
 attrEnd
attrListEnd
classInterfaceBegin loc:(2,1-2,9) nameLoc:(2,11-2,11) name:"A"
]])

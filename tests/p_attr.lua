
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

test_parser_global([[
interface A {
@attr
(void) method1;
@attr(4)
(void) method1;
}
]], [[
translationUnitBegin
 classInterfaceBegin loc:(1,1-1,9) nameLoc:(1,11-1,11) name:"A"
  classMembersBegin loc:(1,13-1,13)
   attrListBegin
    attrBegin loc:(2,1-2,5) name:"attr"
    attrEnd
   attrListEnd
   classMethodBegin loc:(3,1-3,6) optionalVisLoc:<none> vis:public optionalStatic:<none> retLoc:(3,2-3,5) ret:void
    classMethodNameSimple loc:(3,8-3,14) name:"method1"
   classMethodNameEnd optionalFinal:<none> optionalOverride:<none>
   attrListBegin
    attrBegin loc:(4,1-4,5) name:"attr"
     attrParam optionalNameLoc:<none> optionalName:<none> value:0x4
    attrEnd
   attrListEnd
   classMethodBegin loc:(5,1-5,6) optionalVisLoc:<none> vis:public optionalStatic:<none> retLoc:(5,2-5,5) ret:void
    classMethodNameSimple loc:(5,8-5,14) name:"method1"
   classMethodNameEnd optionalFinal:<none> optionalOverride:<none>
  classMembersEnd loc:(6,1-6,1)
 classEnd loc:(6,1-6,1)
translationUnitEnd
]])


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
classBegin optionalVisLoc:<none> vis:private loc:(2,1-2,5) nameLoc:(2,7-2,7) name:"A" isExtern:false isFinal:false
]])

test_parser_attr([[
@attr
interface A {}
]], [[
attrListBegin
 attrBegin loc:(1,1-1,5) name:"attr"
 attrEnd
attrListEnd
classInterfaceBegin optionalVisLoc:<none> vis:private loc:(2,1-2,9) nameLoc:(2,11-2,11) name:"A"
]])

test_parser_global([[
interface A {
@attr
(void) method1;
@attr1
@attr2
(void) method2;
@attr(4)
(void) method3;
@attr1(4)
@attr2(x: 8)
(void) method4;
@attr1(4)
@attr3
(void) method5;
@attr3
@attr4(8)
(void) method6;
}
]], [[
translationUnitBegin
 classInterfaceBegin optionalVisLoc:<none> vis:private loc:(1,1-1,9) nameLoc:(1,11-1,11) name:"A"
  classMembersBegin loc:(1,13-1,13)
   attrListBegin
    attrBegin loc:(2,1-2,5) name:"attr"
    attrEnd
   attrListEnd
   classMethodBegin loc:(3,1-3,6) optionalVisLoc:<none> vis:public optionalStatic:<none> retLoc:(3,2-3,5) ret:void
    classMethodNameSimple loc:(3,8-3,14) name:"method1"
   classMethodNameEnd optionalFinal:<none> optionalOverride:<none>
   attrListBegin
    attrBegin loc:(4,1-4,6) name:"attr1"
    attrEnd
    attrBegin loc:(5,1-5,6) name:"attr2"
    attrEnd
   attrListEnd
   classMethodBegin loc:(6,1-6,6) optionalVisLoc:<none> vis:public optionalStatic:<none> retLoc:(6,2-6,5) ret:void
    classMethodNameSimple loc:(6,8-6,14) name:"method2"
   classMethodNameEnd optionalFinal:<none> optionalOverride:<none>
   attrListBegin
    attrBegin loc:(7,1-7,5) name:"attr"
     attrParam optionalNameLoc:<none> optionalName:<none> value:0x4
    attrEnd
   attrListEnd
   classMethodBegin loc:(8,1-8,6) optionalVisLoc:<none> vis:public optionalStatic:<none> retLoc:(8,2-8,5) ret:void
    classMethodNameSimple loc:(8,8-8,14) name:"method3"
   classMethodNameEnd optionalFinal:<none> optionalOverride:<none>
   attrListBegin
    attrBegin loc:(9,1-9,6) name:"attr1"
     attrParam optionalNameLoc:<none> optionalName:<none> value:0x4
    attrEnd
    attrBegin loc:(10,1-10,6) name:"attr2"
     attrParam optionalNameLoc:(10,8-10,8) optionalName:"x" value:0x8
    attrEnd
   attrListEnd
   classMethodBegin loc:(11,1-11,6) optionalVisLoc:<none> vis:public optionalStatic:<none> retLoc:(11,2-11,5) ret:void
    classMethodNameSimple loc:(11,8-11,14) name:"method4"
   classMethodNameEnd optionalFinal:<none> optionalOverride:<none>
   attrListBegin
    attrBegin loc:(12,1-12,6) name:"attr1"
     attrParam optionalNameLoc:<none> optionalName:<none> value:0x4
    attrEnd
    attrBegin loc:(13,1-13,6) name:"attr3"
    attrEnd
   attrListEnd
   classMethodBegin loc:(14,1-14,6) optionalVisLoc:<none> vis:public optionalStatic:<none> retLoc:(14,2-14,5) ret:void
    classMethodNameSimple loc:(14,8-14,14) name:"method5"
   classMethodNameEnd optionalFinal:<none> optionalOverride:<none>
   attrListBegin
    attrBegin loc:(15,1-15,6) name:"attr3"
    attrEnd
    attrBegin loc:(16,1-16,6) name:"attr4"
     attrParam optionalNameLoc:<none> optionalName:<none> value:0x8
    attrEnd
   attrListEnd
   classMethodBegin loc:(17,1-17,6) optionalVisLoc:<none> vis:public optionalStatic:<none> retLoc:(17,2-17,5) ret:void
    classMethodNameSimple loc:(17,8-17,14) name:"method6"
   classMethodNameEnd optionalFinal:<none> optionalOverride:<none>
  classMembersEnd loc:(18,1-18,1)
 classEnd loc:(18,1-18,1)
translationUnitEnd
]])

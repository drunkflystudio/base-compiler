
test_parser_global([[
interface A {
}
]], [[
translationUnitBegin
 classInterfaceBegin loc:(1,1-1,9) nameLoc:(1,11-1,11) name:"A"
  classMembersBegin loc:(1,13-1,13)
  classMembersEnd loc:(2,1-2,1)
 classEnd loc:(2,1-2,1)
translationUnitEnd
]])

test_parser_global([[
interface A final {
}
]], [[
translationUnitBegin
 classInterfaceBegin loc:(1,1-1,9) nameLoc:(1,11-1,11) name:"A"
(1,13-1,17): unexpected token: 'final'
]])

test_parser_global([[
interface A {
public var x;
}
]], [[
translationUnitBegin
 classInterfaceBegin loc:(1,1-1,9) nameLoc:(1,11-1,11) name:"A"
  classMembersBegin loc:(1,13-1,13)
(2,1-2,6): unexpected token: 'public'
]])

test_parser_global([[
interface A {
(void) method1;
}
]], [[
translationUnitBegin
 classInterfaceBegin loc:(1,1-1,9) nameLoc:(1,11-1,11) name:"A"
  classMembersBegin loc:(1,13-1,13)
   classMethodBegin loc:(2,1-2,6) optionalVisLoc:<none> vis:public optionalStatic:<none> retLoc:(2,2-2,5) ret:void
    classMethodNameSimple loc:(2,8-2,14) name:"method1"
   classMethodNameEnd optionalFinal:<none> optionalOverride:<none>
  classMembersEnd loc:(3,1-3,1)
 classEnd loc:(3,1-3,1)
translationUnitEnd
]])

test_parser_global([[
interface A {
public (void) method1;
}
]], [[
translationUnitBegin
 classInterfaceBegin loc:(1,1-1,9) nameLoc:(1,11-1,11) name:"A"
  classMembersBegin loc:(1,13-1,13)
(2,1-2,6): unexpected token: 'public'
]])

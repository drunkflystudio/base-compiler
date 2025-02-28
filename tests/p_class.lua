
test_parser_global([[
class A {
}
]], [[
translationUnitBegin
 classBegin loc:(1,1-1,5) nameLoc:(1,7-1,7) name:"A" isFinal:false
  classMembersBegin loc:(1,9-1,9)
  classMembersEnd loc:(2,1-2,1)
 classEnd loc:(2,1-2,1)
translationUnitEnd
]])

test_parser_global([[
class A final {
}
]], [[
translationUnitBegin
 classBegin loc:(1,1-1,5) nameLoc:(1,7-1,7) name:"A" isFinal:true
  classMembersBegin loc:(1,15-1,15)
  classMembersEnd loc:(2,1-2,1)
 classEnd loc:(2,1-2,1)
translationUnitEnd
]])

test_parser_global([[
class A {
}
class B : A {
}
class C final : A, B {
}
]], [[
translationUnitBegin
 classBegin loc:(1,1-1,5) nameLoc:(1,7-1,7) name:"A" isFinal:false
  classMembersBegin loc:(1,9-1,9)
  classMembersEnd loc:(2,1-2,1)
 classEnd loc:(2,1-2,1)
 classBegin loc:(3,1-3,5) nameLoc:(3,7-3,7) name:"B" isFinal:false
  classParent loc:(3,11-3,11) name:"A"
  classMembersBegin loc:(3,13-3,13)
  classMembersEnd loc:(4,1-4,1)
 classEnd loc:(4,1-4,1)
 classBegin loc:(5,1-5,5) nameLoc:(5,7-5,7) name:"C" isFinal:true
  classParent loc:(5,17-5,17) name:"A"
  classParent loc:(5,20-5,20) name:"B"
  classMembersBegin loc:(5,22-5,22)
  classMembersEnd loc:(6,1-6,1)
 classEnd loc:(6,1-6,1)
translationUnitEnd
]])

test_parser_global([[
class A {
}
class B : A, {
}
]], [[
translationUnitBegin
 classBegin loc:(1,1-1,5) nameLoc:(1,7-1,7) name:"A" isFinal:false
  classMembersBegin loc:(1,9-1,9)
  classMembersEnd loc:(2,1-2,1)
 classEnd loc:(2,1-2,1)
 classBegin loc:(3,1-3,5) nameLoc:(3,7-3,7) name:"B" isFinal:false
  classParent loc:(3,11-3,11) name:"A"
(3,14-3,14): unexpected token: '{'
]])

test_parser_global([[
class A {
public var field1;
public static var field2;
private var field3 = 4;
public static var field4 = 32;
public const field5;
protected static const field6;
public const field7 = 4;
public static const field8 = 32;
public var (u8) field9;
public static var (u16) field10;
private var (u8) field11 = 5;
public static var (u16) field12 = 10;
public const (u32) field13;
public static const (uintptr) field14;
public const (u32) field15 = 77;
public static const (uintptr) field16 = 2;
public var field17, field18 = 3, field19;
protected const field20, field21, field22 = 4;
public var field23, (u8) field24, field25;
public var (u16) field26, (u8) field27;
public const field28, (u8) field29, field30;
private const (u16) field31, (u8) field32;
}
]], [[
translationUnitBegin
 classBegin loc:(1,1-1,5) nameLoc:(1,7-1,7) name:"A" isFinal:false
  classMembersBegin loc:(1,9-1,9)
   classVariablesBegin loc:(2,8-2,10) visLoc:(2,1-2,6) vis:public optionalStatic:<none> isConst:false
    varBegin loc:(2,12-2,17) name:"field1"
    varEnd
   classVariablesEnd loc:(2,18-2,18)
   classVariablesBegin loc:(3,15-3,17) visLoc:(3,1-3,6) vis:public optionalStatic:(3,8-3,13) isConst:false
    varBegin loc:(3,19-3,24) name:"field2"
    varEnd
   classVariablesEnd loc:(3,25-3,25)
   classVariablesBegin loc:(4,9-4,11) visLoc:(4,1-4,7) vis:private optionalStatic:<none> isConst:false
    varBegin loc:(4,13-4,18) name:"field3"
     varInitializer value:0x4
    varEnd
   classVariablesEnd loc:(4,23-4,23)
   classVariablesBegin loc:(5,15-5,17) visLoc:(5,1-5,6) vis:public optionalStatic:(5,8-5,13) isConst:false
    varBegin loc:(5,19-5,24) name:"field4"
     varInitializer value:0x20
    varEnd
   classVariablesEnd loc:(5,30-5,30)
   classVariablesBegin loc:(6,8-6,12) visLoc:(6,1-6,6) vis:public optionalStatic:<none> isConst:true
    varBegin loc:(6,14-6,19) name:"field5"
    varEnd
   classVariablesEnd loc:(6,20-6,20)
   classVariablesBegin loc:(7,18-7,22) visLoc:(7,1-7,9) vis:protected optionalStatic:(7,11-7,16) isConst:true
    varBegin loc:(7,24-7,29) name:"field6"
    varEnd
   classVariablesEnd loc:(7,30-7,30)
   classVariablesBegin loc:(8,8-8,12) visLoc:(8,1-8,6) vis:public optionalStatic:<none> isConst:true
    varBegin loc:(8,14-8,19) name:"field7"
     varInitializer value:0x4
    varEnd
   classVariablesEnd loc:(8,24-8,24)
   classVariablesBegin loc:(9,15-9,19) visLoc:(9,1-9,6) vis:public optionalStatic:(9,8-9,13) isConst:true
    varBegin loc:(9,21-9,26) name:"field8"
     varInitializer value:0x20
    varEnd
   classVariablesEnd loc:(9,32-9,32)
   classVariablesBegin loc:(10,8-10,10) visLoc:(10,1-10,6) vis:public optionalStatic:<none> isConst:false
    varBegin loc:(10,17-10,22) name:"field9"
     varType loc:(10,13-10,14) type:u8
    varEnd
   classVariablesEnd loc:(10,23-10,23)
   classVariablesBegin loc:(11,15-11,17) visLoc:(11,1-11,6) vis:public optionalStatic:(11,8-11,13) isConst:false
    varBegin loc:(11,25-11,31) name:"field10"
     varType loc:(11,20-11,22) type:u16
    varEnd
   classVariablesEnd loc:(11,32-11,32)
   classVariablesBegin loc:(12,9-12,11) visLoc:(12,1-12,7) vis:private optionalStatic:<none> isConst:false
    varBegin loc:(12,18-12,24) name:"field11"
     varType loc:(12,14-12,15) type:u8
     varInitializer value:0x5
    varEnd
   classVariablesEnd loc:(12,29-12,29)
   classVariablesBegin loc:(13,15-13,17) visLoc:(13,1-13,6) vis:public optionalStatic:(13,8-13,13) isConst:false
    varBegin loc:(13,25-13,31) name:"field12"
     varType loc:(13,20-13,22) type:u16
     varInitializer value:0xa
    varEnd
   classVariablesEnd loc:(13,37-13,37)
   classVariablesBegin loc:(14,8-14,12) visLoc:(14,1-14,6) vis:public optionalStatic:<none> isConst:true
    varBegin loc:(14,20-14,26) name:"field13"
     varType loc:(14,15-14,17) type:u32
    varEnd
   classVariablesEnd loc:(14,27-14,27)
   classVariablesBegin loc:(15,15-15,19) visLoc:(15,1-15,6) vis:public optionalStatic:(15,8-15,13) isConst:true
    varBegin loc:(15,31-15,37) name:"field14"
     varType loc:(15,22-15,28) type:uintptr
    varEnd
   classVariablesEnd loc:(15,38-15,38)
   classVariablesBegin loc:(16,8-16,12) visLoc:(16,1-16,6) vis:public optionalStatic:<none> isConst:true
    varBegin loc:(16,20-16,26) name:"field15"
     varType loc:(16,15-16,17) type:u32
     varInitializer value:0x4d
    varEnd
   classVariablesEnd loc:(16,32-16,32)
   classVariablesBegin loc:(17,15-17,19) visLoc:(17,1-17,6) vis:public optionalStatic:(17,8-17,13) isConst:true
    varBegin loc:(17,31-17,37) name:"field16"
     varType loc:(17,22-17,28) type:uintptr
     varInitializer value:0x2
    varEnd
   classVariablesEnd loc:(17,42-17,42)
   classVariablesBegin loc:(18,8-18,10) visLoc:(18,1-18,6) vis:public optionalStatic:<none> isConst:false
    varBegin loc:(18,12-18,18) name:"field17"
    varEnd
    varBegin loc:(18,21-18,27) name:"field18"
     varInitializer value:0x3
    varEnd
    varBegin loc:(18,34-18,40) name:"field19"
    varEnd
   classVariablesEnd loc:(18,41-18,41)
   classVariablesBegin loc:(19,11-19,15) visLoc:(19,1-19,9) vis:protected optionalStatic:<none> isConst:true
    varBegin loc:(19,17-19,23) name:"field20"
    varEnd
    varBegin loc:(19,26-19,32) name:"field21"
    varEnd
    varBegin loc:(19,35-19,41) name:"field22"
     varInitializer value:0x4
    varEnd
   classVariablesEnd loc:(19,46-19,46)
   classVariablesBegin loc:(20,8-20,10) visLoc:(20,1-20,6) vis:public optionalStatic:<none> isConst:false
    varBegin loc:(20,12-20,18) name:"field23"
    varEnd
    varBegin loc:(20,26-20,32) name:"field24"
     varType loc:(20,22-20,23) type:u8
    varEnd
    varBegin loc:(20,35-20,41) name:"field25"
    varEnd
   classVariablesEnd loc:(20,42-20,42)
   classVariablesBegin loc:(21,8-21,10) visLoc:(21,1-21,6) vis:public optionalStatic:<none> isConst:false
    varBegin loc:(21,18-21,24) name:"field26"
     varType loc:(21,13-21,15) type:u16
    varEnd
    varBegin loc:(21,32-21,38) name:"field27"
     varType loc:(21,28-21,29) type:u8
    varEnd
   classVariablesEnd loc:(21,39-21,39)
   classVariablesBegin loc:(22,8-22,12) visLoc:(22,1-22,6) vis:public optionalStatic:<none> isConst:true
    varBegin loc:(22,14-22,20) name:"field28"
    varEnd
    varBegin loc:(22,28-22,34) name:"field29"
     varType loc:(22,24-22,25) type:u8
    varEnd
    varBegin loc:(22,37-22,43) name:"field30"
    varEnd
   classVariablesEnd loc:(22,44-22,44)
   classVariablesBegin loc:(23,9-23,13) visLoc:(23,1-23,7) vis:private optionalStatic:<none> isConst:true
    varBegin loc:(23,21-23,27) name:"field31"
     varType loc:(23,16-23,18) type:u16
    varEnd
    varBegin loc:(23,35-23,41) name:"field32"
     varType loc:(23,31-23,32) type:u8
    varEnd
   classVariablesEnd loc:(23,42-23,42)
  classMembersEnd loc:(24,1-24,1)
 classEnd loc:(24,1-24,1)
translationUnitEnd
]])

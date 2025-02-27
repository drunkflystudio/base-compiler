
test_parser_types([[
void
]], [[
typeVoid loc:(2,1-2,4)
result loc:(2,1-2,4) type:void
]])

test_parser_types([[
bool
]], [[
typeBool loc:(2,1-2,4)
result loc:(2,1-2,4) type:bool
]])

test_parser_types([[
bit
]], [[
typeBit loc:(2,1-2,3) optionalExpr:<none>
result loc:(2,1-2,3) type:bit
]])

test_parser_types([[
bit[4]
]], [[
typeBit loc:(2,1-2,3) optionalExpr:0x4
result loc:(2,1-2,6) type:bit[0x4]
]])

test_parser_types([[
byte
]], [[
typeUInt8 loc:(2,1-2,4)
result loc:(2,1-2,4) type:u8
]])

test_parser_types([[
sbyte
]], [[
typeInt8 loc:(2,1-2,5)
result loc:(2,1-2,5) type:i8
]])

test_parser_types([[
word
]], [[
typeUInt16 loc:(2,1-2,4)
result loc:(2,1-2,4) type:u16
]])

test_parser_types([[
dword
]], [[
typeUInt32 loc:(2,1-2,5)
result loc:(2,1-2,5) type:u32
]])

test_parser_types([[
int
]], [[
typeInt loc:(2,1-2,3)
result loc:(2,1-2,3) type:int
]])

test_parser_types([[
uint
]], [[
typeUInt loc:(2,1-2,4)
result loc:(2,1-2,4) type:uint
]])

test_parser_types([[
intptr
]], [[
typeIntPtr loc:(2,1-2,6)
result loc:(2,1-2,6) type:intptr
]])

test_parser_types([[
uintptr
]], [[
typeUIntPtr loc:(2,1-2,7)
result loc:(2,1-2,7) type:uintptr
]])

test_parser_types([[
i8
]], [[
typeInt8 loc:(2,1-2,2)
result loc:(2,1-2,2) type:i8
]])

test_parser_types([[
u8
]], [[
typeUInt8 loc:(2,1-2,2)
result loc:(2,1-2,2) type:u8
]])

test_parser_types([[
i16
]], [[
typeInt16 loc:(2,1-2,3)
result loc:(2,1-2,3) type:i16
]])

test_parser_types([[
u16
]], [[
typeUInt16 loc:(2,1-2,3)
result loc:(2,1-2,3) type:u16
]])

test_parser_types([[
i32
]], [[
typeInt32 loc:(2,1-2,3)
result loc:(2,1-2,3) type:i32
]])

test_parser_types([[
u32
]], [[
typeUInt32 loc:(2,1-2,3)
result loc:(2,1-2,3) type:u32
]])

test_parser_types([[
object
]], [[
typeObject loc:(2,1-2,6)
result loc:(2,1-2,6) type:object
]])

test_parser_types([[
void*
]], [[
typeVoid loc:(2,1-2,4)
typePointer loc:(2,1-2,5) pointee:void
result loc:(2,1-2,5) type:void*
]])

test_parser_types([[
int*
]], [[
typeInt loc:(2,1-2,3)
typePointer loc:(2,1-2,4) pointee:int
result loc:(2,1-2,4) type:int*
]])

test_parser_types([[
void**
]], [[
typeVoid loc:(2,1-2,4)
typePointer loc:(2,1-2,5) pointee:void
typePointer loc:(2,1-2,6) pointee:void*
result loc:(2,1-2,6) type:void**
]])

test_parser_types([[
void*********
]], [[
typeVoid loc:(2,1-2,4)
typePointer loc:(2,1-2,5) pointee:void
typePointer loc:(2,1-2,6) pointee:void*
typePointer loc:(2,1-2,7) pointee:void**
typePointer loc:(2,1-2,8) pointee:void***
typePointer loc:(2,1-2,9) pointee:void****
typePointer loc:(2,1-2,10) pointee:void*****
typePointer loc:(2,1-2,11) pointee:void******
typePointer loc:(2,1-2,12) pointee:void*******
typePointer loc:(2,1-2,13) pointee:void********
result loc:(2,1-2,13) type:void*********
]])

test_parser_types([[
a
]], [[
typeIdentifier loc:(2,1-2,1) text:"a"
result loc:(2,1-2,1) type:a
]])

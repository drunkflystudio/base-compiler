
test_lexer([[]], [[
(1,1): <end of file>
]])

test_lexer('\n', [[
(2,1): <end of file>
]])

test_lexer('\n\n', [[
(3,1): <end of file>
]])

test_lexer('\r', [[
(2,1): <end of file>
]])

test_lexer('\r\r', [[
(3,1): <end of file>
]])

test_lexer('\r\n', [[
(2,1): <end of file>
]])

test_lexer('\r\r\n', [[
(3,1): <end of file>
]])

test_lexer('\n\r', [[
(2,1): <end of file>
]])

test_lexer('\n\r\r', [[
(3,1): <end of file>
]])

test_lexer('/*', [[
(1,1-1,2): multi-line comment
(1,3): unterminated comment
(1,3): <end of file>
]])

test_lexer('/**/', [[
(1,1-1,4): multi-line comment
(1,5): <end of file>
]])

test_lexer('/***/', [[
(1,1-1,5): multi-line comment
(1,6): <end of file>
]])

test_lexer('/*/*', [[
(1,1-1,4): multi-line comment
(1,5): unterminated comment
(1,5): <end of file>
]])

test_lexer([[/*
*/ ]], [[
(1,1-1,2): multi-line comment
(2,1-2,2): multi-line comment
(2,4): <end of file>
]])

test_lexer('/**/*/', [[
(1,1-1,4): multi-line comment
(1,5-1,5): '*'
(1,6-1,6): '/'
(1,7): <end of file>
]])

test_lexer('//', [[
(1,1-1,2): single line comment
(1,3): <end of file>
]])

test_lexer('//\n', [[
(1,1-1,2): single line comment
(2,1): <end of file>
]])

test_lexer('-1', [[
(1,1-1,1): '-'
(1,2-1,2): decimal constant (0x1)
(1,3): <end of file>
]])

test_lexer('-\n--\n---\n----\n-=\n--=\n-==\n--==\n', [[
(1,1-1,1): '-'
(2,1-2,2): '--'
(3,1-3,2): '--'
(3,3-3,3): '-'
(4,1-4,2): '--'
(4,3-4,4): '--'
(5,1-5,2): '-='
(6,1-6,2): '--'
(6,3-6,3): '='
(7,1-7,2): '-='
(7,3-7,3): '='
(8,1-8,2): '--'
(8,3-8,4): '=='
(9,1): <end of file>
]])

test_lexer('+\n++\n+++\n++++\n+=\n++=\n+==\n++==\n', [[
(1,1-1,1): '+'
(2,1-2,2): '++'
(3,1-3,2): '++'
(3,3-3,3): '+'
(4,1-4,2): '++'
(4,3-4,4): '++'
(5,1-5,2): '+='
(6,1-6,2): '++'
(6,3-6,3): '='
(7,1-7,2): '+='
(7,3-7,3): '='
(8,1-8,2): '++'
(8,3-8,4): '=='
(9,1): <end of file>
]])

test_lexer('.\n..\n...\n....\n', [[
(1,1-1,1): '.'
(2,1-2,2): '..'
(3,1-3,2): '..'
(3,3-3,3): '.'
(4,1-4,2): '..'
(4,3-4,4): '..'
(5,1): <end of file>
]])

test_lexer(':\n::\n:::\n::::\n', [[
(1,1-1,1): ':'
(2,1-2,1): ':'
(2,2-2,2): ':'
(3,1-3,1): ':'
(3,2-3,2): ':'
(3,3-3,3): ':'
(4,1-4,1): ':'
(4,2-4,2): ':'
(4,3-4,3): ':'
(4,4-4,4): ':'
(5,1): <end of file>
]])

test_lexer('<\n<<\n<<<\n<<<<\n', [[
(1,1-1,1): '<'
(2,1-2,2): '<<'
(3,1-3,2): '<<'
(3,3-3,3): '<'
(4,1-4,2): '<<'
(4,3-4,4): '<<'
(5,1): <end of file>
]])

test_lexer('>\n>>\n>>>\n>>>>\n', [[
(1,1-1,1): '>'
(2,1-2,2): '>>'
(3,1-3,2): '>>'
(3,3-3,3): '>'
(4,1-4,2): '>>'
(4,3-4,4): '>>'
(5,1): <end of file>
]])

test_lexer('=\n==\n===\n====\n', [[
(1,1-1,1): '='
(2,1-2,2): '=='
(3,1-3,2): '=='
(3,3-3,3): '='
(4,1-4,2): '=='
(4,3-4,4): '=='
(5,1): <end of file>
]])

test_lexer('&\n&&\n&&&\n&&&&\n', [[
(1,1-1,1): '&'
(2,1-2,2): '&&'
(3,1-3,2): '&&'
(3,3-3,3): '&'
(4,1-4,2): '&&'
(4,3-4,4): '&&'
(5,1): <end of file>
]])

test_lexer('|\n||\n|||\n||||\n', [[
(1,1-1,1): '|'
(2,1-2,2): '||'
(3,1-3,2): '||'
(3,3-3,3): '|'
(4,1-4,2): '||'
(4,3-4,4): '||'
(5,1): <end of file>
]])

test_lexer('>\n>=\n>>=\n>==\n>>==\n>=>\n>=>=\n=>=>\n', [[
(1,1-1,1): '>'
(2,1-2,2): '>='
(3,1-3,3): '>>='
(4,1-4,2): '>='
(4,3-4,3): '='
(5,1-5,3): '>>='
(5,4-5,4): '='
(6,1-6,2): '>='
(6,3-6,3): '>'
(7,1-7,2): '>='
(7,3-7,4): '>='
(8,1-8,1): '='
(8,2-8,3): '>='
(8,4-8,4): '>'
(9,1): <end of file>
]])

test_lexer('<\n<=\n<<=\n<==\n<<==\n<=<\n<=<=\n=<=<\n', [[
(1,1-1,1): '<'
(2,1-2,2): '<='
(3,1-3,3): '<<='
(4,1-4,2): '<='
(4,3-4,3): '='
(5,1-5,3): '<<='
(5,4-5,4): '='
(6,1-6,2): '<='
(6,3-6,3): '<'
(7,1-7,2): '<='
(7,3-7,4): '<='
(8,1-8,1): '='
(8,2-8,3): '<='
(8,4-8,4): '<'
(9,1): <end of file>
]])

test_lexer('!\n!!\n!=\n!==\n!!=\n', [[
(1,1-1,1): '!'
(2,1-2,1): '!'
(2,2-2,2): '!'
(3,1-3,2): '!='
(4,1-4,2): '!='
(4,3-4,3): '='
(5,1-5,1): '!'
(5,2-5,3): '!='
(6,1): <end of file>
]])

test_lexer([[
abstract
bit
bool
break
byte
case
catch
class
const
continue
default
delete
do
dword
else
enum
extern
false
final
finally
flags
for
friend
goto
if
int
intptr
i8
i16
i32
interface
new
null
object
override
private
protected
public
sbyte
sizeof
static
struct
switch
throw
true
try
uint
uintptr
u8
u16
u32
union
var
void
while
word
]], [[
(1,1-1,8): 'abstract'
(2,1-2,3): 'bit'
(3,1-3,4): 'bool'
(4,1-4,5): 'break'
(5,1-5,4): 'byte'
(6,1-6,4): 'case'
(7,1-7,5): 'catch'
(8,1-8,5): 'class'
(9,1-9,5): 'const'
(10,1-10,8): 'continue'
(11,1-11,7): 'default'
(12,1-12,6): 'delete'
(13,1-13,2): 'do'
(14,1-14,5): 'dword'
(15,1-15,4): 'else'
(16,1-16,4): 'enum'
(17,1-17,6): 'extern'
(18,1-18,5): 'false'
(19,1-19,5): 'final'
(20,1-20,7): 'finally'
(21,1-21,5): 'flags'
(22,1-22,3): 'for'
(23,1-23,6): 'friend'
(24,1-24,4): 'goto'
(25,1-25,2): 'if'
(26,1-26,3): 'int'
(27,1-27,6): 'intptr'
(28,1-28,2): 'i8'
(29,1-29,3): 'i16'
(30,1-30,3): 'i32'
(31,1-31,9): 'interface'
(32,1-32,3): 'new'
(33,1-33,4): 'null'
(34,1-34,6): 'object'
(35,1-35,8): 'override'
(36,1-36,7): 'private'
(37,1-37,9): 'protected'
(38,1-38,6): 'public'
(39,1-39,5): 'sbyte'
(40,1-40,6): 'sizeof'
(41,1-41,6): 'static'
(42,1-42,6): 'struct'
(43,1-43,6): 'switch'
(44,1-44,5): 'throw'
(45,1-45,4): 'true'
(46,1-46,3): 'try'
(47,1-47,4): 'uint'
(48,1-48,7): 'uintptr'
(49,1-49,2): 'u8'
(50,1-50,3): 'u16'
(51,1-51,3): 'u32'
(52,1-52,5): 'union'
(53,1-53,3): 'var'
(54,1-54,4): 'void'
(55,1-55,5): 'while'
(56,1-56,4): 'word'
(57,1): <end of file>
]])

test_lexer([[
:bool
bool:
:bool:
]], [[
(1,1-1,1): ':'
(1,2-1,5): 'bool'
(2,1-2,4): 'bool'
(2,5-2,5): ':'
(3,1-3,1): ':'
(3,2-3,5): 'bool'
(3,6-3,6): ':'
(4,1): <end of file>
]])

test_lexer([[
a
aa
_a
a_
_
__
_4
a4
4a
_bool
bool_
A {
 A
{A
A{
xx;
xx ;
 xx;
 xx
]], [[
(1,1-1,1): identifier "a"
(2,1-2,2): identifier "aa"
(3,1-3,2): identifier "_a"
(4,1-4,2): identifier "a_"
(5,1-5,1): identifier "_"
(6,1-6,2): identifier "__"
(7,1-7,2): identifier "_4"
(8,1-8,2): identifier "a4"
(9,1-9,2): invalid decimal constant
(10,1-10,5): identifier "_bool"
(11,1-11,5): identifier "bool_"
(12,1-12,1): identifier "A"
(12,3-12,3): '{'
(13,2-13,2): identifier "A"
(14,1-14,1): '{'
(14,2-14,2): identifier "A"
(15,1-15,1): identifier "A"
(15,2-15,2): '{'
(16,1-16,2): identifier "xx"
(16,3-16,3): ';'
(17,1-17,2): identifier "xx"
(17,4-17,4): ';'
(18,2-18,3): identifier "xx"
(18,4-18,4): ';'
(19,2-19,3): identifier "xx"
(20,1): <end of file>
]])

test_lexer(' [[ ]] ', [[
(1,2-1,2): '['
(1,3-1,3): '['
(1,5-1,5): ']'
(1,6-1,6): ']'
(1,8): <end of file>
]])

test_lexer([[
 ~ ` = == $ # @ ( ) (( )) [ ] { } {{ }}
 ^ ^^ * ** % %% , ,, ? ?? / \ ; ;;
]], [[
(1,2-1,2): '~'
(1,4-1,4): invalid symbol
(1,6-1,6): '='
(1,8-1,9): '=='
(1,11-1,11): invalid symbol
(1,13-1,13): invalid symbol
(1,15-1,15): '@'
(1,17-1,17): '('
(1,19-1,19): ')'
(1,21-1,21): '('
(1,22-1,22): '('
(1,24-1,24): ')'
(1,25-1,25): ')'
(1,27-1,27): '['
(1,29-1,29): ']'
(1,31-1,31): '{'
(1,33-1,33): '}'
(1,35-1,35): '{'
(1,36-1,36): '{'
(1,38-1,38): '}'
(1,39-1,39): '}'
(2,2-2,2): '^'
(2,4-2,4): '^'
(2,5-2,5): '^'
(2,7-2,7): '*'
(2,9-2,9): '*'
(2,10-2,10): '*'
(2,12-2,12): '%'
(2,14-2,14): '%'
(2,15-2,15): '%'
(2,17-2,17): ','
(2,19-2,19): ','
(2,20-2,20): ','
(2,22-2,22): '?'
(2,24-2,24): '?'
(2,25-2,25): '?'
(2,27-2,27): '/'
(2,29-2,29): invalid symbol
(2,31-2,31): ';'
(2,33-2,33): ';'
(2,34-2,34): ';'
(3,1): <end of file>
]])

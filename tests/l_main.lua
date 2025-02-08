
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
else
enum
false
final
finally
flags
for
friend
goto
if
int
int8
int16
int32
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
uint8
uint16
uint32
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
(14,1-14,4): 'else'
(15,1-15,4): 'enum'
(16,1-16,5): 'false'
(17,1-17,5): 'final'
(18,1-18,7): 'finally'
(19,1-19,5): 'flags'
(20,1-20,3): 'for'
(21,1-21,6): 'friend'
(22,1-22,4): 'goto'
(23,1-23,2): 'if'
(24,1-24,3): 'int'
(25,1-25,4): 'int8'
(26,1-26,5): 'int16'
(27,1-27,5): 'int32'
(28,1-28,9): 'interface'
(29,1-29,3): 'new'
(30,1-30,4): 'null'
(31,1-31,6): 'object'
(32,1-32,8): 'override'
(33,1-33,7): 'private'
(34,1-34,9): 'protected'
(35,1-35,6): 'public'
(36,1-36,5): 'sbyte'
(37,1-37,6): 'sizeof'
(38,1-38,6): 'static'
(39,1-39,6): 'struct'
(40,1-40,6): 'switch'
(41,1-41,5): 'throw'
(42,1-42,4): 'true'
(43,1-43,3): 'try'
(44,1-44,5): 'uint8'
(45,1-45,6): 'uint16'
(46,1-46,6): 'uint32'
(47,1-47,5): 'union'
(48,1-48,3): 'var'
(49,1-49,4): 'void'
(50,1-50,5): 'while'
(51,1-51,4): 'word'
(52,1): <end of file>
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

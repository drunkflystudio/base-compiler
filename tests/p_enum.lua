
test_parser_full([[
enum A {
}
]], [[
translationUnitBegin
translationUnitEnd
]])

test_parser_full([[
enum A {
};
]], [[
translationUnitBegin
(2,2-2,2): unexpected token: ';'
]])

test_parser_full([[
enum A : int {
}
]], [[
]])

test_parser_full([[
enum A {
    F
}
]], [[
]])

test_parser_full([[
enum A {
    F,
}
]], [[
]])

test_parser_full([[
enum A {
    F
    G
}
]], [[
]])

test_parser_full([[
enum A {
    F,
    G
}
]], [[
]])

test_parser_full([[
enum A {
    F,
    G,
}
]], [[
]])

test_parser_full([[
enum A {
    F = 4
}
]], [[
]])

test_parser_full([[
enum A {
    F = 4,
}
]], [[
]])

test_parser_full([[
enum A {
    F =
}
]], [[
]])

test_parser_full([[
enum A {
    F = 4
    G
}
]], [[
]])

test_parser_full([[
enum A {
    F = 4,
    G = 4
}
]], [[
]])

test_parser_full([[
enum A {
    F = 4,
    G = 4,
}
]], [[
]])

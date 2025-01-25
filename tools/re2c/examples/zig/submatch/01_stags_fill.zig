// Generated by re2zig
// re2zig $INPUT -o $OUTPUT

const std = @import("std");

const bufsize = 4095;
const none = std.math.maxInt(usize);

const err = error.SyntaxError;

const SemVer = struct {
    major: u32,
    minor: u32,
    patch: u32,
};

fn s2n(str: []const u8) u32 { // convert a pre-parsed string to a number
    var n: u32 = 0;
    for (str) |c| { n = n * 10 + (c - 48); }
    return n;
}

const State = struct {
    yyinput: [bufsize + 1]u8,
    yycursor: usize,
    yymarker: usize,
    yylimit: usize,
    token: usize,
    // Intermediate tag variables must be part of the lexer state passed to YYFILL.
    // They don't correspond to tags and should be autogenerated by re2c.
    yyt1: usize,
yyt2: usize,
yyt3: usize,

    eof: bool
};

fn fill(st: *State, file: anytype) i32 {
    if (st.eof) { return -1; } // unexpected EOF

    // Error: lexeme too long. In real life can reallocate a larger buffer.
    if (st.token < 1) { return -2; }

    // Shift buffer contents (discard everything up to the current token).
    std.mem.copyBackwards(
        u8, st.yyinput[0..st.yylimit - st.token], st.yyinput[st.token..st.yylimit]);
    st.yycursor -= st.token;
    st.yymarker = @subWithOverflow(st.yymarker, st.token)[0];
    st.yylimit -= st.token;
    // Tag variables need to be shifted like other input positions. The check
    // for NONE is only needed if some tags are nested inside of alternative or
    // repetition, so that they can have NONE value.
    if (st.yyt1 != none) st.yyt1 = @subWithOverflow(st.yyt1, st.token)[0];
if (st.yyt2 != none) st.yyt2 = @subWithOverflow(st.yyt2, st.token)[0];
if (st.yyt3 != none) st.yyt3 = @subWithOverflow(st.yyt3, st.token)[0];

    st.token = 0;

    // Fill free space at the end of buffer with new data from file.
    st.yylimit += file.read(st.yyinput[st.yylimit..bufsize]) catch 0;
    st.yyinput[st.yylimit] = 0; // append sentinel symbol

    // If read less than expected, this is the end of input.
    st.eof = st.yylimit < bufsize;

    return 0;
}

fn parse(st: *State, file: anytype) !std.ArrayList(SemVer) {
    var vers = std.ArrayList(SemVer).init(std.testing.allocator);

    // Final tag variables available in semantic action.
    var t1: usize = 0;
var t2: usize = 0;
var t3: usize = 0;
var t4: usize = 0;


    loop: while (true) {
        st.token = st.yycursor;
        
    var yych: u8 = 0;
    var yystate: u32 = 0;
    yyl: while (true) {
        switch (yystate) {
            0 => {
                yych = st.yyinput[st.yycursor];
                switch (yych) {
                    0x30...0x39 => {
                        st.yycursor += 1;
                        yystate = 3;
                        continue :yyl;
                    },
                    else => {
                        if (st.yylimit <= st.yycursor) {
                            if (fill(st, file) == 0) {
                                yystate = 0;
                                continue :yyl;
                            }
                            yystate = 11;
                            continue :yyl;
                        }
                        st.yycursor += 1;
                        yystate = 1;
                        continue :yyl;
                    },
                }
            },
            1 => {
                yystate = 2;
                continue :yyl;
            },
            2 => { return error.SyntaxError; },
            3 => {
                st.yymarker = st.yycursor;
                yych = st.yyinput[st.yycursor];
                switch (yych) {
                    0x2E => {
                        st.yycursor += 1;
                        yystate = 4;
                        continue :yyl;
                    },
                    0x30...0x39 => {
                        st.yycursor += 1;
                        yystate = 6;
                        continue :yyl;
                    },
                    else => {
                        if (st.yylimit <= st.yycursor) {
                            if (fill(st, file) == 0) {
                                yystate = 3;
                                continue :yyl;
                            }
                        }
                        yystate = 2;
                        continue :yyl;
                    },
                }
            },
            4 => {
                yych = st.yyinput[st.yycursor];
                switch (yych) {
                    0x30...0x39 => {
                        st.yyt1 = st.yycursor;
                        st.yycursor += 1;
                        yystate = 7;
                        continue :yyl;
                    },
                    else => {
                        if (st.yylimit <= st.yycursor) {
                            if (fill(st, file) == 0) {
                                yystate = 4;
                                continue :yyl;
                            }
                        }
                        yystate = 5;
                        continue :yyl;
                    },
                }
            },
            5 => {
                st.yycursor = st.yymarker;
                yystate = 2;
                continue :yyl;
            },
            6 => {
                yych = st.yyinput[st.yycursor];
                switch (yych) {
                    0x2E => {
                        st.yycursor += 1;
                        yystate = 4;
                        continue :yyl;
                    },
                    0x30...0x39 => {
                        st.yycursor += 1;
                        yystate = 6;
                        continue :yyl;
                    },
                    else => {
                        if (st.yylimit <= st.yycursor) {
                            if (fill(st, file) == 0) {
                                yystate = 6;
                                continue :yyl;
                            }
                        }
                        yystate = 5;
                        continue :yyl;
                    },
                }
            },
            7 => {
                yych = st.yyinput[st.yycursor];
                switch (yych) {
                    0x0A => {
                        st.yyt2 = st.yycursor;
                        st.yyt3 = std.math.maxInt(usize);
                        st.yycursor += 1;
                        yystate = 8;
                        continue :yyl;
                    },
                    0x2E => {
                        st.yyt2 = st.yycursor;
                        st.yycursor += 1;
                        yystate = 9;
                        continue :yyl;
                    },
                    0x30...0x39 => {
                        st.yycursor += 1;
                        yystate = 7;
                        continue :yyl;
                    },
                    else => {
                        if (st.yylimit <= st.yycursor) {
                            if (fill(st, file) == 0) {
                                yystate = 7;
                                continue :yyl;
                            }
                        }
                        yystate = 5;
                        continue :yyl;
                    },
                }
            },
            8 => {
                t2 = st.yyt1;
                t3 = st.yyt2;
                t4 = st.yyt3;
                t1 = st.yyt1;
                t1 -= 1;
                
                try vers.append(SemVer {
                    .major = s2n(st.yyinput[st.token..t1]),
                    .minor = s2n(st.yyinput[t2..t3]),
                    .patch = if (t4 == none) 0 else s2n(st.yyinput[t4..st.yycursor - 1]),
                });
                continue :loop;

            },
            9 => {
                yych = st.yyinput[st.yycursor];
                switch (yych) {
                    0x30...0x39 => {
                        st.yyt3 = st.yycursor;
                        st.yycursor += 1;
                        yystate = 10;
                        continue :yyl;
                    },
                    else => {
                        if (st.yylimit <= st.yycursor) {
                            if (fill(st, file) == 0) {
                                yystate = 9;
                                continue :yyl;
                            }
                        }
                        yystate = 5;
                        continue :yyl;
                    },
                }
            },
            10 => {
                yych = st.yyinput[st.yycursor];
                switch (yych) {
                    0x0A => {
                        st.yycursor += 1;
                        yystate = 8;
                        continue :yyl;
                    },
                    0x30...0x39 => {
                        st.yycursor += 1;
                        yystate = 10;
                        continue :yyl;
                    },
                    else => {
                        if (st.yylimit <= st.yycursor) {
                            if (fill(st, file) == 0) {
                                yystate = 10;
                                continue :yyl;
                            }
                        }
                        yystate = 5;
                        continue :yyl;
                    },
                }
            },
            11 => { return vers; },
            else => { @panic("internal lexer error"); },
        }
    }

    }
}

test {
    const fname = "input";
    const content = "1.22.333\n" ** bufsize;

    // Prepare input file: a few times the size of the buffer, containing
    // strings with zeroes and escaped quotes.
    var fw = try std.fs.cwd().createFile(fname, .{});
    try fw.writeAll(content);
    fw.close();

    // Prepare lexer state: all offsets are at the end of buffer.
    var fr = try std.fs.cwd().openFile(fname, .{ .mode = .read_only});
    // Normally file would be part of the state struct, but BufferedReader type is unclear.
    var br = std.io.bufferedReader(fr.reader());
    var st = State{
        .yyinput = undefined,
        .yycursor = bufsize,
        .yymarker = bufsize,
        .yylimit = bufsize,
        .token = bufsize,
        .yyt1 = none,
.yyt2 = none,
.yyt3 = none,

        .eof = false,
    };
    // Sentinel at `yylimit` offset is set to zero, which triggers YYFILL.
    st.yyinput[st.yylimit] = 0;

    // Manually construct expected result.
    var expect = std.ArrayList(SemVer).init(std.testing.allocator);
    for (0..bufsize) |_| try expect.append(SemVer{.major = 1, .minor = 22, .patch = 333});

    // Run the lexer.
    var result = try parse(&st, &br);
    try std.testing.expectEqualDeep(result, expect);

    // Cleanup: free memory and remove input file.
    expect.deinit();
    result.deinit();
    fr.close();
    try std.fs.cwd().deleteFile(fname);
}

/* Generated by re2rust */
// re2rust $INPUT -o $OUTPUT

use std::fs::File;
use std::io::{Read, Write};

const BUFSIZE: usize = 4096;

struct State {
    file: File,
    yyinput: [u8; BUFSIZE],
    yylimit: usize,
    yycursor: usize,
    yymarker: usize,
    token: usize,
    eof: bool,
}

#[derive(PartialEq)]
enum Fill { Ok, Eof, LongLexeme }

fn fill(st: &mut State) -> Fill {
    if st.eof { return Fill::Eof; }

    // Error: lexeme too long. In real life could reallocate a larger buffer.
    if st.token < 1 { return Fill::LongLexeme; }

    // Shift buffer contents (discard everything up to the current token).
    st.yyinput.copy_within(st.token..st.yylimit, 0);
    st.yylimit -= st.token;
    st.yycursor -= st.token;
    st.yymarker = st.yymarker.overflowing_sub(st.token).0; // may underflow if marker is unused
    st.token = 0;

    // Fill free space at the end of buffer with new data from file.
    match st.file.read(&mut st.yyinput[st.yylimit..BUFSIZE - 1]) { // -1 for sentinel
        Ok(n) => {
            st.yylimit += n;
            st.eof = n == 0; // end of file
            st.yyinput[st.yylimit] = 0; // append sentinel
        }
        Err(why) => panic!("cannot read from file: {}", why)
    }

    return Fill::Ok;
}

fn lex(yyrecord: &mut State) -> isize {
    let mut count: isize = 0;

    'lex: loop {
        yyrecord.token = yyrecord.yycursor;
    
{
    #[allow(unused_assignments)]
    let mut yych : u8 = 0;
    let mut yystate : usize = 0;
    'yyl: loop {
        match yystate {
            0 => {
                yych = unsafe {*yyrecord.yyinput.get_unchecked(yyrecord.yycursor)};
                match yych {
                    0x20 => {
                        yyrecord.yycursor += 1;
                        yystate = 3;
                        continue 'yyl;
                    }
                    0x27 => {
                        yyrecord.yycursor += 1;
                        yystate = 5;
                        continue 'yyl;
                    }
                    _ => {
                        if yyrecord.yylimit <= yyrecord.yycursor {
                            if fill(yyrecord) == Fill::Ok {
                                yystate = 0;
                                continue 'yyl;
                            }
                            yystate = 10;
                            continue 'yyl;
                        }
                        yyrecord.yycursor += 1;
                        yystate = 1;
                        continue 'yyl;
                    }
                }
            }
            1 => {
                yystate = 2;
                continue 'yyl;
            }
            2 => { return -1; },
            3 => {
                yych = unsafe {*yyrecord.yyinput.get_unchecked(yyrecord.yycursor)};
                match yych {
                    0x20 => {
                        yyrecord.yycursor += 1;
                        yystate = 3;
                        continue 'yyl;
                    }
                    _ => {
                        if yyrecord.yylimit <= yyrecord.yycursor {
                            if fill(yyrecord) == Fill::Ok {
                                yystate = 3;
                                continue 'yyl;
                            }
                        }
                        yystate = 4;
                        continue 'yyl;
                    }
                }
            }
            4 => { continue 'lex; },
            5 => {
                yyrecord.yymarker = yyrecord.yycursor;
                yych = unsafe {*yyrecord.yyinput.get_unchecked(yyrecord.yycursor)};
                if yych >= 0x01 {
                    yystate = 7;
                    continue 'yyl;
                }
                if yyrecord.yylimit <= yyrecord.yycursor {
                    if fill(yyrecord) == Fill::Ok {
                        yystate = 5;
                        continue 'yyl;
                    }
                    yystate = 2;
                    continue 'yyl;
                }
                yyrecord.yycursor += 1;
                yystate = 6;
                continue 'yyl;
            }
            6 => {
                yych = unsafe {*yyrecord.yyinput.get_unchecked(yyrecord.yycursor)};
                yystate = 7;
                continue 'yyl;
            }
            7 => {
                match yych {
                    0x27 => {
                        yyrecord.yycursor += 1;
                        yystate = 8;
                        continue 'yyl;
                    }
                    0x5C => {
                        yyrecord.yycursor += 1;
                        yystate = 9;
                        continue 'yyl;
                    }
                    _ => {
                        if yyrecord.yylimit <= yyrecord.yycursor {
                            if fill(yyrecord) == Fill::Ok {
                                yystate = 6;
                                continue 'yyl;
                            }
                            yystate = 11;
                            continue 'yyl;
                        }
                        yyrecord.yycursor += 1;
                        yystate = 6;
                        continue 'yyl;
                    }
                }
            }
            8 => { count += 1; continue 'lex; },
            9 => {
                yych = unsafe {*yyrecord.yyinput.get_unchecked(yyrecord.yycursor)};
                if yych <= 0x00 {
                    if yyrecord.yylimit <= yyrecord.yycursor {
                        if fill(yyrecord) == Fill::Ok {
                            yystate = 9;
                            continue 'yyl;
                        }
                        yystate = 11;
                        continue 'yyl;
                    }
                    yyrecord.yycursor += 1;
                    yystate = 6;
                    continue 'yyl;
                }
                yyrecord.yycursor += 1;
                yystate = 6;
                continue 'yyl;
            }
            10 => { return count; },
            11 => {
                yyrecord.yycursor = yyrecord.yymarker;
                yystate = 2;
                continue 'yyl;
            }
            _ => panic!("internal lexer error"),
        }
    }
}
}
}

fn main() {
    let fname = "input";
    let content = b"'qu\0tes' 'are' 'fine: \\'' ";

    // Prepare input file: a few times the size of the buffer, containing
    // strings with zeroes and escaped quotes.
    match File::create(fname) {
        Err(why) => panic!("cannot open {}: {}", fname, why),
        Ok(mut file) => match file.write_all(&content.repeat(BUFSIZE)) {
            Err(why) => panic!("cannot write to {}: {}", fname, why),
            Ok(_) => {}
        }
    };
    let count = 3 * BUFSIZE; // number of quoted strings written to file

    // Reopen input file for reading.
    let file = match File::open(fname) {
        Err(why) => panic!("cannot read file {}: {}", fname, why),
        Ok(file) => file,
    };

    // Initialize lexer state: all offsets are at the end of buffer.
    let yylimit = BUFSIZE - 1;
    let mut st = State {
        file: file,
        // Sentinel (at `yylimit` offset) is set to null, which triggers YYFILL.
        yyinput: [0; BUFSIZE],
        yylimit: yylimit,
        yycursor: yylimit,
        yymarker: yylimit,
        token: yylimit,
        eof: false,
    };

    // Run the lexer.
    assert_eq!(lex(&mut st), count as isize);

    // Cleanup: remove input file.
    match std::fs::remove_file(fname) {
        Err(why) => panic!("cannot remove {}: {}", fname, why),
        Ok(_) => {}
    }
}

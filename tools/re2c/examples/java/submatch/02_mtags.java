// Generated by re2java
// re2java $INPUT -o $OUTPUT

import java.util.*;

class Main {
    static Optional<int[]> parse(String yyinput) {
        int yycursor = 0;
        int yymarker = 0;

        // Final tag variables available in semantic action.
        int t1;int t2;
        List<Integer> t3;List<Integer> t4;

        // Intermediate tag variables used by the lexer (must be autogenerated).
        int yyt1 = -1;int yyt2 = -1;
        List<Integer> yytm3 = new ArrayList<>();List<Integer> yytm4 = new ArrayList<>();

        
{
    char yych = 0;
    int yystate = 0;
    yyl: while (true) {
        switch (yystate) {
            case 0:
                yych = yyinput.charAt(yycursor);
                switch (yych) {
                    case 0x30:
                    case 0x31:
                    case 0x32:
                    case 0x33:
                    case 0x34:
                    case 0x35:
                    case 0x36:
                    case 0x37:
                    case 0x38:
                    case 0x39:
                        yyt1 = yycursor;
                        yycursor += 1;
                        yystate = 3;
                        continue yyl;
                    default:
                        yycursor += 1;
                        yystate = 1;
                        continue yyl;
                }
            case 1:
                yystate = 2;
                continue yyl;
            case 2:
                { return Optional.empty(); }
            case 3:
                yymarker = yycursor;
                yych = yyinput.charAt(yycursor);
                switch (yych) {
                    case 0x00:
                        
                        
                        yyt2 = yycursor;
                        yycursor += 1;
                        yystate = 4;
                        continue yyl;
                    case 0x2E:
                        yyt2 = yycursor;
                        yycursor += 1;
                        yystate = 5;
                        continue yyl;
                    case 0x30:
                    case 0x31:
                    case 0x32:
                    case 0x33:
                    case 0x34:
                    case 0x35:
                    case 0x36:
                    case 0x37:
                    case 0x38:
                    case 0x39:
                        yycursor += 1;
                        yystate = 7;
                        continue yyl;
                    default:
                        yystate = 2;
                        continue yyl;
                }
            case 4:
                t1 = yyt1;
                t2 = yyt2;
                t3 = yytm3;
                t4 = yytm4;
                {
                int[] vers = new int[t3.size() + 1];
                vers[0] = Integer.valueOf(yyinput.substring(t1, t2));
                for (int i = 0; i < t3.size(); ++i) {
                    vers[i + 1] = Integer.valueOf(yyinput.substring(t3.get(i), t4.get(i)));
                }
                return Optional.of(vers);
            }
            case 5:
                yych = yyinput.charAt(yycursor);
                switch (yych) {
                    case 0x30:
                    case 0x31:
                    case 0x32:
                    case 0x33:
                    case 0x34:
                    case 0x35:
                    case 0x36:
                    case 0x37:
                    case 0x38:
                    case 0x39:
                        yytm3.add(yycursor);
                        yycursor += 1;
                        yystate = 8;
                        continue yyl;
                    default:
                        yystate = 6;
                        continue yyl;
                }
            case 6:
                yycursor = yymarker;
                yystate = 2;
                continue yyl;
            case 7:
                yych = yyinput.charAt(yycursor);
                switch (yych) {
                    case 0x00:
                        
                        
                        yyt2 = yycursor;
                        yycursor += 1;
                        yystate = 4;
                        continue yyl;
                    case 0x2E:
                        yyt2 = yycursor;
                        yycursor += 1;
                        yystate = 5;
                        continue yyl;
                    case 0x30:
                    case 0x31:
                    case 0x32:
                    case 0x33:
                    case 0x34:
                    case 0x35:
                    case 0x36:
                    case 0x37:
                    case 0x38:
                    case 0x39:
                        yycursor += 1;
                        yystate = 7;
                        continue yyl;
                    default:
                        yystate = 6;
                        continue yyl;
                }
            case 8:
                yych = yyinput.charAt(yycursor);
                switch (yych) {
                    case 0x00:
                        yytm4.add(yycursor);
                        yycursor += 1;
                        yystate = 4;
                        continue yyl;
                    case 0x2E:
                        yytm4.add(yycursor);
                        yycursor += 1;
                        yystate = 5;
                        continue yyl;
                    case 0x30:
                    case 0x31:
                    case 0x32:
                    case 0x33:
                    case 0x34:
                    case 0x35:
                    case 0x36:
                    case 0x37:
                    case 0x38:
                    case 0x39:
                        yycursor += 1;
                        yystate = 8;
                        continue yyl;
                    default:
                        yystate = 6;
                        continue yyl;
                }
            default:
                throw new IllegalStateException("internal lexer error");
        }
    }
}

    }

    public static void main(String []args) {
        assert Arrays.equals(parse("1\0").get(), new int[]{1});
        assert Arrays.equals(parse("1.2.3.4.5.6.7\0").get(), new int[]{1, 2, 3, 4, 5, 6, 7});
        assert !parse("1.2.\0").isPresent();
    }
};

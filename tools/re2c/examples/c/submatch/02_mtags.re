// re2c $INPUT -o $OUTPUT
#include <assert.h>
#include <stddef.h>
#include <vector>

static const int MTAG_ROOT = -1;

// An m-tag tree is a way to store histories with an O(1) copy operation.
// Histories naturally form a tree, as they have common start and fork at some
// point. The tree is stored as an array of pairs (tag value, link to parent).
// An m-tag is represented with a single link in the tree (array index).
struct Mtag {
    const char *elem; // tag value
    int pred; // index of the predecessor node or root
};
typedef std::vector<Mtag> MtagTrie;

typedef std::vector<int> Ver; // unbounded number of version components

static int s2n(const char *s, const char *e) { // pre-parsed string to number
    int n = 0;
    for (; s < e; ++s) n = n * 10 + (*s - '0');
    return n;
}

// Append a single value to an m-tag history.
static void add_mtag(MtagTrie &trie, int &mtag, const char *value) {
    Mtag m = {value, mtag};
    mtag = (int)trie.size();
    trie.push_back(m);
}

// Recursively unwind tag histories and collect version components.
static void unfold(const MtagTrie &trie, int x, int y, Ver &ver) {
    // Reached the root of the m-tag tree, stop recursion.
    if (x == MTAG_ROOT && y == MTAG_ROOT) return;

    // Unwind history further.
    unfold(trie, trie[x].pred, trie[y].pred, ver);

    // Get tag values. Tag histories must have equal length.
    assert(x != MTAG_ROOT && y != MTAG_ROOT);
    const char *ex = trie[x].elem, *ey = trie[y].elem;

    if (ex != NULL && ey != NULL) {
        // Both tags are valid pointers, extract component.
        ver.push_back(s2n(ex, ey));
    } else {
        // Both tags are NULL (this corresponds to zero repetitions).
        assert(ex == NULL && ey == NULL);
    }
}

static bool parse(const char *str, Ver &ver) {
    const char *YYCURSOR = str, *YYMARKER;
    MtagTrie mt;

    // User-defined tag variables that are available in semantic action.
    const char *t1, *t2;
    int t3, t4;

    // Autogenerated tag variables used by the lexer to track tag values.
    /*!stags:re2c format = 'const char *@@ = NULL;'; */
    /*!mtags:re2c format = 'int @@ = MTAG_ROOT;'; */

    /*!re2c
        re2c:api:style = free-form;
        re2c:define:YYCTYPE = char;
        re2c:define:YYSTAGP = "@@ = YYCURSOR;";
        re2c:define:YYSTAGN = "@@ = NULL;";
        re2c:define:YYMTAGP = "add_mtag(mt, @@, YYCURSOR);";
        re2c:define:YYMTAGN = "add_mtag(mt, @@, NULL);";
        re2c:yyfill:enable = 0;
        re2c:tags = 1;

        num = [0-9]+;

        @t1 num @t2 ("." #t3 num #t4)* [\x00] {
            ver.clear();
            ver.push_back(s2n(t1, t2));
            unfold(mt, t3, t4, ver);
            return true;
        }
        * { return false; }
    */
}

int main() {
    Ver v;
    assert(parse("1", v) && v == Ver({1}));
    assert(parse("1.2.3.4.5.6.7", v) && v == Ver({1, 2, 3, 4, 5, 6, 7}));
    assert(!parse("1.2.", v));
    return 0;
}

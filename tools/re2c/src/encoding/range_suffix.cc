#include "src/encoding/range_suffix.h"
#include "src/regexp/regexp.h"
#include "src/util/range.h"

namespace re2c {

static Regexp* emit(RESpec& spec, RangeSuffix* p, Regexp* re);

Regexp* to_regexp(RESpec& spec, RangeSuffix* p) {
    return p ? emit(spec, p, /*nullptr*/NULL) : re_sym(spec, /*nullptr*/NULL);
}

// Build a regexp from a suffix tree.
Regexp* emit(RESpec& spec, RangeSuffix* p, Regexp* re) {
    if (p == /*nullptr*/NULL) return re;

    RangeMgr& rm = spec.rangemgr;
    Regexp* regexp = /*nullptr*/NULL;
    for (; p != /*nullptr*/NULL; p = p->next) {
        Regexp* re1 = re_cat(spec, re_sym(spec, rm.ran(p->l, p->h + 1)), re);
        regexp = re_alt(spec, regexp, emit(spec, p->child, re1));
    }
    return regexp;
}

} // namespace re2c

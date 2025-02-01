#ifndef _RE2C_RE_TAG_
#define _RE2C_RE_TAG_

#include <stddef.h>
#include <stdint.h>
#include <limits>
#include <string>

namespace re2c {

typedef int32_t tagver_t; /*using tagver_t = int32_t;*/

// default value, lowest priority
static const/*expr*/ tagver_t TAGVER_BOTTOM = std::numeric_limits<tagver_t>::min();
// the absense of tag
static const/*expr*/ tagver_t TAGVER_ZERO = 0;
// current position, highest priority
static const/*expr*/ tagver_t TAGVER_CURSOR = std::numeric_limits<tagver_t>::max();

struct tag_info_t {
    uint32_t idx; // tag index (starting from zero)
    uint32_t neg; // tag sign (1 for negative, 0 for positive)
};

struct Tag {
    static /*constexpr*/ size_t RIGHTMOST() { return std::numeric_limits<size_t>::max(); }
    static /*constexpr*/ size_t FICTIVE() { return std::numeric_limits<size_t>::max() - 1; }
    static /*constexpr*/ size_t NONE() { return std::numeric_limits<size_t>::max() - 2; }
    static /*constexpr*/ uint32_t VARDIST() { return std::numeric_limits<uint32_t>::max(); }

    const char* name;
    size_t lsub;
    size_t hsub;
    size_t base;
    size_t dist;
    size_t lnest;
    size_t hnest;
    bool history;
    bool orbit;
    bool toplevel;
    int32_t height;

    Tag(const char* nm, bool hi, int32_t ht)
        : name(nm),
          lsub(RIGHTMOST()),
          hsub(RIGHTMOST()),
          base(RIGHTMOST()),
          dist(VARDIST()),
          lnest(RIGHTMOST()),
          hnest(RIGHTMOST()),
          history(hi),
          orbit(false),
          toplevel(false),
          height(ht) {}

    Tag(size_t lsub, size_t hsub, bool history, bool orbit, int32_t height)
        : name(/*nullptr*/NULL),
          lsub(lsub),
          hsub(hsub),
          base(RIGHTMOST()),
          dist(VARDIST()),
          lnest(RIGHTMOST()),
          hnest(RIGHTMOST()),
          history(history),
          orbit(orbit),
          toplevel(false),
          height(height) {}
};

inline bool operator == (const tag_info_t& x, const tag_info_t& y) {
    return x.idx == y.idx && x.neg == y.neg;
}

inline bool fixed(const Tag& tag) {
    return tag.dist != Tag::VARDIST();
}

inline bool fictive(const Tag& tag) {
    return tag.lsub == Tag::FICTIVE();
}

inline bool capture(const Tag& tag) {
    return tag.lsub != Tag::RIGHTMOST();
}

inline bool orbit(const Tag& tag) {
    return tag.orbit;
}

inline bool trailing(const Tag& tag) {
    return !capture(tag) && tag.name == /*nullptr*/NULL;
}

inline bool history(const Tag& tag) {
    return tag.history;
}

} // namespace re2c

#endif // _RE2C_RE_TAG_

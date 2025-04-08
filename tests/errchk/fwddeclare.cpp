template<typename T>
struct S1 { T v; };

typedef S1<struct S2> S22;

struct S2 { int i; };

S22 xx;
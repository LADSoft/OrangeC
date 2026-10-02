#define ABC xx
#define DEF yy


R"abc( hi ABC bye )abc" ABC DEF R"abc(hi DEF bye)abc") DEF
ABC
DEF
R"abc( hi 
ABC
DEF
bye
)abc" ABC DEF
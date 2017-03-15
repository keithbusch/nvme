#ifndef _CMD_H
#define _CMD_H

#undef COMMAND_LIST
#define COMMAND_LIST(args...)
#define PROGRAM(n, v, s, l, u)

#define OPTION(t, v, s, l, d, m, i, dep) \
OPTION_LIMIT(t, v, s, l, d, m, i, (t)~0, dep)

#define OPT_FMT() \
OPTION(FMT, format, 'o', "output-format", "Output format: normal|json|binary|hex", "FMT", NORMAL, false)

#define OPT_INC(v, s, l, d) \
OPTION(INC, v, s, l, d, "INC", 0, false)

#define OPT_VERBOSE() \
OPT_INC(verbose, 'v', "verbose", "Be more verbose")

#define OPT_FLAG(v, s, l, d) \
OPTION(FLAG, v, s, l, d, "FLAG", false, false)

#define OPT_U8_LIMIT(v, s, l, d, limit) \
OPTION_LIMIT(U8, v, s, l, d, "NUM", 0, limit, false)

#define OPT_U8(v, s, l, d) \
OPTION(U8, v, s, l, d, "NUM", 0, false)

#define OPT_U16(v, s, l, d) \
OPTION(U16, v, s, l, d, "NUM", 0, false)

#define OPT_U32(v, s, l, d) \
OPTION(U32, v, s, l, d, "NUM", 0, false)

#define OPT_U32_INIT(v, s, l, d, i) \
OPTION(U32, v, s, l, d, "NUM", i, false)

#define OPT_U64(v, s, l, d) \
OPTION(U64, v, s, l, d, "NUM", 0, false)

#define OPT_U8_LIST(v, s, l, d) \
OPTION(U8_LIST, v, s, l, d, "LIST", NULL, false)

#define OPT_U16_LIST(v, s, l, d) \
OPTION(U16_LIST, v, s, l, d, "LIST", NULL, false)

#define OPT_U32_LIST(v, s, l, d) \
OPTION(U32_LIST, v, s, l, d, "LIST", NULL, false)

#define OPT_U64_LIST(v, s, l, d) \
OPTION(U64_LIST, v, s, l, d, "LIST", NULL, false)

#define OPT_STRING(v, s, l, d)	 \
OPTION(STRING, v, s, l, d, "STRING", NULL, false)

#define OPT_FILE_A(v, s, l, d)	 \
OPTION_LIMIT(FILE_A, v, s, l, d, "FILE", NULL, 0, false)

#define OPT_FILE_R(v, s, l, d)	 \
OPTION_LIMIT(FILE_R, v, s, l, d, "FILE", NULL, 0, false)

#define OPT_FILE_W(v, s, l, d)	 \
OPTION_LIMIT(FILE_W, v, s, l, d, "FILE", NULL, 0, false)

#define OPT_FILE_AP(v, s, l, d)	 \
OPTION_LIMIT(FILE_AP, v, s, l, d, "FILE", NULL, 0, false)

#define OPT_FILE_RP(v, s, l, d)	 \
OPTION_LIMIT(FILE_RP, v, s, l, d, "FILE", NULL, 0, false)

#define OPT_FILE_WP(v, s, l, d)	 \
OPTION_LIMIT(FILE_WP, v, s, l, d, "FILE", NULL, 0, false)

#define OPT_NS() \
OPT_U32(namespace_id, 'n', "namespace-id", "Namespace Identifier")

#endif

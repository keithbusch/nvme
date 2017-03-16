#ifndef _PROG_PARSE_H
#define _PROG_PARSE_H

#include <linux/types.h>
#include <getopt.h>
#include <stdbool.h>

enum flags_masks {
	F_VERBOSE_MASK		= 0xf,
	F_VENDOR_SPEC_SHIFT 	= 4,
	F_RESERVED_FIELDS_SHIFT = 5,
};

enum flags {
	F_TERSE			= 0,
	F_NORMAL		= 1,
	F_VERBOSE		= 2,
	F_DETAILED		= 3,

	F_VENDOR_SPEC		= 1 << F_VENDOR_SPEC_SHIFT,
	F_RESERVED_FIELDS	= 1 << F_RESERVED_FIELDS_SHIFT,
};

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef void* U64_LIST;
typedef void* U32_LIST;
typedef void* U16_LIST;
typedef void* U8_LIST;

typedef int INC;
typedef int FMT;
typedef bool FLAG;
typedef char* STRING;

typedef FILE* FILE_A;
typedef FILE* FILE_R;
typedef FILE* FILE_W;
typedef FILE* FILE_AP;
typedef FILE* FILE_RP;
typedef FILE* FILE_WP;

enum output_format {
	NORMAL,
	JSON,
	BINARY,
	HEX,
};

enum option_type {
	TYPE_U8,
	TYPE_U16,
	TYPE_U32,
	TYPE_U64,
	TYPE_FMT,
	TYPE_U8_LIST,
	TYPE_U16_LIST,
	TYPE_U32_LIST,
	TYPE_U64_LIST,
	TYPE_STRING,
	TYPE_FLAG,
	TYPE_INC,
	TYPE_FILE_A,
	TYPE_FILE_R,
	TYPE_FILE_W,
	TYPE_FILE_AP,
	TYPE_FILE_RP,
	TYPE_FILE_WP,
	TYPE_END,
};

struct command_option {
	enum option_type type;
	char *long_option;
	char short_option;
	char *desc;
	char *meta;
	void *value;
	uint64_t limit;
	bool deprecated;
};

struct command {
	const char *name;
	const char *short_desc;
	const char *long_desc;
	const char *usage;
	int (*callback)(int argc, char ** argv, struct command *cmd);
	bool deprecated;
	struct command **subcommands;
	struct command_option options[];
};

struct program {
	char *version;
	struct command command;
};

extern int parse_args(int argc, char **argv, struct command *command);

#endif /* _PROG_PARSE_H */

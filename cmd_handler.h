/*
 * Stage 1
 *
 * Define function prototypes.
 */
#undef PROGRAM
#define PROGRAM(_name, _version, _short_desc, _long_desc, _usage)

#undef COMMAND
#define COMMAND(name, callback, short_desc, long_desc, usage, deprecated, options, action) \
static int callback(int argc, char **argv, struct command *command);

#undef COMMAND_LIST
#define COMMAND_LIST(args...) args

#undef OPTION_LIMIT
#define OPTION_LIMIT(type, var, short_opt, long_opt, desc, meta, value, limit, deprecated)

#undef OPTIONS
#define OPTIONS(args...)  args

#include CMD_INCLUDE(CMD_INC_FILE)

/*
 * Stage 2
 *
 * Define command structures.
 */
#undef PROGRAM
#define PROGRAM(_name, _version, _short_desc, _long_desc, _usage)

#undef OPTIONS
#define OPTIONS(args...)  { args {.type = TYPE_END} }

#undef OPTION_LIMIT
#define OPTION_LIMIT(_type, var, _short_option, _long_option, _desc, _meta, value, _limit, _deprecated) \
{					\
	.type = TYPE_ ## _type,		\
	.long_option = _long_option,	\
	.short_option = _short_option,	\
	.desc = _desc,			\
	.meta = _meta,			\
	.limit = _limit,		\
	.deprecated = _deprecated,	\
},

#undef COMMAND
#define COMMAND(_name, _callback, _short_desc, _long_desc, _usage, _deprecated, _options, action) \
static struct command _callback ## _cmd = {	\
	.name = _name, 				\
	.short_desc = _short_desc, 		\
	.long_desc = _long_desc, 		\
	.usage = _usage, 			\
	.callback = &_callback,			\
	.deprecated = _deprecated,		\
	.options = _options,			\
};

#undef COMMAND_LIST
#define COMMAND_LIST(args...) args

#include CMD_INCLUDE(CMD_INC_FILE)

/*
 * Stage 3
 *
 * Generate list of commands for the plugin.
 */
#undef PROGRAM
#define PROGRAM(_name, _version, _short_desc, _long_desc, _usage)

#undef OPTION_LIMIT
#define OPTION_LIMIT(type, var, short_opt, long_opt, desc, meta, value, limit, deprecated)

#undef OPTIONS
#define OPTIONS(args...)  args

#undef COMMAND
#define COMMAND(name, callback, short_desc, long_desc, usage, deprecated, options, action) \
	&callback ## _cmd,

#undef COMMAND_LIST
#define COMMAND_LIST(args...)	\
static struct command *commands[] = {	\
	args				\
	NULL				\
};

#include CMD_INCLUDE(CMD_INC_FILE)

/*
 * Stage 4
 *
 * Define callback.
 */
#undef PROGRAM
#define PROGRAM(_name, _version, _short_desc, _long_desc, _usage)

#undef OPTION_LIMIT
#define OPTION_LIMIT(type, var, short_opt, long_opt, desc, meta, _value, limit, deprecated) \
	type var = (type)_value;	\
	command->options[option_index++].value = (void*)&var;

#undef OPTIONS
#define OPTIONS(args...)  args

#undef ACTION
#define ACTION(args...) args

#undef COMMAND
#define COMMAND(name, callback, short_desc, long_desc, usage, deprecated, options, action)	\
static int callback(int argc, char **argv, struct command *command)	\
{						\
	int ret, option_index = 0; 		\
	options					\
	ret = parse_args(argc, argv, command);	\
	if (!ret) {				\
		action				\
	}					\
	return ret;				\
}

#undef COMMAND_LIST
#define COMMAND_LIST(args...) args

#include CMD_INCLUDE(CMD_INC_FILE)

/*
 * Stage 5
 *
 * Define and execute the program.
 */
#undef PROGRAM
#define PROGRAM(_name, _version, _short_desc, _long_desc, _usage)	\
static struct program program = {				\
	.version = _version,					\
	.command = {						\
		.name = _name,					\
		.short_desc = _short_desc,			\
		.long_desc = _long_desc,			\
		.usage = _usage,				\
		.subcommands = commands, 			\
	},							\
};								\
int main(int argc, char **argv)					\
{								\
	if (parse_args(argc, argv, &program.command))		\
		exit(errno);					\
	return 0;						\
}

#undef COMMAND
#define COMMAND(name, callback, short_desc, long_desc, usage, deprecated, options, action)

#undef COMMAND_LIST
#define COMMAND_LIST(args...)

#undef OPTION_LIMIT
#define OPTION_LIMIT(type, var, short_opt, long_opt, desc, meta, value, limit, deprecated)

#undef OPTIONS
#define OPTIONS(args...)

#include CMD_INCLUDE(CMD_INC_FILE)

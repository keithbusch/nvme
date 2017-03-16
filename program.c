#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "program.h"

int print_word_wrapped(const char *s, int indent, int start)
{
	const int width = 80;
	const char *c, *t;
	int next_space = -1;
	int last_line = indent;

	while (start < indent) {
		printf(" ");
		start++;
	}

	for (c = s; *c != 0; c++) {
		if (*c == ' ' || next_space < 0) {
			next_space = 0;
			for (t = c + 1; *t != 0 && *t != ' '; t++)
				next_space++;

			if (((int)(c - s) + start + next_space) >
					(last_line - indent + width)) {
				int i;
				last_line = (int) (c-s) + start;
				printf("\n");
				for (i = 0; i < indent; i++)
					printf(" ");
				start = indent;
				continue;
			}
		}
		printf("%c", *c);
	}
	return (int) (c - s) + start - last_line + indent;
}

static int type_to_has_arg(enum option_type t)
{
	switch(t) {
	case TYPE_FLAG:
	case TYPE_INC:
	     return no_argument;
	case TYPE_FMT:
	case TYPE_U8:
	case TYPE_U16:
	case TYPE_U32:
	case TYPE_U64:
	case TYPE_U8_LIST:
	case TYPE_U16_LIST:
	case TYPE_U32_LIST:
	case TYPE_U64_LIST:
	case TYPE_STRING:
	case TYPE_FILE_A:
	case TYPE_FILE_R:
	case TYPE_FILE_W:
	case TYPE_FILE_AP:
	case TYPE_FILE_RP:
	case TYPE_FILE_WP:
		return required_argument;
	default:
		fprintf(stderr, "unknown argument type:%d\n", t);
		return -1;
	}
}

static void short_option_has_arg(struct command_option *option, char **short_opts)
{
	switch(type_to_has_arg(option->type)) {
	case optional_argument:	*(*short_opts)++ = ':'; /* fall through */
	case required_argument:	*(*short_opts)++ = ':'; /* fall through */
	case no_argument:	break;
	default:
		fprintf(stderr, "an error occured parsing short list arguments\n");
	}
}

static void show_option(struct command_option *option)
{
	int has_arg = type_to_has_arg(option->type);
	char buffer[0x1000];
	char *b = buffer;

	b += sprintf(b, "  [ ");
	if (option->long_option) {
		b += sprintf(b, " --%s", option->long_option);
		if (has_arg == optional_argument)
			b += sprintf(b, "[=<%s>]", option->meta ? option->meta : "arg");
		if (has_arg == required_argument)
			b += sprintf(b, "=<%s>", option->meta ? option->meta : "arg");
	}
	if (option->short_option && option->long_option)
		b += sprintf(b, ",");
	if (option->short_option) {
		b += sprintf(b, " -%c", option->short_option);
		if (has_arg == optional_argument)
			b += sprintf(b, " [<%s>]", option->meta ? option->meta : "arg");
		if (has_arg == required_argument)
			b += sprintf(b, " <%s>", option->meta ? option->meta : "arg");
	}
	b += sprintf(b, " ] ");

	if (option->desc)
		b += sprintf(b, "--- ");
	fprintf(stderr, "%s", buffer);

	if (option->desc)
		print_word_wrapped(option->desc, 30, b - buffer);
	fprintf(stderr, "\n");
}

static void show_command(struct command *command, int level)
{
	fprintf(stderr, "  %-*s %s\n", 15, command->name, command->short_desc);

	if (level == 0)
		return;

	fprintf(stderr, "\n");
	print_word_wrapped(command->long_desc, 0, 0);

	if (level == 1)
		return;

	if (command->options[0].type != TYPE_END) {
		int i = 0;

		fprintf(stderr, "\n\nOptions:\n");
		while (command->options[i].type != TYPE_END) {
			show_option(&command->options[i]);
			i++;
		}
	} else
		fprintf(stderr, "\n");
	fprintf(stderr, "\n");
}

static void show_subcommands(struct command **commands)
{
	int i = 0;

	while (commands[i])
		show_command(commands[i++], 0);
}

static int parse_format(int *ret, char *format)
{
	if (!format)
		goto err;
	else if (!strcmp(format, "normal"))
		*ret = NORMAL;
	else if (!strcmp(format, "json"))
		*ret = JSON;
	else if (!strcmp(format, "binary"))
		*ret = BINARY;
	else if (!strcmp(format, "hex"))
		*ret = HEX;
	else
		goto err;
	return 0;
 err:
	errno = EINVAL;
	return -1;
}

static int parse_ulong(U64 *val, char *value, U64 limit)
{
	U64 ret;
	char *endptr;

	if (!value)
		goto err;

	errno = 0;
	ret = strtoll(value, &endptr, 0);
	if (errno || value == endptr) {
		goto err;
	}
	if (ret > limit) {
		fprintf(stderr, "value:%s exceeds limit:%lu\n", value, limit);
		errno = E2BIG;
		goto err;
	}
	*val = ret;
	return 0;
 err:
	errno = errno ? errno : EINVAL;
	return -1;
}

static int parse_u64(U64 *ret, char *value, __u64 limit)
{
	U64 val;
	int err = parse_ulong(&val, value, limit);

	if (!err)
		*ret = val;
	return err;
}

static int parse_u32(U32 *ret, char *value, __u32 limit)
{
	U64 val;
	int err = parse_ulong(&val, value, limit);

	if (!err)
		*ret = (U32)val;
	return err;
}

static int parse_u16(U16 *ret, char *value, __u16 limit)
{
	U64 val;
	int err = parse_ulong(&val, value, limit);

	if (!err)
		*ret = (U16)val;
	return err;
}

static int parse_u8(U8 *ret, char *value, __u8 limit)
{
	U64 val;
	int err = parse_ulong(&val, value, limit);

	if (!err)
		*ret = (U8)val;
	return err;
}

static int parse_list(struct command_option *option, char *arg)
{
	int err;
	char *tmp;
	void *next, *list = malloc(0x1000);

	next = list;
	tmp = strtok(arg, ",");
	while (tmp) {
		if (next - list >= 0x1000) {
			errno = E2BIG;
			err = -1;	
			goto free;
		}

		switch (option->type) {
		case TYPE_U8_LIST:
			err = parse_u8(next, optarg, option->limit);
			next += sizeof(U8);
			break;
		case TYPE_U16_LIST:
			err = parse_u16(next, optarg, option->limit); 
			next += sizeof(U16);
			break;
		case TYPE_U32_LIST:
			err = parse_u32(next, optarg, option->limit);
			next += sizeof(U32);
			break;
		case TYPE_U64_LIST:
			err = parse_u64(next, optarg, option->limit);
			next += sizeof(U64);
			break;
		default:
			err = -1;
			break;
		}
		if (err)
			goto free;
		tmp = strtok (NULL, ",");
	}

	option->value = list;
	return 0;
 free:
	free(list);
	return -1;
}

static int parse_subcommand(int argc, char **argv, struct command *command)
{
	int i;
	struct command **commands = command->subcommands;

	if (argc < 2) {
		show_subcommands(command->subcommands);
		return 0;
	}

	i = 0;
	while (commands[i]) {
		if (commands[i]->subcommands) {
			if (!strcmp(argv[1], commands[i]->name))
				return parse_subcommand(argc - 1, &argv[1], commands[i]);
			if (!strncmp(argv[0], commands[i]->name, strlen(commands[i]->name))) {
				argv[0] += strlen(commands[i]->name);
				while (*argv[0] == '-')
					argv[0]++;
				return parse_subcommand(argc, argv, commands[i]);
			}
		}
		if (!strcmp(argv[optind], commands[i]->name)) {
			if (commands[i]->callback)
				return commands[i]->callback(argc, argv, commands[i]);
			break;
		}
		i++;
	}
	errno = EINVAL;
	return -1;
}

static int __parse_args(int argc, char **argv, int num_opts, struct command *command)
{
	int i, c, err = 0, li = 0, index = 0;
	struct option long_opts[num_opts + 2];
	char short_opts[num_opts * 3 + 1];
	char *short_opts_ptr = short_opts;
	struct command_option *option, *options = command->options;

	optind = 1;

	memset(short_opts, 0, sizeof(short_opts));
	memset(long_opts, 0, sizeof(long_opts));
	for (i = 0; i < num_opts; i++) {
		if (options[i].short_option != 0) {
			*short_opts_ptr++ = options[i].short_option;
			short_option_has_arg(&options[i], &short_opts_ptr);
		}
		if (options[i].long_option && strlen(options[i].long_option)) {
			long_opts[li].name = options[i].long_option;
			long_opts[li].has_arg = type_to_has_arg(options[i].type);
			long_opts[li].flag = NULL;
			long_opts[li].val = 0;
			li++;
		}
	}
	long_opts[li].name = "help";
	long_opts[li].flag = NULL;
	long_opts[li].val = 'h';
	li++;

	long_opts[li].name = NULL;
	long_opts[li].flag = NULL;
	long_opts[li].val  = 0;

	*short_opts_ptr++ = 'h';
	*short_opts_ptr++ = '\0';

	while ((c = getopt_long(argc, argv, short_opts, long_opts, &index)) != -1) {
		switch (c) {
		case 0:
			break;
		case 'h':
		case '?':
			show_command(command, 2);
			return 0;
		default:
			for (index = 0; index < num_opts; index++)
				if (c == options[index].short_option)
					break;
			if (index < num_opts)
				break;
			continue;
		}

		option = &options[index];
		switch (option->type) {
		case TYPE_U8:
			err = parse_u8(option->value, optarg, option->limit);
			break;
		case TYPE_U16:
			err = parse_u16(option->value, optarg, option->limit); 
			break;
		case TYPE_U32:
			err = parse_u32(option->value, optarg, option->limit);
			break;
		case TYPE_U64:
			err = parse_u64(option->value, optarg, option->limit);
			break;
		case TYPE_FMT:
			err = parse_format(option->value, optarg);
			break;
		case TYPE_FLAG:
			*((bool *)option->value) = true;
			break;
		case TYPE_INC:
			(*((U32 *)option->value))++;
			break;
		case TYPE_STRING:
			*((char **)option->value) = optarg;
			break;
		case TYPE_U8_LIST:
		case TYPE_U16_LIST:
		case TYPE_U32_LIST:
		case TYPE_U64_LIST:
			err = parse_list(option, optarg);
			break;
		case TYPE_FILE_A:
		case TYPE_FILE_R:
		case TYPE_FILE_W:
		case TYPE_FILE_AP:
		case TYPE_FILE_RP:
		case TYPE_FILE_WP: {
			const char *fopts = "";

			if (option->type == TYPE_FILE_A)
				fopts = "a";
			else if (option->type == TYPE_FILE_R)
				fopts = "r";
			else if (option->type == TYPE_FILE_W)
				fopts = "w";
			else if (option->type == TYPE_FILE_AP)
				fopts = "a+";
			else if (option->type == TYPE_FILE_RP)
				fopts = "r+";
			else if (option->type == TYPE_FILE_WP)
				fopts = "w+";

			*((FILE **)option->value) = fopen(optarg, fopts);
			if (*((FILE **)option->value) == NULL) {
				fprintf(stderr, "%s: unable to open file %s\n",
					option->long_option, optarg);
				err = -1;
			}
			break;
		}
		default:
			errno = EINVAL;
			err = -1;
			break;
		}
		if (err)
			return err;
	}
	return 0;
}

int parse_args(int argc, char **argv, struct command *command)
{
	int num_opts = 0;

	if (command->subcommands)
		return parse_subcommand(argc, argv, command);
	while (command->options[num_opts].type != TYPE_END)
		num_opts++;
	return __parse_args(argc, argv, num_opts, command);
}

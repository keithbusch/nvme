/*
 * Stage 1
 *
 * Define the structure and prototype
 */
#undef TSTRUCT
#define TSTRUCT(name, entries) \
struct name {	\
	entries	\
};		\
extern void print_ ## name(struct name *tstruct, int pad);

#undef TSTRUCT_entries
#define TSTRUCT_entries(args...) args

#undef __field
#define __field(type, name) \
	type name;

#undef l_field
#define l_field(type, name) \
	type name;

#undef __array
#define __array(type, name, elems) \
	type name[elems];

#undef r_array
#define r_array(type, name, elems) \
	type name[elems];

#undef v_array
#define v_array(type, name, elems) \
	type name[elems];

#undef r_field
#define r_field(type, name) \
	type name;

#undef __array_struct
#define __array_struct(type, name, elems, field_cap)	\
	struct type name[elems];

#include TSTRUCT_INCLUDE(TSTRUCT_INC_FILE)

#ifdef TSTRUCT_SOURCE
/*
 * Stage 2
 *
 * Find the longest string for printing.
 *
 * Note: The compiler squashes the size.
 */
#undef TSTRUCT
#define TSTRUCT(name, entries)	\
static int name ## _pad(int pad)\
{				\
	entries			\
	return pad;		\
}

#undef TSTRUCT_entries
#define TSTRUCT_entries(args...) args

#undef __field
#define __field(type, name) \
	pad = max(pad, sizeof(__stringify(name)));

#undef l_field
#define l_field(type, name) __field(type, name)

#undef r_field
#define r_field(type, name)

#undef __array
#define __array(type, name, elems) \
	pad = max(pad, sizeof(__stringify(name)) + 5);

#undef r_array
#define r_array(type, name, elems)

#undef v_array
#define v_array(type, name, elems)

#undef __array_struct
#define __array_struct(type, name, elem, field_cap)	\
	pad = max(pad, max(type ## _pad(0), sizeof(__stringify(name)) + 4));

#include TSTRUCT_INCLUDE(TSTRUCT_INC_FILE)

/*
 * Stage 3
 *
 * Print the structure
 */
#undef TSTRUCT
#define TSTRUCT(name, entries) \
void print_ ## name(struct name *tstruct, int pad) {	\
	int i; 						\
	pad = name ## _pad(pad);			\
	(void)i;					\
	entries						\
}

#undef TSTRUCT_entries
#define TSTRUCT_entries(args...) args

#define __field_fmt(name, fmt) \
	printf("%*s : "fmt"\n", pad, __stringify(name), tstruct->name);

#undef __field
#define __field(type, name) __field_fmt(name, "%x")

#undef l_field
#define l_field(type, name) __field_fmt(name, "%llx");

#undef r_field
#define r_field(type, name) \

#undef __array
#define __array(type, name, elems) \
	printf("%*s :\n", pad, __stringify(name));	\
	for (i = 0; i < elems; i++) 			\
		printf("%*s[%3d] : %lx\n", pad - 5, __stringify(name), i, (uint64_t)tstruct->name[i]);

#undef r_array
#define r_array(type, name, elems)

#undef v_array
#define v_array(type, name, elems)

#undef __array_struct
#define __array_struct(type, name, elems, field_cap) 			\
	printf("%*s :\n", pad, __stringify(name));			\
	for (i = 0; i < tstruct->field_cap; i++) {			\
		printf("%*s[%3d] :\n", pad - 5, __stringify(name), i);	\
		print_ ## type(&tstruct->name[i], pad);			\
	}

#include TSTRUCT_INCLUDE(TSTRUCT_INC_FILE)

#endif

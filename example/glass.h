#ifndef H_MECHASCRIBE_EXAMPLE_GLASS
#define H_MECHASCRIBE_EXAMPLE_GLASS

#include <stddef.h>

struct eat_glass_sentence
{
	const char* text; // the sentence
	const char* bcp47; // the standard code for the language
	const char* language; // the english name for the language
};

void eat_glass_get_list(
	const struct eat_glass_sentence** out_list,
	size_t* out_size);

#endif

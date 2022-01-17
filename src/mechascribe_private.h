#ifndef H_MECHASCRIBE_PRIVATE
#define H_MECHASCRIBE_PRIVATE

#include "mechascribe.h"
#include "hydraquill.h"
#include "ft2build.h"
#include <stddef.h>
#include <stdint.h>

// required for libraqm since we are using a local freetype
#ifndef FT_FREETYPE_H
#define FT_FREETYPE_H "freetype/freetype.h"
#endif

#ifndef FT_CACHE_H
#define FT_CACHE_H "freetype/ftcache.h"
#endif

#include FT_FREETYPE_H
#include FT_CACHE_H
#include "raqm.h"

// private structures

// the `font_face` was added for completeness but will always be 0
// for TTF and OTF files since they can only contain one single face:
// this feature is only relevant for TTC and OTC font collection files
struct mechascribe_font_node
{
	char* font_path;
	FT_Long font_face;
	FTC_Scaler scaler;
	struct mechascribe_font_node* next;
};

struct mechascribe_font_range
{
	struct mechascribe_font_node* font;
	size_t len;
};

// main context
struct mechascribe
{
	// main font list > emoji font > last resort font
	struct mechascribe_font_node font_emoji;
	struct mechascribe_font_node* font_list;
	struct mechascribe_font_node* font_fallback_list;

	// font loading helpers
	const char* font_emoji_name;
	const char** font_fallback_name_list;
	size_t font_fallback_count;

	// list of contiguous characters using the same font
	struct mechascribe_font_range* font_range_list;
	size_t font_range_count;

	// text info
	uint32_t* text;
	size_t text_len;
	enum mechascribe_direction text_direction;

	// third-party contexts
	raqm_t* raqm;
	raqm_glyph_t* raqm_glyphs;

	FT_Library freetype;
	FTC_Manager freetype_face_cache;
	FTC_CMapCache freetype_charmap_cache;
	FTC_ImageCache freetype_image_cache;
};

#endif

#ifndef H_MECHASCRIBE
#define H_MECHASCRIBE

#include <stddef.h>
#include <stdint.h>

// public structures

enum mechascribe_error
{
	MECHASCRIBE_ERROR_OK = 0,
	MECHASCRIBE_ERROR_ALLOC,
	MECHASCRIBE_ERROR_MKDIR_MAX,
	MECHASCRIBE_ERROR_MKDIR,
	MECHASCRIBE_ERROR_FONT_INTEGRITY,
	MECHASCRIBE_ERROR_FONT_LIST,
	MECHASCRIBE_ERROR_FONT_UNPACK,
	MECHASCRIBE_ERROR_FONT_LASTRESORT,
	MECHASCRIBE_ERROR_RAQM_INIT,
	MECHASCRIBE_ERROR_RAQM_TEXT,
	MECHASCRIBE_ERROR_RAQM_DIRECTION,
	MECHASCRIBE_ERROR_RAQM_LANGUAGE,
	MECHASCRIBE_ERROR_RAQM_FONT_RANGE,
	MECHASCRIBE_ERROR_RAQM_TEXT_LAYOUT,
	MECHASCRIBE_ERROR_RAQM_GLYPHS,
	MECHASCRIBE_ERROR_RAQM_CHAR_TO_CUR,
	MECHASCRIBE_ERROR_RAQM_CUR_TO_CHAR,
	MECHASCRIBE_ERROR_FREETYPE_INIT,
	MECHASCRIBE_ERROR_FREETYPE_FACE_CACHE,
	MECHASCRIBE_ERROR_FREETYPE_CHARMAP_CACHE,
	MECHASCRIBE_ERROR_FREETYPE_IMAGE_CACHE,
	MECHASCRIBE_ERROR_FREETYPE_FACE_LOOKUP,
	MECHASCRIBE_ERROR_FREETYPE_GET_GLYPH,
};

enum mechascribe_direction
{
	MECHASCRIBE_DIRECTION_DEFAULT = 0,
	MECHASCRIBE_DIRECTION_LEFT_TO_RIGHT,
	MECHASCRIBE_DIRECTION_RIGHT_TO_LEFT,
	MECHASCRIBE_DIRECTION_TOP_TO_BOTTOM,
};

enum mechascribe_render_type
{
	MECHASCRIBE_RENDER_TYPE_CLASSIC = 0,
	MECHASCRIBE_RENDER_TYPE_SDF,
};

enum mechascribe_buffer_type
{
	MECHASCRIBE_RENDER_TYPE_BYTE = 0,
	MECHASCRIBE_RENDER_TYPE_BGRA,
};

struct mechascribe_buffer
{
	void* lock;
	void* buffer;
	int buffer_pitch;
	enum mechascribe_buffer_type buffer_type;

	int glyph_x;
	int glyph_y;
	unsigned glyph_width;
	unsigned glyph_height;

	signed long advance_x;
	signed long advance_y;
};

// private context (defined in mechascribe_private.h)

struct mechascribe;

// library management

// create a mechascribe context
enum mechascribe_error mechascribe_init(
	struct mechascribe** ctx);

// get messages for each error code
void mechascribe_init_errors(
	char** msg);

// destroy a mechascribe context and its associated resources
void mechascribe_free(
	struct mechascribe* ctx);

// load Noto font files
enum mechascribe_error mechascribe_prepare_fonts(
	struct mechascribe* ctx,
	const char* font_folder,
	const char** font_fallback_name_list,
	size_t font_fallback_count);

// extract Noto font files
enum mechascribe_error mechascribe_unpack_fonts(
	struct mechascribe* ctx,
	const char* archive_file,
	const char* font_folder);

// set font height
void mechascribe_set_font_height(
	struct mechascribe* ctx,
	unsigned font_height);

// text preparation

// set the text to render
enum mechascribe_error mechascribe_set_text(
	struct mechascribe* ctx,
	const char* str_utf8);

// set the general writing direction to use (automatically detected by default)
enum mechascribe_error mechascribe_set_direction(
	struct mechascribe* ctx,
	enum mechascribe_direction direction);

// set the language to use for a given character range (optional)
enum mechascribe_error mechascribe_set_language(
	struct mechascribe* ctx,
	const char* bcp47_code,
	size_t char_first,
	size_t char_count);

// text layout

// find a suitable font for each character of the text and create common ranges
enum mechascribe_error mechascribe_associate_fonts(
	struct mechascribe* ctx);

// perform text shaping and layout through libraqm
enum mechascribe_error mechascribe_perform_layout(
	struct mechascribe* ctx);

// generate glyph information for use with our rendering code
enum mechascribe_error mechascribe_generate_glyphs(
	struct mechascribe* ctx,
	size_t* glyph_count);

// render the glyph using FreeType
enum mechascribe_error mechascribe_render_glyph(
	struct mechascribe* ctx,
	size_t glyph_id,
	enum mechascribe_render_type render_type,
	struct mechascribe_buffer* buffer);

// release the glyph buffer lock
void mechascribe_render_glyph_done(
	struct mechascribe* ctx,
	void* lock);

// destroy the internal raqm context
void mechascribe_render_done(
	struct mechascribe* ctx);

// utils

// get the position of a given character
enum mechascribe_error mechascribe_char_to_cursor_pos(
	struct mechascribe* ctx,
	size_t index,
	int* x,
	int* y);

// get the character at a given position
enum mechascribe_error mechascribe_cursor_pos_to_char(
	struct mechascribe* ctx,
	int x,
	int y,
	size_t* index);

#endif

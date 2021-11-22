#include "mechascribe.h"
#include "mechascribe_private.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

static FT_Error get_face(
	FTC_FaceID id,
	FT_Library library,
	FT_Pointer data,
	FT_Face* face)
{
	// create the face
	struct mechascribe_font_node* node =
		(struct mechascribe_font_node*) id;

	FT_Error freetype_err =
		FT_New_Face(
			library,
			node->font_path,
			node->font_face,
			face);

	if (freetype_err != FT_Err_Ok)
	{
		return freetype_err;
	}

	// select the charmap type
	freetype_err =
		FT_Select_Charmap(
			*face,
			FT_ENCODING_UNICODE);

	if (freetype_err != FT_Err_Ok)
	{
		return freetype_err;
	}

	// store the face_id as client data in the face
	(*face)->generic.data = id;
	(*face)->generic.finalizer = NULL;

	return FT_Err_Ok;
}

enum mechascribe_error mechascribe_init(
	struct mechascribe** ctx)
{
	struct mechascribe* tmp = malloc(sizeof (struct mechascribe));

	if (tmp == NULL)
	{
		return MECHASCRIBE_ERROR_ALLOC;
	}

	// fonts
	tmp->font_list = NULL;
	tmp->font_fallback_list = NULL;
	tmp->font_fallback_count = 0;

	// helpers
	tmp->font_fallback_name_list = NULL;

	// ranges
	tmp->font_range_list = NULL;
	tmp->font_range_count = 0;

	// text
	tmp->text = NULL;
	tmp->text_len = 0;
	tmp->text_direction = MECHASCRIBE_DIRECTION_DEFAULT;

	// init raqm
	tmp->raqm_glyphs = NULL;
	tmp->raqm = NULL;

	// init freetype
	FT_Error freetype_err = FT_Init_FreeType(&(tmp->freetype));

	if (freetype_err != FT_Err_Ok)
	{
		free(tmp);
		return MECHASCRIBE_ERROR_FREETYPE_INIT;
	}

	// allocate freetype face cache
	freetype_err =
		FTC_Manager_New(
			tmp->freetype,
			0,
			0,
			0,
			get_face,
			NULL,
			&(tmp->freetype_face_cache));

	if (freetype_err != FT_Err_Ok)
	{
		FT_Done_FreeType(tmp->freetype);
		free(tmp);
		return MECHASCRIBE_ERROR_FREETYPE_FACE_CACHE;
	}

	// allocate freetype charmap cache
	freetype_err =
		FTC_CMapCache_New(
			tmp->freetype_face_cache,
			&(tmp->freetype_charmap_cache));

	if (freetype_err != FT_Err_Ok)
	{
		FTC_Manager_Done(tmp->freetype_face_cache);
		FT_Done_FreeType(tmp->freetype);
		free(tmp);
		return MECHASCRIBE_ERROR_FREETYPE_CHARMAP_CACHE;
	}

	// allocate freetype image cache
	freetype_err =
		FTC_ImageCache_New(
			tmp->freetype_face_cache,
			&(tmp->freetype_image_cache));

	if (freetype_err != FT_Err_Ok)
	{
		FTC_Manager_Done(tmp->freetype_face_cache);
		FT_Done_FreeType(tmp->freetype);
		free(tmp);
		return MECHASCRIBE_ERROR_FREETYPE_IMAGE_CACHE;
	}

	// save context
	*ctx = tmp;

	return MECHASCRIBE_ERROR_OK;
}

void mechascribe_init_errors(char** msg)
{
	msg[MECHASCRIBE_ERROR_OK] =
		"no error";
	msg[MECHASCRIBE_ERROR_ALLOC] =
		"failed memory allocation";
	msg[MECHASCRIBE_ERROR_MKDIR_MAX] =
		"failed path length check";
	msg[MECHASCRIBE_ERROR_MKDIR] =
		"could not make directory";
	msg[MECHASCRIBE_ERROR_FONT_INTEGRITY] =
		"failed font integrity check";
	msg[MECHASCRIBE_ERROR_FONT_LIST] =
		"could not load some fonts";
	msg[MECHASCRIBE_ERROR_FONT_UNPACK] =
		"could not unpack some fonts";
	msg[MECHASCRIBE_ERROR_FONT_LASTRESORT] =
		"could not find some glyphs in any font";
	msg[MECHASCRIBE_ERROR_RAQM_INIT] =
		"could not initialize libraqm";
	msg[MECHASCRIBE_ERROR_RAQM_TEXT] =
		"could not set text with libraqm";
	msg[MECHASCRIBE_ERROR_RAQM_DIRECTION] =
		"could not set direction with libraqm";
	msg[MECHASCRIBE_ERROR_RAQM_LANGUAGE] =
		"could not set language with libraqm";
	msg[MECHASCRIBE_ERROR_RAQM_FONT_RANGE] =
		"could not set font with libraqm";
	msg[MECHASCRIBE_ERROR_RAQM_TEXT_LAYOUT] =
		"could not perform layout with libraqm";
	msg[MECHASCRIBE_ERROR_RAQM_GLYPHS] =
		"could not get layed-out glyphs with libraqm";
	msg[MECHASCRIBE_ERROR_RAQM_CHAR_TO_CUR] =
		"could not get character from position with libraqm";
	msg[MECHASCRIBE_ERROR_RAQM_CUR_TO_CHAR] =
		"could not get position from character with libraqm";
	msg[MECHASCRIBE_ERROR_FREETYPE_INIT] =
		"could not initialize FreeType";
	msg[MECHASCRIBE_ERROR_FREETYPE_FACE_CACHE] =
		"could not create a new face cache with FreeType";
	msg[MECHASCRIBE_ERROR_FREETYPE_CHARMAP_CACHE] =
		"could not create a new charmap cache with FreeType";
	msg[MECHASCRIBE_ERROR_FREETYPE_IMAGE_CACHE] =
		"could not create a new image cache with FreeType";
	msg[MECHASCRIBE_ERROR_FREETYPE_FACE_LOOKUP] =
		"could not find font face with FreeType";
	msg[MECHASCRIBE_ERROR_FREETYPE_GET_GLYPH] =
		"could not render glyph with FreeType";
}

void mechascribe_free(
	struct mechascribe* ctx)
{
	// main font list
	struct mechascribe_font_node* node = ctx->font_list;
	struct mechascribe_font_node* tmp;

	while (node != NULL)
	{
		tmp = node->next;

		free(node->font_path);
		free(node->scaler);
		free(node);

		node = tmp;
	}

	for (size_t i = 0; i < ctx->font_fallback_count; ++i)
	{
		free(ctx->font_fallback_list[i].font_path);
		free(ctx->font_fallback_list[i].scaler);
	}

	if (ctx->font_fallback_list != NULL)
	{
		free(ctx->font_fallback_list);
	}

	// freetype
	FTC_Manager_Done(ctx->freetype_face_cache);
	FT_Done_FreeType(ctx->freetype);

	// main context
	free(ctx);
}

void mechascribe_set_font_height(
	struct mechascribe* ctx,
	unsigned font_height)
{
	struct mechascribe_font_node* node = ctx->font_list;

	while (node != NULL)
	{
		node->scaler->height = font_height;
		node->scaler->width = font_height;
		node = node->next;
	}

	for (size_t i = 0; i < ctx->font_fallback_count; ++i)
	{
		ctx->font_fallback_list[i].scaler->height = font_height;
		ctx->font_fallback_list[i].scaler->width = font_height;
	}
}

enum mechascribe_error mechascribe_set_text(
	struct mechascribe* ctx,
	const char* input)
{
	const char* utf8 = input;
	ptrdiff_t len = strlen(input);
	bool fallback = false;

	ctx->font_range_count = 0;

	// allocate font range array
	ctx->font_range_list =
		malloc(
			len * (sizeof (struct mechascribe_font_range)));

	if (ctx->font_range_list == NULL)
	{
		return MECHASCRIBE_ERROR_ALLOC;
	}

	// allocate utf-32 string
	ctx->text =
		malloc(
			len * (sizeof (uint32_t)));

	if (ctx->text == NULL)
	{
		free(ctx->font_range_list);
		return MECHASCRIBE_ERROR_ALLOC;
	}

	// convert text from utf-8 to utf-32
	struct mechascribe_font_node* node_range = NULL;
	struct mechascribe_font_node* node_last = NULL;
	struct mechascribe_font_node* node_tmp = ctx->font_list;

	struct mechascribe_font_range* range_tmp = ctx->font_range_list;
	uint32_t* utf32 = ctx->text;
	FT_UInt glyph = 0;

	while (len > (utf8 - input))
	{
		if (0xf0 == (0xf8 & utf8[0]))
		{
			*utf32 =
				((0x07 & utf8[0]) << 18)
				| ((0x3f & utf8[1]) << 12)
				| ((0x3f & utf8[2]) << 6)
				| (0x3f & utf8[3]);

			utf8 += 4;
		}
		else if (0xe0 == (0xf0 & utf8[0]))
		{
			*utf32 =
				((0x0f & utf8[0]) << 12)
				| ((0x3f & utf8[1]) << 6)
				| (0x3f & utf8[2]);

			utf8 += 3;
		}
		else if (0xc0 == (0xe0 & utf8[0]))
		{
			*utf32 =
				((0x1f & utf8[0]) << 6)
				| (0x3f & utf8[1]);

			utf8 += 2;
		}
		else
		{
			*utf32 = utf8[0];

			utf8 += 1;
		}

		// the unoptimized sequence that follows
		// was chosen for its readability
		glyph = 0;

		// search for the character's glyph in the last font
		// we used unless this font was not unicode's lastresort
		if (fallback == false)
		{
			glyph =
				FTC_CMapCache_Lookup(
					ctx->freetype_charmap_cache,
					node_tmp,
					-1,
					*utf32);
		}

		// reset `node_last` and `node_tmp` and
		// search in the main list from the beginning
		if (glyph == 0)
		{
			fallback = false;
			node_last = NULL;
			node_tmp = ctx->font_list;

			glyph =
				FTC_CMapCache_Lookup(
					ctx->freetype_charmap_cache,
					node_tmp,
					-1,
					*utf32);
		}

		// break the loop here assuming it will be optimized away
		// because doing otherwise makes the code utterly unreadable
		while (glyph == 0)
		{
			node_last = node_tmp;
			node_tmp = node_tmp->next;

			if (node_tmp == NULL)
			{
				node_last = NULL;
				size_t i = 0;

				while (i < ctx->font_fallback_count)
				{
					node_tmp = &(ctx->font_fallback_list[i]);

					glyph =
						FTC_CMapCache_Lookup(
							ctx->freetype_charmap_cache,
							node_tmp,
							-1,
							*utf32);

					if (glyph != 0)
					{
						break;
					}

					++i;
				}

				if (glyph != 0)
				{
					fallback = ((ctx->font_fallback_count - i) <= 1);
					break;
				}

				// this should never happen
				free(ctx->text);
				free(ctx->font_range_list);
				return MECHASCRIBE_ERROR_FONT_LASTRESORT;
			}

			glyph =
				FTC_CMapCache_Lookup(
					ctx->freetype_charmap_cache,
					node_tmp,
					-1,
					*utf32);
		}

		// select and initialize a new range if the adequate font
		// for this glyph is not the same as for the previous ones
		if (node_tmp != node_range)
		{
			// save the range font
			node_range = node_tmp;

			// move range font on top of the font list
			if (node_last != NULL)
			{
				node_last->next = node_tmp->next;
				node_tmp->next = ctx->font_list;
				ctx->font_list = node_tmp;
			}

			// select a new range slot
			range_tmp = ctx->font_range_list + ctx->font_range_count;
			++(ctx->font_range_count);

			// add some new range info in the new slot
			range_tmp->font = node_range;
			range_tmp->len = 1;
		}
		// increment the number of glyphs using
		// this range's font if it still the same
		else
		{
			++(range_tmp->len);
		}

		++utf32;
	}

	// initialize raqm
	ctx->raqm = raqm_create();

	if (ctx->raqm == NULL)
	{
		free(ctx->text);
		free(ctx->font_range_list);
		return MECHASCRIBE_ERROR_RAQM_INIT;
	}

	// send the utf-32 encoded text to libraqm
	ctx->text_len = utf32 - ctx->text;

	bool raqm_err =
		raqm_set_text(
			ctx->raqm,
			ctx->text,
			ctx->text_len);

	if (raqm_err == false)
	{
		free(ctx->text);
		free(ctx->font_range_list);
		raqm_destroy(ctx->raqm);
		return MECHASCRIBE_ERROR_RAQM_TEXT;
	}

	return MECHASCRIBE_ERROR_OK;
}

enum mechascribe_error mechascribe_set_direction(
	struct mechascribe* ctx,
	enum mechascribe_direction direction)
{
	bool raqm_err;
	ctx->text_direction = direction;

	switch (direction)
	{
		case MECHASCRIBE_DIRECTION_LEFT_TO_RIGHT:
		{
			raqm_err =
				raqm_set_par_direction(
					ctx->raqm,
					RAQM_DIRECTION_LTR);
			break;
		}
		case MECHASCRIBE_DIRECTION_RIGHT_TO_LEFT:
		{
			raqm_err =
				raqm_set_par_direction(
					ctx->raqm,
					RAQM_DIRECTION_RTL);
			break;
		}
		case MECHASCRIBE_DIRECTION_TOP_TO_BOTTOM:
		{
			raqm_err =
				raqm_set_par_direction(
					ctx->raqm,
					RAQM_DIRECTION_TTB);
			break;
		}
		case MECHASCRIBE_DIRECTION_DEFAULT:
		default:
		{
			raqm_err = true;
			break;
		}
	}

	if (raqm_err == false)
	{
		return MECHASCRIBE_ERROR_RAQM_DIRECTION;
	}

	return MECHASCRIBE_ERROR_OK;
}

// automatically computes the text length if char_first == char_count == 0
enum mechascribe_error mechascribe_set_language(
	struct mechascribe* ctx,
	const char* bcp47_code,
	size_t char_first,
	size_t char_count)
{
	// select length
	size_t len;

	if ((char_first == 0) && (char_count == 0))
	{
		len = ctx->text_len;
	}
	else
	{
		len = char_count;
	}

	// set language
	bool raqm_err =
		raqm_set_language(
			ctx->raqm,
			bcp47_code,
			char_first,
			len);

	if (raqm_err == false)
	{
		return MECHASCRIBE_ERROR_RAQM_LANGUAGE;
	}

	return MECHASCRIBE_ERROR_OK;
}

enum mechascribe_error mechascribe_associate_fonts(
	struct mechascribe* ctx)
{
	FT_Error freetype_err;
	bool raqm_err;

	FT_Size size;
	size_t beg = 0;
	size_t i = 0;

	while (i < ctx->font_range_count)
	{
		freetype_err =
			FTC_Manager_LookupSize(
				ctx->freetype_face_cache,
				ctx->font_range_list[i].font->scaler,
				&size);

		if (freetype_err != FT_Err_Ok)
		{
			return MECHASCRIBE_ERROR_FREETYPE_FACE_LOOKUP;
		}

		raqm_err =
			raqm_set_freetype_face_range(
				ctx->raqm,
				size->face,
				beg,
				ctx->font_range_list[i].len);

		if (raqm_err == false)
		{
			return MECHASCRIBE_ERROR_RAQM_FONT_RANGE;
		}

		beg += ctx->font_range_list[i].len;
		++i;
	}

	return MECHASCRIBE_ERROR_OK;
}

enum mechascribe_error mechascribe_perform_layout(
	struct mechascribe* ctx)
{
	bool raqm_err = raqm_layout(ctx->raqm);

	if (raqm_err == false)
	{
		return MECHASCRIBE_ERROR_RAQM_TEXT_LAYOUT;
	}

	return MECHASCRIBE_ERROR_OK;
}

enum mechascribe_error mechascribe_generate_glyphs(
	struct mechascribe* ctx,
	size_t* glyph_count)
{
	ctx->raqm_glyphs =
		raqm_get_glyphs(
			ctx->raqm,
			glyph_count);

	if (ctx->raqm_glyphs == NULL)
	{
		return MECHASCRIBE_ERROR_RAQM_GLYPHS;
	}

	return MECHASCRIBE_ERROR_OK;
}

enum mechascribe_error mechascribe_render_glyph(
	struct mechascribe* ctx,
	size_t glyph_id,
	enum mechascribe_render_type render_type,
	struct mechascribe_buffer* buffer)
{
	raqm_glyph_t* raqm_glyph = &(ctx->raqm_glyphs[glyph_id]);
	FT_Glyph freetype_glyph;
	FTC_Node freetype_node;
	FT_Int32 load_flags;

	switch (render_type)
	{
		case MECHASCRIBE_RENDER_TYPE_SDF:
		{
			// not documented but it seems to work just fine
			load_flags = (((FT_Int32) FT_RENDER_MODE_SDF) & 15 ) << 16;
			break;
		}
		case MECHASCRIBE_RENDER_TYPE_CLASSIC:
		default:
		{
			load_flags = FT_LOAD_COLOR;
			break;
		}
	}

	FT_Error freetype_err =
		FTC_ImageCache_LookupScaler(
			ctx->freetype_image_cache,
			((struct mechascribe_font_node*)
				(raqm_glyph->ftface->generic.data))->scaler,
			FT_LOAD_RENDER | load_flags,
			raqm_glyph->index,
			&freetype_glyph,
			&freetype_node);

	if (freetype_err != FT_Err_Ok)
	{
		return MECHASCRIBE_ERROR_FREETYPE_GET_GLYPH;
	}

	buffer->lock = freetype_node;
	buffer->buffer = ((FT_BitmapGlyph) freetype_glyph)->bitmap.buffer;
	buffer->buffer_pitch = ((FT_BitmapGlyph) freetype_glyph)->bitmap.pitch;

	switch (((FT_BitmapGlyph) freetype_glyph)->bitmap.pixel_mode)
	{
		case FT_PIXEL_MODE_GRAY:
		{
			buffer->buffer_type = MECHASCRIBE_RENDER_TYPE_BYTE;
			break;
		}
		case FT_PIXEL_MODE_BGRA:
		{
			buffer->buffer_type = MECHASCRIBE_RENDER_TYPE_BGRA;
			break;
		}
	}

	buffer->glyph_x = ((FT_BitmapGlyph) freetype_glyph)->left;
	buffer->glyph_y = ((FT_BitmapGlyph) freetype_glyph)->top;
	buffer->glyph_width = ((FT_BitmapGlyph) freetype_glyph)->bitmap.width;
	buffer->glyph_height = ((FT_BitmapGlyph) freetype_glyph)->bitmap.rows;
	buffer->advance_x = freetype_glyph->advance.x;
	buffer->advance_y = freetype_glyph->advance.y;

	return MECHASCRIBE_ERROR_OK;
}

void mechascribe_render_glyph_done(
	struct mechascribe* ctx,
	void* lock)
{
	FTC_Node_Unref(lock, ctx->freetype_face_cache);
}

void mechascribe_render_done(
	struct mechascribe* ctx)
{
	raqm_destroy(ctx->raqm);
	free(ctx->text);
	free(ctx->font_range_list);
}

enum mechascribe_error mechascribe_char_to_cursor_pos(
	struct mechascribe* ctx,
	size_t index,
	int* x,
	int* y)
{
	bool raqm_err =
		raqm_index_to_position(
			ctx->raqm,
			&index,
			x,
			y);

	if (raqm_err == false)
	{
		return MECHASCRIBE_ERROR_RAQM_CHAR_TO_CUR;
	}

	return MECHASCRIBE_ERROR_OK;
}

enum mechascribe_error mechascribe_cursor_pos_to_char(
	struct mechascribe* ctx,
	int x,
	int y,
	size_t* index)
{
	bool raqm_err =
		raqm_position_to_index(
			ctx->raqm,
			x,
			y,
			index);

	if (raqm_err == false)
	{
		return MECHASCRIBE_ERROR_RAQM_CUR_TO_CHAR;
	}

	return MECHASCRIBE_ERROR_OK;
}

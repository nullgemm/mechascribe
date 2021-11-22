#include "mechascribe.h"
#include "glass.h"
#include "spng.h"

#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

// platform-specific includes for file handling
#if defined(EXAMPLE_PLATFORM_LINUX)

#include <endian.h>
#include <unistd.h>
#define O_BINARY 0

#elif defined(EXAMPLE_PLATFORM_MINGW)

#include "endian_mingw.h"
#include <unistd.h>

#elif defined(EXAMPLE_PLATFORM_MACOS)

#include "endian_macos.h"
#include <unistd.h>
#define O_BINARY 0

#endif

// test settings

#define FONT_HEIGHT 20

// png rendering code

int png_writer(spng_ctx* ctx, void* user, void* dst_src, size_t length)
{
	int* fd = user;
	ssize_t err = write(*fd, dst_src, length);

	if (err == -1)
	{
		return 1;
	}

	return 0;
}

enum mechascribe_error render_glyphs(
	struct mechascribe* mechascribe,
	uint32_t* drawing_buffer,
	size_t drawing_buffer_width,
	size_t drawing_buffer_height,
	size_t line_y)
{
	enum mechascribe_error mechascribe_err;

	// generate glyphs
	size_t glyph_count;

	mechascribe_err =
		mechascribe_generate_glyphs(
			mechascribe,
			&glyph_count);

	if (mechascribe_err != MECHASCRIBE_ERROR_OK)
	{
		return mechascribe_err;
	}

	// render glyphs
	struct mechascribe_buffer mechascribe_buffer;
	signed long cumulative_advance_x = 0;
	signed long cumulative_advance_y = 0;
	size_t line_x = 42;
	size_t i = 0;

	uint32_t pixel;
	size_t glyph_pitch;
	size_t glyph_buffer_x;
	size_t glyph_buffer_y;
	size_t glyph_width;
	size_t glyph_height;
	size_t off_png;
	size_t off_glyph;

	while (i < glyph_count)
	{
		// render the glyph
		mechascribe_err =
			mechascribe_render_glyph(
				mechascribe,
				i,
				MECHASCRIBE_RENDER_TYPE_CLASSIC,
				&mechascribe_buffer);

		if (mechascribe_err != MECHASCRIBE_ERROR_OK)
		{
			return mechascribe_err;
		}

		glyph_pitch = mechascribe_buffer.buffer_pitch;
		glyph_buffer_x = mechascribe_buffer.glyph_x;
		glyph_buffer_y = mechascribe_buffer.glyph_y;
		glyph_width = mechascribe_buffer.glyph_width;
		glyph_height = mechascribe_buffer.glyph_height;

		// copy the glyph
		for (size_t glyph_y = 0; glyph_y < glyph_height; ++glyph_y)
		{
			for (size_t glyph_x = 0; glyph_x < glyph_width; ++glyph_x)
			{
				off_png =
					(drawing_buffer_width
					* (line_y + glyph_y - glyph_buffer_y + cumulative_advance_y))
					+ line_x
					+ glyph_buffer_x
					+ glyph_x
					+ cumulative_advance_x;

				switch (mechascribe_buffer.buffer_type)
				{
					case MECHASCRIBE_RENDER_TYPE_BGRA:
					{
						off_glyph =
							glyph_y
							* glyph_width
							+ glyph_x;

						pixel =
							htole32(
								((uint32_t*) mechascribe_buffer.buffer)[off_glyph]);

						// convert to ABGR
						drawing_buffer[off_png] =
							(pixel & 0xFF00FF00)
							| ((pixel & 0x00FF0000) >> 16)
							| ((pixel & 0x000000FF) << 16);

						break;
					}
					case MECHASCRIBE_RENDER_TYPE_BYTE:
					{
						off_glyph =
							glyph_y
							* glyph_pitch
							+ glyph_x;

						if (((uint8_t*) mechascribe_buffer.buffer)[off_glyph] != 0)
						{
							pixel =
								0xFF
								- ((uint8_t*) mechascribe_buffer.buffer)[off_glyph];

							drawing_buffer[off_png] =
								0xFF000000
								| (pixel << 16)
								| (pixel << 8)
								| pixel;
						}

						break;
					}
				}
			}
		}

		// release the buffer lock when we are done copying
		mechascribe_render_glyph_done(mechascribe, mechascribe_buffer.lock);

		// we must truncate the advance as per FreeType's
		// rules (if I understand them correctly)
		cumulative_advance_x += (mechascribe_buffer.advance_x >> 16);
		cumulative_advance_y += (mechascribe_buffer.advance_y >> 16);

		++i;
	}

	return MECHASCRIBE_ERROR_OK;
}

// main mechascribe usage example

int main(void)
{
	enum mechascribe_error mechascribe_err;
	struct mechascribe* mechascribe;

	mechascribe_err = mechascribe_init(&mechascribe);

	if (mechascribe_err != MECHASCRIBE_ERROR_OK)
	{
		// TODO log
		return 1;
	}

	const char* fallback_fonts[] =
	{
		"Twemoji.ttf",
		"NotoSansSymbols2-Regular.ttf",
		"NotoSansSymbols-Regular.ttf",
		"LastResortHE-Regular.ttf",
	};

	mechascribe_err =
		mechascribe_prepare_fonts(
			mechascribe,
			"noto",
			fallback_fonts,
			(sizeof (fallback_fonts)) / (sizeof (char*)));

	switch (mechascribe_err)
	{
		case MECHASCRIBE_ERROR_FONT_INTEGRITY:
		{
			// TODO log

			mechascribe_err =
				mechascribe_unpack_fonts(
					mechascribe,
					"noto.bin.zst",
					"noto");

			if (mechascribe_err != MECHASCRIBE_ERROR_OK)
			{
				// TODO log
				return 1;
			}

			mechascribe_err =
				mechascribe_prepare_fonts(
					mechascribe,
					"noto",
					fallback_fonts,
					(sizeof (fallback_fonts)) / (sizeof (char*)));

			if (mechascribe_err != MECHASCRIBE_ERROR_OK)
			{
				// TODO log
				return 1;
			}

			break;
		}
		case MECHASCRIBE_ERROR_FONT_LIST:
		{
			// TODO log
			return 1;
		}
		case MECHASCRIBE_ERROR_OK:
		{
			break;
		}
		default:
		{
			// TODO log
			return 1;
		}
	}

	mechascribe_set_font_height(mechascribe, FONT_HEIGHT);

	// init drawing buffer
	size_t drawing_buffer_width = 1080;
	size_t drawing_buffer_height = 3940;
	size_t drawing_buffer_size =
		drawing_buffer_width
		* drawing_buffer_height
		* (sizeof (uint32_t));
	uint32_t* drawing_buffer =
		malloc(drawing_buffer_size);

	if (drawing_buffer == NULL)
	{
		mechascribe_free(mechascribe);
		return 1;
	}

	memset(drawing_buffer, 0xFF, drawing_buffer_size);

	// get sentences
	const struct eat_glass_sentence* text_list;
	size_t text_list_size;

	eat_glass_get_list(&text_list, &text_list_size);

	// cycle sentences
	size_t i = 0;

	while (i < text_list_size)
	{
		mechascribe_err =
			mechascribe_set_text(
				mechascribe,
				text_list[i].text);

		if (mechascribe_err != MECHASCRIBE_ERROR_OK)
		{
			free(drawing_buffer);
			mechascribe_free(mechascribe);
			return 1;
		}

#if 0
		// set general text direction (not required)
		mechascribe_err = mechascribe_set_direction(
			mechascribe,
			MECHASCRIBE_DIRECTION_DEFAULT);

		if (mechascribe_err != MECHASCRIBE_ERROR_OK)
		{
			mechascribe_free(mechascribe);
			return 1;
		}
#endif

		// automatically compute the text length
		mechascribe_err =
			mechascribe_set_language(
				mechascribe,
				text_list[i].bcp47,
				0,
				0);

		if (mechascribe_err != MECHASCRIBE_ERROR_OK)
		{
			free(drawing_buffer);
			mechascribe_free(mechascribe);
			return 1;
		}

		fflush(stdout);

		// generate font ranges
		mechascribe_err =
			mechascribe_associate_fonts(
				mechascribe);

		if (mechascribe_err != MECHASCRIBE_ERROR_OK)
		{
			free(drawing_buffer);
			mechascribe_free(mechascribe);
			return 1;
		}

		mechascribe_err =
			mechascribe_perform_layout(
				mechascribe);

		if (mechascribe_err != MECHASCRIBE_ERROR_OK)
		{
			free(drawing_buffer);
			mechascribe_free(mechascribe);
			return 1;
		}

		//TODO detect baseline starting point and get it with mechascribe
		// delegate rendering
		mechascribe_err =
			render_glyphs(
				mechascribe,
				drawing_buffer,
				drawing_buffer_width,
				drawing_buffer_height,
				42 + (i + 1) * FONT_HEIGHT * 5 / 4);

		if (mechascribe_err != MECHASCRIBE_ERROR_OK)
		{
			free(drawing_buffer);
			mechascribe_free(mechascribe);
			return 1;
		}

		// reset 
		mechascribe_render_done(mechascribe);

		++i;
	}

	// open new png file
	int png =
		open(
			"glass.png",
			O_WRONLY | O_CREAT | O_BINARY,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	if (png == -1)
	{
		free(drawing_buffer);
		mechascribe_free(mechascribe);
		return 1;
	}

	// save the drawing buffer as png
	int libspng_err;

	spng_ctx *enc = spng_ctx_new(SPNG_CTX_ENCODER);

	if (enc == NULL)
	{
		close(png);
		free(drawing_buffer);
		mechascribe_free(mechascribe);
		return 1;
	}

	// ihdr
	struct spng_ihdr ihdr =
	{
		.height = drawing_buffer_height,
		.width = drawing_buffer_width,
		.bit_depth = 8,
		.color_type = SPNG_COLOR_TYPE_TRUECOLOR_ALPHA,
	};

	libspng_err = spng_set_ihdr(enc, &ihdr);

	if (libspng_err != 0)
	{
		spng_ctx_free(enc);
		close(png);
		free(drawing_buffer);
		mechascribe_free(mechascribe);
		return 1;
	}

	// set file writer
	libspng_err = spng_set_png_stream(enc, png_writer, &png);

	if (libspng_err != 0)
	{
		spng_ctx_free(enc);
		close(png);
		free(drawing_buffer);
		mechascribe_free(mechascribe);
		return 1;
	}

	// encode
	libspng_err =
		spng_encode_image(
			enc,
			drawing_buffer,
			drawing_buffer_size,
			SPNG_FMT_PNG,
			SPNG_ENCODE_FINALIZE);

	if (libspng_err != 0)
	{
		spng_ctx_free(enc);
		close(png);
		free(drawing_buffer);
		mechascribe_free(mechascribe);
		return 1;
	}

	spng_ctx_free(enc);
	close(png);
	free(drawing_buffer);
	mechascribe_free(mechascribe);

	return 0;
}

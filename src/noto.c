#include "mechascribe_private.h"
#include "hydraquill.h"
#include "handy.h"
#include "sha2.h"
#include "zstd.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// platform-specific includes for file handling

#if defined(MECHASCRIBE_PLATFORM_LINUX)

#include <unistd.h>
#define O_BINARY 0

#elif defined(MECHASCRIBE_PLATFORM_MINGW)

#include <unistd.h>

#elif defined(MECHASCRIBE_PLATFORM_MACOS)

#include <unistd.h>
#define O_BINARY 0

#endif

// maximum file name length

#if defined(NAME_MAX)
#define MECHASCRIBE_NAME_MAX NAME_MAX
#elif defined(_POSIX_PATH_MAX)
#define MECHASCRIBE_NAME_MAX _POSIX_PATH_MAX
#else
#define MECHASCRIBE_NAME_MAX 1024
#endif

// private functions

// use safe min and max for the specific type cifra needs
inline size_t min(size_t x, size_t y)
{
	if (x > y)
	{
		return y;
	}

	return x;
}

inline size_t max(size_t x, size_t y)
{
	if (x > y)
	{
		return x;
	}

	return y;
}

static char* path_build(const char* path, const char* name)
{
	size_t path_len = strlen(path);
	size_t out_len;
	char* out;

	if ((path_len > 0) && (path[path_len - 1] != '/'))
	{
		out_len = path_len + strlen(name) + 1;
		out = malloc(out_len + 1);

		if (out == NULL)
		{
			return NULL;
		}

		strcpy(out, path);
		out[path_len] = '/'; // overwrite the previous NUL
		strcpy(out + path_len + 1, name);
	}
	else
	{
		out_len = path_len + strlen(name);
		out = malloc(out_len + 1);

		if (out == NULL)
		{
			return NULL;
		}

		strcpy(out, path);
		strcpy(out + path_len, name); // overwrite the previous NUL
	}

	return out;
}

// hydraquill processing callback for loading the font paths
enum hydraquill_error path_list(
	void* context,
	int font_file,
	const char* font_dir,
	const char* font_name,
	uint8_t* font_hash,
	uint32_t font_size)
{
	struct mechascribe_font_node* node = NULL;
	struct mechascribe* ctx = context;
	char* font_path = path_build(font_dir, font_name);
	size_t i = 0;

	while (i < ctx->font_fallback_count)
	{
		if (strcmp(font_name, ctx->font_fallback_name_list[i]) == 0)
		{
			node = &(ctx->font_fallback_list[i]);
			node->font_path = font_path;
			node->next = NULL;
			break;
		}

		++i;
	}

	if (node == NULL)
	{
		if (strcmp(font_name, ctx->font_emoji_name) == 0)
		{
			node = &(ctx->font_emoji);
			node->font_path = font_path;
			node->next = NULL;
		}
		else
		{
			// create a font node and save it as the new beginning of the main list
			node = malloc(sizeof (struct mechascribe_font_node));

			if (node == NULL)
			{
				return HYDRAQUILL_ERROR_ALLOC;
			}

			node->font_path = font_path;
			node->next = ctx->font_list;
			ctx->font_list = node;
		}
	}

	// initialize common node fields
	node->font_face = 0;
	node->scaler = malloc(sizeof (FTC_ScalerRec));

	if (node->scaler == NULL)
	{
		return HYDRAQUILL_ERROR_ALLOC;
	}

	node->scaler->face_id = node;
	node->scaler->width = 0;
	node->scaler->height = 0;
	node->scaler->pixel = 1;
	node->scaler->x_res = 0;
	node->scaler->y_res = 0;

	return HYDRAQUILL_ERROR_OK;
}

// hydraquill processing callback for checking the sha256 sums
enum hydraquill_error sha256(
	void* context,
	int font_file,
	const char* font_dir,
	const char* font_name,
	uint8_t* font_hash,
	uint32_t font_size)
{
	int err;
	uint8_t buf[64];
	uint8_t checksum[32] = {0};
	cf_sha256_context ctx = {0};

	// hash
	cf_sha256_init(&ctx);

	do
	{
		err = read(font_file, buf, 64);
		cf_sha256_update(&ctx, buf, err);
	}
	while (err == 64);

	cf_sha256_digest_final(&ctx, checksum);

	// check
	err = memcmp(checksum, font_hash, 32);

	if (err != 0)
	{
		return HYDRAQUILL_ERROR_SHA256;
	}

	return HYDRAQUILL_ERROR_OK;
}

// hydraquill unpacking callback for decoding the `.zst` archive
enum hydraquill_error zstd_decode_file(int output_file, int input_file)
{
	// init stream
	ZSTD_DStream* stream = ZSTD_createDStream();
	ZSTD_initDStream(stream);

	// init output buffer
	size_t out_buf_size = ZSTD_DStreamOutSize();
	void* out_buf = malloc(out_buf_size);

	if (out_buf == NULL)
	{
		return HYDRAQUILL_ERROR_ALLOC;
	}

	// init input buffer
	size_t in_buf_size = ZSTD_DStreamInSize();
	void* in_buf = malloc(in_buf_size);

	if (in_buf == NULL)
	{
		free(out_buf);
		return HYDRAQUILL_ERROR_ALLOC;
	}

	// decode the archive
	size_t read_size;
	size_t write_size;
	ZSTD_inBuffer in = {0};
	ZSTD_outBuffer out = {0};

	do
	{
		// read
		read_size = read(input_file, in_buf, in_buf_size);
		in.size = read_size;
		in.src = in_buf;
		in.pos = 0;

		while (in.pos < in.size)
		{
			// decode
			out.pos = 0;
			out.dst = out_buf;
			out.size = out_buf_size;
			ZSTD_decompressStream(stream, &out, &in);

			// write
			write_size = write(output_file, out_buf, out.pos);

			if (write_size < out.pos)
			{
				ZSTD_freeDStream(stream);
				free(out_buf); 
				free(in_buf);

				return HYDRAQUILL_ERROR_WRITE;
			}
		}
	}
	while (in.pos > 0);

	// free resources
	ZSTD_freeDStream(stream);
	free(out_buf);
	free(in_buf);

	return HYDRAQUILL_ERROR_OK;
}

// cross-platform recursive mkdir
enum mechascribe_error path_create(const char* path_out)
{
	// fail if the path is too long or the string terminator can't fit
	if (strlen(path_out) >= MECHASCRIBE_NAME_MAX)
	{
		return MECHASCRIBE_ERROR_MKDIR_MAX;
	}

	// create each folder of the path that does not exist
	int err;
	char folder_tmp[MECHASCRIBE_NAME_MAX];
	struct stat stat_info;

	const char* path_cur = path_out;
	const char* path_last;

	ptrdiff_t path_end;
	ptrdiff_t path_len;
	ptrdiff_t folder_len;

	while (*path_cur != '\0')
	{
		path_last = path_cur;

		// find the next folder in the full path
		do
		{
			++path_cur;
		}
		while((*path_cur != '\0') && (*path_cur != '/'));

		// add this folder to the checked path
		folder_len = path_cur - path_last;
		path_len = path_last - path_out;
		path_end = path_cur - path_out;

		strncpy(
			folder_tmp + path_len,
			path_out + path_len,
			folder_len);

		folder_tmp[path_end] = '\0';

		// skip w$ prefixes
#if defined(MECHASCRIBE_PLATFORM_MINGW)
		if ((folder_len == 1) || (folder_tmp[path_end - 1] == ':'))
#else
		if (folder_len == 1)
#endif
		{
			continue;
		}

		// skip existing folders
		err = stat(folder_tmp, &stat_info);

		if (err >= 0)
		{ 
			continue;
		}

		// create missing folders
#if defined(MECHASCRIBE_PLATFORM_MINGW)
		err = mkdir(folder_tmp);
#else
		err = mkdir(
			folder_tmp,
			S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
#endif

		if (err < 0)
		{
			return MECHASCRIBE_ERROR_MKDIR;
		}
	}

	return MECHASCRIBE_ERROR_OK;
}

// public functions

// loads the extracted font files in the mechascribe context
// `font_folder`, `font_emoji` and `font_lastresort` must be non-NULL
// an error is returned if any of `font_emoji` or `font_lastresort` is missing
enum mechascribe_error mechascribe_prepare_fonts(
	struct mechascribe* ctx,
	const char* font_folder,
	const char* font_emoji_name,
	const char** font_fallback_name_list,
	size_t font_fallback_count)
{
	// check the unpacked font files
	enum hydraquill_error hydraquill_err =
		hydraquill_process_fonts(
			sha256,
			font_folder,
			NULL);

	if (hydraquill_err != HYDRAQUILL_ERROR_OK)
	{
		return MECHASCRIBE_ERROR_FONT_INTEGRITY;
	}

	// allocate the font fallback list
	ctx->font_fallback_list =
		malloc(
			font_fallback_count * (sizeof (struct mechascribe_font_node)));

	if (ctx->font_fallback_list == NULL)
	{
		return MECHASCRIBE_ERROR_ALLOC;
	}

	memset(
		ctx->font_fallback_list,
		0,
		font_fallback_count * (sizeof (struct mechascribe_font_node)));

	// temporarily save the special font info
	ctx->font_emoji_name = font_emoji_name;
	ctx->font_fallback_name_list = font_fallback_name_list;
	ctx->font_fallback_count = font_fallback_count;

	// process and save the font file paths in the list
	hydraquill_err = hydraquill_process_fonts(
		path_list,
		font_folder,
		ctx);

	// reset the saved special font info as soon as possible
	ctx->font_emoji_name = NULL;
	ctx->font_fallback_name_list = NULL;

	// check for errors afterwards
	if (hydraquill_err != HYDRAQUILL_ERROR_OK)
	{
		free(ctx->font_fallback_list);
		return MECHASCRIBE_ERROR_FONT_LIST;
	}

	return MECHASCRIBE_ERROR_OK;
}

// extracts the content of the font archive in a specific folder
// `archive_file` and `font_folder` must be non-NULL
enum mechascribe_error mechascribe_unpack_fonts(
	struct mechascribe* ctx,
	const char* archive_file,
	const char* font_folder)
{
	// create the output font folder if necessary
	enum mechascribe_error mechascribe_err = path_create(font_folder);

	if (mechascribe_err != MECHASCRIBE_ERROR_OK)
	{
		return mechascribe_err;
	}

	// unpack the font archive
	enum hydraquill_error hydraquill_err =
		hydraquill_unpack_file(
			zstd_decode_file,
			font_folder,
			archive_file);

	if (hydraquill_err != HYDRAQUILL_ERROR_OK)
	{
		return MECHASCRIBE_ERROR_FONT_UNPACK;
	}

	return MECHASCRIBE_ERROR_OK;
}

#ifdef _WIN32
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#endif
#include <cstdio>
#include "../ScoUtils/Sco.h"
#include "../ScoUtils/ScoReader.h"
#include "../StbImage/stb_image_write.h"

#ifdef _DEBUG
#pragma comment(lib, "../Debug/StbImage.lib")
#pragma comment(lib, "../Debug/ScoUtils.lib")
#else
#pragma comment(lib, "../Release/StbImage.lib")
#pragma comment(lib, "../Release/ScoUtils.lib")
#endif

const char *get_filename_ext(const char *filename)
{
#ifdef _WIN32
	return PathFindExtensionA(filename);
#else
	const char *dot = strrchr(filename, '.');
	if (!dot || dot == filename) return "";
	return dot;
#endif
}

int file_exist(const char *fname)
{
#ifdef _WIN32
	DWORD dwAttributes = GetFileAttributesA(fname);
	return dwAttributes == INVALID_FILE_ATTRIBUTES || dwAttributes & FILE_ATTRIBUTE_DIRECTORY ? 0 : 1;
#else
	FILE *file;
	if ((file = fopen(fname, "r")))
	{
		fclose(file);
		return 1;
	}
	return 0;
#endif
}

char* get_file_name_from_full_path(char *path)
{
#ifdef _WIN32
	return PathFindFileNameA(path);
#else
	char *f = strrchr(path, '/');
	if (!f) f = path; else f++;
	return f;
#endif
}

int save_heightmap_as_png(char *out, sco_file_t *src)
{
	printf("\n | Saving heightmap as: %s...\n", out);
	printf(" |    Number of layers: %d\n", src->ground_paint->num_layers);
	printf(" |              Size X: %d\n", src->ground_paint->size_x);
	printf(" |              Size Y: %d\n\n", src->ground_paint->size_y);

	int x = src->ground_paint->size_x;
	int y = src->ground_paint->size_y; /* terrible hack of death!  >:( */
	int img;
	unsigned char *foo = (unsigned char*)malloc(x * y * sizeof(char) * 3);
	const char *ext = _strupr((char *)get_filename_ext(out));

	int count = 0;
	for (int gol = 0; gol <= src->ground_paint->num_layers; ++gol)
	{
		//printf("\n\nlayer %d\n spec_id[%s] continuity_count[%p]\n", gol, src->ground_paint->layers[gol].ground_spec_id, src->ground_paint->layers[gol].continuity_count);
		if (src->ground_paint->layers[gol].ground_spec_no == GROUND_PAINT_ELEVATION_MAGIC && src->ground_paint->layers[gol].cells) 
		{
			if (src->ground_paint->layers[gol].continuity_count) 
			{
				for (int goy = 0; goy < x; ++goy)
				{
					for (int gox = 0; gox < x; ++gox)
					{
						foo[count] = (float)src->ground_paint->layers[gol].cells[gox * y + goy] + 16.0f;
						count++;
					}
				}
			}
			else break;
		}
	}

	if (count == 0) return 1;

	printf(" | Output format is %s\n\n", ext);

	if (_strcmpi(ext, ".TGA") == 0)
		img = stbi_write_tga(out, x, y, 1, foo);
	else if (_strcmpi(ext, ".BMP") == 0)
		img = stbi_write_bmp(out, x, y, 1, foo);
	else
		img = stbi_write_png(out, x, y, 1, foo, 0);

	if (img != 0) 
	{
		printf(" | >> Done! :)\n");
		return EXIT_SUCCESS;
	}

	printf(" | >> Meh, doesn't works! :(\n");
	return EXIT_FAILURE;
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		//get the app name
		auto exec = get_file_name_from_full_path(argv[0]);
		printf("\n | SceneOBJ -> PNG Heightmap -- By Janycz\n | Based on the work of Disgrntld, cmpxchg8b and Swyter.\n\n");
		printf(">>   Usage: %s <input SCO filename> <optional output filename in PNG, TGA or BMP>\n\n", exec);
		return EXIT_FAILURE;
	}

	char *out;
	if (argc == 2)
	{
		char ext[] = ".png";
		out = (char*)malloc(strlen(argv[1]) + strlen(ext) + 1);
		strcpy(out, argv[1]);
		strcat(out, ext);
	}
	else
		out = _strdup(argv[2]);


	if (strcmp(_strupr((char *)get_filename_ext(argv[1])), ".SCO") != 0) 
	{
		printf("\n | This only works with Mount&Blade SceneObj files... :)\n\n");
		return EXIT_FAILURE;
	}

	FILE *in = fopen(argv[1], "rb");

	if (!in || !out)
	{
		printf("ERROR: file %s or %s not found\n", argv[1], argv[2]);
		return EXIT_FAILURE;
	}

	printf("Reading %s\n", argv[1]);
	sco_file_t sco_file;

	read_sco_file(in, &sco_file);
	int result = save_heightmap_as_png(out, &sco_file);

	free(out);
	return result;
}


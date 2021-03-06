#ifdef _WIN32
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#endif
#include <cstdio>
#include "../ScoUtils/ScoReader.h"
#include "../ScoUtils/ScoWriter.h"
#include "../StbImage/stb_image.h"

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

int save_heightmap_as_sco(char *img, sco_file_t *src)
{
	int x, y, comp;
	unsigned char *foo = stbi_load(img, &x, &y, &comp, 0);

	printf("\n |   Loading heightmap: %s...\n", img);
	printf(" |    Heightmap Size X: %d\n", x);
	printf(" |    Heightmap Size Y: %d\n\n", y);
	printf(" |  Heightmap channels: %d\n\n", comp);
	printf(" |        Scene Size X: %d\n", src->ground_paint->size_x);
	printf(" |        Scene Size Y: %d\n\n", src->ground_paint->size_y);

	if (src->ground_paint->size_x != x && src->ground_paint->size_y != y) 
	{
		printf(">>  Scene and heightmap sizes aren't exactly matching... :(\n");
		return 0;
	}

	int count = 0;
	for (int gol = 0; gol < src->ground_paint->num_layers; ++gol)
	{
		//printf("\n\nlayer %d\n spec_id[%s] continuity_count[%p]\n", gol, src->ground_paint->layers[gol].ground_spec_id, src->ground_paint->layers[gol].continuity_count);
		if (src->ground_paint->layers[gol].ground_spec_no == GROUND_PAINT_ELEVATION_MAGIC && src->ground_paint->layers[gol].cells) 
		{
			if (src->ground_paint->layers[gol].continuity_count) 
			{
				if (comp > 1) 
				{
					int channelcount = 0;
					int everything = y * x;
					for (int i = 0; i < everything; ++i)
					{
						foo[i] = foo[channelcount];
						channelcount += comp;
					}
				}

				for (int gox = 0; gox < x; ++gox)
				{
					for (int goy = 0; goy < y; ++goy)
					{
						if ((float)src->ground_paint->layers[gol].cells[count] != foo[goy * x + gox])
							src->ground_paint->layers[gol].cells[count] = (unsigned char)foo[goy * x + gox] - 16.0f;
						
						count++;
					}
				}
			}
		}
	}

	return count == 0 ? 0 : 1;
}

int main(int argc, char **argv)
{
	if (argc == 1)
	{
		auto exec = get_file_name_from_full_path(argv[0]);
		printf("\n | PNG Heightmap -> SceneOBJ  -- By Janycz\n | Based on the work of Disgrntld, cmpxchg8b and Swyter.\n\n");
		printf(">>   Usage: %s <input SceneOBJ> <input PNG, PSD, TGA, BMP, JPG or GIF filename> <Optional SCO output>\n     A backup is recommended... Both input files need to have the very same dimensions, otherwise there'll be distorsions\n", exec);
		return EXIT_FAILURE;
	}
	
	if (argc == 2 || argc == 3) 
	{
		char sco[] = "ed.sco";

		argv[3] = (char*)malloc(strlen(argv[1]) + strlen(sco) - 3);
		strcpy(argv[3], argv[1]);
		argv[3][strlen(argv[3]) - 3] = 0;
		strcat(argv[3], sco);

		printf("Assuming that the output is the input...\n<%s>\n\n", argv[3]);
	}

	if (argc == 2)
	{
		char ext[] = ".png";

		argv[2] = (char*)malloc(strlen(argv[1]) + strlen(ext));
		strcpy(argv[2], argv[1]);
		strcat(argv[2], ext);

		if (file_exist(argv[2])) 
			printf("Okay, I've found a previously exported heightmap.\n<%s>\nI suppose that you want me to use that. :)\n\n", argv[2]);
		else
		{
			printf("Please provide a heightmap file at least.\nWe aren't that smart. :(\n");
			return EXIT_FAILURE;
		}
	}

	char *imgext = _strupr((char*)get_filename_ext(argv[2]));

	if (strcmp(_strupr((char *)get_filename_ext(argv[1])), ".SCO") != 0 || 
		strcmp(imgext, ".PNG") != 0 && strcmp(imgext, ".PSD") != 0 && strcmp(imgext, ".TGA") != 0 &&
		strcmp(imgext, ".BMP") != 0 && strcmp(imgext, ".JPG") != 0 && strcmp(imgext, ".GIF") != 0) 
	{
		printf("\n | This only works with Mount&Blade SceneObj files and heightmap images... :)\n\n");
		return EXIT_FAILURE;
	}

	FILE *in = fopen(argv[1], "rb");
	char *img = argv[2];
	FILE *out = fopen(argv[3], "wb");

	if (!in || !out)
	{
		printf("ERROR: file %s or %s not found\n", argv[1], argv[2]);
		return EXIT_FAILURE;
	}

	printf("Reading %s\n", argv[1]);
	sco_file_t sco_file;

	read_sco_file(in, &sco_file);

	if (save_heightmap_as_sco(img, &sco_file)) 
	{
		write_sco_file(out, &sco_file);
		printf(" | Reimporting completed, take a look in-game...\n\n");
		return EXIT_SUCCESS;
	}
	printf(" | Failed miserably.");
	return EXIT_FAILURE;
}


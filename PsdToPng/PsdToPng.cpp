#ifdef _WIN32
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#endif
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#define STBI_NO_HDR
#define STBI_NO_FAILURE_STRINGS
#include "../StbImage/stb_image.h"
#include "../StbImage/stb_image_write.h"

#ifdef _DEBUG
#pragma comment(lib, "../Debug/StbImage.lib")
#else
#pragma comment(lib, "../Release/StbImage.lib")
#endif

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

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		auto exec = get_file_name_from_full_path(argv[0]);
		printf("PSD, GIF, JPG, TGA, PIC -> PNG -- By Janycz\n | Based on the work of Swyter \n\n");
		printf("Usage: %s <input filename> <optional output filename>\n", exec);
		system("pause");
		return EXIT_FAILURE;
	}
	if (argc == 2)
	{
		char ext[] = ".png";
		argv[2] = (char*)malloc(strlen(argv[1]) + strlen(ext) + 1);
		strcpy(argv[2], argv[1]);
		strcat(argv[2], ext);
	}
	printf("%s -> %s\n", argv[1], argv[2]);

	int x, y, comp;

	unsigned char *foo = stbi_load(argv[1], &x, &y, &comp, 0);
	int img = stbi_write_png(argv[2], x, y, comp, foo, 0);

	if (img != 0) 
		printf("[!] image creation successful\n");
	else 
		printf("[!] meh, doesn't works!\n");

	stbi_image_free(foo);

	return img;
}


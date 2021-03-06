#include <stdio.h>
#include <stdlib.h>

#include "../ScoUtils/Sco.h"
#include "../ScoUtils/ScoReader.h"

#ifdef _DEBUG
#pragma comment(lib, "../Debug/ScoUtils.lib")
#else
#pragma comment(lib, "../Release/ScoUtils.lib")
#endif

typedef struct face
{
	int v1;
	int v2;
	int v3;
} face_t;

int write_obj(char* file_name, vector_t *vertex_positions, int number_of_vertices, face_t *facelist, int number_of_faces)
{
	FILE *file = fopen(file_name, "wt");
	if (!file)
	{
		printf("ERROR: file %s could not be opened.\n", file_name);
		return EXIT_FAILURE;
	}

	for (int n = 0; n < number_of_vertices; ++n)
	{
		fprintf(file, "v ");
		fprintf(file, "%f ", vertex_positions->x);
		fprintf(file, "%f ", vertex_positions->y);
		fprintf(file, "%f ", vertex_positions->z);
		fprintf(file, "\n");
		++vertex_positions;
	}

	for (int n = 0; n < number_of_faces; ++n)
	{
		fprintf(file, "f ");
		fprintf(file, "%i ", facelist->v1);
		fprintf(file, "%i ", facelist->v2);
		fprintf(file, "%i ", facelist->v3);
		fprintf(file, "\n");
		++facelist;
	}

	if (fclose(file))
	{
		printf("ERROR: file %s could not be closed.\n", file_name);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int fill_facelist(face_t * facelist, int size_x, int size_y)
{
	face_t *face = facelist;
	for (int x = 1; x < size_x; ++x)
	{
		for (int y = 1; y < size_y; ++y)
		{
			face->v1 = y + (x - 1) * size_y;
			face->v2 = y + 1 + (x - 1) * size_y;
			face->v3 = y + x * size_y;
			++face;
			face->v1 = y + 1 + (x - 1) * size_y;
			face->v2 = y + 1 + x * size_y;
			face->v3 = y + x * size_y;
			++face;
		}
	}
	return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		printf("Usage: %s file_in file_out\n", argv[0]);
		return EXIT_FAILURE;
	}

	FILE *in = fopen(argv[1], "rb");

	if (!in)
	{
		printf("ERROR: file %s not found\n", argv[1]);
		return EXIT_FAILURE;
	}

	printf("Reading %s\n", argv[1]);
	sco_file_t sco_file;
	read_sco_file(in, &sco_file);

	vector_t *vertex_positions = (vector_t*)malloc(sco_file.ground_paint->size_x*sco_file.ground_paint->size_y * sizeof(vector_t));
	vector_t *positions = vertex_positions;

	for (int layer = 0; layer < sco_file.ground_paint->num_layers; ++layer)
	{
		if (sco_file.ground_paint->layers[layer].ground_spec_no == GROUND_PAINT_ELEVATION_MAGIC && sco_file.ground_paint->layers[layer].cells)
		{
			for (int x = 0; x < sco_file.ground_paint->size_x; ++x)
			{
				for (int y = 0; y < sco_file.ground_paint->size_y; ++y)
				{
					positions->x = x;
					positions->y = y;
					positions->z = sco_file.ground_paint->layers[layer].cells[y + x * sco_file.ground_paint->size_y];
					++positions;
				}
			}
		}
	}

	int number_of_faces;
	if (sco_file.ground_paint->size_x>0 && sco_file.ground_paint->size_y>0)
		number_of_faces = (sco_file.ground_paint->size_x - 1)*(sco_file.ground_paint->size_y - 1) * 2;
	else
		return EXIT_FAILURE;

	face_t *facelist = (face_t*)malloc(number_of_faces * sizeof(face_t));
	if (!facelist) return EXIT_FAILURE;

	fill_facelist(facelist, sco_file.ground_paint->size_x, sco_file.ground_paint->size_y);

	printf("Writing %s\n", argv[2]);

	if (!write_obj(argv[2], vertex_positions, positions - vertex_positions, facelist, number_of_faces))
	{
		printf("Succesfully written to obj file.");
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

#ifndef INCLUDE_STB_IMAGE_WRITE_H
#define INCLUDE_STB_IMAGE_WRITE_H

#ifdef __cplusplus
extern "C" {
#endif
	extern int stbi_write_png(char const *filename, int w, int h, int comp, const void *data, int stride_in_bytes);
	extern int stbi_write_bmp(char const *filename, int w, int h, int comp, const void *data);
	extern int stbi_write_tga(char const *filename, int w, int h, int comp, const void *data);
#ifdef __cplusplus
}
#endif

#endif //INCLUDE_STB_IMAGE_WRITE_H
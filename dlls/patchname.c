#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    FILE *f;
    long file_size;
    size_t size, i, count = 0;
    unsigned char *data;

    static const unsigned char old_name[] = "_ProcessPrng@8";
    static const unsigned char new_name[] = "ProcessPrng\0\0\0";
    const size_t name_len = sizeof(old_name) - 1;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s file.dll\n", argv[0]);
        return 1;
    }

    f = fopen(argv[1], "rb");
    if (!f) {
        perror("fopen");
        return 1;
    }

    if (fseek(f, 0, SEEK_END) != 0 ||
        (file_size = ftell(f)) < 0) {
        perror("file size");
        fclose(f);
        return 1;
    }

    rewind(f);
    size = (size_t)file_size;
    data = (unsigned char *)malloc(size ? size : 1);

    if (!data) {
        fprintf(stderr, "out of memory\n");
        fclose(f);
        return 1;
    }

    if (size && fread(data, 1, size, f) != size) {
        perror("fread");
        free(data);
        fclose(f);
        return 1;
    }
    fclose(f);

    for (i = 0; i + name_len <= size; ++i) {
        if (memcmp(data + i, old_name, name_len) == 0) {
            memcpy(data + i, new_name, name_len);
            ++count;
            i += name_len - 1;
        }
    }

    if (!count) {
        puts("String not found.");
        free(data);
        return 0;
    }

    f = fopen(argv[1], "r+b");
    if (!f) {
        perror("fopen for writing");
        free(data);
        return 1;
    }

    if (fwrite(data, 1, size, f) != size) {
        perror("fwrite");
        fclose(f);
        free(data);
        return 1;
    }

    fclose(f);
    free(data);

    printf("Replaced %lu occurrence(s).\n", (unsigned long)count);
    return 0;
}

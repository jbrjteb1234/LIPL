// utility/source_file.c
#include "source_file.h"
#include "safe_memory.h"

#define INITIAL_READ_BUFFER_SIZE 4096u

bool read_source_file(FILE* file, const char* name, source_file* out){
    size_t capacity = INITIAL_READ_BUFFER_SIZE;
    size_t length = 0;
    char* buffer = (char*)safe_malloc(capacity);

    for (;;){
        /* always leave room for the terminating NUL */
        if (length + 1u >= capacity){
            capacity *= 2u;
            buffer = (char*)safe_realloc(buffer, capacity);
        }
        size_t got = fread(buffer + length, 1, capacity - 1u - length, file);
        length += got;
        if (got == 0){
            break;
        }
    }

    if (ferror(file)){
        safe_free((void**)&buffer);
        return false;
    }
    buffer[length] = '\0';
    out->name = name;
    out->text = buffer;
    out->length = length;
    return true;
}

void free_source_file(source_file* source){
    safe_free((void**)&source->text);
    source->length = 0;
}

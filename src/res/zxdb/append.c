#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv) {
    int count = 0, ok = 0;
    if( argc != 3 ) puts("append exe_file resource_file"), exit(1);
    else
    for( FILE *in = fopen(argv[2], "rb"); in; fclose(in), in = 0) {
        for( FILE *out = fopen(argv[1], "a+b"); out; fclose(out), out = 0, ok = 1) { 
            int ch; do fputc(ch = fgetc(in), out), count += ch != EOF; while(ch != EOF);
            printf("%d bytes\n", count);
            fputc((count >>  0) & 255, out);
            fputc((count >>  8) & 255, out);
            fputc((count >> 16) & 255, out);
            fputc((count >> 24) & 255, out);
        }
    }
    puts(ok ? "Ok" : "Error");
    exit(-!ok);
}

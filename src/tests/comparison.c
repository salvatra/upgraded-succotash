#include <tests/comparison.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 4096

gboolean compare_files(const char *generated_path, const char *expected_path, int *diff_line)
{
    FILE *fp_gen = fopen(generated_path, "r");
    FILE *fp_exp = fopen(expected_path, "r");

    if (!fp_gen || !fp_exp)
    {
        if (diff_line)
            *diff_line = -1;
        if (fp_gen)
            fclose(fp_gen);
        if (fp_exp)
            fclose(fp_exp);
        return FALSE;
    }

    char buf_gen[BUFFER_SIZE];
    char buf_exp[BUFFER_SIZE];
    int line_num = 1;
    gboolean are_equal = TRUE;

    while (TRUE)
    {
        char *res_gen = fgets(buf_gen, sizeof(buf_gen), fp_gen);
        char *res_exp = fgets(buf_exp, sizeof(buf_exp), fp_exp);

        if (!res_gen && !res_exp)
        {
            break;
        }

        if (!res_gen || !res_exp)
        {
            are_equal = FALSE;
            if (diff_line)
                *diff_line = line_num;
            break;
        }

        if (strcmp(buf_gen, buf_exp) != 0)
        {
            are_equal = FALSE;
            if (diff_line)
                *diff_line = line_num;
            break;
        }

        line_num++;
    }

    fclose(fp_gen);
    fclose(fp_exp);

    return are_equal;
}
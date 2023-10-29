#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

extern int opterr;

int main(int argc, char *argv[])
{
    char *fortune_file_name = NULL;
    char *batch_name = NULL;
    char *output_file_name = NULL;

    int fortune_number = 0;

    char opt = 0;
    char count_mode = 0;

    if (argc < 5)
    {
        printf("USAGE: \n\tbadger-fortune -f <file> -n <number> (optionally: -o <output file>) \n\t\t OR \n\tbadger-fortune -f <file> -b <batch file> (optionally: -o <output file>)\n");
        return 1;
    }
    opterr = 0;
    while ((opt = getopt(argc, argv, "f:b:n:o:")) != -1)
    {
        switch (opt)
        {
        case 'f':
            fortune_file_name = optarg;
            break;
        case 'o':
            output_file_name = optarg;
            break;
        case 'n':
            if (count_mode != 0)
            {
                printf("ERROR: You can't specify a specific fortune number in conjunction with batch mode\n");
                return 1;
            }
            count_mode = opt;
            fortune_number = atoi(optarg);
            break;
        case 'b':
            if (count_mode != 0)
            {
                printf("ERROR: You can't use batch mode when specifying a fortune number using -n\n");
                return 1;
            }
            count_mode = opt;
            batch_name = optarg;
            break;
        case '?':
            printf("ERROR: Invalid Flag Types\n");
            return 1;
        }
    }

    // if(strcmp(argv[1], "-f") == 1 && (strcmp(argv[3], "-n") == 1 || strcmp(argv[3], "-b") == 1))
    // {
    //     printf("ERROR: Invalid Flag Types\n");
    // }

    if (fortune_file_name == NULL)
    {
        printf("ERROR: No fortune file was provided\n");
        return 1;
    }
    FILE *fortune_file = fopen(fortune_file_name, "r");
    if (fortune_file == NULL)
    {
        printf("ERROR: Can't open fortune file\n");
        return 1;
    }
    char *line = NULL;
    size_t len = 0;

    int max_fortune_length;
    int fortune_count;

    fscanf(fortune_file, "%d \n %d", &fortune_count, &max_fortune_length);
    fscanf(fortune_file, "%*[^\n]\n");

    //////////////////////////////////////////////////
    char *pharse = calloc(max_fortune_length, sizeof(char));
    char **fortunes = calloc(fortune_count, sizeof(char *));
    {
        int i = 0;
        while ((getline(&line, &len, fortune_file)) != -1)
        {
            if (strcmp(line, "%\n") == 0)
            {
                fortunes[i++] = pharse;
                pharse = calloc(max_fortune_length, sizeof(char));
            }
            else
            {
                strcat(pharse, line);
            }
            free(line);
            line = NULL;
        }
        if (i == 0)
        {
            printf("ERROR: Fortune File Empty\n");
            return 1;
        }
        fortunes[i++] = pharse;
        fclose(fortune_file);
    }
    //////////////////////////////////////////////////

    FILE *output_file = stdout;
    if (argc > 5)
    {
        if (strcmp(argv[5], "-o") == 0)
        {
            output_file = fopen(output_file_name, "w");
            // TODO
        }
    }

    if (strcmp(argv[3], "-n") == 0)
    {
        if (fortune_number <= 0 || fortune_number > fortune_count)
        {
            printf("ERROR: Invalid Fortune Number\n");
            return 1;
        }

        fprintf(output_file, "%s", fortunes[fortune_number]);
    }
    if (strcmp(argv[3], "-b") == 0)
    {
        FILE *fp2 = fopen(batch_name, "r");
        if (fp2 == NULL)
        {
            printf("ERROR: Can't open batch file\n");
            return 1;
        }
        char c;
        int fortune_number_count = 0;
        int *fortune_numbers;
        for (c = getc(fp2); c != EOF; c = getc(fp2))
        {
            if (c == '\n') // Increment count if this character is newline
                fortune_number_count++;
        }
        if (fortune_number_count == 0)
        {
            printf("ERROR: Batch File Empty\n");
            return 1;
        }
        fortune_numbers = malloc(fortune_number_count * sizeof(int));
        char *line2 = NULL;
        size_t len2 = 0;

        rewind(fp2);
        {
            int i = 0;
            while ((getline(&line2, &len2, fp2)) != -1)
            {
                sscanf(line2, "%d", fortune_numbers + i);
                i++;
                free(line2);
                line2 = NULL;
            }
        }
        for (int j = 0; j < fortune_number_count; j++)
        {
            fortune_number = fortune_numbers[j];
            if (fortune_number <= 0 || fortune_number > fortune_count)
            {
                fprintf(output_file, "ERROR: Invalid Fortune Number\n\n");
                continue;
            }
            fprintf(output_file, "%s\n\n", fortunes[fortune_number]);
        }
        // close the file
        fclose(fp2);
    }
}
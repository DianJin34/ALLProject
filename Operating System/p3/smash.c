#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <limits.h>

char **split_by_delimited(const char *str, const char *delimited, int *count)
{
    char *element = NULL;
    char *str_copy = strdup(str);
    *count = 1;
    for (size_t i = 0; str_copy[i]; i++)
    {
        size_t j = 0;
        // contains method to check it
    contains:
        if (!delimited[j])
        {
            continue;
        }
        // if the contains method is these and it works
        if (*(delimited +j) != *(str_copy + i))
        {
            j++;
            goto contains;
        }
        // else what will be happend is it checked next
        else
        {
            (*count)++;
            continue;
        }
    }
    char **splited_string = calloc(*count, sizeof(char *));
    int index = 0;
    element = strtok(str_copy, delimited);
    while (element != NULL)
    {
        *(splited_string + index++) = strdup(element);
        element = strtok(NULL, delimited);
    }
    // free the string copy
    free(str_copy);
    return splited_string;
}


int lexer(char *line, char ***args, int *num_args)
{
    *num_args = 0;
    // count number of args
    char *l = strdup(line);
    if (l == NULL)
    {
        return -1;
    }
    char *token = strtok(l, " \t\n");
    while (token != NULL)
    {
        (*num_args)++;
        token = strtok(NULL, " \t\n");
    }
    free(l);
    // split line into args
    *args = calloc(sizeof(char **), *num_args);
    *num_args = 0;
    token = strtok(line, " \t\n");
    while (token != NULL)
    {
        char *token_copy = strdup(token);
        if (token_copy == NULL)
        {
            return -1;
        }
        (*args)[(*num_args)++] = token_copy;
        token = strtok(NULL, " \t\n");
    }
    return 0;
}

void error_case()
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

void handle_pip(int argc, char **argv, int input, int *output)
{
    // handle_token external command
    int child_process = fork();
    // in the new process
    if (child_process == 0)
    {
        if (input != -1 && input != STDIN_FILENO)
        {
            dup2(input, STDIN_FILENO);
        }
        // close other side of output (input from other file)
        if (*output != -1)
        {
            close(*output);
        }
        if (*(output + 1) != -1 && *(output + 1) != STDOUT_FILENO)
        {
            dup2(*(output+1), STDOUT_FILENO);
        }

        if (strcmp(*argv, "pwd") == 0)
        {
            // invalid param count
            if (argc != 1)
            {
                // perror("Error at pwd arg != 1\n");
                error_case();
                exit(0);
            }
            char cwd[FILENAME_MAX];
            if (getcwd(cwd, FILENAME_MAX) == NULL)
            {
                // perror("error at cwd!\n");
                error_case();
                exit(0);
            }
            printf("%s\n", cwd);
            exit(0);
        }


        execv(*argv, argv);
        error_case();
        exit(0);
    }
    // in the old process
    else
    {
        int status;
        waitpid(child_process, &status, 0);
    }
}

/// handle_token at path by give argument
///
/// path : path to executable
/// argc : number of argument in argv
/// argv : arguments
void handle_token(char *path, int argc, char *argv[])
{
    //////////////////////////////////////////////////
    // No argument case
    if (argc == 0)
    {
        return;
    }

    //////////////////////////////////////////////////
    // exit case
    if (strcmp(*argv, "exit") == 0)
    {
        // invalid param count
        if (argc != 1)
        {
            // perror("Error on exit too many arg\n");
            error_case();
            return;
        }
        exit(0);
    }
    //////////////////////////////////////////////////
    // cd case
     if ((strcmp(*argv, "cd") == 0))
    {
        if (argc != 2)
        {
            error_case();
            return;
        }
        else if (chdir(*(argv + 1)) != 0)
        {
            error_case();
        }
        return;
    }
    //////////////////////////////////////////////////
    // loop case
    else if (strcmp(*argv, "loop") == 0)
    {
        // invalid param count
        if (argc == 1)
        {
            // perror("Erroe on loop but no arg\n");
            error_case();
            return;
        }
        int count = atoi(*(argv + 1));
        // Handle a loop
        for (size_t i = 0; i < count; i++)
        {
            char **n_Argv = calloc(argc - 2 + 1, sizeof(char *));
            // shift argument
            for (int i = 2; i < argc; i++)
            {
                n_Argv[i - 2] = strdup(argv[i]);
            }
            handle_token(*n_Argv, argc - 2, n_Argv);
            free(n_Argv);
        }
        return;
    }

    //////////////////////////////////////////////////
    // for redirection and pip
    int Redirection = 0;
    int p_count = 1;
    for (size_t i = 0; i < argc; i++)
    {
        if (strcmp(*(argv+i), "|") == 0)
        {
            p_count++;
        }
        if (strcmp(*(argv+i), ">") == 0)
        {
            // invalid redirection
            if (i != argc - 2)
            {
                error_case();
                return;
            }
            else
            {
                Redirection = 1;
            }
        }
    }
    //////////////////////////////////////////////////
    // pip case
    // create a table for all command connected by |
    char ***p_argvs = calloc(p_count, sizeof(char **));
    int *p_argcs = calloc(p_count, sizeof(int));
    int p_index = 0;
    int index = 0;

    for (size_t i = 0; i < p_count; i++)
    {
        p_argvs[i] = calloc(argc + 1, sizeof(char *));
    }

    // create the table of arguments for all pipes
    for (size_t i = 0; i < argc; i++)
    {
        if (strcmp(*(argv+i), "|") == 0)
        {
            *(p_argcs + p_index) = index;
            p_index++;
            index = 0;
            continue;
        }
        *(*(p_argvs + p_index) + index++) = *(argv + i);
    }
    *(p_argcs + p_index) = index;

    int p_input_before = STDIN_FILENO;
    int last_p = p_count - 1;
    for (int i = 0; i < last_p; i++)
    {
        int p_fd[2];
        pipe(p_fd);
        handle_pip(*(p_argcs+i), *(p_argvs+i), p_input_before, p_fd);
        // don't close stdin
        if (i != 0)
        {
            close(p_input_before);
        }
        // passs
        p_input_before = *p_fd;
        close(p_fd[1]);
    }

    // check the last process's redirection
    int p_fd[2] = {-1, STDOUT_FILENO};
    if (Redirection)
    {
        fclose(fopen(*(argv + argc - 1), "w"));
        *(p_fd+1) = open(*(*(p_argvs+last_p)+*(p_argcs+last_p) - 1), O_WRONLY, 0666);
        if (*(p_fd + 1) == -1)
        {
            error_case();
            return;
        }
        *(*(p_argvs+last_p)+ *(p_argcs+last_p) - 1) = NULL;
        *(*(p_argvs+last_p)+ *(p_argcs+last_p) - 2) = NULL;
        *(p_argcs+last_p) -= 2;
    }
    // start the last process
    handle_pip(*(p_argcs+last_p), *(p_argvs+last_p), p_input_before, p_fd);
    if (p_input_before != STDIN_FILENO)
    {
        close(p_input_before);
    }

    for (size_t i = 0; i < p_count; i++)
    {
        free(*(p_argvs+i));
    }
    free(p_argvs);
}

/// check command is valid
int is_valid(const char *command)
{
    int r_count = 0;
    for (int i = 0; command[i]; i++)
    {
        if (*(command+i) == '>')
        {
            // cannot have a command start with >
            if (i == 0)
            {
                return 0;
            }
            r_count++;
        }

        // too many redirection
        if (r_count > 1)
        {
            return 0;
        }
    }
    return 1;
}


int main()
{
    while (1)
    {
        // input line
        char *input;
        // size of the input
        size_t size;

        // command info
        char **tokens;
        int command_count;

        printf("smash> ");

        // get the new line
        getline(&input, &size, stdin);
        fflush(stdout);
        // split
        tokens = split_by_delimited(input, ";", &command_count);
        for (int i = 0; i < command_count; i++)
        {
            if (!is_valid(*(tokens+i)))
            {
                error_case();
                continue;
            }
            // handle_token path
            char *path;
            char **exec_argv;
            int exec_argc;

            path = calloc(strlen(tokens[i]), sizeof(char));
            sscanf(tokens[i], "%s", path);
            lexer(tokens[i], &exec_argv, &exec_argc);
            handle_token(path, exec_argc, exec_argv);
        }
    }

    return 0;
}

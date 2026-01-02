#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <glib.h>

#include "interactive/completion.h"
#include "core/dataset.h"

static GPtrArray *active_airport_codes = NULL;
static GPtrArray *active_aircraft_manufs = NULL;
static GPtrArray *active_nationalities = NULL;

void update_completion_context(Dataset *ds)
{
    if (!ds)
    {
        active_airport_codes = NULL;
        active_aircraft_manufs = NULL;
        active_nationalities = NULL;
        return;
    }
    active_airport_codes = get_dataset_airport_codes(ds);
    active_aircraft_manufs = get_dataset_aircraft_manufacturers(ds);
    active_nationalities = get_dataset_nationalities(ds);
}

// --- Generators  ---

char *main_command_gen(const char *str, int state)
{
    static char *main_commands[] = {"dataset", "1", "queries", "2", "view", "3",
                                    "clear", "4", "exit", "quit", "5", NULL};
    static int idx, len;
    char *cmd;

    if (!state)
    {
        idx = 0;
        len = strlen(str);
    }

    while ((cmd = main_commands[idx++]))
    {
        if (!strncmp(cmd, str, len))
        {
            return strdup(cmd);
        }
    }
    return NULL;
}

char *query_command_gen(const char *str, int state)
{
    static char *query_commands[] = {"1", "2", "3", "4", "5", "6", NULL};
    static int idx, len;
    char *cmd;

    if (!state)
    {
        idx = 0;
        len = strlen(str);
    }

    while ((cmd = query_commands[idx++]))
    {
        if (!strncmp(cmd, str, len))
            return strdup(cmd);
    }
    return NULL;
}

char *view_command_gen(const char *str, int state)
{
    static char *view_commands[] = {"1", "2", "3", "4", "5", NULL};
    static int idx, len;
    char *cmd;

    if (!state)
    {
        idx = 0;
        len = strlen(str);
    }

    while ((cmd = view_commands[idx++]))
    {
        if (!strncmp(cmd, str, len))
            return strdup(cmd);
    }
    return NULL;
}

char *airport_code_gen(const char *str, int state)
{
    static guint idx;
    static int len;

    if (!active_airport_codes)
        return NULL;

    if (!state)
    {
        idx = 0;
        len = strlen(str);
    }

    while (idx < active_airport_codes->len)
    {
        char *code = g_ptr_array_index(active_airport_codes, idx++);
        if (strncmp(code, str, len) == 0)
        {
            return strdup(code);
        }
    }
    return NULL;
}

char *aircraft_manufs_gen(const char *str, int state)
{
    static guint idx;
    static int len;

    if (!active_aircraft_manufs)
        return NULL;

    if (!state)
    {
        idx = 0;
        len = strlen(str);
    }

    while (idx < active_aircraft_manufs->len)
    {
        char *manuf = g_ptr_array_index(active_aircraft_manufs, idx++);
        if (strncmp(manuf, str, len) == 0)
        {
            return strdup(manuf);
        }
    }
    return NULL;
}

char *nationality_gen(const char *str, int state)
{
    static guint idx;
    static int len;

    if (!active_nationalities)
        return NULL;

    if (!state)
    {
        idx = 0;
        len = strlen(str);
    }

    while (idx < active_nationalities->len)
    {
        char *nat = g_ptr_array_index(active_nationalities, idx++);
        if (strncmp(nat, str, len) == 0)
        {
            return strdup(nat);
        }
    }
    return NULL;
}

char *shell_cmd_generator(const char *text, int state)
{
    static FILE *fp = NULL;
    static char buffer[1024];
    static int prepend_bang = 0;

    if (!state)
    {
        if (fp)
            pclose(fp);

        const char *query = text;
        prepend_bang = 0;

        if (text[0] == '!')
        {
            prepend_bang = 1;
            query++;
        }

        char cmd[2048];
        // use bash's compgen to get list of commands
        snprintf(cmd, sizeof(cmd), "bash -c \"compgen -c %s\"", query);
        fp = popen(cmd, "r");
    }

    if (!fp)
        return NULL;

    if (fgets(buffer, sizeof(buffer), fp))
    {
        // remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
            buffer[len - 1] = '\0';

        if (prepend_bang)
        {
            char *result = malloc(len + 2); // +1 for '!', +1 for '\0'
            if (result)
            {
                sprintf(result, "!%s", buffer);
                return result;
            }
        }
        return strdup(buffer);
    }

    pclose(fp);
    fp = NULL;
    return NULL;
}

// --- Completion hooks (public) ---

char **main_completion(const char *str, int start, int end)
{
    (void)end;

    // enable default filename completion for shell commands
    if (rl_line_buffer && rl_line_buffer[0] == '!')
    {
        // check if we are completing the command itself (no spaces after '!')
        int is_command = 1;
        for (int i = 1; i < start; i++)
        {
            if (rl_line_buffer[i] == ' ')
            {
                is_command = 0;
                break;
            }
        }

        if (is_command)
        {
            rl_attempted_completion_over = 1;
            return rl_completion_matches(str, shell_cmd_generator);
        }
        else
        {
            rl_attempted_completion_over = 0;
            return NULL;
        }
    }

    rl_attempted_completion_over = 1;
    return rl_completion_matches(str, main_command_gen);
}

char **query_completion(const char *str, int start, int end)
{
    (void)start;
    (void)end;
    rl_attempted_completion_over = 1;
    return rl_completion_matches(str, query_command_gen);
}

char **view_completion(const char *str, int start, int end)
{
    (void)start;
    (void)end;
    rl_attempted_completion_over = 1;
    return rl_completion_matches(str, view_command_gen);
}

char **airport_code_completion(const char *str, int start, int end)
{
    (void)start;
    (void)end;
    rl_attempted_completion_over = 1;
    return rl_completion_matches(str, airport_code_gen);
}

char **aircraft_manufs_completion(const char *str, int start, int end)
{
    (void)start;
    (void)end;
    rl_attempted_completion_over = 1;
    return rl_completion_matches(str, aircraft_manufs_gen);
}

char **nationality_completion(const char *str, int start, int end)
{
    (void)start;
    (void)end;
    rl_attempted_completion_over = 1;
    return rl_completion_matches(str, nationality_gen);
}
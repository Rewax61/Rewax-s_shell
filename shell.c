#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#define MAX_HISTORY_LENGHT 100
#define MAX_COMMAND_LENGHT 200

void init(char * prompt);
void cmd_copy(int cursor_pos, char c, char * cmd);
void cmd_backspace(int cursor_pos, char * cmd);

int main() {
    /* Désactiver le mode canonique et l'affichage de l'entrée */
    struct termios original_attributes;
    tcgetattr(STDIN_FILENO, &original_attributes);
    struct termios new_attributes = original_attributes;
    new_attributes.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_attributes);


    char history[MAX_HISTORY_LENGHT][MAX_COMMAND_LENGHT];
    int history_lenght = 0;
   
    char * prompt = malloc(1 * sizeof(char));
    init(prompt);

    while (1) {
        int cursor_pos = 0;
        int cursor_limit = 0;
        char * cmd = malloc(1 * sizeof(char)); cmd[0] = '\0';
        char c = '\0';
        printf("%s", prompt);
        int history_pos = history_lenght;
        while (1) {
            c = getchar();
            if (c == '\n') {
                break;
            }
            switch (c) {
            case '\033':
                getchar();
                char e = getchar();
                switch (e) {
                    case 'A': /* Up */
                        if (! (history_pos > 0))
                            break;
                        if (cursor_pos != 0)
                            printf("\033[%dD", cursor_pos);
                        printf("\033[K");
                        history_pos--;
                        printf("%s", history[history_pos]);
                        cursor_pos = strlen(history[history_pos]);
                        cursor_limit = cursor_pos;
                        strcpy(cmd, history[history_pos]);
                        break;
                    case 'B': /* Down */
                        if (history_pos == history_lenght)
                            break;
                        if (cursor_pos != 0)
                            printf("\033[%dD", cursor_pos);
                        printf("\033[K");
                        history_pos++;
                        printf("%s", history[history_pos]);
                        cursor_pos = strlen(history[history_pos]);
                        cursor_limit = cursor_pos;
                        strcpy(cmd, history[history_pos]);
                        break;
                    case 'C': /* Right */
                        if (cursor_limit > cursor_pos) {
                            printf("\033[1C");
                            cursor_pos++;
                        }
                        break;
                    case 'D': /* Left */
                        if (cursor_pos > 0) {
                            printf("\033[1D");
                            cursor_pos--;
                        }
                        break;
                    default:
                        break;
                }
                break;
            case 0x7F: /* Backspace */
                if (cursor_pos > 0) {
                    cmd_backspace(cursor_pos, cmd);
                    if (cursor_pos != 0)
                        printf("\033[%dD", cursor_pos);
                    printf("\033[K");
                    printf("%s", cmd);
                    cursor_pos--;
                    cursor_limit--;
                    if (cursor_limit-cursor_pos != 0)
                        printf("\033[%dD", cursor_limit-cursor_pos);
                }
                break;
            default:
                cmd_copy(cursor_pos, c, cmd);
                if (cursor_pos != 0)
                    printf("\033[%dD", cursor_pos);
                printf("\033[K");
                printf("%s", cmd);
                cursor_pos++;
                cursor_limit++;
                if (cursor_limit-cursor_pos != 0)
                    printf("\033[%dD", cursor_limit-cursor_pos);
                break;
            }          
        }
        putchar('\n');
        
        strcpy(history[history_lenght], cmd);
        history_lenght++;
        
        if (! strcmp(cmd, "exit")) {
            break;
        } else if (! strcmp(cmd, "help")) {
            printf("Available commands: \n\texit: quit the shell\n\tclear: clean the shell\n\trestart: reboot the shell\n\thello: show a huge \"Hello World!\"\n\tbell: play alert sound\n");
        } else if (! strcmp(cmd, "hello")) {
            printf(" /$$   /$$           /$$ /$$                 /$$      /$$                     /$$       /$$ /$$\n"
"| $$  | $$          | $$| $$                | $$  /$ | $$                    | $$      | $$| $$\n"
"| $$  | $$  /$$$$$$ | $$| $$  /$$$$$$       | $$ /$$$| $$  /$$$$$$   /$$$$$$ | $$  /$$$$$$$| $$\n"
"| $$$$$$$$ /$$__  $$| $$| $$ /$$__  $$      | $$/$$ $$ $$ /$$__  $$ /$$__  $$| $$ /$$__  $$| $$\n"
"| $$__  $$| $$$$$$$$| $$| $$| $$  \\ $$      | $$$$_  $$$$| $$  \\ $$| $$  \\__/| $$| $$  | $$|__/\n"
"| $$  | $$| $$_____/| $$| $$| $$  | $$      | $$$/ \\  $$$| $$  | $$| $$      | $$| $$  | $$    \n"
"| $$  | $$|  $$$$$$$| $$| $$|  $$$$$$/      | $$/   \\  $$|  $$$$$$/| $$      | $$|  $$$$$$$ /$$\n"
"|__/  |__/ \\_______/|__/|__/ \\______/       |__/     \\__/ \\______/ |__/      |__/ \\_______/|__/\n");
        } else if (! strcmp(cmd, "clear")) {
            printf("\033[2J\033[0;0H");
        } else if (! strcmp(cmd, "restart")) {
            init(prompt);
        } else if (! strcmp(cmd, "bell")) {
            printf("\a");
        } else {
            fprintf(stderr, "%s: command not found\ntype \"help\" for more information\n", cmd);
        }
    }




    /* Réactiver les paramètres par défault */
    tcsetattr(STDIN_FILENO, TCSANOW, &original_attributes);

    return 0;
}


void init(char * prompt) {
    prompt = realloc(prompt, 17 * sizeof(char));
    strcpy(prompt, "Rewax's Shell> ");
    

    printf("===============================================================\n");
    printf("# ______                        _       _____ _          _ _  #\n"
           "# | ___ \\                      ( )     /  ___| |        | | | #\n"
           "# | |_/ /_____      ____ ___  _|/ ___  \\ `--.| |__   ___| | | #\n"
           "# |    // _ \\ \\ /\\ / / _` \\ \\/ / / __|  `--. \\ '_ \\ / _ \\ | | #\n"
           "# | |\\ \\  __/\\ V  V / (_| |>  <  \\__ \\ /\\__/ / | | |  __/ | | #\n"
           "# \\_| \\_\\___| \\_/\\_/ \\__,_/_/\\_\\ |___/ \\____/|_| |_|\\___|_|_| #\n");
    printf("===============================================================\n\n");
}

void cmd_copy(int cursor_pos, char c, char * cmd) {
    size_t lenght = strlen(cmd);
    char * buffer = malloc((lenght + 1) * sizeof(char));
    buffer[lenght] = '\0';
    strcpy(buffer, cmd);
    cmd = realloc(cmd, lenght+2 * sizeof(char));
    for (int i = 0; i < cursor_pos; i++) {
        cmd[i] = buffer[i];
    }
    cmd[cursor_pos] = c;
    int max_size = lenght+1;
    for (int e = cursor_pos+1; e < max_size;e++) {
        cmd[e] = buffer[e-1];
    }

    cmd[lenght+1] = '\0';
}

void cmd_backspace(int cursor_pos, char * cmd) {
    size_t lenght = strlen(cmd);
    char * buffer = malloc((lenght + 1) * sizeof(char));
    buffer[lenght] = '\0';
    strcpy(buffer, cmd);
    cmd = realloc(cmd, lenght * sizeof(char));
    for (int i = 0; i < cursor_pos-1; i++) {
        cmd[i] = buffer[i];
    }
    int max_size = lenght-2;
    for (int e = cursor_pos-1; e <= max_size; e++) {
        cmd[e] = buffer[e+1];
    }

    cmd[lenght-1] = '\0';
}

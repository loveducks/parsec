#include<stddef.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_SIZE 1000000

char *get_clipboard();
char* print_json_pretty(const char *json);
void remove_empty_space(char *json);

// xclip -selection clipboard -o | less
// xclip -selection primary -o | less

int main(void) {
	char *s = get_clipboard();
	if (s) {
		FILE *pipe = popen("less", "w");
		if (!pipe) {
			perror("less command failed!");
			return 1;
		}
		fprintf(pipe, "%s\n", s);
		pclose(pipe);
		free(s);
	} else {
		printf("Failed to read clipboard.\n");
	}
	return 0;
}

char *get_clipboard() {
    FILE *fp;
    char buffer[MAX_SIZE];
    size_t size = 1;

    char *result = malloc(size);
    if (!result)
        return NULL;

    result[0] = '\0';

    fp = popen("xclip -selection primary -o", "r");

    if (fp == NULL) {
        perror("Primary empty! Trying clipboard...\n");
        fp = popen("xclip -selection clipboard -o", "r");
    }

    if (fp == NULL) {
        perror("Primary Clipboard empty! \n");
        free(result);
        return NULL;
    }

    int *tabs = malloc(sizeof(int));
    *tabs = 0;
    while(fgets(buffer, sizeof(buffer), fp)) {
        size_t len = strlen(buffer);
        char *temp = realloc(result, size + len);
        remove_empty_space(buffer);

        if (!temp) {
            free(result);
            pclose(fp);
            return NULL;
        }

        result = temp;
        strcat(result, buffer);
        size += len;
    }

    char* pretty = print_json_pretty(result);

    free(tabs);
    free(result);

    pclose(fp);
    return pretty;
}

void remove_empty_space(char *json) {
    while(*json) {
        if (*json == ' ' || *json == '\n' || *json == '\t') {
            char *temp = json;
            while (*temp) {
                *temp = *(temp + 1);
                temp++;
            }
        } else {
            json++;
        }
    }
}

char* print_json_pretty(const char *json) {
	int tabs = 0;
	size_t size = 128;
	size_t pos = 0;

	char *formatted = malloc(size);

	for (int i = 0; json[i]; i++) {
		char c = json[i];

		if (pos + 10 >= size) {
			size *= 2;
			formatted = realloc(formatted, size);
		}

		if (c == '{' || c == '[') {
			formatted[pos++] = c;
			formatted[pos++] = '\n';
			tabs++;

			for (int t = 0; t < tabs; t++) {
				formatted[pos++] = ' ';
				formatted[pos++] = ' ';
            }

		} else if (c == '}' || c == ']') {
			formatted[pos++] = '\n';
			tabs--;

			for (int t = 0; t < tabs; t++) {
				formatted[pos++] = ' ';
				formatted[pos++] = ' ';
            }

			formatted[pos++] = c;

		} else if (c == ',') {
			formatted[pos++] = c;
			formatted[pos++] = '\n';

			for (int t = 0; t < tabs; t++) {
				formatted[pos++] = ' ';
				formatted[pos++] = ' ';
            }

		} else {
			formatted[pos++] = c;
		}
	}

	formatted[pos] = '\0';
	return formatted;
}

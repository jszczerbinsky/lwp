#include "main.h"
#include <stdio.h>
#include <stdlib.h>

char* dict_get(Dict* dict, const char* key) {
	while (dict) {
		if (strcmp(key, dict->key) == 0)
			return dict->val;
		dict = dict->next;
	}

	return NULL;
}

Dict* dict_add(Dict* dict, const char* key, const char* val) {
	char* existing_val = dict_get(dict, key);

	if (existing_val) {
		strcpy(existing_val, val);
		return dict;
	} else {
		Dict* new_pair = malloc(sizeof(Dict));
		strncpy(new_pair->key, key, DICT_KEY_MAX - 1);
		strncpy(new_pair->val, val, DICT_VAL_MAX - 1);
		new_pair->key[DICT_KEY_MAX - 1] = '\0';
		new_pair->key[DICT_VAL_MAX - 1] = '\0';
		new_pair->next = dict;
		return new_pair;
	}
}

void dict_free(Dict* dict) {
	while (dict) {
		Dict* next = dict->next;
		free(dict);
		dict = next;
	}
}

Dict* dict_read(const char* path) {
	FILE* f = fopen(path, "r");

	Dict* dict = NULL;

	if (!f) {
		printlog(LOG_ERROR, "Failed read config file - can't open %s", path);
		return NULL;
	}

	char keybuff[DICT_KEY_MAX];
	char valbuff[DICT_VAL_MAX];
	keybuff[0] = '\0';
	valbuff[0] = '\0';

	int	 buff_i = 0;
	int	 iskey = 1;
	char c;
	while ((c = fgetc(f)) != EOF) {
		char* buff = iskey ? keybuff : valbuff;
		int	  buffmax = iskey ? DICT_KEY_MAX : DICT_VAL_MAX;

		if (c == '=') {
			iskey = 0;
			buff_i = 0;
		} else if (c == '\n') {
			iskey = 1;
			buff_i = 0;

			dict = dict_add(dict, keybuff, valbuff);

			keybuff[0] = '\0';
			valbuff[0] = '\0';

		} else {
			if (buff_i == buffmax - 1) {
				printlog(LOG_WARNING,
						 "Failed read line in config file %s - %s is longer "
						 "than %d chars",
						 path, iskey ? "key" : "value", buffmax);
			} else {
				buff[buff_i] = c;
				buff_i++;
				buff[buff_i] = '\0';
			}
		}
	}
	fclose(f);

	return dict;
}

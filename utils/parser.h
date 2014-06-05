#ifndef _PARSER_API_
#define _PARSER_API_

extern void handle_CLI(void (*single_char_CLI)(const char *c),
				void (*single_char_CLI_with_num)(const char *c),
				void (*multi_char_CLI)(const char *c));

#endif //_PARSER_API_

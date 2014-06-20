#ifndef _TERMIOS_CONTROL_API_
#define _TERMIOS_CONTROL_API_
#include <stdio.h>
#include <limits.h>                /* CHAR_MAX */
#include <stdbool.h>               /* bool, true, false */
#include <termios.h>               /* TERM IO API */

static struct termios old_term_attr, new_term_attr;

/* Below ANSI terminal sequences do not work in emacs */
static inline void term_control_seq_exec_api(const char c[])
{
	if(c) printf("%s", c);
	fflush(stdout);
}

/* ANSI/VT100 Terminal Control Escape Sequence - <ESC>[2J *
 * Erase screen with bgnd color + moves cursor to home    *
 * position which is upper left of screen by default      */
static inline void term_clear_screen()
{
	const char clr[] = {  27, '[', '2', 'J', '\0' };
	term_control_seq_exec_api(clr);
}

/* ANSI/VT100 Terminal Ctrl Esc Seq - <ESC>[{ROW};{COL}H  *  
 * Set cursor position to top left of the screen (home)   */
static inline void term_move_cursor_to_top_left()
{
	const char top_left[] = { 27, '[', '1', ';', '1', 'H','\0' };
	term_control_seq_exec_api(top_left);
}

/* ANSI/VT100 Ctrl Esc Seq - <ESC>[{attr1};...;{attrn}m   *  
 * Set attributes mode, 0=reset, 4=underscore 5=blink     */
static inline void term_emphasized_print_str(const char str[])
{
	const char emp[] = { 27, '[', '4', ';', '5', 'm', '\0' };
	const char emp_res[] = { 27, '[', '0', 'm', '\0' };
	term_control_seq_exec_api(emp);
	printf("%s\n", str);
	term_control_seq_exec_api(emp_res);
}

/* Initialize new terminal i/o settings */
void termios_init(bool echo)
{
	/* grab old terminal i/o settings */
	tcgetattr(fileno(stdin), &old_term_attr);
	/* make new settings same as old settings */
	new_term_attr = old_term_attr;
	/* disable buffered i/o */
	new_term_attr.c_lflag &= ~ICANON;
	/* set echo mode */
	if(echo)
		new_term_attr.c_lflag |= ECHO;
	else
		new_term_attr.c_lflag &= ~ECHO;
	new_term_attr.c_cc[VTIME] = 0;
	new_term_attr.c_cc[VMIN] = 0;
	/* apply these new terminal i/o settings now */
	tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);
}

/* Restore old terminal i/o settings */
void termios_reset(void)
{
	tcsetattr(0, TCSANOW, &old_term_attr);
}

#endif //_TERMIOS_CONTROL_API_

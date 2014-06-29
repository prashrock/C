#ifndef _TERMIOS_CONTROL_API_
#define _TERMIOS_CONTROL_API_
#include <stdio.h>          /* printf(3) */
#include <limits.h>         /* CHAR_MAX */
#include <stdbool.h>        /* bool, true, false */
#include <termios.h>        /* TERM IO API */
#include <stdlib.h>         /* exit(3) */
#include <unistd.h>         /* fork(3), chdir(3), sysconf(3) */
#include <signal.h>         /* signal(3) */
#include <sys/stat.h>       /* umask(3) */
#include <syslog.h>         /* syslog(3), openlog(3), closelog(3) */
#include <string.h>         /* strlen */

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
static inline void termios_init(bool echo)
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
static inline void termios_reset(void)
{
	tcsetattr(0, TCSANOW, &old_term_attr);
}

/* Need to figure out a way to re-route TTY to current PTY session */
/* Not using perror, as stdout may be routed to /dev/null :)       */
static inline bool daemonize(char* name, char* path, char *infile,
							 char* outfile, char* errfile)
{
    /* Our process ID and Session ID */
	pid_t pid, sid;
	int fd;
	
	if(!path) { path="/tmp/"; }
	if(!name) { name="medaemon"; }
	
	/* Force standard in to /dev/null if not instructed otherwise      *
	 * If user wants to leave out STDOUT/STDERR as it is, honor that   */
	if(!infile) { infile="/dev/null"; }

    /* Fork off the parent process */
	pid = fork();
	if(pid < 0){ //failed fork
		fprintf(stderr,"Error - Fork failed\n");
		return false;
	}
	/* If PID is good, exit Parent process */
	if(pid > 0){
		printf("%s Daemon[%d] created successfully\n", name, pid);	
		exit(EXIT_SUCCESS);
	}
	/* Change the file mode mask */
	umask(0);

	//open syslog
	openlog(name, LOG_CONS | LOG_PID, LOG_DAEMON);

	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0) {
		fprintf(stderr,"Error - failed setsid\n");
		return false;
	}
	
    /* Change the current working directory */
	if ((chdir(path)) < 0) {
		fprintf(stderr,"Error - failed chdir\n");
		return false;
	}

    //catch/ignore signals -- need to revisit this...
	//signal(SIGCHLD,SIG_IGN);
	//signal(SIGHUP,SIG_IGN);

	/* Close out the standard file descriptors and all open FD's */
	close(STDIN_FILENO);
	if(outfile) close(STDOUT_FILENO);
	if(errfile) close(STDERR_FILENO);
	for( fd = sysconf(_SC_OPEN_MAX); fd > STDERR_FILENO; --fd )
		close(fd);
	
	//reopen stdin, stdout, stderr
	stdin = fopen(infile,"r");                 //fd = 0
	if(outfile) stdout = fopen(outfile,"w+");  //fd = 1
	if(errfile) stderr = fopen(errfile,"w+");  //fd = 2
	
	return true;
}


#endif //_TERMIOS_CONTROL_API_

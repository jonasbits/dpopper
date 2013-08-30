

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <varargs.h>

#define NO_USER		1
#define NO_PASS		2
#define LOGGED_IN	3

void readcommand( char* buffer, int bufferlen );
void cleanexit();
void setsignals();
void blocksignals();
void do_prelogin_usage();
void do_loggedin_usage();
static void bail();

FILE*	in;
FILE*	out;

#define	YES	1
#define NO	2

#define TIMEOUT		(10*60)		/* 10 minute timeout by default */

void main( int argc, char** argv[] ){

	char	buffer[ 1024 ];
	char	username[ 64 ];
	int	state = NO_USER;
	int	valid_command;

	in = fdopen( fileno( stdin ), "rb" );
	if( in == NULL ){
		perror( "Opening input stream" );
		syslog( LOG_EMERG, "Opening input stream failed with error %m" );
		return;
	}

	out = fdopen( fileno( stdout ), "wb" );
	if( out == NULL ){
		syslog( LOG_EMERG, "Opening output stream failed with error %m" );
		perror( "Opening output stream" );
		return;
	}

	openlog( (char*) argv[0], LOG_PID, LOG_MAIL );

	setsignals();

	if( gethostname( buffer, sizeof( buffer ) ) == -1 ){
		perror( "Get hostname failed!" );
		syslog( LOG_EMERG, "gethostname() failed with error %m" );
		strcat( buffer, "unknown" );
	}

	srandom( getpid() );

	fprintf( out, "+OK KP's DPopper V1.00 (kpielorz@tdx.com) Ready <%i%i@%s>\r\n", getpid(), random(), buffer );

	while( state != LOGGED_IN ){

		valid_command = NO;

		readcommand( buffer, sizeof( buffer ) );

		if( strncasecmp( buffer, "QUIT", 4 ) == 0 ){
			if( strlen( buffer ) > 4 ){
				do_prelogin_usage();
			} else {
				fprintf( out, "+OK Oh dear, not having much luck today are we?\r\n" );
				cleanexit();
			}
		}

		if( strcasecmp( buffer, "USER" ) == 0 ){
			if( state == NO_USER ){
				fprintf( out, "-ERR Invalid username.\r\n" );
			} else {
				fprintf( out, "-ERR Already selected a username.\r\n" );
			}
			valid_command = YES;
		}

		if( strncasecmp( buffer, "USER ", 5 ) == 0 ){
			if( state == NO_USER ){
				fprintf( out, "+OK %s selected.\r\n", buffer+5 );
				username[0] = 0;
				strncat( username, buffer + 5, sizeof( username ) );
				state = NO_PASS;
			} else {
				fprintf( out, "-ERR Already selected a username.\r\n" );
			}
			valid_command = YES;
		}

		if( strncasecmp( buffer, "PASS", 4 ) == 0 ){
			if( state == NO_USER ){
				fprintf( out, "-ERR Select a username first\r\n" );
			} else {
				if( strlen( buffer ) > 4 ){
					fprintf( out, "+OK Congratulations!\r\n" );
					syslog( LOG_INFO, "Allowed user \"%s\" to poll for mail.", (char*) username );
					state = LOGGED_IN;
				} else {
					fprintf( out, "-ERR dpopper: Invalid password or username (check case).\r\n" );
					state = NO_USER;	/* This is the same behaviour as the Real POP3 :) */
				}
			}
			valid_command = YES;
		}

		if( strcasecmp( buffer, "APOP" ) == 0 ){
			fprintf( out, "-ERR Woah! - I feel good!\r\n" );
			valid_command = YES;
		}

		if( strncasecmp( buffer, "APOP ", 5 ) == 0 ){
			fprintf( out, "-ERR cucipop: Invalid password or username (check case)\r\n" );
			valid_command = YES;
		}

		if( strcasecmp( buffer, "WHO" ) == 0 ){
			fprintf( out, "+OK I am the doppleganger.\r\n" );
			valid_command = YES;
		}

		if( valid_command == NO ){
			do_prelogin_usage();
		}

	}

	while( 1 ){

		valid_command = NO;

		readcommand( buffer, sizeof( buffer ) );

		if( strcasecmp( buffer, "STAT" ) == 0 ){
			fprintf( out, "+OK 0 0\r\n" );
			valid_command = YES;
		}

		if( strcasecmp( buffer, "WHO" ) == 0 ){
			fprintf( out, "+OK I am the doppleganger\r\n" );
			valid_command = YES;
		}

		if( strcasecmp( buffer, "QUIT" ) == 0 ){
			fprintf( out, "+OK Thanks for trying to make my day...\r\n" );
			valid_command = YES;
			cleanexit();
		}

		if( strcasecmp( buffer, "LIST" ) == 0 ){
			fprintf( out, "+OK 0 messages (0 octets)\r\n.\r\n" );
			valid_command = YES;
		}

		if( strncasecmp( buffer, "LIST ", 5 ) == 0 ){
			fprintf( out, "-ERR No such message\r\n" );
			valid_command = YES;
		}

		if( strncasecmp( buffer, "RETR", 4 ) == 0 ){
			fprintf( out, "-ERR No such message\r\n" );
			valid_command = YES;
		}

		if( strncasecmp( buffer, "TOP ", 4 ) == 0 ){
			fprintf( out, "-ERR No such message\r\n" );
			valid_command = YES;
		}

		if( strncasecmp( buffer, "DELE", 4 ) == 0 ){
			fprintf( out, "-ERR No such message\r\n" );
			valid_command = YES;
		}

		if( strncasecmp( buffer, "UIDL ", 5 ) == 0 ){
			fprintf( out, "-ERR No such message\r\n" );
			valid_command = YES;
		}

		if( strcasecmp( buffer, "UIDL" ) == 0 ){
			fprintf( out, "+OK But please remember to DELETE messages SOMETIME!\r\n.\r\n" );
			valid_command = YES;
		}

		if( strcasecmp( buffer, "NOOP" ) == 0 ){
			fprintf( out, "+OK\r\n" );
			valid_command = YES;
		}

		if( strcasecmp( buffer, "RSET" ) == 0 ){
			fprintf( out, "+OK 0 messages (0 octets)\r\n" );
			valid_command = YES;
		}

		if( valid_command == NO ){
			do_loggedin_usage();
		}

	}

}

void do_prelogin_usage(){

	fprintf( out, "-ERR Invalid command, try one of: USER name, PASS string, APOP name digest, QUIT\r\n" );

}

void do_loggedin_usage(){

	fprintf( out, "-ERR Invalid command, try one of: STAT, LIST [msg], RETR msg, TOP msg n, DELE msg, UIDL [msg], NOOP, RSET, QUIT\r\n" );

}

void cleanexit(){

	fflush( out );
	closelog();
	exit( 0 );

}

void readcommand( char* buffer, int bufferlen ){

	char*	ptr;
	
	alarm( TIMEOUT );

	fflush( out );
	fgets( buffer, bufferlen, in );

	if( ( ptr = (char*) strchr( buffer, '\r' ) ) ){
		*ptr = 0;
	}
	if( ( ptr = (char*) strchr( buffer, '\n' ) ) ){
		*ptr = 0;
	} 	

	alarm( 0 );

}

void setsignals(){

	signal( SIGHUP, bail );
	signal( SIGINT, bail );
	signal( SIGTERM, bail );
	signal( SIGPIPE, bail );
	signal( SIGURG, bail );
	signal( SIGALRM, bail );

}

void blocksignals(){

	signal( SIGHUP, SIG_IGN );
	signal( SIGINT, SIG_IGN );
	signal( SIGTERM, SIG_IGN );
	signal( SIGPIPE, SIG_IGN );
	signal( SIGURG, SIG_IGN );
	signal( SIGALRM, SIG_IGN );

}

static void bail(){

	char	abnormal[]="-ERR Abnormal exit\r\n";

	blocksignals();

	syslog( LOG_EMERG, "Abnormal exit / signal received." );
	write( 1, abnormal, sizeof( abnormal ) );

	exit( 0 );


}

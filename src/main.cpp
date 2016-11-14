/**
 * Header files
 */
#include "support.h"
#include "log.h"
#include "schedular.h"

/**
 * options structure
 * keep track of the following flags
 * debug: to enable/didsable debugging
 * verbose mode output
 * type of scheduling (see support.h)
 * source filename to read processes
 */
struct _opts {
	int debug;		/* Debug */
	int verbose;		/* Verbose output */
	unsigned int type;	/* type of scheduling */
	unsigned int time;	/* time quantum for round robin */
	unsigned int jobs;	/* number of random jobs to create */
	char* filename;		/* file name of source file */
} opts;

/**
 * Signal handler in case of Ctrl+C or Segmentation fault
 * Not doing anything for the moment
 */
void signal_handler(int arg __attribute__((unused)))
{
	/**
	 * TODO:
	 * Please write some code here
	 */
}

/**
 * Usage:
 * Format of the binary
 * Display help how to use the binary
 */
void Usage()
{
	printf("Usage: sched [options]\n"
		"\n"
		"    Schedualing policy\n"
		"    sched [-v] -[R <k>|S|F] [-f <filename> |-r n]\n"
		"\n"
		"    -v, --verbose           More output\n"
		"    -R, --roundrobin NUMBER Set policy as round robin with time quntam\n"
		"    -S, --shortest          Set policy as shortest remaining job first\n"
		"    -F, --firstcome         Set policy as first come first serve\n"
		"    -f, --filename FILENAME Use file for input processes\n"
		"    -r, --random NUMBER     Use random number of jobs\n"
#ifdef DEBUG
		"    -d, --debug             Show debugging information\n"
#endif // DEBUG
		"\n");
}

/**
 * parse_options:
 * argc: number of parameters provided
 * argv: parameters list
 * 
 * Parse all the available options, and update the requirements
 * If there is any issue, display the help
 */
int parse_options(int argc, char **argv)
{
	debug_log("Entering %s ...", __FUNCTION__);	/* tracing code for debugging */

	const char *pOpt = "-vR:SFf:r:"; /* Format of application */
	const struct option cOpt[] = {
#ifdef DEBUG
		{ "debug",	no_argument,		NULL, 'd' },	/* debug */
#endif
		{ "verbose",	no_argument,		NULL, 'v' },	/* verbose */
		{ "roundrobin",	required_argument,	NULL, 'R' },	/* round roubin, requires another argument for time quantum */
		{ "shortest",	no_argument,		NULL, 'S' },	/* SRJF */
		{ "firstcome",	no_argument,		NULL, 'F' },	/* FCFS */
		{ "filename",	required_argument,	NULL, 'f' },	/* filename, requires another argument for name */
		{ "random",	required_argument,	NULL, 'r' },	/* random, requires another argument for number of jobs */
		{ NULL, 0, NULL, 0 }
	};

	int nRes = 0;
	int c = 0;
	int err = 0;
	bool bIsType = false;
	bool bIsSource = false;
	memset(&opts, 0, sizeof(opts));

	/* Parse the options */
	while ((c = getopt_long(argc, argv, pOpt, cOpt, NULL)) != -1 && err == 0) {
		switch (c) {
#ifdef DEBUG
		case 'd':
			opts.debug = 1;		/* debugging logs */
			break;
#endif // DEBUG
		case 'v':
			opts.verbose = 1;	/* verbose mode output */
			break;
		case 'R':
			if (bIsType)		/* we already have type, this shouldn't happen */
				err = 1;
			else {
				bIsType = true;
				opts.type = RR;				/* set type of job as Round Robin */
				opts.time = atoll(argv[optind-1]);	/* get round robin time quantum */
			}
			break;
		case 'S':
			if (bIsType)		/* we already have type, this shouldn't happen */
				err = 1;
			else {
				bIsType = true;
				opts.type = SRJF;	/* set type of job as SRJF */
			}
			break;
		case 'F':
			if (bIsType)		/* we already have type, this shouldn't happen */
				err = 1;
			else {
				opts.type = FIFO;	/* set type of job as FIFO */
				bIsType = true;
			}
			break;
		case 'f':
			if (bIsSource)
				err = 1;	/* we already have source, this shouldn't happen */
			else {
				bIsSource = true;
				opts.filename = argv[optind-1];	/* get input filename */
			}
			break;
		case 'r':
			if (bIsSource)
				err = 1;		/* We already have source, this shouldn't happen */
			else {
				bIsSource = true;
				opts.type |= RAND;	/* random jobs */
				opts.jobs = atoll(argv[optind-1]);	/* get number of jobs to create randomly */
			}
			break;
		default:
			perr_printf("Invalid arguments");
			err = 1;
			break;
		}
	}

	if (!bIsType || !bIsSource)	/* Do we have any error? */
		err = 1;

	if (err) {
		Usage();	/* We do have an error, display program usage and return */
		nRes = -1;
	}

	debug_log("Exiting %s with code %d (0x%x)...", __FUNCTION__, nRes, nRes);	/* exit trace for debugging */

	return nRes;
}

/**
 * main:
 * argc: number of arguments from outside
 * argv: actual list of arguments
 */
int main(int argc, char* argv[])
{
	/* Set SIGINT handler. */
	if (signal(SIGINT, signal_handler) == SIG_ERR) {
		perr_printf("Failed to set SIGINT handler");
		return 1;
	}
	/* Set SIGTERM handler. */
	if (signal(SIGTERM, signal_handler) == SIG_ERR) {
		perr_printf("Failed to set SIGTERM handler");
		return 1;
	}

	/* parse the options */
	if (parse_options(argc, argv)) {
		return 1;
	}

	/* Initialize the CSchedular class and start the process */
	CSchedular sched(opts.type, opts.time, opts.filename, opts.jobs, opts.verbose);
	sched.Start();

	return 0;
}

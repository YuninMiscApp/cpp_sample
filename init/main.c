#include <stdio.h>	   
#include <stdlib.h>	 
#include <getopt.h>

#include "cpu.h"
#include "mem.h"
#include "task.h"



void usage_show(void)
{
	printf("================ usage: ===============\n");
	printf("./system_monitor -c\n");
	printf("./system_monitor -m\n");
	printf("./system_monitor -t [task_name]\n");
	printf("=======================================\n");
}

int
main(int argc, char **argv)
{
	int c;
	int digit_optind = 0;
	char *process_name = NULL;
	
	if (argc < 2) {
		usage_show();
		return -1;
	}
	
	while (1) {
		int this_option_optind = optind ? optind : 1;
		int option_index = 0;
		static struct option long_options[] = {
			{"cpu",	    no_argument, 0,  'c' },
			{"mem",     no_argument, 0,  'm' },
			{"task",    no_argument, 0,  't' },
			{"verbose", no_argument, 0,  'v' },
			{"create",  required_argument, 0, 'c'},
			{"file",    required_argument, 0, 'f'},
			{0,		    0,				   0,  0}
		};

		c = getopt_long(argc, argv, "cmtvc:f:012",
		long_options, &option_index);
		if (c == -1)
		break;

		switch (c) {
		case 0:
			printf("option %s", long_options[option_index].name);
			if (optarg)
				printf(" with arg %s", optarg);
			printf("\n");
		break;

		case '0':
		case '1':
		case '2':
			if (digit_optind != 0 && digit_optind != this_option_optind)
				printf("digits occur in two different argv-elements.\n");
			digit_optind = this_option_optind;
			printf("option %c\n", c);
			break;

		case 'c':
			printf("====> running CPU thread \n");
			cpu_show();
			break;

		case 'm':
			printf("====> running MEM thread \n");
			mem_show();
			break;

		case 't':
			process_name = argv[2];
			printf("====> running TASK (%s) thread \n",process_name);
			task_show(process_name);
			break;

		case 'v':
		case '?':
			usage_show();
			break;

		default:
			printf("?? getopt returned character code 0%o ??\n", c);
		}
	}

	if (optind < argc) {
		printf("non-option ARGV-elements: ");
		while (optind < argc)
			printf("%s ", argv[optind++]);
		printf("\n");
	}

	exit(EXIT_SUCCESS);
}





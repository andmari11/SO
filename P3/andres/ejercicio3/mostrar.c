#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{

	int opt;
	bool final = false;
	int offset = 0;
	while ((opt = getopt(argc, argv, "n:e")) != -1)
	{
		switch (opt)
		{
		case 'n':
			offset = atoi(optarg);
			break;
		case 'e':
			final = true;
			break;
		default:
			fprintf(stderr, "Uso: %s [-n N] [-e] archivo\n", argv[0]);
			exit(-1);
		}
	}

	int file = open(argv[optind], O_RDONLY);

	if (!final)
	{
		if (lseek(file, offset, SEEK_SET) == -1)
		{
			perror("error offset");
			exit(-1);
		}
	}
	else{

		if (lseek(file, -offset, SEEK_END) == -1)
		{
			perror("error offset");
			exit(-1);
		}
	}

	char letra;

	while (read(file, &letra, 1) > 0)
	{

        printf("%c", letra);
	}

	return 0;
}

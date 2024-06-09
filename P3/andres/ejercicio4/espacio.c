#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

/* Forward declaration */
int get_size_dir(char *dname, size_t *blocks);

/* Gets in the blocks buffer the size of file fname using lstat. If fname is a
 * directory get_size_dir is called to add the size of its contents.
 */
int get_size(char *fname, size_t *blocks)
{
	struct stat file_stat;
	if (lstat(fname, &file_stat) == -1)
	{

		perror("lstat");
		exit(-1);
	}

	if (S_ISREG(file_stat.st_mode))
	{

		*blocks += file_stat.st_size / 512;
	}
}

/* Gets the total number of blocks occupied by all the files in a directory. If
 * a contained file is a directory a recursive call to get_size_dir is
 * performed. Entries . and .. are conveniently ignored.
 */
int get_size_dir(char *dname, size_t *blocks)
{
	DIR *directorio = opendir(dname);

	if (directorio == NULL)
	{
		perror("opendir");
		exit(-1);
	}

	struct dirent *entrada;

	while (entrada = readdir(directorio), entrada != NULL)
	{
		if (strcmp(entrada->d_name, ".") && strcmp(entrada->d_name, ".."))
		{
			struct stat file_stat;
			if (lstat(entrada->d_name, &file_stat) == -1)
			{

				perror("lstat");
				exit(-1);
			}
			if (S_ISREG(file_stat.st_mode))
			{

				get_size(entrada->d_name, blocks);
			}
			else
			{
				get_size_dir(entrada->d_name, blocks);
			}
		}
	}
	closedir(directorio);
}

/* Processes all the files in the command line calling get_size on them to
 * obtain the number of 512 B blocks they occupy and prints the total size in
 * kilobytes on the standard output
 */
int main(int argc, char *argv[])
{

	if (argc < 2)
	{

		perror("error parametros");
		exit(-1);
	}

	size_t ret = 0;

	char dir_actual[1024];
	if (getcwd(dir_actual, sizeof(dir_actual)) == NULL)
	{
		perror("Error al obtener el directorio actual");
		return 1;
	}
	get_size_dir(dir_actual, &ret);

	printf("%zuK %s\n", ret, argv[1]);

	return 0;
}

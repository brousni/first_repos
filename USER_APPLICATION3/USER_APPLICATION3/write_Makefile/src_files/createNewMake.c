#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main ( void ) {
	
	system("cls");
	FILE *fp1;
	FILE *fp2;
	FILE *fp3;
	
	int sign_cnt = 0;
	int SUB_line;
	int SRCS_line;
	int Make_line;
	int line_num = 1;
	int i = 0;
	int j = 0;
	char read_stuff[1000];
	char project_name[100];
	
	/* HERE GOES SUBDIR READER */
	
	i = 0;

	if ((fp1 = fopen ("../../Debug/Makefile", "r")) == NULL)
	return 1;
	fp2 = fopen ("newList.txt", "w");

	while(fgets(read_stuff, 1000, (fp1)) != NULL)
	{
		fprintf(fp2, "%s", read_stuff);
	}
	
	fclose (fp1);
	fclose (fp2);

	fp1 = fopen ("newList.txt", "r");
	
	while(fgets(read_stuff, 1000, (fp1)) != NULL) {
		if((strstr(read_stuff, "SUBDIRS")) != NULL) {
			//printf("A match found on line: %d\n", line_num);
			//printf("\n%s\n", read_stuff);
			SUB_line = line_num;
			
		}
		line_num++;
	}
	
	line_num = 0;
	
	fclose(fp1);

	j = 0;

	fp1 = fopen ("newList.txt", "r");
	fp2 = fopen ("newList1.txt", "w");
	
	while(fgets(read_stuff, 1000, (fp1)) != NULL) {
		
		if( j > SUB_line -2) {
			fprintf(fp2, "%s", read_stuff);
		}
		j++;
	}
	j = 0;
	
	fclose(fp1);
	fclose(fp2);

	fp1 = fopen ("newlist1.txt", "r");

	while(fgets(read_stuff, 1000, (fp1)) != NULL && sign_cnt < 1) {
		if((strstr(read_stuff, "#")) != NULL) {
			//printf("A match found on line: %d\n", line_num);
			//printf("\n%s\n", read_stuff);
			SUB_line = line_num;
			sign_cnt++;
		}
		line_num++;
	}
	line_num = 0;
	sign_cnt = 0;
	
	fclose(fp1);
	
	fp1 = fopen ("newList1.txt", "r");
	fp2 = fopen ("SUBDIRS.txt", "w");
	
	while(fgets(read_stuff, 1000, (fp1)) != NULL) {
		
		if( j < SUB_line ) {
			fprintf(fp2, "%s", read_stuff);
		}
		j++;
	}
	j = 0;
	
	fclose(fp1);
	fclose(fp2);
	
	remove("newList1.txt");
	remove("newList.txt");

	
	/* HERE ENDS SUBDIR READER */
	
	/* HERE GOES SRCS READER */
	
	if ((fp1 = fopen ("../../Debug/Makefile", "r")) == NULL)
	return 1;
	fp2 = fopen ("newList.txt", "w");

	while(fgets(read_stuff, 1000, (fp1)) != NULL) {
		
		fprintf(fp2, "%s", read_stuff);
	}
	
	fclose (fp1);
	fclose (fp2);

	fp1 = fopen ("newList.txt", "r");
	
	while(fgets(read_stuff, 1000, (fp1)) != NULL) {
		if((strstr(read_stuff, "C_SRCS +=  \\")) != NULL) {
			//printf("A match found on line: %d\n", line_num);
			//printf("\n%s\n", read_stuff);
			SRCS_line = line_num;
		}
		line_num++;
	}
	line_num = 0;
	
	fclose(fp1);

	fp1 = fopen ("newList.txt", "r");
	fp2 = fopen ("newList1.txt", "w");
	
	while(fgets(read_stuff, 1000, (fp1)) != NULL)
	{
		if( j > SRCS_line-2 )
		{
			fprintf(fp2, "%s", read_stuff);
		}
		j++;
	}
	j = 0;
	
	fclose(fp1);
	fclose(fp2);

	fp1 = fopen ("newlist1.txt", "r");

	while(fgets(read_stuff, 1000, (fp1)) != NULL && sign_cnt < 1)
	{
		if((strstr(read_stuff, "PREPROCESSING_SRCS")) != NULL)
		{
			//printf("A match found on line: %d\n", line_num);
			//printf("\n%s\n", read_stuff);
			SRCS_line = line_num;
			sign_cnt++;
		}
		line_num++;
	}
	line_num = 0;
	sign_cnt = 0;
	fclose(fp1);
	
	fp1 = fopen ("newList1.txt", "r");
	fp2 = fopen ("SRCS.txt", "w");
	
	while(fgets(read_stuff, 1000, (fp1)) != NULL)
	{
		if( j < SRCS_line )
		{
			fprintf(fp2, "%s", read_stuff);
		}
		j++;
	}
	j = 0;
	
	fclose(fp1);
	fclose(fp2);
	
	remove("newList1.txt");
	remove("newList.txt");
	/* HERE ENDS SRCS READER */
	

	printf("Please enter name of project\n");
	scanf("%s", &project_name);

	fp1 = fopen ("SUBDIRS.txt", "r");
	fp2 = fopen ("../../ext_make/Makefile", "w");
	
	while(fgets(read_stuff, 1000, (fp1)) != NULL) {
		fprintf(fp2, "%s", read_stuff);
	}

	fclose(fp1);
	fclose(fp2);
	fp1 = fopen ("SRCS.txt", "r");
	fp2 = fopen ("../../ext_make/Makefile", "a+");
	
	fprintf(fp2, "%s", "\n\n");
	while(fgets(read_stuff, 1000, (fp1)) != NULL) {
		fprintf(fp2, "%s", read_stuff);
	}

	fclose(fp1);
	fclose(fp2);
		
	fp1 = fopen ("Makefile", "r");
	fp2 = fopen ("../../ext_make/Makefile", "a+");
	
	fprintf(fp2, "%s", "#project name\n#Note: to work with Atmel studio target file must equal Atmel studio project name\n");
	fprintf(fp2, "%s", "PRO=");
	fprintf(fp2, "%s", project_name);
	fprintf(fp2, "%s", ".elf\n");
	
	fclose(fp1);
	fclose(fp2);
	fp3 = fopen ("Makefile", "r");
	
	while(fgets(read_stuff, 1000, (fp3)) != NULL)
	{
		if((strstr(read_stuff, "#project")) != NULL)
		{
			//printf("A match found on line: %d\n", line_num);
			//printf("\n%s\n", read_stuff);
			Make_line = line_num;
		}
		line_num++;
	}
	line_num = 0;
	
	fclose(fp3);

	j = 0;

	fp1 = fopen ("Makefile", "r");
	fp2 = fopen ("../../ext_make/Makefile", "a+");
	
	while(fgets(read_stuff, 1000, (fp1)) != NULL) {
		
		if( j > Make_line+2 ) {
			fprintf(fp2, "%s", read_stuff);
		}
		j++;
	}

	remove("SUBDIRS.txt");
	remove("SRCS.txt");
	return 0;
}

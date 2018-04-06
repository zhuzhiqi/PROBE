#include "global.h"


unsigned int count_lines_tlv(char *infile){

	FILE* fp_tlv;
	char line[MAX_STR];
	fp_tlv=fopen(infile,"rt");
	if(fp_tlv==NULL){
		printf("cannot open %s\n",infile);
		exit(-1);
	}
	unsigned int count=0;
	while(fgets(line,MAX_STR,fp_tlv)!=NULL){
		count++;
	}
	fclose(fp_tlv);
	return count;

}
double error_real(void){

	unsigned int golden_lines,out0_lines;
	char cmd[MAX_STR];

	out0_lines=count_lines_tlv(outfile);	
	golden_lines=count_lines_tlv(goldenfile);	
	printf("[out0]:%u,[golden]:%u\n",golden_lines,out0_lines);

	if(out0_lines!=golden_lines){
		sprintf(cmd,"Fatal: golden and out0 lines unmatched\n");
		print_char_red(cmd);
		return 99999;
	}

	unsigned int out0,golden,num=0;
    double ED=0,AED=0;
    FILE *fp_o,*fp_golden;
    if((fp_o= fopen(outfile, "r")) == NULL){
	printf(" \"%s \" could not be opened.\n",outfile);
	exit(1);
    }
    if((fp_golden= fopen(goldenfile, "r")) == NULL){
	printf(" \"%s \" could not be opened.\n",goldenfile);
	exit(1);
    }

    ED=0; num=0;
    for (;;){
	if ( ( fscanf(fp_golden, "%u", &golden) == EOF)  ) break ;
		fscanf(fp_o, "%u", &out0);
		if(golden!=0) {
		ED += (abs(golden-out0))/(double)(golden);
		num++;  }
    }

    AED=ED/(double)num;
    fclose(fp_o);
    fclose(fp_golden);

	return AED;

}

double error(void){

	unsigned int golden_lines,out0_lines;
	char cmd[MAX_STR];

	out0_lines=count_lines_tlv(outfile);	
	golden_lines=count_lines_tlv(goldenfile);	
	printf("[out0]:%u,[golden]:%u\n",golden_lines,out0_lines);

	if(out0_lines!=golden_lines){
		sprintf(cmd,"Fatal: golden and out0 lines unmatched\n");
		print_char_red(cmd);
		return 99999;
	}

	unsigned int out0,golden,num=0;
    double ED=0,AED=0;
    FILE *fp_o,*fp_golden;
    if((fp_o= fopen(outfile, "r")) == NULL){
	printf(" \"%s \" could not be opened.\n",outfile);
	exit(1);
    }
    if((fp_golden= fopen(goldenfile, "r")) == NULL){
	printf(" \"%s \" could not be opened.\n",goldenfile);
	exit(1);
    }

    ED=0; num=0;
    for (;;){
	if ( ( fscanf(fp_golden, "%u", &golden) == EOF)  ) break ;
		fscanf(fp_o, "%u", &out0);
		if(golden!=0) {
		ED += (abs(golden-out0))/(double)(1<<BITWIDTH);
		num++;  }
    }

    AED=ED/(double)num;
    fclose(fp_o);
    fclose(fp_golden);

	return AED;

}


void print_char_blue(char * strprint){
	printf("\n\033[01;34m	%s \033[0m\n",strprint);
}
void print_int_blue(int strprint){
	printf("\n\033[01;34m %d \033[0m\n",strprint);
}
void print_double_blue(double strprint){
	printf("\n\033[01;34m %lf \033[0m\n",strprint);
}

void print_char_red(char * strprint){
	printf("\n\033[01;31m %s \033[0m\n",strprint);
}
void print_int_red(int strprint){
	printf("\n\033[01;31m %d \033[0m\n",strprint);
}
void print_double_red(double strprint){
	printf("\n\033[01;31m %lf \033[0m\n",strprint);
}

// look for the maximum integer in an array
int max_int_array(int *ary, int len) {
  int i, result;
  if (len<0) {
    return 0;
  }
  result = ary[0];
  for (i=1; i<len; i++) {
    result = MAX(ary[i], result);
  }
  return result;
}

// look for the minimum integer in an array
int min_int_array(int *ary, int len) {
  int i, result;
  if (len<0) {
    return 0;
  }
  result = ary[0];
  for (i=1; i<len; i++) {
    result = MIN(ary[i], result);
  }
  return result;
}

// look for the maximum double value in an array
double max_double_array(double *ary, int len) {
  int i;
  double result;
  if (len<0) {
    return 0;
  }
  result = ary[0];
  for (i=1; i<len; i++) {
    result = MAX(ary[i], result);
  }
  return result;
}

// look for the minimum double value in an array
double min_double_array(double *ary, int len) {
  int i;
  double result;
  if (len<0) {
    return 0;
  }
  result = ary[0];
  for (i=1; i<len; i++) {
    result = MIN(ary[i], result);
  }
  return result;
}


// Quick Qorting algorithm for int-type
// dir : sorting way : DECR-decreasing, INCR-increasing
void int_quick_sort(int *d, int start, int end, char dir) {
  int key;
  int i, j, middle;
  if (start >= end)
    return;
  key = d[end];
  i = start;
  for (j=start; j<end; j++) {
    if (dir==DECR) {
      if (d[j] >= key) {
	int_swap(d+i, d+j);
	i++;
      }
    } else {
      if (d[j] <= key) {
	int_swap(d+i, d+j);
	i++;
      }
    }
  }
  int_swap(d+end, d+i);
  
  middle = i;
  int_quick_sort(d, start,    middle-1, dir);
  int_quick_sort(d, middle+1, end, dir);
}


void int_quick_sort_index(int *d, int *ind, int start, int end, char dir) {
  int key;
  int i, j, middle;
  if (start >= end)
    return;
  key = d[end];
  i = start;
  for (j=start; j<end; j++) {
    if (dir==DECR) {
      if (d[j] >= key) {
	int_swap(ind+i, ind+j);
	int_swap(d+i, d+j);
	i++;
      }
    } else {
      if (d[j] <= key) {
	int_swap(ind+i, ind+j);
	int_swap(d+i, d+j);
	i++;
      }
    }
  }
  int_swap(ind+end, ind+i);
  int_swap(d+end, d+i);
  
  middle = i;
  int_quick_sort_index(d, ind, start,    middle-1, dir);
  int_quick_sort_index(d, ind, middle+1, end, dir);
}



// Quick Sorting algorithm for double-type
// dir : sorting way : DECR-decreasing, INCR-increasing
void double_quick_sort(double *d, int start, int end, char dir) {
  double key;
  int i, j, middle;
  if (start >= end)
    return;
  key = d[end];
  i = start;
  for (j=start; j<end; j++) {
    if (dir==DECR) {
      if (!LESS(d[j],  key)) {
	double_swap(d+i, d+j);
	i++;
      }
    } else {
      if (!GREATER(d[j],  key)) {
	double_swap(d+i, d+j);
	i++;
      }
    }
  }
  double_swap(d+end, d+i);
  
  middle = i;
  double_quick_sort(d, start,    middle-1, dir);
  double_quick_sort(d, middle+1, end, dir);
}

void double_quick_sort_index(double *d, int *ind, int start, int end, char dir) {
  double key;
  int i, j, middle;
  if (start >= end)
    return;
  key = d[end];
  i = start;
  for (j=start; j<end; j++) {
    if (dir==DECR) {
      if (!LESS(d[j],  key)) {
	int_swap(ind+i, ind+j);
	double_swap(d+i, d+j);
	i++;
      }
    } else {
      if (!GREATER(d[j],  key)) {
	int_swap(ind+i, ind+j);
	double_swap(d+i, d+j);
	i++;
      }
    }
  }
  int_swap(ind+end, ind+i);
  double_swap(d+end, d+i);
  
  middle = i;
  double_quick_sort_index(d, ind, start,    middle-1, dir);
  double_quick_sort_index(d, ind, middle+1, end, dir);
}

// Swaping for int-type
void int_swap(int *a, int *b) {
  int tmp;
  tmp = *a;
  *a = *b;
  *b = tmp;
}

// Swaping for double-type
void double_swap(double *a, double *b){
  double tmp; 
  tmp = *a;
  *a = *b;
  *b = tmp;
}

//GCD : the greatest common divisor
int greatestCommonDivisor(int v1, int v2) {
  int a, b, r;
  if (!(v1 && v2)) {
    printf("Error: zero value is used in LCD calculation\n");
    exit(1);
  }
  b = v1;
  r = v2;
  do {
    a = MAX(b, r);
    b = MIN(b, r);
    r = a % b;
  } while (r);
  return b;
}

// LCM : the least common multiplier
int leastCommonMultiplier(int v1, int v2) {
  if (!(v1 && v2)) {
    printf("Error: zero value is used in LCM calculation\n");
    exit(1);
  }
  return (v1*v2/greatestCommonDivisor(v1, v2));
}

// Reverse array with int-type
void reverse_array(int *ary, int length){
  int ary_tmp[length];
  int i;
  for (i=0; i<length; i++)
    ary_tmp[i] = ary[i];
  for (i=0; i<length; i++)
    ary[i] = ary_tmp[length-1-i];
}

// Permutation searching
// elements in ary should be different from each other
char next_permutation(int *ary, int length) {
  int i, j;
  for (i=length-1; i>0; i--) {
    if (ary[i-1] < ary[i]) {
      j = length;
      while(!(ary[i-1] < ary[--j]));
      int_swap(&ary[i-1], &ary[j]);
      reverse_array(ary+i, length-i);
      return 1;
    }
  }
  return 0;
}
char *remove_space(char* input_string){

	unsigned int kk=0,jj;
	char* output_string;
	output_string=malloc(sizeof(char)*strlen(input_string));
	strcpy(output_string,input_string);	
	for(jj=0;jj<strlen(output_string);jj++){
		if((output_string[jj]!=' ')&&(output_string[jj]!='\t'))
				output_string[kk++]=output_string[jj];
	}
	output_string[kk]='\0';
	//for(jj=0;jj<strlen(output_string);jj++)
	//	printf("\'%c\'",output_string[jj]);
	//	printf("\n");
	return output_string;

}

void get_local_time(){
    char *wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    char *month[] = {"Jan", "Feb", "Mar", "Apr", "May", "June", "July","Aug","Sep","Oct","Nov","Dec"};
    time_t timep;
    struct tm *p;
    time(&timep);
    p = localtime(&timep); 
    printf ("%s/%d/%d ",  month[p->tm_mon], p->tm_mday,(1900+p->tm_year));
    printf("%s %d:%d:%d\n", wday[p->tm_wday], p->tm_hour, p->tm_min, p->tm_sec);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

#include <ctype.h>


#define PRINT_REG_SIGNAL 1

#define ZERO     1e-10
#define INFINITE 1e20
#define MAX_STR  9999
#define INCR     0
#define DECR     1

//tlv file numbers
#define NUMS 10000
#define MAX_STDEV 0.5 // maximum default standard deviation allowed
#define MAX_V2V 0.05
#define ERROR_THRESHOLD 0.5 //20% error tolerance allowed
#define CLOCK 20  // 20ns max period == 50MHz 
#define BITWIDTH 8 //for error calculation
#define MIN_FUs 0
#define MAX_FUs 1

#define DFG_FILE "DFG.dat"
#define SIYUAN_LOG "siyuan.rpt"
#define NEW_LOG "siyuan_v1.rpt"
#define BLIB_PATH  "/home/siyuan/legolas/libs/Nangate.BLIB"
#define FLIB_PATH  "/home/siyuan/legolas/libs/Nangate.FLIB"
#define LIBS_PATH  "/home/siyuan/legolas/libs"

extern char process[MAX_STR];
extern char infile[MAX_STR]; //e.g: uut.cpp
extern char outfile[MAX_STR]; //e.g: out0.tlv
extern char goldenfile[MAX_STR]; //e.g: out0.tlv


#define GEN 1
#define NOGEN 0
#define Gen_VALID 1
#define NOGen_VALID 1
#define RUN 0
#define NORUN 0


#define MAX(x, y)     ((x)>(y)?(x):(y))
#define MIN(x, y)     ((x)<(y)?(x):(y))
#define ABS(x)        ((x)<0?(-(x)):(x))
#define EQU(x, y)     (ABS((x)-(y))<ZERO)
#define GREATER(x, y) ((x)-(y)>ZERO)
#define LESS(x, y)    ((y)-(x)>ZERO)
#define NRAND(x)      (rand()%(x))
#define APX_EQU(x,y)  (ABS(x-y)/x)<=MAX_V2V

#define COMBRPT "comb.log"
#define PERMRPT "perm.log"

#define GET_TIME_INIT(num) struct timeval _timers[num]
#define GET_TIME_VAL(num) gettimeofday(&_timers[num], NULL)
#define TIME_VAL_TO_MS(num) (((double)_timers[num].tv_sec*1000.0)+((double)_timers[num].tv_usec/1000.0))

//DFG Creator
typedef struct dfg
{
	char * name;
	struct dfg* next;
	struct dfg* prev;
}SDFG;

typedef struct signals
{
	char * name;
	unsigned int size;
	char adjacent[200][MAX_STR];
	unsigned int adjacent_size[200];
	unsigned int adj_nums;
	struct signals* next;
	struct signals* prev;
	bool probe;
	
	
}SSIGNALS;
SDFG *gen_dfg_node();
void insert_SDFG_node(SDFG *first_output, SDFG *sig_new);
SSIGNALS *gen_signal_list_node();
void insert_signal_list_node(SSIGNALS *first_output, SSIGNALS *sig_new);
SSIGNALS *read_dfg_name(char *infile);
SSIGNALS *read_dfg_flow(SSIGNALS* signal_lists,char* infile);
void Trace_back(SSIGNALS* curr_lists,SSIGNALS* global_lists);
void Trace_back_regular(SSIGNALS* curr_lists,SSIGNALS* global_lists);
void create_file(char * infile);
SSIGNALS * DFG_creator(char * infile);

/* define for function.c */
void print_char_blue(char * strprint);
void print_int_blue(int strprint);
void print_double_blue(double strprint);
void print_char_red(char * strprint);
void print_int_red(int strprint);
void print_double_red(double strprint);
unsigned int count_lines_tlv(char *infile);
int  max_int_array(int *ary, int len);
int  min_int_array(int *ary, int len);
double max_double_array(double *ary, int len);
double min_double_array(double *ary, int len);
void int_quick_sort(int *d, int start, int end, char dir);
void double_quick_sort(double *d, int start, int end, char dir);
void int_quick_sort_index(int *d, int *index, int start, int end, char dir);
void double_quick_sort_index(double *d, int *ind, int start, int end, char dir);
void int_swap(int *a, int *b);
void double_swap(double *a, double *b);
int  greatestCommonDivisor(int v1, int v2);
int  leastCommonMultiplier(int v1, int v2);
void reverse_array(int *ary, int length);
char next_permutation(int *ary, int length);
char next_combination(int *ary, int length);
char next_packing(int *ary, int *ubound, int length);
void get_local_time();

/* define for CWB */
char* specialConvert( const char* str,const char* splitChar);
unsigned int high_level_synthesis(char *infile,char* process,bool FUs,bool valid_signal);
void gen_run_cycle_sim(bool gen,bool run,unsigned int cycles,char* process);
void gen_run_rtl_sim(bool gen,bool run,unsigned int cycles,char* process,char *hdl);
unsigned int gen_run_logic_synthesis(bool gen,bool run,char* tools,char* process);
void gen_run_c_sim(bool gen,bool run,unsigned int cycles,char* process);
char* adjust_file(char *fullname);
void rewrite_verilog(char *vfile);
void add_register(char *vfile);
void change_verilog(void);
unsigned int latency_from_high_level_synthesis(char* process);
unsigned int latency_from_cycle_accurate_sim(char* process);

/* function operator */
void inparse(char *infile);
unsigned int op_bdltran(char *infile,char* process,char*subproc,char* lmspec);

/* function */
void reverse_array(int *ary, int length);
char next_permutation(int *ary, int length) ;
void rm_combination(void);
void rm_permutation(void);
void run_comb(int M);
void combination(int N, int M, int m);
void run_perm(int M);
void permutation(int N, int M, int m);
double error(void);
double error_real(void);
unsigned int read_latency(char* process);
char *remove_space(char* input_string);
double read_logic_synthesis_power(char* tools,char* process);
double prime_time_power(char * process);

/* distribution.c */
typedef struct rtl_input
{
	char *name;
	unsigned int size;
	struct rtl_input *prev;
	struct rtl_input *next;

}struct_rtl_input;
unsigned int gen_rand(unsigned int);
unsigned int gen_norm(unsigned int);
unsigned int gen_pskew(unsigned int);
unsigned int gen_nskew(unsigned int);
unsigned int* gen_distribution(unsigned int numbers,unsigned int max,char* distri);
void parse_rtl_get_input_tlv(char *infile,char *distri, char* process);

/*variable to constant/variable  */
typedef struct cvar_struct{
  char *m_name;
  char *m_code;
  int m_num; 
  int m_bw;
  float m_mean;
  float m_stdev;
  int m_max;
  bool m_mark;
  struct cvar_struct *m_prevp;
  struct cvar_struct *m_nextp;
  
  int t_time;
  int p_time;
  
  bool probe;
  
  bool withsign;
  
  bool rec_start;
  
}CVar;
void read_vcd(char *vcd_file,char * process,SSIGNALS *);
void vcd_signals_stdev(char *vcd_file, CVar *v2c_var);
float vcd_single_stdev(char *vcd_file, CVar *var);
void vcd_signals_mean(char *vcd_file, CVar *v2c_var);
float vcd_single_mean(char *vcd_file, CVar *var,int s_time);
signed extract_value_vcd(char *line,CVar *v2c_var);
void read_vcd_signals(char *vcd_file);
CVar *gen_v2c_node();
void insert_v2c_node(CVar *v2c_var, CVar *var_new);
void v2c_approx(CVar *v2c_var, float stdev,char * process,SSIGNALS *);
void v2v_approx(CVar *v2c_var, float stdev,char * process,SSIGNALS *);
void results_all(CVar *v2c_var,char * process);
void get_accuracy(unsigned int i,char *process,char *new_process);
char* insert_into_string(char s[], char t[], int i);
unsigned int hls_apx(char* process);


void probe(char *probe_file, CVar *v2c_var);


int start_time(char *vcd_file, CVar *var);


//optimal design
typedef struct clock_gating_varible
{
	struct clock_gating_varible* next;
	struct clock_gating_varible* prev;
	char * name;
	int mean;
	unsigned int size;
	char * file_name;
	double power;
	double area;
	double error;
	bool optimal;
	unsigned int adj_nums;
	char adjacent[200][MAX_STR];
	unsigned int adjacent_size[200];
	unsigned int apx_type; //1: v2c 2:v2v
	char *v2v_name; //valid iff v2v flag is 1

}SCGVar;
SCGVar *gen_SCGVar_node();
void insert_SCGVar_node(SCGVar *first_output, SCGVar *sig_new);
void rewrite_report_file();
void combine_files(SCGVar *new_sig,unsigned int apx_num);
void sim_combined_file(unsigned int apx_in_nums);


// read the power waveform
void read_waveform_file(char *process);
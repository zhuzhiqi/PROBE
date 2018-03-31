#include "global.h"




char process[MAX_STR]="uut";
char infile[MAX_STR]="uut.cpp"; //e.g: uut.cpp
char outfile[MAX_STR]="out0.tlv"; //e.g: out0.tlv
char goldenfile[MAX_STR]="golden.tlv"; //e.g: golden.tlv

char distribution[MAX_STR] ="distribution";


void print_version()
{
	printf("%-15s\n","\n\n-------------------------");
	printf("%-15s\n","Apprximation Process");
	printf("%-15s\n","Writen By Siyuan Xu");
	printf("%-s %-s\n", __DATE__, __TIME__);
	printf("%-15s\n","-------------------------\n\n");

}


void read_input_arguments(int argc, char *argv[]){

    int x=0;


	if(argc<2){
		printf("please input the right argument\n");
		exit(0);
	}
    /* Read all input arguments */
    for(x=1; x< argc; x++){

      /* Read in file */
      if(!strcmp(argv[x], "-process")){
		x++;
		sprintf(process,"%s",argv[x]);
      }
     else if(strstr(argv[x], ".cpp")||strstr(argv[x], ".c")){
		sprintf(infile,"%s",argv[x]);
      }
      else if(!strcmp(argv[x], "-outfile")){
		x++;
		sprintf(outfile,"%s",argv[x]);
      }
      else if(!strcmp(argv[x], "-goldenfile")){
		x++;
		sprintf(goldenfile,"%s",argv[x]);
      }
	  
	  else if(!strcmp(argv[x], "-distribution")){
		x++;
		sprintf(distribution,"%s",argv[x]);
      }
	  
	  
	  else
		{
		  printf("input: %s\n",argv[x]);
			printf("please input the right argument\n");
			exit(0);
		}
    }
 }




int main(int argc, char *argv[])
{
	//print_version();

	srand(time(NULL));
	GET_TIME_INIT(10);
	GET_TIME_VAL(0);

    read_input_arguments(argc, argv);

	char rtl_file[MAX_STR];
	sprintf(rtl_file,"%s_E.v",process);
	
	
	
	unsigned int area_hls=0;
	unsigned int area_ls=0;
	double power_dc=0;

	double power_pt;
	FILE *fp_siyuan;
	char distri[]="rand";
	char cmd[MAX_STR];

 	//High level synthesis and obtain the parameter and golden output
	//get the input port of the org rtl and generate the tlv file
	
	
	
	
	#ifdef MINFUS
		area_hls=high_level_synthesis(infile,process,MIN_FUs,NOGen_VALID);
	#else
		area_hls=high_level_synthesis(infile,process,MAX_FUs,NOGen_VALID);
	#endif

	/*
	int latency_QoR;
	latency_QoR = latency_from_high_level_synthesis(process);
	printf("QoR latency = %d\n", latency_QoR);
	*/

	
	//Create Data flow graph -- DFG --  for the Verilog
	SSIGNALS * signal_list;	
	signal_list=DFG_creator(rtl_file);
	
		



	

	
	
	//RTL sim
	

	//parse_rtl_get_input_tlv(rtl_file,distri);
	parse_rtl_get_input_tlv(rtl_file,distribution,process);
	gen_run_rtl_sim(GEN,RUN,0,process,"");
	
	//gen_run_cycle_sim(GEN,RUN,0,process);


	
	
	//add the register
	sprintf(cmd,"%s_E.v",process);
	add_register(cmd);

	
	// rename %s_E.v and tem_%s_E.v
	sprintf(cmd,"mv %s_E.v ori_%s_E.v",process,process);
	system(cmd);
	
	sprintf(cmd,"mv temp_%s_E.v %s_E.v",process,process);
	system(cmd);

	

	system("chmod +x ./vr_vsim.bat "); 
	system("./vr_vsim.bat >>LOG_Sim");
	
	//***Cycle accurate *****
	//sprintf(cmd,"chmod +x ./%s.exe ",process);
	//system(cmd); 
	//sprintf(cmd, "./%s.exe >>LOG_Sim",process);
	//system(cmd);
	
	
	
	//read VCD file, analyze the V2C and VCD node
	sprintf(cmd,"%s_E.vcd",process);
	read_vcd(cmd,process,signal_list);
	
	

	
	
/*
	// Move the design which doesn't add the register for accurate power estimation
	system("rm -rf ./Org_Design");
	system("mkdir ./Org_Design");
	sprintf(cmd,"cp -rf %s_E.v ./Org_Design",process);system(cmd);


	//HLS again for valid signalgen in order to get the cycle-accurate transaction mode sim 
	#ifdef MINFUS
		area_hls=high_level_synthesis(infile,process,MIN_FUs,Gen_VALID);
	#else
		area_hls=high_level_synthesis(infile,process,MAX_FUs,Gen_VALID);
	#endif

	//Create Data flow graph -- DFG --  for the Verilog
	SSIGNALS * signal_list;
	signal_list=DFG_creator(rtl_file);

	//logic synthesis, get the power throught PrimeTime tool 
	area_ls=gen_run_logic_synthesis(GEN,RUN,"dc",process);
    parse_rtl_get_input_tlv(rtl_file,distri);
	gen_run_rtl_sim(GEN,RUN,0,process,"");
	power_dc=read_logic_synthesis_power("dc",process);
	power_pt=prime_time_power(process);
	sprintf(cmd,"mv %s_E.vcd Org_%s_E.vcd",process,process);system(cmd);
	

	//add the register inorder for approximation (V2V/V2C)
	sprintf(cmd,"%s_E.v",process);
	add_register(cmd);
	gen_run_rtl_sim(GEN,RUN,0,process,"");

	//obtain the golden output for the comparison 
	sprintf(cmd,"mv %s %s",outfile,goldenfile);
	system(cmd);

	//Orginal design information... Recording..
	printf("%s_ORG::Area[HLS] is %d, Area[LS]:is %d, Power[DC]:is %0.4lf,Power[PT]:is %0.4lfuw\n",process,area_hls,area_ls,power_dc,power_pt*1e6);
	fprintf(fp_siyuan,"%s_ORG::Area[HLS] is %d, Area[LS]:is %d, Power[DC]:is %0.4lf,Power[PT]:is %0.4lfuw\n",process,area_hls,area_ls,power_dc,power_pt*1e6);
	sprintf(cmd,"cp -rf ./Org_Design/%s_E.v ./",process);system(cmd);
	fclose(fp_siyuan);


	//once we obtain the vcd file , move the orginal file into the main folder and start the approximation
	sprintf(cmd,"%s_E.vcd",process);
	read_vcd(cmd,process,signal_list);

	//obtain the combined file -- Input Apx_in [bitw:0]
	//rewrite_report_file();


	GET_TIME_VAL(1);
	fp_siyuan = fopen(SIYUAN_LOG, "at");
	printf("Total Running Time : %lf seconds\n",(TIME_VAL_TO_MS(1)-TIME_VAL_TO_MS(0))/1000.0);
	fprintf(fp_siyuan,"Total Running Time : %lf seconds\n",(TIME_VAL_TO_MS(1)-TIME_VAL_TO_MS(0))/1000.0);
	fclose(fp_siyuan);
*/
	return 0;

}


#include "global.h"

char* specialConvert(const char* str,const char* splitChar)
{
    const char *p = str;
    char *buffer=malloc(sizeof(char)*MAX_STR);
    int i = 0;
    while(*p)
    {
        if( *splitChar != *p )
        {
            if( i<MAX_STR )
            {
                buffer[i++] = *p;
            }
            else
            {
                break;
            }
        }
        p++;
    }
    return buffer;
}

unsigned int read_latency(char* process){


	char cmd[MAX_STR],line[MAX_STR],syrfile[MAX_STR],*result;
	char splitChar=',';
	FILE *fp_syr;

	unsigned int latency;
			sprintf(syrfile,"%s.QOR",process);
			if( (fp_syr=fopen(syrfile,"rt"))==NULL){ 
				sprintf(cmd, "Cannot open %s\n",syrfile);
				//print_char_red(cmd);
				exit(-1);
			}

			while(fgets(line, MAX_STR, fp_syr) != NULL){

				if(strstr(line,"Latency Index"))
					{ 
					result = strtok(line, ":");
					result = strtok( NULL, "\t" );
					latency = atoi(specialConvert(result,&splitChar));		
					break;
					}
			}
			fclose(fp_syr);
			return latency;

}

unsigned int high_level_synthesis(char *infile,char* process,bool FUs,bool valid_signal){

	char cmd[MAX_STR],line[MAX_STR],syrfile[MAX_STR],*result;
    unsigned int area=0;
	char splitChar=',',string_v[MAX_STR];
	FILE *fp_syr;
	

	

	// parse the input file
	if(strstr(infile,".cpp")){
      sprintf(cmd,"scpars %s -lic_wait=120",infile); 
      system(cmd);
   }
    else{
      sprintf(cmd,"bdlpars %s -lic_wait=120",infile); 
	  system(cmd);
	}

	
	
	
	

	//generate the valid signal or not..
	if(valid_signal==Gen_VALID)
		sprintf(string_v,"-TT414 -Zport_valid_sig_gen ");
	else
		sprintf(string_v," ");
	
	

	
	
  // HLS 
  if(strstr(infile,".sc") || strstr(infile,".cpp"))
    sprintf(cmd,"bdltran -lic_wait=120 -oI -OS -OR  -Zresource_fcnt=GENERATE -Zresource_mcnt=GENERATE  %s -c%d -s -lfl /eda/cwb/cyber_561/LINUX/packages/asic_45.FLIB  -lb /eda/cwb/cyber_561/LINUX/packages/asic_45.BLIB  %s.IFF -oI  >>LOG_HLS",string_v, CLOCK*100,  process);
  else
    sprintf(cmd,"bdltran -lic_wait=120 -oI -OS -OR  -Zresource_fcnt=GENERATE -Zresource_mcnt=GENERATE  %s -c%d -s -lfl /eda/cwb/cyber_561/LINUX/packages/asic_45.FLIB -lb /eda/cwb/cyber_561/LINUX/packages/asic_45.BLIB  %s.IFF -oI  >>LOG_HLS",string_v, CLOCK*100, process);
  system(cmd);

  // verilog gen
    sprintf(cmd," veriloggen -lic_wait=120 -D0  %s_E.IFF >>LOG_HLS",  process);
	system(cmd);
	
	
	sprintf(syrfile,"%s.QOR",process);
	if( (fp_syr=fopen(syrfile,"rt"))==NULL){ 
		sprintf(cmd, "Cannot open %s\n",syrfile);
		//print_char_red(cmd);
		exit(-1);
	}

	while(fgets(line, MAX_STR, fp_syr) != NULL){

		if(strstr(line,"ALUT"))
			{ 
			result = strtok(line, ":");
			result = strtok( NULL, "\t" );
			area = atoi(specialConvert(result,&splitChar));		
			break;
			}
		if(strstr(line,"Total Area"))
			{ 
			result = strtok(line, ":");
			result = strtok( NULL, "\t" );
			area = atoi(specialConvert(result,&splitChar));		
			break;
			}
			

	}
	
	//printf("area = %d\n",area);
	fclose(fp_syr);
	
	

/*
	if(FUs==MIN_FUs){
	  // Create the constrain file 
	  if(strstr(infile,".sc") || strstr(infile,".cpp"))
		sprintf(cmd,"bdltran -lic_wait=120 -oI -OS -OR -Zmix_sign_share=REG -Zflib_out_multi=NO -Zflib_out_sign=EACH -Zflib_out_limit=L1:M1:S1  -c%d -s -lfl %s  -lb %s  -Zflib_fcnt_out %s.IFF >>LOG_HLS", CLOCK*100, FLIB_PATH, BLIB_PATH, process);
	  else
		sprintf(cmd,"bdltran -lic_wait=120 -oI -OS -OR -Zmix_sign_share=REG -Zflib_out_multi=NO -Zflib_out_sign=EACH -Zflib_out_limit=L1:M1:S1 -c%d -s -lfl %s -lb %s -Zflib_fcnt_out %s.IFF >>LOG_HLS", CLOCK*100, FLIB_PATH,  BLIB_PATH, process);
	  system(cmd);
	}
	else{
	  // Create the constrain file 
	  if(strstr(infile,".sc") || strstr(infile,".cpp"))
		sprintf(cmd,"bdltran -lic_wait=120 -oI -OS -OR -Zmix_sign_share=REG -Zflib_out_multi=NO -Zflib_out_sign=EACH -Zflib_out_limit=L1000:M1000:S1000 %s -c%d -s -lfl %s  -lb %s  -Zflib_fcnt_out %s.IFF >>LOG_HLS",string_v, CLOCK*100, FLIB_PATH, BLIB_PATH, process);
	  else
		sprintf(cmd,"bdltran -lic_wait=120 -oI -OS -OR -Zmix_sign_share=REG -Zflib_out_multi=NO -Zflib_out_sign=EACH -Zflib_out_limit=L1000:M1000:S1000 -c%d -s -lfl %s -lb %s -Zflib_fcnt_out %s.IFF >>LOG_HLS", CLOCK*100, FLIB_PATH,  BLIB_PATH, process);
	  system(cmd);

	}


  // HLS 
  if(strstr(infile,".sc") || strstr(infile,".cpp"))
    sprintf(cmd,"bdltran -lic_wait=120 -oI -OS -OR -Zresource_fcnt=USE -Zresource_mcnt=USE  %s -c%d -s -lfl %s  -lb %s  %s.IFF -oI -lfc %s-auto.FCNT +lfl %s-auto.FLIB >>LOG_HLS",string_v, CLOCK*100, FLIB_PATH, BLIB_PATH, process,process,process);
  else
    sprintf(cmd,"bdltran -lic_wait=120 -oI -OS -OR -Zresource_fcnt=USE -Zresource_mcnt=USE  %s -c%d -s -lfl %s -lb %s  %s.IFF -oI -lfc %s-auto.FCNT +lfl %s-auto.FLIB >>LOG_HLS",string_v, CLOCK*100, FLIB_PATH,  BLIB_PATH, process,process,process);
  system(cmd);

  // verilog gen
    sprintf(cmd," veriloggen -lic_wait=120 -D0  %s_E.IFF >>LOG_HLS",  process);
	system(cmd);

	system("rm -rf *_IP.v");


	sprintf(syrfile,"%s.QOR",process);
	if( (fp_syr=fopen(syrfile,"rt"))==NULL){ 
		sprintf(cmd, "Cannot open %s\n",syrfile);
		print_char_red(cmd);
		exit(-1);
	}

	while(fgets(line, MAX_STR, fp_syr) != NULL){

		if(strstr(line,"ALUT"))
			{ 
			result = strtok(line, ":");
			result = strtok( NULL, "\t" );
			area = atoi(specialConvert(result,&splitChar));		
			break;
			}
		if(strstr(line,"Total Area"))
			{ 
			result = strtok(line, ":");
			result = strtok( NULL, "\t" );
			area = atoi(specialConvert(result,&splitChar));		
			break;
			}
			

	}
	fclose(fp_syr);
	*/
	return area;
}

//gen_run_cycle_sim
void gen_run_cycle_sim(bool gen,bool run,unsigned int cycles,char* process){

  char cmd[MAX_STR];

  if(gen==GEN){
	  sprintf(cmd, "cmscgen %s_C.IFF           \
				  -input=file:transaction 	   \
				  -file_out=transaction -b10   \
				  -org_type=pin -enum_int=NO   \
				  -out_dir=. -dump=VCD 		   \
				  -dump_signal=CYCLEMODEL 	   \
				  -line_num=NO -lic_wait=120", process); 
	  system(cmd);

	  if(cycles==0)
			sprintf(cmd, "mkmfsim %s_C.MakeInfo -GNU -I\".\" -target %s.exe ", process,process); 
	  else {
			sprintf(cmd, "mkmfsim %s_C.MakeInfo -GNU -I\".\"  \
						 -target %s.exe -cycles=%d ", process,process,cycles); }
	  system(cmd);
  }
   if(run==RUN){
	  // Compile model
	   system("make -f Makefile.GNU clean");
	   system("make -f Makefile.GNU ");
	  // Execute model
	   sprintf(cmd,"chmod +x ./%s.exe ",process);
	   system(cmd); 
	   sprintf(cmd, "./%s.exe >>LOG_Sim",process);
	   system(cmd);
   }
}


unsigned int latency_from_cycle_accurate_sim(char* process){

 char cmd[MAX_STR];

  sprintf(cmd, "tbgen %s_E.IFF -max_cycle=1000 -input=random -b10 -finish \
					-scr=modelsim-all  -verbose_level=RESULT 		\
					-lic_wait=120  >>LOG_Sim", process);
  system(cmd); 

  system("chmod +x ./vr_vsim.bat "); 
  system("./vr_vsim.bat >>LOG_Sim"); 


  FILE *fp_out0_v;
  unsigned int out0_v;
  bool start=false;
  unsigned int latency;
  fp_out0_v=fopen("out0_v.clv","rt");
  while(fscanf(fp_out0_v,"%u",&out0_v)!= EOF){

	if((start==true)&&(out0_v==1))
		break;

	if(out0_v==1)
		start=true;

	if(start==true)
		latency++;

  }
  fclose(fp_out0_v);
  return latency;
}

unsigned int latency_from_high_level_synthesis(char* process){
	char splitChar=',';
	FILE *fp_QOR;
	char syrfile[MAX_STR],cmd[MAX_STR],line[MAX_STR],*result;
	unsigned int latency;
	sprintf(syrfile,"%s.QOR",process);
	if( (fp_QOR=fopen(syrfile,"rt"))==NULL){ 
		sprintf(cmd, "Cannot open %s\n",syrfile);
		//print_char_red(cmd);
		exit(-1);
	}

	while(fgets(line, MAX_STR, fp_QOR) != NULL){

		if(strstr(line,"Latency Index"))
			{ 
			result = strtok(line, ":");
			result = strtok( NULL, "\t" );
			latency = atoi(specialConvert(result,&splitChar));		
			break;
			}
	}
	fclose(fp_QOR);
	return latency;
	
}

//gen_run_rtl_sim
void gen_run_rtl_sim(bool gen,bool run,unsigned int cycles,char* process,char* hdl){

  printf("--Running RTL Simulation...\n");
  char cmd[MAX_STR];
  	
  if(gen==GEN){
	if(cycles==0)
	/*
	{
    sprintf(cmd, "tbgen %s_E.IFF -max_cycle=100000 -output=file -b10 	 \
					-compare=transaction -finish	 \
					-scr=modelsim-all  -dump=VCD 	 \
					-dump_signal=PORT:ALL 			 \
					-lic_wait=120 %s >>LOG_Sim", process,hdl);system(cmd); }
	*/
	
	{
    sprintf(cmd, "tbgen %s_E.IFF -max_cycle=100000 -output=file -b10 	 \
					-finish	 \
					-scr=modelsim-all  -dump=VCD 	 \
					-dump_signal=PORT:ALL 			 \
					-lic_wait=120 %s >>LOG_Sim", process,hdl);system(cmd); }
					
	else
	/*
	{
    sprintf(cmd, "tbgen %s_E.IFF -max_cycle=100000  -output=file -b10 	\
					-compare=transaction -finish	\
					-scr=modelsim-all  -dump=VCD 	\
					-dump_signal=PORT:ALL 			\
					-lic_wait=120 -cycles=%d %s >>LOG_Sim", process,cycles,hdl);system(cmd); 

	}
    */
	{
    sprintf(cmd, "tbgen %s_E.IFF -max_cycle=100000  -output=file -b10 	\
					-finish	\
					-scr=modelsim-all  -dump=VCD 	\
					-dump_signal=PORT:ALL 			\
					-lic_wait=120 -cycles=%d %s >>LOG_Sim", process,cycles,hdl);system(cmd); 

	}
	//printf("zzzzzz\n");	

  }

  if(run==RUN){
    system("chmod +x ./vr_vsim.bat "); 
	system("./vr_vsim.bat >>LOG_Sim"); 
  }



}
//gen_run_logic_synthesis
unsigned int gen_run_logic_synthesis(bool gen,bool run,char* tools,char* process){


	printf("--Running Logic Synthesis[%s]...\n",tools);
	char cmd[MAX_STR],line[MAX_STR],syrfile[MAX_STR],*result;
    unsigned int area=0;
	char splitChar=',';
	char LSsetup[MAX_STR];
	FILE *fp_syr;

	if(strstr(tools,"dc")){
		sprintf(LSsetup,"-syn_tool=dc -dc_scr=tcl  -syn_tool_setup=%s/siyuan.setup ",LIBS_PATH);
	}
	else if (strstr(tools,"qtscv"))
	{
		sprintf(LSsetup,"-syn_tool=qts   -all_pins_virtual=YES     	\
						-fpga_family=cycloneV 	\
						-fpga_device=5CSEMA5 	\
						-fpga_package=FC31 		\
						-fpga_speed_grade=-6	");
	}
	if(gen==GEN){
		sprintf(cmd, "LSscrgen *_E.v -EE %s_E.IFF  %s -lic_wait=120  >> LOG_LS", process,LSsetup); 
			system(cmd);
			//print_char_red(cmd);
		if (strstr(tools,"qts")){
				sprintf(cmd, "perl -p -i -e 's/\\(null\\)/\\$pin_name != \"clk\" && \\$pin_name != \"rst\"/g' ./%s_E.tcl",process); system(cmd);
				//print_char_red(cmd);
		}
		else if (strstr(tools,"dc"))
		{
				sprintf(cmd, "perl -p -i -e 's/_qor/_power/g' ./%s_E.dcscr",process); system(cmd);
				//print_char_red(cmd);

		}


	}
	
		if(strstr(tools,"dc")){
			if(run==RUN){
				sprintf(cmd, "dc_shell -f %s_E.dcscr >> LOG_LS", process); 
				system(cmd);
			}
			sprintf(syrfile,"%s_gate_area.rpt",process);
		}
		else if (strstr(tools,"qts"))
		{
			if(run==RUN){


				sprintf(cmd, "quartus_sh -t %s_E.tcl  >> LOG_LS", process); system(cmd);
			}
			sprintf(syrfile,"%s_E.fit.rpt",process);
		}		
	
	if( (fp_syr=fopen(syrfile,"rt"))==NULL){ 
		sprintf(cmd, "Cannot open %s\n",syrfile);
		//print_char_red(cmd);
		exit(-1);
	}

	while(fgets(line, MAX_STR, fp_syr) != NULL){


		if(strstr(line,"Total cell area:"))
			{ 
			result = strtok(line, ":");
			result = strtok( NULL, " " );
			//result = strtok( NULL, " " );
			area = atoi(result);	
			break;
			}
		if(strstr(line,"; Combinational ALUT usage for logic"))
			{ 
			result = strtok(line, ";");
			result = strtok( NULL, ";" );
			area = atoi(specialConvert(result,&splitChar));		
			break;
			}

	}
	fclose(fp_syr);
	return area;

}

double prime_time_power(char * process){

    printf("--Running PrimeTime Simulation...\n");
	FILE *fp_scr;
	char cmd[MAX_STR],line[MAX_STR],*result;
	double power;
	char splitChar=',';	

	sprintf(cmd,"%s_E.ptscr",process);
	fp_scr=fopen(cmd,"wt");
	fprintf(fp_scr,"set power_enable_analysis TRUE\n");
 	fprintf(fp_scr,"source %s/siyuan_pt.setup\n",LIBS_PATH);
	fprintf(fp_scr,"read_db $target_library\n");
	fprintf(fp_scr,"read_verilog %s_gate.v\n",process);
	fprintf(fp_scr,"current_design \"%s\"\n",process);
	fprintf(fp_scr,"link\n");
	fprintf(fp_scr,"read_sdc ./%s_gate.sdc \n",process);
	fprintf(fp_scr,"set power_analysis_mode \"time_based\"\n");
	fprintf(fp_scr,"read_vcd %s_E.vcd -strip_path T_%s_00/U_%s\n",process,process,process);
	fprintf(fp_scr,"check_power\n");
	fprintf(fp_scr,"set_power_analysis_options -waveform_format out -waveform_output %s_power_waveform\n",process); 
	fprintf(fp_scr,"update_power\n");
	fprintf(fp_scr,"redirect %s_pt_gate_power.rpt {report_power}\n",process);
	fprintf(fp_scr,"quit\n");	
	fclose(fp_scr);

	sprintf(cmd,"pt_shell -file %s_E.ptscr >>LOG_PT",process);
	system(cmd);
	sprintf(cmd,"%s_pt_gate_power.rpt",process);
	fp_scr=fopen(cmd,"rt");

	while(fgets(line, MAX_STR, fp_scr) != NULL){

		if(strstr(line,"Total Power"))
			{ 
			result = strtok(line, "=");
			result = strtok(NULL, "e" );
			power = atof(specialConvert(result,&splitChar));	
			result = strtok(NULL, " " );
			power *= pow(10,atoi(result));	
			break;
			}
	}

	fclose(fp_scr);
	return power;

}



double read_logic_synthesis_power(char* tools,char* process){

	char cmd[MAX_STR],line[MAX_STR],syrfile[MAX_STR],*result;
	char splitChar=',';
	FILE *fp_syr;
	double power;

	if(strstr(tools,"dc")){
		sprintf(syrfile,"%s_gate_power.rpt",process);
		if( (fp_syr=fopen(syrfile,"rt"))==NULL){ 
			sprintf(cmd, "Cannot open %s\n",syrfile);
			//print_char_red(cmd);
			exit(-1);
		}
	while(fgets(line, MAX_STR, fp_syr) != NULL){

		if(strstr(line,"Total Dynamic Power"))
			{ 
			result = strtok(line, "=");
			result = strtok( NULL, " " );
			power = atof(specialConvert(result,&splitChar));		
			}

		if(strstr(line,"Cell Leakage Power"))
			{ 
			result = strtok(line, "=");
			result = strtok( NULL, " " );
			power += atof(specialConvert(result,&splitChar));		
			break;
			}

	}
	
	}

	fclose(fp_syr);
	return power;

}



void gen_run_c_sim(bool gen,bool run,unsigned int cycles,char* process){

  char cmd[MAX_STR];
  if(gen==GEN){
	if(cycles==0){
		sprintf(cmd, "bmcppgen %s.IFF -tb=auto_io -b10 -undefine_value=zero -I-l", process);system(cmd);
					}
	else{
		sprintf(cmd, "bmcppgen %s.IFF -tb=auto_io -b10 -undefine_value=zero -I-l", process);system(cmd); 
	 
	 }


	 sprintf(cmd, "mkmfsim -GNU -I\".\" -cycle %d -target run_swsim.exe %s_0.MakeInfo", cycles,process);system(cmd); 
	
  }



  if(run==RUN){
    system("make -f Makefile.GNU"); 
	system("./run_swsim.exe"); 
  }



}

char* adjust_file(char *fullname){

   int from,to,i;
   char *newstr,*temp;
   if(fullname!=NULL)
   {
      if((temp=strchr(fullname,'.'))==NULL)//if not find dot
         newstr = fullname;
      else
      {
         from = strlen(fullname) - 1;
         to = (temp-fullname);  //the first dot's index;
         for(i=from;i<=to;i--)	
            if(fullname[i]=='.')break;//find the last dot
         newstr = (char*)malloc(i+1);
         strncpy(newstr,fullname,i);
         *(newstr+i)=0;
      }	
   }
   return newstr;
 }
void rewrite_verilog(char *vfile)
{

	//print_char_red(vfile);
	FILE *fp_syr;
  // Variables declaration
	char line[MAX_STR],*name,syrfile[MAX_STR],cmd[MAX_STR];

	sprintf(syrfile,"%s.v",vfile);
	if( (fp_syr=fopen(syrfile,"r"))==NULL) {
		printf("1cannot open %s\n",syrfile);
		system("pwd");
		exit(-1);
	}

	while(fgets(line, MAX_STR, fp_syr) != NULL){


	if(strstr(line,"wire") && strstr(line,"[")&& strstr(line,":")&& strstr(line,"]")&& (!strstr(line,"divisor"))&& (!strstr(line,"so"))&& (!strstr(line,"o1"))){
	
			name =strtok(line,"	");
			name =strtok(NULL,"	");
			name=strtok(NULL," ;"); //printf("1name is %s\n",name);
		//	name=strtok(NULL,"]"); //printf("2name is %s\n",name);

	
	sprintf(cmd,"perl -p -i -e \"s/%s/%s_%s/g\" ./%s.v",name,name,vfile,vfile);
	//printf("%s\n",cmd);
	system(cmd);

	}


	}
	fclose(fp_syr);


   }

void add_register(char *vfile)
{

	//print_char_red(vfile);
	//printf("\n");
	FILE *fp_syr,*fp_temp;
  // Variables declaration
	char line[MAX_STR],*token,*name,syrfile[MAX_STR],line_std[MAX_STR];

	sprintf(syrfile,"%s",vfile);
	if( (fp_syr=fopen(syrfile,"r"))==NULL) {
		printf("1cannot open %s\n",syrfile);
		system("pwd");
		exit(-1);
	}
	sprintf(syrfile,"temp_%s",vfile);
	if( (fp_temp=fopen(syrfile,"w"))==NULL) {
		printf("2cannot open %s\n",syrfile);
		exit(-1);
	}


	
	
	while(fgets(line, MAX_STR, fp_syr) != NULL){


	if(strstr(line,"wire") && strstr(line,"[")&& strstr(line,":")&& strstr(line,"]")&& (!strstr(line,"divisor"))){

		   // printf("%s\n",line);
			fprintf(fp_temp,"%s",line);
			sprintf(line_std,"%s",line);

			token =strtok(line_std," ;");
			token =strtok(line_std,"[");
			token =strtok(NULL,"[");

			fprintf(fp_temp,"reg\t[%s_reg;\n",token);	
			
			token =strtok(line," ;");
			name=strtok(line,"]"); //printf("1name is %s\n",name);
			name=strtok(NULL,"]"); //printf("2name is %s\n",name);


	fprintf(fp_temp,"\nalways @ ( * )\n");	
	fprintf(fp_temp,"\t%s_reg<= %s;\n",name,name);
	

	}
	else
	{
			fprintf(fp_temp,"%s",line);
	}

	}
	fclose(fp_temp);
	fclose(fp_syr);


	   //sprintf(line, "mv temp_%s %s", vfile,vfile);system(line); 
   

   }
void change_verilog(void){

			system("ls *.v > verilog_list");
			FILE *fp_vlist;
			char line_list[MAX_STR];
			fp_vlist=fopen("verilog_list","rt");
			if(fp_vlist==NULL){
				exit(-1);
			}
			while(fscanf(fp_vlist,"%s",line_list)!=EOF){//every verilog file
			//	printf("%s\n",line_list);
				
				char *xx;
				xx=adjust_file(line_list);
				//printf("%s\n",xx);
				rewrite_verilog(xx);
				add_register(xx);
				
			}
			fclose(fp_vlist);
		///*** Test ***//

}



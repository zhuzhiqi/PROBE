#include "global.h"

float stdev = MAX_STDEV;
CVar *apx_var = NULL;
double prob=0;
double glob_std_dev=0;
double glob_mean=0;
char dis_rst[MAX_STR];
bool initial = true;
//int t_time;
//int pre_time;
//bool v_value;




bool first_time;

bool start;

void read_vcd(char *vcd_file,char * process,SSIGNALS *signal_list){


  printf("\n--- Start Faults Detection Process --- \n");
  
  #ifdef PRINT_REG_SIGNAL 
  system("mkdir data");
  #endif

  char PROBE[MAX_STR];
  sprintf(PROBE,"probes.dat");

/*

  FILE *vcd = NULL;
  char rstline[MAX_STR];
  vcd = fopen(vcd_file, "rt");
  if(vcd == NULL){
    printf("\nCould not open file %s \n\n", vcd_file);
    exit(EXIT_FAILURE);
  }

  while(fgets(rstline, MAX_STR, vcd) != NULL){
		if(strstr(rstline, "rst"))
			break;
  }

  
  //printf("test = %s\n",rstline);
  
  char *token;
  char rst_code[MAX_STR];
  //char dis_rst[MAX_STR];
  token = strtok(rstline, " ");
  token = strtok(NULL, " ");
  token = strtok(NULL, " ");
  token = strtok(NULL, " ");
  sprintf(rst_code,"%s",token);
  sprintf(dis_rst,"1");
  strcat(dis_rst,rst_code);
*/  
    
  // Read signals
  read_vcd_signals(vcd_file);
  
    //probe signal results
  probe(PROBE, apx_var); 
  
  // for each signal compute average
  vcd_signals_mean(vcd_file, apx_var);
  // for each signal compute stdev
  vcd_signals_stdev(vcd_file, apx_var);
  
  
 
  
  //all the results
  results_all(apx_var,process);
  
  

  

  //results can be approximate --V2C,V2V
 // v2c_approx(apx_var, stdev,process,signal_list);
 // v2v_approx(apx_var, stdev,process,signal_list);

 // printf("\n\n");
 // get_local_time();
 // printf("--- End Approx Process --- \n");
  
}

void vcd_signals_stdev(char *vcd_file, CVar *apx_var){

  // Variables declaration
  CVar *var;
  for(var=apx_var; var; var=var->m_nextp){
	if(var->probe){
		
		var->m_stdev = vcd_single_stdev(vcd_file, var);
		
	}
  }

}


float vcd_single_stdev(char *vcd_file, CVar *var){
	
	FILE *var_file = NULL;
	char line[MAX_STR];
	float sum=0.0,tmp=0.0;
	float value;
	
	
	char file_name[MAX_STR];
	sprintf(file_name,"./data/%s.csv",var->m_name);
	if(var->probe){
	var_file = fopen(file_name, "a+");
	if(var_file == NULL){
		printf("\nCould not open file\n\n");
		exit(EXIT_FAILURE);
	}
	
	
	
	while(fgets(line, sizeof(line), var_file)!=NULL){
		
		
		value = atof(line);
		tmp =  pow(((float)value -var->m_mean),2);
		sum += tmp;
		
	}
	}
	
	return (pow((sum/var->m_num),0.5));
	
	
}


/*
float vcd_single_stdev(char *vcd_file, CVar *var){

  FILE *ptr_file = NULL;
  char line[MAX_STR];
  bool flag_start = false;
  int count = 0;
  unsigned value=0;
  float sum=0.0,tmp=0.0;
   
  unsigned pre = 0;
  
  int t_time;
  

  ptr_file = fopen(vcd_file, "rt");
  if(ptr_file == NULL){
    printf("\nCould not open file %s \n\n", vcd_file);
    exit(EXIT_FAILURE);
  }


  // Read file data and generate structure linked list 
  while(fgets(line, MAX_STR, ptr_file) != NULL){  

  

	initial = true;

    // Read name of FUs from constraint file "NAME XXX"
    //if(strstr(line, "$dumpvars")){
    if(strstr(line, dis_rst)){
      flag_start = true;
      continue;
    }

    if(flag_start == false)
      continue;

  	//------------debounce-------------------------------
	
	char t_label = '#';
	char linehead = line[0];
	char *t_token;
		
	if( t_label == linehead){	
		t_token = strtok(line,"#");
		t_time = atoi(t_token);
		
	}
  
  
	char* code;
	
	
    if(code = strstr(line, var->m_code)){
		if((strlen(code)-1) ==strlen(var->m_code)){
			if(strstr(line, "XX"))
				continue;
    
			else{
				
					var->p_time = var->t_time;
					var->t_time = t_time;

				
					pre = value;	
					value = extract_value_vcd(line);
					
					
					if(value == 0 && initial) {
						continue;
					}
					else{
						
						if(((var->t_time - var->p_time) >100)){
					
							count ++;
							//value = extract_value_vcd(line);
							
							tmp =  pow(((float)pre -var->m_mean),2);
							sum += tmp;
							initial = false;
						}
					}

			}
		}
    }
	
  }
  fclose (ptr_file);

  printf("num = %d\n",count);
  printf("name = %s\n",var->m_name);
  printf("num = %d\n",count);
  

  return (pow((sum/count),0.5));

}
*/




//------------------
int  start_time(char *vcd_file, CVar *var){
	
	FILE *ptr_file = NULL;
	char line[MAX_STR];
	bool flag_start = false;
	float value = 0;  
	float pre = 0; 
	int t_time;
	
	int ret_time;
	
	ptr_file = fopen(vcd_file, "rt");
	if(ptr_file == NULL){
		printf("\nCould not open file %s \n\n", vcd_file);
		exit(EXIT_FAILURE);
	}
	
	while(fgets(line, MAX_STR, ptr_file) != NULL){ 
	
	
		//--------------------------------------------------
	
		char t_label = '#';
		char linehead = line[0];
		char *t_token;
		
		if( t_label == linehead){	
			t_token = strtok(line,"#");
			t_time = atoi(t_token);
		
		}

		//---------------------------------------------------
	
	
		if(strstr(line, "$dumpvars")){
			flag_start = true;
			continue;
		}
		
		
		if(flag_start == false)
			continue;
  

		char *code;
		
		
		
		if(code = strstr(line, var->m_code)){ 	

			if((strlen(code)-1) == strlen(var->m_code)){

				if(strstr(line, "xx")){
					continue;
				}
				else{
	
					//var->p_time = var->t_time;
					var->t_time = t_time;
					pre = value;	
					value = extract_value_vcd(line,var);				
				
					if((!first_time)&&(pre!=0)){
					
		
						ret_time = var->t_time;
						var->rec_start = true;
						first_time = true;										
					}
				}
	
			}	
	
		}
	
	
	}
	
	fclose(ptr_file);
	return ret_time;
	
}



//------------------
void vcd_signals_mean(char *vcd_file, CVar *apx_var){

  // Variables declaration
  CVar *var;
  
  
  //-------recording start time-------------------
  int s_time = 0;
  int tmp_time;

  for(var=apx_var; var; var=var->m_nextp){
	if((var->m_bw) >1){
		first_time = false;   
		tmp_time = start_time(vcd_file,var);
		if( var->probe){
			
			if(tmp_time > s_time) s_time = tmp_time;
		}
	}
  }
  
  printf("start_time = %d\n",s_time);
 //------------------------------------------------ 
 
  
  for(var=apx_var; var; var=var->m_nextp){
	
	//first_time = false;
	if(var->probe){
		//printf("var name = %s\n",var->m_name);
		var->m_mean = vcd_single_mean(vcd_file, var,s_time);	
	}
  }

}

float vcd_single_mean(char *vcd_file, CVar *var, int s_time){

  FILE *ptr_file = NULL;
  char line[MAX_STR];
  bool flag_start = false;
  float value = 0;  
  float pre = 0;
  
  int t_time;
  
  FILE *data_signal;
  

  ptr_file = fopen(vcd_file, "rt");
  if(ptr_file == NULL){
    printf("\nCould not open file %s \n\n", vcd_file);
    exit(EXIT_FAILURE);
  }



  
  /* Read file data and generate structure linked list */
  while(fgets(line, MAX_STR, ptr_file) != NULL){  
  
	initial= true;
	
	//------------debounce-------------------------------
	
	char t_label = '#';
	char linehead = line[0];
	char *t_token;
		
	if( t_label == linehead){	
		t_token = strtok(line,"#");
		t_time = atoi(t_token);
		
	}

	//---------------------------------------------------		







    if(strstr(line, "$dumpvars")){
    //if(strstr(line, dis_rst)){
      flag_start = true;
      continue;
    }

    if(flag_start == false)
      continue;
  

	char *code;
	
	
	



  	if(code = strstr(line, var->m_code)){ 	
		
		//printf("code = %s\n",var->m_code);
		if((strlen(code)-1) ==strlen(var->m_code)){
			//printf("Debuggin = %s\n",var->m_code);
			if(strstr(line, "XX")){
				//printf("Debugging = %s\n",line); 	
				continue;
			}
			else{
	
				
	
				var->p_time = var->t_time;
				var->t_time = t_time;

				pre = value;	
				value = extract_value_vcd(line,var);
				
						
				if(value == 0 && initial) {
					continue;
				}
				else{

					if(((var->t_time - var->p_time) >100)){ //debouncing 
					

					
						if(var->probe){
							
							

							
							if(var->t_time >= s_time){	
							
						//--------multiple cycles signal-------------	
						
						if((var->t_time - var->p_time)>20000){
							
							if (var->t_time > s_time){
								int cyc_num = (var->t_time - var->p_time)/20000;
								//printf("cyc_num = %d\n",cyc_num);		
								//printf("name = %s\n",var->m_name);
								//printf("time = %d\n",var->t_time);
								//printf("pre value = %f\n",pre);
								
								int cyc_tmp;
								for(cyc_tmp = 0;cyc_tmp<cyc_num-1;cyc_tmp++){
								var->m_num ++;	
								var->m_mean += (float)pre;
								#ifdef PRINT_REG_SIGNAL
								FILE* fp_signal;
								char file_name[MAX_STR];
								sprintf(file_name,"./data/%s.csv",var->m_name);
								fp_signal=fopen(file_name,"at");
								fprintf(fp_signal,"%f\n",pre);
								fclose(fp_signal);
								//data_signal = fp_signal;
								#endif
								if(var->m_max < pre)  var->m_max = pre;
								}
								
							}
							
						}
						
						//--------------------------------------------

								var->m_num ++;	
								var->m_mean += (float)pre;
								#ifdef PRINT_REG_SIGNAL
								FILE* fp_signal;
								char file_name[MAX_STR];
								sprintf(file_name,"./data/%s.csv",var->m_name);
								fp_signal=fopen(file_name,"at");
								fprintf(fp_signal,"%f\n",pre);
								fclose(fp_signal);
								//data_signal = fp_signal;
								#endif
								if(var->m_max < pre)  var->m_max = pre;
							}
							initial = false;
							
						}
						
					}
						
				}
						
			}
		}	  
		  

	}
	
  }


	fclose (ptr_file);
 
	
  
	//----------delete first line----------
	var->m_num = var->m_num - 1;
	
	char data_name[MAX_STR];
	sprintf(data_name,"./data/%s.csv",var->m_name);
	if(var->probe){
	data_signal = fopen(data_name, "a+");
	if(data_signal == NULL){
		printf("\nCould not open file\n\n");
		exit(EXIT_FAILURE);
	}


	FILE *tmp_file;
	int linenum = 0;
	char tmpline [1024];
	char  *tmpfilename="./data/tmp.csv";
	tmp_file = fopen(tmpfilename,"w+");
	
	while(fgets(tmpline, sizeof(tmpline), data_signal)!=NULL){
		
		if(linenum)
			fprintf(tmp_file,"%s",tmpline);
		
		linenum++;
		
	}
	
	fclose(data_signal);
	fclose(tmp_file);
	
	
	remove(data_name);
	rename(tmpfilename,data_name);
	
	}
    //-------------------------------------
	
	
 return(var->m_mean/var->m_num);

}

signed extract_value_vcd(char *line, CVar *apx_var){

  char *num;
  //unsigned decimalval = 0;
  signed decimalval = 0;
  int stringpos;


//printf("testing = %d\n",apx_var->withsign);  
  
  
  num = line;
  if(num[0] == 'b')
    num=line+1;
  else{
    return (atoi(num)); //problem
	printf("%d\n",atoi(num));
  }

//printf("testing = %d\n",apx_var->withsign);	 
//printf("lafjkjflajf = %d\n",apx_var->m_bw);
 
  //if(!apx_var->withsign){
  
	for (stringpos = 0; stringpos<strlen(num); stringpos++){
		if(num[stringpos] == ' '){
			num[stringpos] = '\0';
			break;
		}
	}

	for (stringpos=0; stringpos<strlen(num); stringpos++) {

		decimalval = decimalval<<1;   
		if (num[stringpos]=='1') decimalval += 1;

	}
	
//printf("decimalval = %d\n",decimalval);

  
  //}
  /*
  if(apx_var->withsign){

	 
	 
	for (stringpos = 1; stringpos<strlen(num); stringpos++){
		if(num[stringpos] == ' '){
			num[stringpos] = '\0';
			break;
		}
	}

	for (stringpos=1; stringpos<strlen(num); stringpos++) {
		decimalval = decimalval<<1;   
		if (num[stringpos]=='1') decimalval += 1;
		
		

	}	
	
	for (stringpos=0; stringpos<strlen(num); stringpos++) {
		//printf("ggggg = %c\n",num[stringpos]);
	}

	//printf("num = %s\n",num);
	//printf("strlen(num) = %d\n",strlen(num));
	//printf("bitwidth = %d\n",apx_var->m_bw);

	if(apx_var->m_bw == strlen(num)){
		
		//printf("zzq\n");
	 
		if (num[0] == '1'){
			decimalval = 0- decimalval;
			// printf("tetetetet = %d\n",decimalval);
		}
	
	}

  }
  */
  //printf("%u\n",decimalval);

  /* char *binstring = "10000001"; //binary for 129
  unsigned decimalval = 0;
  int stringpos;

  for (stringpos=strlen(binstring)-1; stringpos>=0; stringpos--) {
    decimalval = decimalval<<1;    
    if (binstring[stringpos]=='1') decimalval += 1;
  }
  */


  return decimalval;

}
void read_vcd_signals(char *vcd_file){

  FILE *ptr_file = NULL;
  char line[MAX_STR], *token,line_std[MAX_STR];
  CVar *var_new;


  ptr_file = fopen(vcd_file, "rt");
  if(ptr_file == NULL){
    printf("\nCould not open file %s \n\n", vcd_file);
    exit(EXIT_FAILURE);
  }


  /* Read file data and generate structure linked list */
  while(fgets(line, MAX_STR, ptr_file) != NULL){  
  
  

    // Read name of FUs from constraint file "NAME XXX"
    if(strstr(line, "$var reg")){

      // Ignore automatically generated signals
      if(strstr(line, "CLOCK"))		
	continue;	  
      else if(strstr(line, "RESET"))
	continue;
      else if(strstr(line, "Cysim_State"))
	continue;
      else if(strstr(line, "o1"))
	continue;	  
      else if(strstr(line, "t1"))
	continue;
      else if(strstr(line, "t2"))
	continue;
      else if(strstr(line, "t3"))
	continue;
      else if(strstr(line, "t4"))
	continue;
      else if(strstr(line, "t5"))
	continue;
      else if(strstr(line, "t6"))
	continue;
      else if(strstr(line, "B01"))
	continue;
      else if(strstr(line, "control"))
	continue;
      else if(strstr(line, "comp"))
	continue;

      var_new = gen_v2c_node();	  
      if(apx_var == NULL)
			apx_var = var_new;
      else{
			insert_v2c_node(apx_var, var_new);
	  }

      token = strtok(line," ");
      while (token != NULL){
			if(!strcmp("reg", token)){
				token = strtok(NULL," ");
				var_new->m_bw = atoi(token);
				token = strtok(NULL," ");
				// Read signal code
				var_new->m_code = (char*)malloc(strlen(token)+1);
				strcpy(var_new->m_code, token);

				// read signal name
				token = strtok(NULL," ");
				sprintf(line_std,"%s",token);
				var_new->m_name = (char*)malloc(strlen(token)+1);
				strcpy(var_new->m_name, token);

				if(strstr(line_std,"reg")) var_new->m_name[strlen(var_new->m_name)-4]='\0';
						var_new->m_mark=false;	  

				break;
			}

			token = strtok(NULL," ");
	
      }
	  //printf("name is %s,code is %s, bw is %d\n",var_new->m_name,var_new->m_code,var_new->m_bw);
    }
  }

  fclose (ptr_file);
  
  
  CVar *var;


}

CVar *gen_v2c_node(){

  // Variables declaration
  CVar *var;

  var = (CVar *)malloc(sizeof(CVar));
  var->m_name = 0;
  var->m_code = 0;
  var->m_bw = 0;

  var->m_num = 0;
  var->m_mean = 0.0;
  var->m_stdev = 0.0;
  var->m_max = 0.0;

  var->m_nextp= NULL;
  var->m_prevp = NULL;

  return var;

}
void insert_v2c_node(CVar *apx_var, CVar *var_new){

  // Variables declaration
  CVar *var= NULL;

  // Go to the end of the list
  for(var=apx_var; var->m_nextp != NULL; var=var->m_nextp);

  //insert var in list
  var->m_nextp = var_new;
  var_new->m_prevp = var;


}
void results_all(CVar *apx_var, char * process){

  /* Variables declartion */
    CVar *var;
	
	FILE* fp_siyuan;
	fp_siyuan = fopen("data.csv", "a");
	
	

	 fprintf(fp_siyuan,"%s ---- Info\n",process);
	 //fprintf(fp_siyuan,"\n\nAll Signals detailed information:\n"); 
	 fprintf(fp_siyuan,"%s,%s,%s,%s,%s\n", "name", "bw", "mean", "stdev","pct");	

	 printf("\n\nAll Signals detailed information:\n"); 
	 printf("%s\t%s\t%s\t%s\t%s\t%s\n", "name", "bw", "code","mean", "stdev","pct");	
	for(var=apx_var; var; var=var->m_nextp){
		if(var->probe){
			printf("%16s%5d\t%2s\t%0.2f\t%0.2f\t%0.4f\n", var->m_name, var->m_bw, var->m_code,var->m_mean, var->m_stdev, var->m_stdev/var->m_mean);	
			fprintf(fp_siyuan,"%s,%5d,%0.2f,%0.2f,%0.4f\n", var->m_name, var->m_bw,var->m_mean, var->m_stdev, var->m_stdev/var->m_mean);	
		}
	}//every register signal
	
	
	fclose(fp_siyuan);

}


void v2c_approx(CVar *apx_var, float stdev,char * process,SSIGNALS *signal_list){

  /* Variables declartion */
    CVar *var;
	FILE *fp_org,*fp_tmp,*fp_pruned;
	char line[MAX_STR],std[MAX_STR],org_verilog[MAX_STR],tmp_verilog[MAX_STR],apx_verilog[MAX_STR],papx_verilog[MAX_STR],cmd[MAX_STR],*token,pruned_verilog[MAX_STR];
	bool sim;
	unsigned int i;

	char FU_verilog[MAX_STR];

    printf("\n\nSignals within %0.2f STDEV of mean (Variable to Constant) \n", stdev); 
	//printf("%s\t%s\t%s\t%s\t%s\n", "name", "bw", "code","mean", "stdev");	


	system("ls *.v |grep -v USR |grep -v BAS |grep -v COM| grep -v FIO|grep -v SIG |grep -v tb  > verilog_list");
	sprintf(org_verilog,"%s_E.v",process);
	sprintf(tmp_verilog,"tmp_%s_E.v",process);
	sprintf(pruned_verilog,"pruned_%s_E.v",process);
	sprintf(FU_verilog,"CGFU_%s_E.v",process);

	SSIGNALS *sig_tmp;

	for(var=apx_var; var; var=var->m_nextp){
  
		if(abs(var->m_mean)== 0)
		  continue;
		else if(var->m_bw <=1)
		  continue;
		else if(var->m_stdev == 0.0)
		  continue;
	

	if( (var->m_stdev / var->m_mean)<=stdev ){

			prob=var->m_stdev / var->m_mean;
			glob_std_dev=var->m_stdev;
			glob_mean=var->m_mean;


			sprintf(std,"%s =",var->m_name);
			if((fp_org=fopen(org_verilog,"rt"))==NULL)
				printf("error in %s\n",org_verilog);

			if((fp_tmp=fopen(tmp_verilog,"wt"))==NULL)
				printf("error in %s\n",tmp_verilog);
			if((fp_pruned=fopen(pruned_verilog,"wt"))==NULL)
				printf("error in %s\n",pruned_verilog);


			sim=false;

			unsigned int module_num=0,current_module=0;

			while( fgets(line, MAX_STR, fp_org) != NULL ){
				if(strstr(line,"options:")){
						//do nothing 
					}
				else if(strstr(line,std) && strstr(line,"assign") && (!strstr(line,"in0")) && (!strstr(line,"in1"))&& (!strstr(line,"in2"))&& (!strstr(line,"in3"))&& (!strstr(line,"in4"))) { //replace
						fprintf(fp_tmp,"//TRY.\t%s",line);
						fprintf(fp_pruned,"//TRY.\t%s",line);
						token=strtok(line,"=");
						token=strtok(NULL,";");
						fprintf(fp_tmp,"assign  %s = rtl_sel_%s?'d%d:%s;//inserted signal\n",var->m_name,var->m_name,(int)var->m_mean,token);
						current_module=module_num;
						//TVLSI Version
						fprintf(fp_pruned,"assign  %s = 'd%d;//inserted signal\n",var->m_name,(int)var->m_mean);
						sim=true;
						for(sig_tmp=signal_list;sig_tmp!=NULL;sig_tmp=sig_tmp->next){
							if(!strcmp(var->m_name,sig_tmp->name))break;
						}
						//printf("true\n");
				}
				else if(strstr(line,"module")&& !strstr(line,"endmodule"))
				{
					module_num++;
					fprintf(fp_tmp,"%s",line);	
					fprintf(fp_pruned,"%s",line);
				}
				else {
						fprintf(fp_tmp,"%s",line);
						fprintf(fp_pruned,"%s",line);
				}
					
			}
			fclose(fp_org);
			fclose(fp_tmp);
			fclose(fp_pruned);




			if(sim){

				//add the clock gating to the top realted input signal...

				if((fp_org=fopen(tmp_verilog,"rt"))==NULL)
						printf("error in %s\n",org_verilog);
				if((fp_tmp=fopen("clock_gating_FU.v","wt"))==NULL)
						printf("error in %s\n",tmp_verilog);
					
				while( fgets(line, MAX_STR, fp_org) != NULL ){
					if(strstr(line,"assign")&&strstr(line,"=")&&(!strstr(line,"//TRY."))&&(!strstr(line,"//inserted"))){
						//printf("%s",line);
						char line_tmp[MAX_STR];
						sprintf(line_tmp,"%s",line);
						char *out1,*in1;
						token=strtok(line_tmp,"	");
						token=strtok(NULL,"=");
						in1=remove_space(token);
						token=strtok(NULL,";");
						out1=remove_space(token);
						int i;
						bool flag_enter=false;
						for(i=0;i<sig_tmp->adj_nums;i++){
							if(!strcmp(in1,sig_tmp->adjacent[i])){
								flag_enter=true;
								//printf("%s[%d] ",sig_tmp->adjacent[i],sig_tmp->adjacent_size[i]);
								break;
							}
						}
						if(flag_enter==true){
							fprintf(fp_tmp,"//TRY.\t%s",line);	
							fprintf(fp_tmp,"assign	%s = {%d{~rtl_sel_%s}} & %s; //inserted signal- Clockgating FUs \n",in1,sig_tmp->adjacent_size[i],sig_tmp->name,out1);
						}
						else
						{
							fprintf(fp_tmp,"%s",line);
						}
					}
					else
					{

						fprintf(fp_tmp,"%s",line);
					}
					
				}
				fclose(fp_tmp);
				fclose(fp_org);
				sprintf(cmd,"mv clock_gating_FU.v %s",tmp_verilog);
				system(cmd);

				//insert the select port to the module...


				//printf("current module %d\n",current_module);
				char tmp1_verilog[MAX_STR];
				sprintf(tmp1_verilog,"tmp1_%s",tmp_verilog);
				fp_org=fopen(tmp_verilog,"rt");
				fp_tmp=fopen(tmp1_verilog,"wt");	
				bool controlCur=true,controlTop=true;
				module_num=0;
				while( fgets(line, MAX_STR, fp_org) != NULL ){
				//printf("%s\n",line);
				if(strstr(line,"module") && !strstr(line,"endmodule"))
				{
					module_num++;
				}
				//printf("module_num %d\n",module_num);
				//perform only if there is a FSM and data path
				char select_signal[MAX_STR];		
				if((current_module>1) &&(module_num==1) && (controlTop==true)){ //top module 1
					//printf("Insert selection into the top module port..\n");
					/* insert into the module */
					while(!strstr(line,";")){		
						fprintf(fp_tmp,"%s",line);
						fgets(line, MAX_STR, fp_org);
					}
					sprintf(select_signal,", rtl_sel_%s",var->m_name);
					token=insert_into_string(line, select_signal, (int)(strchr(line,')')-line-1)) ;
					fprintf(fp_tmp,"%s",token);	
					/* insert the input port */
					fprintf(fp_tmp,"input rtl_sel_%s;\n",var->m_name);
					controlTop=false;
				}
				else if((current_module>1)&&strstr(line,"INST_dat")){
					sprintf(select_signal,".rtl_sel_%s(rtl_sel_%s),",var->m_name,var->m_name);
					token=insert_into_string(line, select_signal, (int)(strchr(line,'(')-line+1)) ;
					fprintf(fp_tmp,"%s",token);	
				}
				else if((module_num==current_module) && (controlCur==true)){
					//printf("Insert selection into the current module port..\n");
					/* insert into the module */
					while(!strstr(line,";")){
						fprintf(fp_tmp,"%s",line);
						fgets(line, MAX_STR, fp_org);		
					}
					sprintf(select_signal,", rtl_sel_%s",var->m_name);
					token=insert_into_string(line, select_signal, (int)(strchr(line,')')-line-1)) ;
					fprintf(fp_tmp,"%s",token);	
					/* insert the input port */
					fprintf(fp_tmp,"input rtl_sel_%s;\n",var->m_name);
					controlCur=false;
				}
				else{
					fprintf(fp_tmp,"%s",line);
					//printf("%s",line);
				}
			
				}
				fclose(fp_org);
				fclose(fp_tmp);
				sprintf(cmd,"mv %s %s",tmp1_verilog,tmp_verilog);
				system(cmd);
				for (i=0;i<10000;i++)
				{
					sprintf(apx_verilog,"Apx%u_%s",i,org_verilog);
					sprintf(papx_verilog,"Pruned_Apx%u_%s",i,org_verilog);
					if((fp_tmp=fopen(apx_verilog,"rt"))==NULL){
						sprintf(cmd,"mv %s %s",tmp_verilog,apx_verilog); system(cmd);
						sprintf(cmd,"mv %s %s",pruned_verilog,papx_verilog); system(cmd);
						//sprintf(cmd,"grep -ri \"//insert\" ./%s",apx_verilog); system(cmd);
						FILE* fp_siyuan;
						fp_siyuan = fopen("siyuan.rpt", "at");
						//printf("(v2c)%-20s%5d\t%s\t%0.2f\t%0.2f\t%s\n", var->m_name, var->m_bw, var->m_code,var->m_mean, var->m_stdev,apx_verilog);	
						printf("(v2c)%16s[%0.2lf,%0.2lf] <- [%0.2lf] --%s \n",var->m_name,var->m_mean,var->m_stdev,var->m_mean,apx_verilog);
						fprintf(fp_siyuan,"(v2c)%s,%s,%d,", apx_verilog,var->m_name, var->m_bw);	
						int k;
						for(k=0;k<sig_tmp->adj_nums;k++){
								fprintf(fp_siyuan,"%s,%d,",sig_tmp->adjacent[k],sig_tmp->adjacent_size[k]);
						}
							fprintf(fp_siyuan,"END_DFG\n");
						fclose(fp_siyuan);


						//fprintf(fp_siyuan,"%s%5d\t%s\t%0.2f\t%0.2f\t%s\n", var->m_name, var->m_bw, var->m_code,var->m_mean, var->m_stdev,apx_verilog);	
						
						get_accuracy(i,process,apx_verilog);//now we are focus on the Clock Gating one
						#ifdef SIM_Prune
						get_accuracy(i,process,papx_verilog);
						#endif


						break;
					}
					else
						fclose(fp_tmp);
				}
			}
			else
			{
				sprintf(cmd,"rm -rf %s",tmp_verilog);system(cmd);
				sprintf(cmd,"rm -rf %s",pruned_verilog);system(cmd);
			}

	}//results within std_dev

	}//every register signal

	//sprintf(cmd,"mv *Apx*.v ./V2C_Designs/"); system(cmd);

}

void v2v_approx(CVar *apx_var, float stdev,char * process,SSIGNALS *signal_list){

  /* Variables declartion */
    CVar *var;
	CVar *var2;
	FILE *fp_org,*fp_tmp,*fp_pruned;
	char line[MAX_STR],std[MAX_STR],org_verilog[MAX_STR],tmp_verilog[MAX_STR],apx_verilog[MAX_STR],papx_verilog[MAX_STR],cmd[MAX_STR],*token,pruned_verilog[MAX_STR];
	bool sim;
	unsigned int i;

	char FU_verilog[MAX_STR];

    printf("\n\nSignals within %0.2f MEAN/STDEV (Variable to Variable) \n", MAX_V2V); 
	//printf("%s\t%s\t%s\t%s\t%s\n", "name", "bw", "code","mean", "stdev");	


	system("ls *.v |grep -v USR |grep -v BAS |grep -v COM| grep -v FIO|grep -v SIG |grep -v tb  > verilog_list");
	sprintf(org_verilog,"%s_E.v",process);
	sprintf(tmp_verilog,"tmp_%s_E.v",process);
	sprintf(pruned_verilog,"pruned_%s_E.v",process);
	sprintf(FU_verilog,"CGFU_%s_E.v",process);

	SSIGNALS *sig_tmp;

	for(var=apx_var; var; var=var->m_nextp){

		for(var2=apx_var; var2; var2=var2->m_nextp){
  
			if(abs(var2->m_mean)== 0)
			  continue;
			else if(var2->m_bw <=1)
			  continue;
			else if(var2->m_stdev == 0.0)
			  continue;

			//skip the same signal..
			if( !strcmp(var2->m_name,var->m_name)){
				//printf("same name : %s-%s\n",var2->m_name,var->m_name);
				continue;
			}

		

		if( APX_EQU(var2->m_mean,var->m_mean) && APX_EQU(var2->m_stdev,var->m_stdev)  ){


			
	
			prob=var->m_stdev / var->m_mean;
			glob_std_dev=var->m_stdev;
			glob_mean=var->m_mean;


			sprintf(std,"%s =",var->m_name);
			if((fp_org=fopen(org_verilog,"rt"))==NULL)
				printf("error in %s\n",org_verilog);

			if((fp_tmp=fopen(tmp_verilog,"wt"))==NULL)
				printf("error in %s\n",tmp_verilog);
			if((fp_pruned=fopen(pruned_verilog,"wt"))==NULL)
				printf("error in %s\n",pruned_verilog);


			sim=false;

			unsigned int module_num=0,current_module=0;

			while( fgets(line, MAX_STR, fp_org) != NULL ){
				if(strstr(line,"options:")){
						//do nothing 
					}
				else if(strstr(line,std) && strstr(line,"assign") && (!strstr(line,"in0")) && (!strstr(line,"in1"))&& (!strstr(line,"in2"))&& (!strstr(line,"in3"))&& (!strstr(line,"in4"))) { //replace
						fprintf(fp_tmp,"//TRY.\t%s",line);
						fprintf(fp_pruned,"//TRY.\t%s",line);
						token=strtok(line,"=");
						token=strtok(NULL,";");
						if(!strcmp(remove_space(token),var2->m_name))//same signal, skip..
							continue;
						fprintf(fp_tmp,"assign  %s = rtl_sel_%s?%s:%s;//inserted signal\n",var->m_name,var->m_name,var2->m_name,token);
						current_module=module_num;
						//TVLSI Version
						fprintf(fp_pruned,"assign  %s = %s;//inserted signal\n",var->m_name,var2->m_name);
						sim=true;
						for(sig_tmp=signal_list;sig_tmp!=NULL;sig_tmp=sig_tmp->next){
							if(!strcmp(var->m_name,sig_tmp->name))break;
						}
						//printf("true\n");
				}
				else if(strstr(line,"module")&& !strstr(line,"endmodule"))
				{
					module_num++;
					fprintf(fp_tmp,"%s",line);	
					fprintf(fp_pruned,"%s",line);
				}
				else {
						fprintf(fp_tmp,"%s",line);
						fprintf(fp_pruned,"%s",line);
				}
					
			}
			fclose(fp_org);
			fclose(fp_tmp);
			fclose(fp_pruned);




			if(sim){

				//add the clock gating to the top realted input signal...

				if((fp_org=fopen(tmp_verilog,"rt"))==NULL)
						printf("error in %s\n",org_verilog);
				if((fp_tmp=fopen("clock_gating_FU.v","wt"))==NULL)
						printf("error in %s\n",tmp_verilog);
					
				while( fgets(line, MAX_STR, fp_org) != NULL ){
					if(strstr(line,"assign")&&strstr(line,"=")&&(!strstr(line,"//TRY."))&&(!strstr(line,"//inserted"))){
						//printf("%s",line);
						char line_tmp[MAX_STR];
						sprintf(line_tmp,"%s",line);
						char *out1,*in1;
						token=strtok(line_tmp,"	");
						token=strtok(NULL,"=");
						in1=remove_space(token);
						token=strtok(NULL,";");
						out1=remove_space(token);
						int i;
						bool flag_enter=false;
						for(i=0;i<sig_tmp->adj_nums;i++){
							if(!strcmp(in1,sig_tmp->adjacent[i])){
								flag_enter=true;
								//printf("%s[%d] ",sig_tmp->adjacent[i],sig_tmp->adjacent_size[i]);
								break;
							}
						}
						if(flag_enter==true){
							fprintf(fp_tmp,"//TRY.\t%s",line);	
							fprintf(fp_tmp,"assign	%s = {%d{~rtl_sel_%s}} & %s; //inserted signal- Clockgating FUs \n",in1,sig_tmp->adjacent_size[i],sig_tmp->name,out1);
						}
						else
						{
							fprintf(fp_tmp,"%s",line);
						}
					}
					else
					{

						fprintf(fp_tmp,"%s",line);
					}
					
				}
				fclose(fp_tmp);
				fclose(fp_org);
				sprintf(cmd,"mv clock_gating_FU.v %s",tmp_verilog);
				system(cmd);

				//insert the select port to the module...


				//printf("current module %d\n",current_module);
				char tmp1_verilog[MAX_STR];
				sprintf(tmp1_verilog,"tmp1_%s",tmp_verilog);
				fp_org=fopen(tmp_verilog,"rt");
				fp_tmp=fopen(tmp1_verilog,"wt");	
				bool controlCur=true,controlTop=true;
				module_num=0;
				while( fgets(line, MAX_STR, fp_org) != NULL ){
				//printf("%s\n",line);
				if(strstr(line,"module") && !strstr(line,"endmodule"))
				{
					module_num++;
				}
				//printf("module_num %d\n",module_num);
				//perform only if there is a FSM and data path
				char select_signal[MAX_STR];		
				if((current_module>1) &&(module_num==1) && (controlTop==true)){ //top module 1
					//printf("Insert selection into the top module port..\n");
					// insert into the module 
					while(!strstr(line,";")){		
						fprintf(fp_tmp,"%s",line);
						fgets(line, MAX_STR, fp_org);
					}
					sprintf(select_signal,", rtl_sel_%s",var->m_name);
					token=insert_into_string(line, select_signal, (int)(strchr(line,')')-line-1)) ;
					fprintf(fp_tmp,"%s",token);	
					// insert the input port 
					fprintf(fp_tmp,"input rtl_sel_%s;\n",var->m_name);
					controlTop=false;
				}
				else if((current_module>1)&&strstr(line,"INST_dat")){
					sprintf(select_signal,".rtl_sel_%s(rtl_sel_%s),",var->m_name,var->m_name);
					token=insert_into_string(line, select_signal, (int)(strchr(line,'(')-line+1)) ;
					fprintf(fp_tmp,"%s",token);	
				}
				else if((module_num==current_module) && (controlCur==true)){
					//printf("Insert selection into the current module port..\n");
					// insert into the module 
					while(!strstr(line,";")){
						fprintf(fp_tmp,"%s",line);
						fgets(line, MAX_STR, fp_org);		
					}
					sprintf(select_signal,", rtl_sel_%s",var->m_name);
					token=insert_into_string(line, select_signal, (int)(strchr(line,')')-line-1)) ;
					fprintf(fp_tmp,"%s",token);	
					// insert the input port 
					fprintf(fp_tmp,"input rtl_sel_%s;\n",var->m_name);
					controlCur=false;
				}
				else{
					fprintf(fp_tmp,"%s",line);
					//printf("%s",line);
				}
			
				}
				fclose(fp_org);
				fclose(fp_tmp);
				sprintf(cmd,"mv %s %s",tmp1_verilog,tmp_verilog);
				system(cmd);
				for (i=0;i<10000;i++)
				{
					sprintf(apx_verilog,"Apx%u_%s",i,org_verilog);
					sprintf(papx_verilog,"Pruned_Apx%u_%s",i,org_verilog);
					if((fp_tmp=fopen(apx_verilog,"rt"))==NULL){
						sprintf(cmd,"mv %s %s",tmp_verilog,apx_verilog); system(cmd);
						sprintf(cmd,"mv %s %s",pruned_verilog,papx_verilog); system(cmd);
						//sprintf(cmd,"grep -ri \"//insert\" ./%s",apx_verilog); system(cmd);
						FILE* fp_siyuan;
						fp_siyuan = fopen("siyuan.rpt", "at");
						//printf("(v2v)%-20s%5d\t%s\t%0.2f\t%0.2f\t%s\n", var->m_name, var->m_bw, var->m_code,var->m_mean, var->m_stdev,apx_verilog);	
						printf("(v2v)%16s[%0.2lf,%0.2lf] <- %16s[%0.2lf,%0.2lf] --%s\n",var->m_name,var->m_mean,var->m_stdev,var2->m_name,var2->m_mean,var2->m_stdev,apx_verilog);
						fprintf(fp_siyuan,"(v2v)%s,%s,%d,%s,", apx_verilog,var->m_name, var->m_bw,var2->m_name);	
						int k;
						for(k=0;k<sig_tmp->adj_nums;k++){
								fprintf(fp_siyuan,"%s,%d,",sig_tmp->adjacent[k],sig_tmp->adjacent_size[k]);
						}
							fprintf(fp_siyuan,"END_DFG\n");
						fclose(fp_siyuan);


						//fprintf(fp_siyuan,"%s%5d\t%s\t%0.2f\t%0.2f\t%s\n", var->m_name, var->m_bw, var->m_code,var->m_mean, var->m_stdev,apx_verilog);	
						
						get_accuracy(i,process,apx_verilog);//now we are focus on the Clock Gating one
						#ifdef SIM_Prune
						get_accuracy(i,process,papx_verilog);
						#endif


						break;
					}
					else
						fclose(fp_tmp);
				}
			}
			else
			{
				sprintf(cmd,"rm -rf %s",tmp_verilog);system(cmd);
				sprintf(cmd,"rm -rf %s",pruned_verilog);system(cmd);
			}
	
	}//results within std_dev

	}//every register signal

	}

	//sprintf(cmd,"mv *Apx*.v ./V2V_Designs/");system(cmd);

}

void get_accuracy(unsigned int i,char *process,char *new_process){

		//printf("Apx%u_%s",i,process);
		char rtl_file[MAX_STR];
		char ProjectSpace[MAX_STR];
		char ApxWorkSpace[MAX_STR];
		char cmd[MAX_STR];

		getcwd(ProjectSpace, MAX_STR);
		sprintf(ApxWorkSpace,"%s/Apx_Work",ProjectSpace);
		if(chdir(ApxWorkSpace)<0){
			sprintf(cmd,"mkdir %s",ApxWorkSpace);
			system(cmd);
		}

		

		
		//printf("Apx%u_%s\n",i,process);
		chdir(ApxWorkSpace);
			//system("cp -rf ../../interp/* ./");
			//parse file, HLS, dc_shell, pt_shell, simulation
			system("rm -rf *.IFF");
			sprintf(cmd,"cp -rf ../%s ./%s.v",new_process,process);
			system(cmd);
			sprintf(rtl_file,"%s.v",process);

		#ifdef GET_ERROR
			unsigned area_ls;
			double power_pt;
			double error_new;
			char distri[]="rand";
			inparse(rtl_file);
			area_ls=hls_apx(process);

			//original testvectors
			sprintf(cmd,"cp -rf ../%s ./",goldenfile);system(cmd);
			system("cp -rf ../*.tlv ./");

			sprintf(rtl_file,"%s_E.v",process);
			parse_rtl_get_input_tlv(rtl_file,distri);
			gen_run_rtl_sim(GEN,RUN,0,process,"");
			//if(strstr(new_process,"Pruned"))
			//	system("cp -rf ../ave8_gate.v ./");
			//else
			//	system("cp -rf ave8_gate.v ../");
			power_pt=prime_time_power(process);	
			#ifdef REAL_ERROR
				error_new=error_real();
			#else
				error_new=error();
			#endif
		#endif

		chdir(ProjectSpace);

	
		#ifdef GET_ERROR
		//save the simulation results for debugging
		if(error_new>=999){
			sprintf(cmd,"rm -rf %s",new_process);system(cmd);
			sprintf(cmd,"rm -rf Pruned_%s",new_process);system(cmd);
			sprintf(cmd,"rm -rf ./Apx_Work/*");system(cmd);
		}
		else{
			sprintf(cmd,"mkdir -p ./Apx_Sim/%s",new_process);system(cmd);
			sprintf(cmd,"mv ./Apx_Work/* ./Apx_Sim/%s/",new_process);system(cmd);
			printf("Area %d, Power %0.2lf uW, Error %lf, mean %lf, std %lf, std_prob %lf\n",area_ls,power_pt*1e6,error_new,glob_mean,glob_std_dev,prob);
			FILE *fp_apx;
			fp_apx=fopen(SIYUAN_LOG,"at");
			fprintf(fp_apx,"%s: Area %d, Power %0.2lf uW, Error %lf, mean %lf, std %lf, std_prob %lf\n",new_process,area_ls,power_pt*1e6,error_new,glob_mean,glob_std_dev,prob);
			fclose(fp_apx);
		}
		#endif

}

char* insert_into_string(char s[], char t[], int i) {  
    char string[MAX_STR];  
    int len = strlen(s);  
    if(len<=0 || i>len) {  
        printf("Error!\n");  
        exit(1);  
    }  
    if(len == 0) {  
        strcpy(s, t);  
    } else {  
        strncpy(string, s, i);  
        string[i] = '\0';  
        strcat(string, t);  
        strcat(string, s+i);  
        strcpy(s, string);  
    }  
    return s;  
}  

unsigned int hls_apx(char* process){

	char cmd[MAX_STR],line[MAX_STR],syrfile[MAX_STR],*result;
    unsigned int area=0;
	char splitChar=',';
	FILE *fp_syr;


  /* HLS */
   sprintf(cmd,"bdltran -lic_wait=120  -Zflib_out_multi=NO -Zflib_out_sign=EACH -o-I -OS -OR  \
	-Zmix_sign_share=REG 				   \
			-Zflib_out_limit=L1000:M1000:S1000 				 \
			-TT414 -Zport_valid_sig_gen 					 \
			-c%d -s   -lfl %s  -lb %s						 \
			-Zflib_fcnt_out=%s-auto %s.IFF >> LOG_HLS", 						 \
			CLOCK*100, FLIB_PATH, BLIB_PATH, process,process);
	system(cmd);

    sprintf(cmd,"bdltran -lic_wait=120  -Zflib_out_multi=NO -Zflib_out_sign=EACH -o-I -OS -OR  \
	-Zmix_sign_share=REG 				   \
		-Zresource_fcnt=USE 			   \
		-Zresource_mcnt=USE  			   \
		-TT414  -Zport_valid_sig_gen 	   \
		-c%d -cdp0 -s -lfl %s  -lb %s  		   \
		%s.IFF -o-I 					   \
		-lfc %s-auto.FCNT +lfl %s-auto.FLIB  >> LOG_HLS", \
		CLOCK*100, FLIB_PATH, BLIB_PATH, process,process,process);
	system(cmd);

  /* verilog gen*/
    sprintf(cmd," veriloggen -lic_wait=120 -D0  *%s_E.IFF >> LOG_HLS",  process);
	system(cmd);

	/*dc_shell  */
	sprintf(cmd, "	LSscrgen *_E.v -EE %s_E.IFF  \
					 -lic_wait=120 -syn_tool=dc -dc_scr=tcl -syn_tool_setup=%s/siyuan.setup", \
					process,LIBS_PATH); 
	system(cmd);
	sprintf(cmd, "	dc_shell -f %s_E.dcscr >> LOG_LS", process); 
	system(cmd);

	sprintf(syrfile,"%s_gate_area.rpt",process);
	if( (fp_syr=fopen(syrfile,"rt"))==NULL){ 
		sprintf(cmd, "Cannot open %s\n",syrfile);
		print_char_red(cmd);
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

void probe(char *probe_file, CVar *apx_var){
	
	
  CVar *var;
  
  char probe_line[MAX_STR];
  FILE *PROBES;
  
  
  for(var=apx_var; var; var=var->m_nextp){	
  
	 PROBES = fopen(probe_file,"r");
	 
	 while( fgets(probe_line, MAX_STR, PROBES) != NULL ){
		
		 char *token;
		 token=strtok(probe_line,"\n");
		 
		 
		 
		 
		 if(!strcmp(var->m_name,token)){
			
		 	var->probe = true;
			
			
			//-------------------------------------
			/*
			char var_name[MAX_STR];
			char *ret;
			strcpy(var_name,var->m_name);
	 
			while((ret= strstr(var_name,"u"))||(ret=strstr(var_name,"s"))){
		 
		
				if(isdigit(var_name[ret-var_name-1])){
					//printf("res = %s\n",ret);
					if (ret[0]=='u')
						var->withsign = false;
				
					if (ret[0]=='s')
						var->withsign = true;
			
					break;
				}
				else{
			
					strcpy(var_name,ret+1);
				}

			}
			*/
	 
			//-------------------------------------
			
		 }
	 }
	 
	  fclose(PROBES);
  }
	
 
}
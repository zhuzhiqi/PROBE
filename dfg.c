#include "global.h"




SDFG *gen_dfg_node(){
  // Variables declaration
  SDFG *var;
  var = (SDFG *)malloc(sizeof(SDFG));
  var->name = NULL;
  var->next= NULL;
  var->prev = NULL;
  return var;

}
void insert_SDFG_node(SDFG *first_output, SDFG *sig_new){

  SDFG *sig= NULL;
  for(sig=first_output; sig->next != NULL; sig=sig->next);
  sig->next = sig_new;
  sig_new->prev = sig;

}

SSIGNALS *gen_signal_list_node(){

  // Variables declaration
  SSIGNALS *var;
  var = (SSIGNALS *)malloc(sizeof(SSIGNALS));
  var->name = NULL;
  var->size = 0;
  //var->adjacent = {0};
  var->adj_nums = 0;
  var->next= NULL;
  var->prev = NULL;
  return var;

}
void insert_signal_list_node(SSIGNALS *first_output, SSIGNALS *sig_new){

  SSIGNALS *sig= NULL;
  for(sig=first_output; sig->next != NULL; sig=sig->next);
  sig->next = sig_new;
  sig_new->prev = sig;

}


SSIGNALS *read_dfg_name(char *infile){



	SSIGNALS *first_signal=NULL,*new_signal=NULL;
	FILE* fp_in;
	char line[MAX_STR],*token;
	bool dfg_start=false;
	bool multi_module=false;



	fp_in=fopen(infile,"r");
	if(fp_in==NULL){
		printf("cannot open %s\n",infile);
	//	fclose(fp_in);
		exit(-1);
	}

	while(fgets(line,MAX_STR,fp_in)!=NULL){

		if(strstr(line,"INST_dat")){
			multi_module=true;
			break;
		}
	}
	fclose(fp_in);

	fp_in=fopen(infile,"r");
	if(fp_in==NULL){
		printf("cannot open %s\n",infile);
		exit(-1);
	}
	

	while(fgets(line,MAX_STR,fp_in)!=NULL){
		


		dfg_start = true;
		//start reading the input/wire/reg name
		if(dfg_start==true){

		
		
		//print_char_red("start reading signal lists\n");
			//if((strstr(line,"input")||strstr(line,"wire")||strstr(line,"reg"))&&(!strstr(line,"CLOCK"))&&(!strstr(line,"RESET"))&&(!strstr(line,"C_"))&&(!strstr(line,"U_"))&&(!strstr(line,".input"))&&(!strstr(line,"module"))&&(!strstr(line,","))&&(!strstr(line,")"))&&(!strstr(line,"("))){
			if(strstr(line,"input")||strstr(line,"wire")||strstr(line,"reg")){



				char *input_line;
				char *wire_line;
				char *reg_line;
				input_line = strstr(line,"input");
				wire_line = strstr(line,"wire");
				reg_line = strstr(line,"reg");
				
				if((input_line == line) ||(wire_line == line) ||(reg_line == line)){
				//printf("zxzxzxzxxxzxzxzxz\n");
				//printf("%s\n",line);


					new_signal=gen_signal_list_node();
	
					if(first_signal==NULL)
						first_signal=new_signal;
					else
						insert_signal_list_node(first_signal,new_signal);
			
					if(strstr(line,"[")&&strstr(line,"]")){
	
						unsigned int tmp1,tmp2;
						token=strtok(line,"[");
						token=strtok(NULL,":");
						tmp1=atoi(token);
						token=strtok(NULL,"]");
						tmp2=atoi(token);
						new_signal->size=tmp1>tmp2?tmp1+1:tmp2+1;
						token=strtok(NULL,";");
					}
					else
					{
						token=strtok(line,"\t");
						token=strtok(NULL,";");
						new_signal->size=1;
					}
					new_signal->name=malloc(sizeof(char)*strlen(token));
					strcpy(new_signal->name,token);	
					new_signal->name=remove_space(new_signal->name);
				}

			}

		}
		


	}

	//printf("break1 goes here\n");
	fclose(fp_in);
	return first_signal;

}

SSIGNALS* read_dfg_flow(SSIGNALS* signal_lists,char* infile){

	FILE* probes;
	



	FILE* fp_in;
	char line[MAX_STR];
	bool dfg_start=false;
	bool multi_module=false;
	
	bool rdcont = false;
	
	char inst_line[MAX_STR];

	
	SSIGNALS* sig_tmp=signal_lists;
	fp_in=fopen(infile,"r");
	if(fp_in==NULL){
		printf("cannot open %s\n",infile);
		exit(-1);
	}

	while(fgets(line,MAX_STR,fp_in)!=NULL){

		if(strstr(line,"INST_dat")){
			multi_module=true;
			break;
		}
	}

	fclose(fp_in);

	fp_in=fopen(infile,"r");
	if(fp_in==NULL){
		printf("cannot open %s\n",infile);
		exit(-1);
	}

	while(fgets(line,MAX_STR,fp_in)!=NULL){
		
			
			char *token;
			char *out1;
			char *ports;
			int in_port = 0;
			char *in[MAX_STR];
				
			
			if((strstr(line,"INST_"))&&(strstr(line,".i1"))&&(strstr(line,");"))) {
				
					dfg_start = true;
				
					rdcont = false;
					strcpy(inst_line,line);
			}
			

			else if (((strstr(line,"INST_"))&&(strstr(line,".i1"))&&(strstr(line,") ,")))||(rdcont)){
				
				dfg_start = true;
				
				if ((strstr(line,"INST_"))&&(strstr(line,".i1"))){
						strcpy(inst_line,"");
						rdcont = true;
				}
				
				
				if(rdcont) {
					int n = strlen(line) -1;
					strncat(inst_line,line,n);
				}

				if(strstr(line,") );")) {
					strcat(inst_line,"\n"); 
					rdcont = false;
				}
				
				
			}

			
			if((!rdcont)&&(dfg_start)){

					ports = strchr(inst_line,'(');
		
					while(ports != NULL){
						ports = strchr(ports+1,'(');
						//printf("ports here\n");
						in_port++;
					}
					
					in_port = in_port -2;
										
					int i;
					token=strtok(inst_line,"(");
					for(i=0; i<in_port;i++){
						
						
						token=strtok(NULL,"(");
						token=strtok(NULL,")");
						
						in[i] = remove_space(token);
						
						
					}
					
					token=strtok(NULL,"(");
					token=strtok(NULL,")");
					out1=remove_space(token);
					
					dfg_start = false;
				
				
			}
		
			
		
			
			
			probes=fopen("./probes.dat","a+");
			for(sig_tmp=signal_lists;sig_tmp!=NULL;sig_tmp=sig_tmp->next){
				if(!strcmp(out1,sig_tmp->name)){
						
					fprintf(probes,"%s\n",sig_tmp->name);

					int n ;
					sig_tmp->probe = true;
					for(n = 0; n<in_port; n++){
						strcpy(sig_tmp->adjacent[sig_tmp->adj_nums++],in[n]);
					}

					
				}
			}
			fclose(probes);


	
	}

	fclose(fp_in);

	return signal_lists;




}

void Trace_back_regular(SSIGNALS* curr_lists,SSIGNALS* global_lists){

	FILE*fp_dfg;
	if((curr_lists->adj_nums==0)){
			fp_dfg=fopen(DFG_FILE,"at");
			fprintf(fp_dfg,"%-16s ",curr_lists->name);
			fclose(fp_dfg);

			return;
	}

	int i;
	SSIGNALS*sig_tmp=NULL;
	//printf("%s adj_nums %d\n",curr_lists->name,curr_lists->adj_nums);
	for(i=0;i<curr_lists->adj_nums;i++){
		//printf("1\n");
		for(sig_tmp=global_lists;sig_tmp!=NULL;sig_tmp=sig_tmp->next){
			//printf("2\n");
			if(strstr(curr_lists->adjacent[i],sig_tmp->name)){
				Trace_back_regular(sig_tmp,global_lists);
				break;
			}
		}
	}
	//printf("3\n");
	//printf("4\n");
}


void Trace_back(SSIGNALS* curr_lists,SSIGNALS* global_lists){

	FILE*fp_dfg;
	if((curr_lists->adj_nums==1)){

		SSIGNALS*sig_tmp1=NULL;
		for(sig_tmp1=global_lists;sig_tmp1!=NULL;sig_tmp1=sig_tmp1->next){
				//printf("2\n");
				if(strstr(curr_lists->adjacent[0],sig_tmp1->name)){
					break;
				}
		}
		if(sig_tmp1->adj_nums==0){
			fp_dfg=fopen(DFG_FILE,"at");
			fprintf(fp_dfg,"%s,%d,",curr_lists->name,curr_lists->size);
			fclose(fp_dfg);
			//printf("%s,%d,",curr_lists->name,curr_lists->size);
			return;
		}

	}


	int i;
	SSIGNALS*sig_tmp=NULL;
	//printf("%s adj_nums %d\n",curr_lists->name,curr_lists->adj_nums);
	for(i=0;i<curr_lists->adj_nums;i++){
		//printf("1\n");
		for(sig_tmp=global_lists;sig_tmp!=NULL;sig_tmp=sig_tmp->next){
			//printf("2\n");
			if(!strcmp(curr_lists->adjacent[i],sig_tmp->name)){
				Trace_back(sig_tmp,global_lists);
				break;
			}
		}
	}
	//printf("3\n");
	//printf("4\n");
}




void create_file(char * infile){

	//SDFG *first_dfg=NULL,*new_dfg=NULL;
	//
	//new_dfg=gen_SDFG_node();
	//if(first_dfg==NULL)
	//	first_dfg=new_dfg;
	//else
	//	insert_SDFG_node(first_dfg,new_dfg);


	//obtain the Dat signal lists
	SSIGNALS* signal_lists=NULL,*sig_tmp;	

	signal_lists=read_dfg_name(infile);

	signal_lists=read_dfg_flow(signal_lists,infile);

	FILE *fp_dfg;
	fp_dfg=fopen(DFG_FILE,"at");
	fprintf(fp_dfg,"--- Created Data Flow Graph  ---\n");
	printf("-- Created Data Flow Graph  --\n");

	int i;
	for(sig_tmp=signal_lists;sig_tmp!=NULL;sig_tmp=sig_tmp->next){
		

		//printf("%16s [%d] <- ",sig_tmp->name,sig_tmp->adj_nums);
		fprintf(fp_dfg,"%16s [%d] <- ",sig_tmp->name,sig_tmp->adj_nums);
		for(i=0;i<sig_tmp->adj_nums;i++){
			//printf("%-16s ",sig_tmp->adjacent[i]);
			fprintf(fp_dfg,"%-16s ",sig_tmp->adjacent[i]);
		}
		fprintf(fp_dfg,"\n");
		//printf("\n");
	
	}
	fprintf(fp_dfg,"\n\n--- Trace Back to the Top  ---\n");
	//printf("\n\n--- Trace Back to the Top  ---\n");
	fclose(fp_dfg);

	for(sig_tmp=signal_lists;sig_tmp!=NULL;sig_tmp=sig_tmp->next){
		fp_dfg=fopen(DFG_FILE,"at");
		fprintf(fp_dfg,"sel %16s <- ",sig_tmp->name);
		fclose(fp_dfg);
		//printf("%16s <- ",sig_tmp->name);
		Trace_back_regular(sig_tmp,signal_lists);
		fp_dfg=fopen(DFG_FILE,"at");
		fprintf(fp_dfg,"\n");
		fclose(fp_dfg);
		//printf("\n");
	}


	//printf("\n\n---- Trace Back for approximation ----\n");
	fp_dfg=fopen(DFG_FILE,"at");
	fprintf(fp_dfg,"\n\n-- Trace Back for approximation --\n");
	fclose(fp_dfg);
	for(sig_tmp=signal_lists;sig_tmp!=NULL;sig_tmp=sig_tmp->next){
		//printf("%s,%d,",sig_tmp->name,sig_tmp->size);
		fp_dfg=fopen(DFG_FILE,"at");
		fprintf(fp_dfg,"%s,%d,",sig_tmp->name,sig_tmp->size);
		fclose(fp_dfg);
		Trace_back(sig_tmp,signal_lists);
		//printf("END_DFG\n");
		fp_dfg=fopen(DFG_FILE,"at");
		fprintf(fp_dfg,"END_DFG\n");
		fclose(fp_dfg);

	}
	//printf("\n\n");
	
	//--------------------------
	/*
	for(sig_tmp=signal_lists;sig_tmp!=NULL;sig_tmp=sig_tmp->next){
		if(sig_tmp)
		printf("signal name = %s\n",sig_tmp->name);
	}
	*/
	//--------------------------

}

SSIGNALS * DFG_creator(char *infile){
	
	//parse the RTL file and process
	create_file(infile);



	
	FILE* fp_dfg;
	char line[MAX_STR],*token;
	bool dfg_start=false;
	SSIGNALS *first_signal=NULL,*new_signal=NULL;
	
	fp_dfg=fopen(DFG_FILE,"rt");
	if(fp_dfg==NULL){
		printf("cannot open %s\n",DFG_FILE);
		exit(-1);
	}
	while(fgets(line,MAX_STR,fp_dfg)!=NULL){

		if(dfg_start==true){
			new_signal=gen_signal_list_node();
			if(first_signal==NULL)
				first_signal=new_signal;
			else
				insert_signal_list_node(first_signal,new_signal);
			

			token=strtok(line,",");
			new_signal->name=malloc(sizeof(char)*strlen(token));
			strcpy(new_signal->name,token);	
			
			
			
			new_signal->name=remove_space(new_signal->name);//printf("token1 is %s\n",token);
			
			
			token=strtok(NULL,",");
			new_signal->size=atoi(token);	//printf("token2 is %s\n",token);
			
			token=strtok(NULL,",");//printf("token is %s\n",token);
			while(!strstr(token,"END_DFG")){
				
				strcpy(new_signal->adjacent[new_signal->adj_nums],token);
				token=strtok(NULL,",");
				new_signal->adjacent_size[new_signal->adj_nums]=atoi(token);
				new_signal->adj_nums++;
				token=strtok(NULL,",");
			}

		}

		if(strstr(line,"---- Trace Back for approximation ----"))
			dfg_start=true;
		
		//printf("name = %s\n",new_signal->name);

	}
	fclose(fp_dfg);

	//int i;
	//SSIGNALS * sig_tmp;
	//for(sig_tmp=first_signal;sig_tmp!=NULL;sig_tmp=sig_tmp->next){
	//	printf("%s[%d] <- ",sig_tmp->name,sig_tmp->size);
	//	//fprintf(fp_dfg,"%16s [%d] <- ",sig_tmp->name,sig_tmp->adj_nums);
	//	for(i=0;i<sig_tmp->adj_nums;i++){
	//		printf("%s[%d] ",sig_tmp->adjacent[i],sig_tmp->adjacent_size[i]);
	//		//fprintf(fp_dfg,"%-16s ",sig_tmp->adjacent[i]);
	//	}
	//	//fprintf(fp_dfg,"\n");
	//	printf("\n");
	//
	//}
	return first_signal;
}

#include "global.h"




unsigned int gen_rand(unsigned int max){
	return(rand()%(max+1));
	//printf("tesing = ")
}

unsigned int gen_norm(unsigned int max){

	int mu=(max+1)/2;
	int sigma=(max+1)/16.7;
	//int sigma=(max+1)/50;
	double x=0;
	double z=0;
	double z0= (rand()%1000000)*0.000001;

	if (z0<0.5){
	   z=z0;
	}
	else{
	   z=1-z0;
	}
	double t=sqrt(log(1/pow(z,2)));
	double c0 = 2.515517;
	double c1 = 0.802853;
	double c2 = 0.010328;
	double d1 = 1.432788;
	double d2 = 0.189269;
	double d3 = 0.001308;
	x=t-((c0+c1*t+c2*pow(t,2))/(1+d1*t+d2*pow(t,2)+d3*pow(t,3)));
	if (z0 >= 0.5){
	   x=-x;
	}
	x=mu+(sigma*x);

	if (x>max)
	{
		x=max;
	}
	else if(x<0)
	{
		x=0;
	}
	
	//printf("Norm Distribution: mu = %d, sigma =  %d\n",mu, sigma);

	return (unsigned int)x;
}



unsigned int gen_pskew(unsigned int max){

	int mu=0;
	int n_max=0-max;
	int sigma=(max+1)/3.4;
	double x=0;
	double z=0;
	double z0= (rand()%1000000)*0.000001;

	if (z0<0.5){
	   z=z0;
	}
	else{
	   z=1-z0;
	}
	double t=sqrt(log(1/pow(z,2)));
	double c0 = 2.515517;
	double c1 = 0.802853;
	double c2 = 0.010328;
	double d1 = 1.432788;
	double d2 = 0.189269;
	double d3 = 0.001308;
	x=t-((c0+c1*t+c2*pow(t,2))/(1+d1*t+d2*pow(t,2)+d3*pow(t,3)));
	if (z0 >= 0.5){
	   x=-x;
	}
	x=mu+(sigma*x);

	if (x>max)
	{
		x=max;
	}
	else if(x<n_max)
	{
		x=n_max;
	}
	if(x<0)
	{
		x=-x;
	}
	return (unsigned int)x;
}
unsigned int gen_nskew(unsigned int max){
	int mu=max;
	int sigma=(max+1)/3.4;
	double x=0;
	double z=0;
	double z0= (rand()%1000000)*0.000001;

	if (z0<0.5){
	   z=z0;
	}
	else{
	   z=1-z0;
	}
	double t=sqrt(log(1/pow(z,2)));
	double c0 = 2.515517;
	double c1 = 0.802853;
	double c2 = 0.010328;
	double d1 = 1.432788;
	double d2 = 0.189269;
	double d3 = 0.001308;
	x=t-((c0+c1*t+c2*pow(t,2))/(1+d1*t+d2*pow(t,2)+d3*pow(t,3)));
	if (z0 >= 0.5){
	   x=-x;
	}
	x=mu+(sigma*x);

	if (x>2*max)
	{
		x=2*max;
	}
	else if(x<0)
	{
		x=0;
	}

	if(x>max)
	{
		x=2*max-x;
	}


	return (unsigned int)x;
}

unsigned int *gen_distribution(unsigned int numbers,unsigned int max,char *distri){

		unsigned int nums;
		unsigned int *out_put;
		out_put=malloc(sizeof(unsigned int)*(numbers+1));
		for(nums=0;nums<numbers;nums++){	
		//for(nums=0;nums<1000;nums++){	

				if(strstr(distri,"rand"))
					out_put[nums]=gen_rand((unsigned int)max);
				else if(strstr(distri,"norm"))
					out_put[nums]=gen_norm((unsigned int)max);
				else if(strstr(distri,"pskew"))
					out_put[nums]=gen_pskew((unsigned int)max);
				else
					out_put[nums]=gen_nskew((unsigned int)max);

		}

		return out_put;
}


struct_rtl_input *gen_struct_rtl_input_node(){

  // Variables declaration
  struct_rtl_input *var;
  var = (struct_rtl_input *)malloc(sizeof(struct_rtl_input));
  var->name = NULL;
  var->size = 0;
  var->next= NULL;
  var->prev = NULL;

  return var;

}

void insert_struct_rtl_input_node(struct_rtl_input *first_output, struct_rtl_input *sig_new){
  // Variables declaration
  struct_rtl_input *sig= NULL;
  // Go to the end of the list
  for(sig=first_output; sig->next != NULL; sig=sig->next);
  //insert var in list
  sig->next = sig_new;
  sig_new->prev = sig;
}

void parse_rtl_get_input_tlv(char *infile,char* distri, char* process){

	FILE *fp_in;
	char *token,line[MAX_STR];
	fp_in=fopen(infile,"rt");
	unsigned int nums;
	unsigned int *data;
	FILE *fp_tlv;
	char cmd[MAX_STR];

	char *comment;
	int line_num;
	char header[MAX_STR];
	char path[MAX_STR];

	
	
	
	struct_rtl_input* srtl_input=NULL,*srtl_input_new=NULL,*rtl_tmp=NULL;
	while(fgets(line,MAX_STR,fp_in)!=NULL){

		
		if(strstr(line,"endmodule"))
			break;

		//if((strstr(line,"input"))&&(!strstr(line,"CLOCK"))&&(!strstr(line,"RESET"))&&(!strstr(line,".input"))&&(!strstr(line,"module"))&&(!strstr(line,","))&&(!strstr(line,")"))&&(!strstr(line,"(")))
		if(strstr(line,"input"))
		{



			char *input_line;
			input_line = strstr(line,"input");

			
			if(input_line == line){
		
			
				srtl_input_new=gen_struct_rtl_input_node();
				
				
				if(srtl_input==NULL)
					srtl_input=srtl_input_new;
				
				else
					insert_struct_rtl_input_node(srtl_input,srtl_input_new);
	
				
				
				if(strstr(line,"[")&&strstr(line,"]")) //multi-bit input
				{
	
					unsigned int tmp1,tmp2;
					token=strtok(line,"[");
					token=strtok(NULL,":");
					tmp1=atoi(token);
					token=strtok(NULL,"]");
					tmp2=atoi(token);
					srtl_input_new->size=tmp1>tmp2?tmp1+1:tmp2+1;
					token=strtok(NULL,";");
		
				}
				else
				{
					token=strtok(line,"\t");
					token=strtok(NULL,";");
					
					
					
					srtl_input_new->size=1;
				}
				
				
			
			
				srtl_input_new->name=malloc(sizeof(char)*strlen(token));
	
				strcpy(srtl_input_new->name,token);	
				srtl_input_new->name=remove_space(srtl_input_new->name);
				
				
	
				//---------trace back to SystemC to determine data type----------		
				comment = strtok(NULL,".");
				comment = strtok(NULL,":");
				strcpy(header,comment);
				comment = strtok(NULL,"\n");
				line_num= atoi(comment);
				sprintf(path,"../%s%s",process,header);
	
				
	
				//printf("name = %s\n", srtl_input_new->name);
				//printf("header = %s\n",header);
				//printf("line num = %d\n",line_num);
				//printf("path = %s\n",path);
				
				
				if(fopen(path,"r+")!=NULL){
					
					FILE *headfile;
					headfile =fopen(path,"r+");
					
					char headline[MAX_STR];
					int n = 1;
					while(fgets(headline,MAX_STR,headfile)!=NULL){
						if(n==line_num){
							//printf("line = %s\n",headline);
							//printf("name = %s\n", srtl_input_new->name);
							if(strstr(headline,"sc_int")){
								srtl_input_new->size = srtl_input_new->size -1;
								//printf("sc_int = %s\n",headline);
								
							}
							//else printf("sc_uint = %s\n",headline);
						}
						n++;
					}
					
					fclose(headfile);
				}
			}
			
			//-------------------------------------------------------------------

		}
	

		

	}
	fclose(fp_in);

	//system("pwd");
	
	for(rtl_tmp=srtl_input;rtl_tmp!=NULL;rtl_tmp=rtl_tmp->next)
	{
		unsigned int max_dis;	
		//sprintf(cmd,"%s.tlv",rtl_tmp->name);
		sprintf(cmd,"%s.tlv",rtl_tmp->name);
		if((fp_tlv=fopen(cmd,"rt"))==NULL){
			//fclose(fp_tlv);
			fp_tlv=fopen(cmd,"wt");

			if(strstr(rtl_tmp->name,"sel"))
				max_dis=0;
			else
				max_dis=(unsigned int)pow(2,rtl_tmp->size)-1;

			printf("--Create input testvectors %s[bw:%u][nums:%u]--\n",rtl_tmp->name,rtl_tmp->size,NUMS);

			data=gen_distribution(NUMS,max_dis-1,distri);
			for(nums=0;nums<NUMS;nums++){
				if(strstr(rtl_tmp->name,"sel")) //test
					#ifdef APX2ORG
						fprintf(fp_tlv,"%u\n",0);//test
					#else
						fprintf(fp_tlv,"%u\n",1);//test
					#endif
				else
					fprintf(fp_tlv,"%u\n",data[nums]);
			}
			fclose(fp_tlv);
	    }
	  else
	  {
		fclose(fp_tlv);
	  }
	}

}


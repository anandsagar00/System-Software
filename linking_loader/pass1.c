#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define ll long long int

typedef struct ESTAB
{
    char name[30];
    ll addr;
    ll length;
}ESTAB;

ll hex_to_int(char hex[])
{
    ll ans=0;
    for(int i=0;hex[i]!='\0' && hex[i]!='\n';i++)
    {
        if(hex[i]>='0' && hex[i]<='9')
        ans=(ans*16)+(hex[i]-'0');
        else if(hex[i]=='A' || hex[i]=='a')
        ans=(ans*16)+10; 
        else if(hex[i]=='B' || hex[i]=='b')
        ans=(ans*16)+11; 
         else if(hex[i]=='C' || hex[i]=='c')
        ans=(ans*16)+12;
         else if(hex[i]=='D' || hex[i]=='d')
        ans=(ans*16)+13;
         else if(hex[i]=='E' || hex[i]=='e')
        ans=(ans*16)+14;
         else if(hex[i]=='F' || hex[i]=='f')
        ans=(ans*16)+15;  
    }
    return ans;
}

int search_estab(ESTAB current[],int len,char name[])
{
    for(int i=0;i<len;i++)
    {
        if(strcmp(name,current[i].name)==0)
        return 1;
    }
    return 0;
}

int main()
{
    ll PROGADDR;
    //as shown in book I will enter prog addr as 16384 = 4000 in hex
    printf("ENTER PROG-ADDR : ");
    scanf("%lld",&PROGADDR);
    ll CSADDR=0;
    ll CSLENGTH=0;
    FILE *input=fopen("input.txt","r");
    FILE *estab=fopen("estab.txt","w");

    ESTAB current[100];
    int curr_estab_len=0;
    int lines=1;
    CSADDR=PROGADDR;
    while (!feof(input))
    {
        char buffer[1000];
        fgets(buffer,1000,input);
        
        //taking care of the header record
        if(buffer[0]=='H' || buffer[0]=='h')
        {
            //H^proga^start_addr^prog_len
            char *tokens=strtok(buffer,"^ \n");//now pointing to H
            tokens=strtok(NULL,"^ \n");//now pointing to prog_name

            if(!search_estab(current,curr_estab_len,tokens))
            {
                //if the program name is not found in ESTAB
                strcpy(current[curr_estab_len].name,tokens);
                tokens=strtok(NULL,"^ \n");//now this is pointing to program start addr
                CSADDR=CSADDR+hex_to_int(tokens)+CSLENGTH;
                current[curr_estab_len].addr=CSADDR;
                tokens=strtok(NULL,"^ \n");//now this is pointing to program length
                current[curr_estab_len].length=hex_to_int(tokens);
                CSLENGTH=hex_to_int(tokens);
                curr_estab_len++;
            }
            else
            {
                printf("ERROR ! Duplicate program name at line %d\n",lines);
            }
        }
        else if(buffer[0]=='D' || buffer[0]=='d')
        {
            //taking care of the define record
            //D^LISTA^0001B
            char *tokens=strtok(buffer,"^ \n");
            tokens=strtok(NULL,"^ \n");//now pointing to LISTA
            while(tokens)
            {
                //D record always oocurs in pair or name,address
                if(!search_estab(current,curr_estab_len,tokens))//if name not found in estab
                {
                    strcpy(current[curr_estab_len].name,tokens);
                    tokens=strtok(NULL,"^ \n");//now this is pointing to address after LISTA
                    current[curr_estab_len].addr=CSADDR+hex_to_int(tokens);
                    current[curr_estab_len].length=-1;//indicating not a prog name
                    curr_estab_len++;
                }
                else
                {
                    printf("Error ! duplicate symbol at line %d\n",lines);
                }
                tokens=strtok(NULL,"^ \n");
            }
        }
        lines++;
    }
    for(int i=0;i<curr_estab_len;i++)
    {
        if(current[i].length!=-1)
        {
            fprintf(estab,"%s %llx %llx\n",current[i].name,current[i].addr,current[i].length);
        }
        else
        fprintf(estab,"%s %llx\n",current[i].name,current[i].addr);
    }
    fclose(estab);
    fclose(input);
}
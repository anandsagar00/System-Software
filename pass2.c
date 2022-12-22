#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define ll long long int

ll hex_to_int(char hex[])
{
    ll ans=0;
    for(int i=0;hex[i]!='\0' && hex[i]!='\n';i++)
    {
        if(hex[i]>='0' && hex[i]<='9')
        ans=(ans*16)+(hex[i]-'0');
        else
        {
            if(hex[i]=='A' || hex[i]=='a')
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
    }
    return ans;
}

int findLength()
{
    FILE *pass1=fopen("output_pass1.txt","r");
    char buffer[100];
    fgets(buffer,100,pass1);
    ll start=-1,end=-1;
    while(!feof(pass1))
    {
        fgets(buffer,100,pass1);
        char *tokens=strtok(buffer," \n");
        if(start==-1)
        {
            start=hex_to_int(tokens);
        }
        end=hex_to_int(tokens);
    }
    return end-start;
}

int searchSymtab(char label[], char address[])
{
    FILE *symtab = fopen("symtab.txt", "r");
    while (!feof(symtab))
    {
        char buffer[100];
        fgets(buffer, 100, symtab);
        char *tokens = strtok(buffer, " \n");
        while (tokens)
        {
            if (strcmp(tokens, label) == 0)
            {
                tokens = strtok(NULL, " \n");
                strcpy(address, tokens);
                fclose(symtab);
                return 1;
            }
            tokens = strtok(NULL, " \n");
        }
    }
    return 0;
}
int searchOptab(char operation[], char opcode[20])
{
    FILE *optab = fopen("optab.txt", "r");
    while (!feof(optab))
    {
        char buffer[100];
        fgets(buffer, 100, optab);
        char *tokens = strtok(buffer, " \n");
        while (tokens)
        {
            if (strcmp(tokens, operation) == 0)
            {
                tokens = strtok(NULL, " \n");
                if (tokens)
                    strcpy(opcode, tokens);
                fclose(optab);
                return 1;
            }
            tokens = strtok(NULL, " \n");
        }
    }
    return 0;
}

void generate()
{
    FILE *pass2=fopen("pass2_object_code.txt","r");
    FILE *output=fopen("pass2_output.txt","w");
    ll length=findLength();

    char buffer[100];

    fgets(buffer,100,pass2);

    //Header Record
    char *tokens=strtok(buffer," \n");
    tokens=strtok(NULL," \n");
    fprintf(output,"H^ %s",tokens);//printing the name of the prog

    tokens=strtok(NULL," \n");
    tokens=strtok(NULL," \n");
    char start_addr[30];
    strcpy(start_addr,tokens);
    fprintf(output,"^%s ^%llX",tokens,length);//printing the starting address

    int curr_text_count=0; // we will be changing line at a multiple of 10

    while(!feof(pass2))
    {
        ll start_addr=-1;
        fgets(buffer,100,pass2);
        tokens=strtok(buffer," \n");
        char copy_token[30];
        int flag_end=0;
        while(tokens)
        {
            if(strcmp("END",tokens)==0)
            flag_end=1;
            if(start_addr==-1)
            start_addr=hex_to_int(tokens);
            strcpy(copy_token,tokens);
            tokens=strtok(NULL," \n");
        }
        if(strcmp("---",copy_token)!=0 && flag_end!=1)
        {
            if(curr_text_count%10==0)
            {
                // i.e insert a new text record in new line
                fprintf(output,"\nT^%llX^%s",start_addr,copy_token);
            }
            else
            fprintf(output,"^%s",copy_token);
        }
        else if(flag_end==1)
        {
            fprintf(output,"\nE ^%llX",start_addr);
            break;
        }
        curr_text_count++;
    }
    
    fclose(pass2);
    fclose(output);
}

void pass2_objcode()
{
    // I am assuming that none of the OPCODE are wrong
    FILE *pass1 = fopen("output_pass1.txt", "r");
    FILE *obj = fopen("pass2_object_code.txt", "w");
    int lines = 1;
    while (!feof(pass1))
    {
        char buffer[1000];
        fgets(buffer, 1000, pass1);
        // now again there are 3 cases 2 word , 3 word or 4 words in a line

        char addr[20], label[20], mnemonic[20], oprand[20];

        int word_count = 0;
        char *tokens = strtok(buffer, " \n");
        while (tokens)
        {
            if (word_count == 0)
                strcpy(addr, tokens);
            else if (word_count == 1)
                strcpy(label, tokens);
            else if (word_count == 2)
                strcpy(mnemonic, tokens);
            else if (word_count == 3)
                strcpy(oprand, tokens);
            tokens = strtok(NULL, " \n");
            word_count++;
        }

        if (word_count == 3)
        {
            if (strcmp("START", label) == 0)
            {
                fprintf(obj, "%s %s %s ---\n", addr, label, mnemonic);
            }
            else
            {
                char opcodeOPTAB[20];
                if (searchOptab(label, opcodeOPTAB))
                {
                    // operation found in optab
                    char addrSYMTAB[30];
                    if (searchSymtab(mnemonic, addrSYMTAB))
                    {
                        // label exists in SYMTAB
                        fprintf(obj, "%s %s %s %s%s\n", addr, label, mnemonic, opcodeOPTAB, addrSYMTAB);
                    }
                    else
                    {
                        char *tokensSYMTAB = strtok(mnemonic, ",\n"); // to handle operations like ADD TABLE,X
                        int flag = 0;
                        while (tokensSYMTAB)
                        {
                            if (searchSymtab(tokensSYMTAB, addrSYMTAB))
                            {
                                fprintf(obj, "%s %s %s %s%s\n", addr, label, mnemonic, opcodeOPTAB, addrSYMTAB);
                                flag = 1;
                                break;
                            }
                            tokensSYMTAB = strtok(NULL, " \n");
                        }
                        if (flag)
                            printf("\nERROR at line %d , NO SYMBOL FOUND\n", lines);
                    }
                }
                else
                    printf("\nERROR at line %d , OPCODE ( %s ) NOT FOUND\n", lines, label);
            }
        }
        else if (word_count == 4)
        {
            if (strcmp("START", mnemonic) == 0)
            {
                fprintf(obj, "%s %s %s %s ---\n", addr, label, mnemonic, oprand);
            }
            else
            {
                char addressSYMTAB[20], opcodeOPTAB[20];
                if (strcmp("WORD", mnemonic) == 0 || strcmp("BYTE", mnemonic) == 0)
                {
                    if (searchSymtab(label, addressSYMTAB))
                    {
                        if (strcmp("BYTE", mnemonic) == 0)
                        {
                            fprintf(obj, "%s %s %s %s 000001\n", addr, label, mnemonic, oprand);
                        }
                        else
                        {
                            searchOptab(mnemonic, opcodeOPTAB);
                            fprintf(obj, "%s %s %s %s %s", addr, label, mnemonic, oprand, opcodeOPTAB);
                            int len_oprand = strlen(oprand);
                            for (int i = 1; i <= 6 - len_oprand; i++)
                                fprintf(obj, "0");
                            fprintf(obj, "%s\n", oprand);
                        }
                    }
                }
                else
                {
                    if (searchSymtab(label, addressSYMTAB))
                    {
                        if (searchOptab(mnemonic, opcodeOPTAB))
                        {
                            fprintf(obj, "%s %s %s %s ---\n", addr, label, mnemonic, oprand);
                        }
                        else
                        {
                            fprintf(obj, "%s %s %s %s ---\n", addr, label, mnemonic, oprand);
                        }
                    }
                    else
                        printf("\nERROR in line %d , LABEL NOT FOUND\n", lines);
                }
            }
        }
        else
        {
            char opcodeOPTAB[20];
            if (searchOptab(label, opcodeOPTAB))
            {
                if(strcmp(label,"END")==0)
                fprintf(obj, "%s %s ---\n", addr, label);
                else
                fprintf(obj, "%s %s %s0000\n", addr, label, opcodeOPTAB);
            }
            else
                printf("\nERROR at line %d , OPCODE ( %s ) NOT FOUND\n", lines, label);
        }
        lines++;
    }
    fclose(pass1);
    fclose(obj);
    generate();
}


int main()
{
    pass2_objcode();
}
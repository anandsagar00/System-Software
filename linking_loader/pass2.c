#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ll long long int

typedef struct obj_code
{
    // this structure will store each line of the input file , so that we can re-iterate and change as per M record
    char buffer[1000];
} obj_code;

typedef struct ESTAB
{
    char name[30];
    char addr[30];
    char len[30];
} ESTAB;

int search_estab(ESTAB estab[], ll len, char symbol[])
{
    for (int i = 0; i < len; i++)
    {
        if (strcasecmp(symbol, estab[i].name) == 0)
            return i;
    }
    return -1;
}

ll hex_to_int(char hex[])
{
    ll ans = 0;
    for (int i = 0; hex[i] != '\0' && hex[i] != '\n'; i++)
    {
        if (hex[i] >= '0' && hex[i] <= '9')
            ans = (ans * 16) + (hex[i] - '0');
        else if (hex[i] == 'A' || hex[i] == 'a')
            ans = (ans * 16) + 10;
        else if (hex[i] == 'B' || hex[i] == 'b')
            ans = (ans * 16) + 11;
        else if (hex[i] == 'C' || hex[i] == 'c')
            ans = (ans * 16) + 12;
        else if (hex[i] == 'D' || hex[i] == 'd')
            ans = (ans * 16) + 13;
        else if (hex[i] == 'E' || hex[i] == 'e')
            ans = (ans * 16) + 14;
        else if (hex[i] == 'F' || hex[i] == 'f')
            ans = (ans * 16) + 15;
    }
    return ans;
}

int main()
{
    FILE *estab_file = fopen("estab.txt", "r");
    FILE *input = fopen("input.txt", "r");

    ll PROGADDR = 0, CSADDR = 0, CSLENGTH = 0;

    printf("ENTER PROG ADDR : ");
    scanf("%lld", &PROGADDR);
    CSADDR = PROGADDR;

    ESTAB external_symbol_table[100];
    ll current_estab_len = 0;

    obj_code object_code_table[20];
    ll object_table_len = 0;

    // reading estab in the array

    while (!feof(estab_file))
    {
        char buffer[1000];
        fgets(buffer, 1000, estab_file);
        char *tokens = strtok(buffer, " \n");

        int word_count = 0; // there may be 2 or 3 words in estab , (3 in case of progname,startAddr,len) ,(2 when lista,addr)
        while (tokens)
        {
            if (word_count == 0) // pointing to name
                strcpy(external_symbol_table[current_estab_len].name, tokens);
            else if (word_count == 1) // pointing to address
                strcpy(external_symbol_table[current_estab_len].addr, tokens);
            else // pointing to prog_length
                strcpy(external_symbol_table[current_estab_len].len, tokens);

            tokens = strtok(NULL, " \n");
            word_count++;
        }
        if (word_count == 2)
            strcpy(external_symbol_table[current_estab_len].len, "*");
        current_estab_len++;
    }

    fclose(estab_file);

    int lines = 1;

    while (!feof(input))
    {
        char buffer[1000];
        fgets(buffer, 1000, input);

        if (buffer[0] == 'H' || buffer[0] == 'h')
        {
            // setting the CSADDR and CSLENGTH
            // H^prog_name^start_addr^len
            char *tokens = strtok(buffer, "^ \n"); // pointing to H
            tokens = strtok(NULL, "^ \n");         // pointing to prog name
            tokens = strtok(NULL, "^ \n");         // pointing to Start Addr
            CSADDR = CSADDR + hex_to_int(tokens) + CSLENGTH;
            tokens = strtok(NULL, "^ \n"); // pointing to prog length
            CSLENGTH = hex_to_int(tokens);
        }
        else if (buffer[0] == 'T' || buffer[0] == 't')
        {
            // text record
            strcpy(object_code_table[object_table_len].buffer, buffer); // copied line in our DS , for later refrence
            object_table_len++;

            // T^start_addr^length^ob1^ob2^...

            ll start_addr_old;
            ll start_addr = 0;
            char *tokens = strtok(buffer, "^ \n"); // now pointing to T
            tokens = strtok(NULL, "^ \n");         // pointing to start addr

            start_addr_old = hex_to_int(tokens);
            start_addr = CSADDR + hex_to_int(tokens);

            tokens = strtok(NULL, "^ \n"); // pointing to length of line
            tokens = strtok(NULL, "^ \n"); // pointing to first object code

            ll i = 0;
            while (tokens)
            {
                printf("Moved object code (%s) from [%llX] to [%llX]\n", tokens, start_addr_old + i, start_addr + i);
                i += 3;
                tokens = strtok(NULL, "^ \n");
            }
        }
        else if (buffer[0] == 'M' || buffer[0] == 'm')
        {
            char *tokens = strtok(buffer, "^ \n");
            tokens = strtok(NULL, "^ \n"); // now pointing to start addr
            
            ll start_addr_M = hex_to_int(tokens);

            tokens = strtok(NULL, "^ \n"); // now pointing to length of modification
            tokens = strtok(NULL, "^ \n"); // now pointing to +lista

            ll sign = 1;

            if (tokens[0] == '-')
                sign = -1;
            char symbol[20];
            int a=1;
            strcpy(symbol,tokens);
            //copying symbol without sign
            for(int a=0;symbol[a]!='\0';a++)
            symbol[a]=symbol[a+1];

            int search_index = search_estab(external_symbol_table, current_estab_len, symbol);

            if (search_index == -1)
                printf("ERROR at line %d no symbol found\n", lines);
            else
            {
                ll value_to_be_added = sign * hex_to_int(external_symbol_table[search_index].addr);

                for (int i = 0; i < object_table_len; i++)
                {
                    // T^start^len^ob1^ob2....
                    char my_line[1000];
                    strcpy(my_line,object_code_table[i].buffer);
                    printf("\nline : %s\n",my_line);
                    char *tokens_t = strtok(my_line, "^ \n"); // pointing to T
                    tokens_t = strtok(NULL, "^ \n");                              // pointing to start addr

                    ll start_addr_T = hex_to_int(tokens_t);
                    int flag_M=0;
                    if (start_addr_M - start_addr_T < 30)
                    {

                        tokens_t = strtok(NULL, "^ \n"); // pointing to len now
                        tokens_t = strtok(NULL, "^ \n"); // pointing to first object code
                        int j = 0;
                        while (tokens_t)
                        {
                            if ((start_addr_M - (start_addr_T + j)) < 3)
                            {
                                printf("Modified (%s) at [%llX] to (%llX)\n", tokens_t, start_addr_T + j, hex_to_int(tokens_t) + value_to_be_added);
                                flag_M=1;
                                break;
                            }
                            j += 3;
                            tokens_t = strtok(NULL, "^ \n");
                        }
                    }
                    if(flag_M)
                    break;
                }
            }
        }
        lines++;
    }
}
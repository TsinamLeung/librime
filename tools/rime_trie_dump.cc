#include <rime_api.h>
#include <rime/dict/table.h>
#include <iostream>
#include <regex>

void print_help() {
    std::cerr << "Options: \n\n" << std::endl;
}
void print_utf8_char(char* ch)
{
    // https://stackoverflow.com/a/44399943/14195426
    for (char *s = ch; *s; ) 
    {
        char u[5];
        char *p = u;
        *p++ = *s++;
        if ((*s & 0xC0) == 0x80) *p++ = *s++;
        if ((*s & 0xC0) == 0x80) *p++ = *s++;
        if ((*s & 0xC0) == 0x80) *p++ = *s++;
        *p = 0; 
        printf("%s", u);
    }
}
void print_entry(rime::TableAccessor& accessor,rime::Table& table)
{
    while(!accessor.exhausted())
    {
        const rime::Code& code = accessor.code();
        double credibility = accessor.credibility();
        const rime::table::Entry* entry = accessor.entry();
        
        std::string codeOutput = "";
        for(int syllableID : code) 
        {
            codeOutput += std::to_string(syllableID) + std::string(" ");
        }
        
        std::cout << "Code: " << codeOutput << std::endl;
        std::cout << "Text: " << table.GetEntryText(*entry) << std::endl;
        std::cout << "Weight: " << entry->weight << std::endl;
        std::cout << "Credibility: " << credibility << std::endl;
        std::cout <<"\n---" << std::endl; 
        accessor.Next();
    }
    std::cout << "Accessor Exhausted" << std::endl;
}

void query_process(rime::Table& table)
{
    while(true)
    {
        int syllableId = -1;
        std::cin >> syllableId;
        if(syllableId < 0)
        {
            std::cerr << "[Invalid input]" << std::endl;
            return;
        }
        std::cout << "Corresponding syllable: " << table.GetSyllableById(syllableId) << std::endl;;
        auto accessor = table.QueryWords(syllableId);
        print_entry(accessor,table);
    }
}

int main(int argc,char* argv[])
{
    std::ios_base::sync_with_stdio(false);
    if (argc == 1)
    {
        return 1;
    } else if(argc == 2){
        const char* trieName = argv[argc - 1];
        rime::Table table(trieName);
        if(!table.Load())
        {
            std::cerr << "Load Failed! Please check your path" << std::endl;
            return 3;
        }
        query_process(table);
    } else {
        std::cerr << 
        "[Invalid Arguments]Does not accepts mroe than 1 aruguments \n\n"
        "Try --help for more information" 
        << std::endl;
        return 2;
    }
    return 0;
}

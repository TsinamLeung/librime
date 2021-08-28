#include <rime_api.h>
#include <rime/dict/table.h>
#include <rime/dict/prism.h>
#include <rime/dict/reverse_lookup_dictionary.h>

#include <iostream>
#include <regex>
#include <vector>

#define MAX_LINE 120
#define SAFE_WORD "{EXIT}"
void print_help()
{
    std::cerr << "Options: \n\n"
              << std::endl;
}
void print_utf8_char(char *ch)
{
    // https://stackoverflow.com/a/44399943/14195426
    for (char *s = ch; *s;)
    {
        char u[5];
        char *p = u;
        *p++ = *s++;
        if ((*s & 0xC0) == 0x80)
            *p++ = *s++;
        if ((*s & 0xC0) == 0x80)
            *p++ = *s++;
        if ((*s & 0xC0) == 0x80)
            *p++ = *s++;
        *p = 0;
        printf("%s", u);
    }
}
void print_entry(rime::TableAccessor &accessor, rime::Table &table)
{
    while (!accessor.exhausted())
    {
        const rime::Code &code = accessor.code();
        double credibility = accessor.credibility();
        const rime::table::Entry *entry = accessor.entry();

        std::string codeOutput = "";
        for (int syllableID : code)
        {
            codeOutput += std::to_string(syllableID) + std::string(" ");
        }

        std::cout << "Code: " << codeOutput << std::endl;
        std::cout << "Text: " << table.GetEntryText(*entry) << std::endl;
        std::cout << "Weight: " << entry->weight << std::endl;
        std::cout << "Credibility: " << credibility << std::endl;
        std::cout << "\n---" << std::endl;
        accessor.Next();
    }
    std::cout << "Accessor Exhausted" << std::endl;
}

void query_process(rime::Table &table)
{
    while (true)
    {
        int syllableId = -1;
        std::cin >> syllableId;
        if (syllableId < 0)
        {
            std::cerr << "[Invalid input]" << std::endl;
            return;
        }
        std::cout << "Corresponding syllable: " << table.GetSyllableById(syllableId) << std::endl;
        ;
        auto accessor = table.QueryWords(syllableId);
        print_entry(accessor, table);
    }
}

void query_process(rime::Prism &prism)
{
    std::cout << "Please select search mode for prism:\n"
                 "[C] for Common Prefix search\n"
                 "[E] for Expand search\n"
                 "[X] for Exact search"
              << std::endl;
    std::cout << "type " << SAFE_WORD << " to exit";

    std::string line;
    std::getline(std::cin, line);
    auto findMode = [line](std::string mode)
    {
        return line.rfind(mode) != std::string::npos;
    };
    int mode = -1;
    auto outputMode = [mode](std::string description)
    {
        std::cout << "Selected Mode : " << description << std::endl;
    };
    if (findMode("C"))
    {
        mode = 0;
        outputMode("Common Prefix Search");
    }
    else if (findMode("E"))
    {
        mode = 1;
        outputMode("Expand search");
    }
    else if (findMode("X"))
    {
        mode = 2;
        outputMode("Exact Mode");
    }
    else
    {
        std::cerr << "[Undefined Search Mode]" << std::endl;
        return;
    }

    while (true)
    {
        std::getline(std::cin, line);
        if (line.rfind(SAFE_WORD) != std::string::npos)
        {
            break;
        }
        if (mode == 0 || mode == 1)
        {
            std::vector<rime::Prism::Match> match;
            if (mode == 0)
            {
                prism.CommonPrefixSearch(line, &match);
            }
            else if (mode == 1)
            {
                prism.ExpandSearch(line, &match, -1);
            }
            for (rime::Prism::Match &eachMatch : match)
            {
                auto syllableAccessor = prism.QuerySpelling(eachMatch.value);
                std::cout << "Prism ID: " << eachMatch.value
                          << " Syllable ID: "
                          << syllableAccessor.syllable_id() << std::endl;
            }
            std::cout << "\nSearch Exhausted\n"
                      << std::endl;
        }
        else if (mode == 2)
        {
            int syllableID;
            if (prism.GetValue(line, &syllableID))
            {
                std::cout << "Syllable ID : " << syllableID << std::endl;
            }
            else
            {
                std::cerr << "Could not find corresponding syllable ID" << std::endl;
            }
            std::cout << "\nSearch Exhausted\n"
                      << std::endl;
        }
    }
}

void query_process(rime::ReverseDb &reverse)
{
    std::string line;
    std::cout << "type " << SAFE_WORD << " to exit" << std::endl;
    while (true)
    {
        std::getline(std::cin, line);
        if (line.rfind(SAFE_WORD) != std::string::npos)
        {
            return;
        }
        std::string res;
        if (reverse.Lookup(line, &res))
        {
            std::cout << res << std::endl;
        }
        else
        {
            std::cerr << "Cannot Found \"" << line << "\""
                      << "\nline length with " << line.length()
                      << std::endl;
        }
    }
}
int detectExtension(std::string inp)
{
    auto findExtension = [inp](std::string extension)
    {
        return inp.rfind(extension) != std::string::npos;
    };
    if (findExtension(".table.bin"))
    {
        return 0;
    }
    else if (findExtension(".prism.bin"))
    {
        return 1;
    }
    else if (findExtension(".reverse.bin"))
    {
        return 2;
    }
    return -1;
}
int main(int argc, char *argv[])
{
    std::ios_base::sync_with_stdio(false);
    if (argc == 1)
    {
        return 1;
    }
    else if (argc == 2)
    {
        const char *trieName = argv[argc - 1];
        int r = detectExtension(trieName);
        if (r == 0)
        {
            rime::Table table(trieName);
            if (!table.Load())
            {
                std::cerr << "Load Failed! Please check your file path " << trieName << std::endl;
                return 3;
            }
            query_process(table);
        }
        else if (r == 1)
        {
            rime::Prism prism(trieName);
            if (!prism.Load())
            {
                std::cerr << "Load Failed! Please check your file path " << trieName << std::endl;
                return 4;
            }
            query_process(prism);
        }
        else if (r == 2)
        {
            rime::ReverseDb reverseDB(trieName);
            if (!reverseDB.Load())
            {
                std::cerr << "Load Failed! Please check your file path " << trieName << std::endl;
                return 5;
            }
            query_process(reverseDB);
        }
        else
        {
            std::cerr << "[Unsupported file]" << std::endl;
        }
    }
    else
    {
        std::cerr << "[Invalid Arguments]Does not accepts mroe than 1 aruguments \n\n"
                     "Try --help for more information"
                  << std::endl;
        return 2;
    }
    return 0;
}

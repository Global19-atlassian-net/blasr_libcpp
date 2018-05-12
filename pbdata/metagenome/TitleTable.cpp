#include <pbdata/metagenome/TitleTable.hpp>

TitleTable::TitleTable()
{
    table = NULL;
    tableLength = 0;
}

TitleTable::~TitleTable() { Free(); }

void TitleTable::Copy(char **src, int nSrc)
{
    Free();  //Free before copy
    table = ProtectedNew<char *>(nSrc);
    tableLength = nSrc;
    int i;
    for (i = 0; i < nSrc; i++) {
        int lenStrI = strlen(src[i]);
        table[i] = ProtectedNew<char>(lenStrI + 1);
        memcpy(table[i], src[i], lenStrI);
        table[i][lenStrI] = '\0';
    }
}

void TitleTable::Write(std::string &name)
{
    std::ofstream out;
    CrucialOpen(name, out, std::ios::out);
    Write(out);
}

void TitleTable::Write(std::ofstream &out)
{
    int i;
    for (i = 0; i < tableLength; i++) {
        out << table[i] << std::endl;
    }
}

void TitleTable::Read(std::string &inFileName)
{
    std::ifstream in;
    CrucialOpen(inFileName, in, std::ios::in);
    Read(in);
}

void TitleTable::CopyFromVector(std::vector<std::string> &titles)
{
    Free();  //Free before copy.
    tableLength = titles.size();
    table = ProtectedNew<char *>(tableLength);
    int i;
    for (i = 0; i < tableLength; i++) {
        table[i] = ProtectedNew<char>(titles[i].size() + 1);
        memcpy(table[i], titles[i].c_str(), titles[i].size());
        table[i][titles[i].size()] = '\0';
    }
}

void TitleTable::Read(std::ifstream &in)
{
    std::vector<std::string> titles;
    while (in) {
        char title[1024];
        in.getline(title, 1024);
        if (not std::string(title).empty()) titles.push_back(title);
    }
    if (titles.size() > 0) {
        CopyFromVector(titles);
    } else {
        tableLength = 0;
        table = NULL;
    }
}

void TitleTable::Free()
{
    int i;
    for (i = 0; i < tableLength; i++) {
        if (table[i]) {
            delete[] table[i];
            table[i] = NULL;
        }
    }
    if (table) {
        delete[] table;
    };
    table = NULL;
    tableLength = 0;
}

bool TitleTable::Lookup(std::string title, int &index)
{
    int i;
    for (i = 0; i < tableLength; i++) {
        if (table[i] == title) {
            index = i;
            return true;
        }
    }
    return false;
}

void TitleTable::ResetTableToIntegers(char **table, int *tableLengths, int nTable)
{

    int i;
    for (i = 0; i < nTable; i++) {
        delete[] table[i];
        std::stringstream namestrm;
        namestrm << i;
        std::string name;
        name = namestrm.str();
        table[i] = ProtectedNew<char>(name.size() + 1);
        memcpy(table[i], name.c_str(), name.size());
        table[i][name.size()] = '\0';
        tableLengths[i] = (int)name.size() + 1;
    }
}

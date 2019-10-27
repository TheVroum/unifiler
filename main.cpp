#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <array>
#include <cassert>
#include <QDirIterator>
#include <QString>
#include <QDebug>





using namespace std;


void usage()
{
    cout << "Hello World!" << endl;
}

bool compareExt(string toCheck, vector <string> toFind)
{
    for(const auto &a : toFind)
    {
        if(toCheck.size() < a.size())
            continue;
        if(a == QString::fromStdString(toCheck).right(a.size()).toStdString())
            return true;
    }
    return false;
}


int main(int argc, char **argv)
{
    const size_t paramTypeNumber = 2;
    enum paramType
    {
        targetHeaderExtension,
        targetBodyExtension
    };

    vector <string> targetFolders;
    array <vector <string>, paramTypeNumber> params;

    vector <string> args(argv, argv + argc);
    string outFileName = args[0] + ".out";
    if(args.size() <= 1)
    {
        usage();
    }
    assert(args.size() > 1);//donne au moins le dossier
    assert(args[args.size() - 1].size());//le dernier parametre n'est pas vide
    assert(args[args.size() - 1].front() != '-' && args[args.size() - 1].front() != '/');//le dernier parametre n'est pas une commande

    args.erase(args.begin());

    for(size_t i = 0; i < args.size(); ++i)
        if(args[i].front() == '-'  || args[i].front() == '/')
        {
            assert(args[i + 1/*pas out of range car testé dans les assertions précédentes.*/].front() != '-'
                    && args[i + 1].front() != '/');
            assert(args[i].size() > 1);
            if(args[i][1] == 'h')
                params[targetHeaderExtension].push_back(args[i + 1]);
            else
                params[targetBodyExtension].push_back(args[i + 1]);
        }

    if(params[targetHeaderExtension].empty() && params[targetBodyExtension].empty())
    {
        params[targetHeaderExtension].push_back(".h");
        params[targetHeaderExtension].push_back(".hpp");
        params[targetHeaderExtension].push_back(".h++");
        params[targetHeaderExtension].push_back(".hxx");

        params[targetBodyExtension].push_back(".c");
        params[targetBodyExtension].push_back(".cpp");
        params[targetBodyExtension].push_back(".c++");
        params[targetBodyExtension].push_back(".cxx");
        params[targetBodyExtension].push_back(".ino");
    }

    for(size_t i = args.size() - 1; i > 0; --i)//vérifier si c'est >0 ou >1
        if(args[i - 1].front() == '-'  || args[i].front() == '/')
            args.erase(args.begin() + (i - 1), args.begin() + i), i--;
    assert(args.size());
    args.swap(targetFolders);
    string outputFileHeaders, outputFileBody;

    for(auto a : targetFolders)
    {
        QDirIterator it(QString::fromStdString(a), QDirIterator::Subdirectories);
        while( it.hasNext())
        {
            string buff = it.next().toStdString();
            qDebug() << QString::fromStdString(buff);
            ifstream ifs(buff);
            bool bod = compareExt(buff, params[targetBodyExtension]);
            bool head = compareExt(buff, params[targetHeaderExtension]);
            assert(!(head && bod));
            if(!(head || bod))
                continue;
            auto &perfRef = (head ? outputFileHeaders : outputFileBody);
            for(char ch; ifs.get(ch);)
                perfRef.push_back(ch);
            perfRef.push_back('\n');
        }
    }
    outputFileHeaders += outputFileBody;
    ofstream ofs(outFileName);
    ofs.write(outputFileHeaders.c_str(), outputFileHeaders.size());
    ofs.close();
    return 0;
}

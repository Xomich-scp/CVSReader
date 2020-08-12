#ifndef CSVREADER_H
#define CSVREADER_H
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <iterator>
#include <string>
#include <thread>
#include <mutex>
#include <algorithm>
#include <regex>

#include "OrGraph.h"
using namespace std;

/*
 * A class to read data from a csv file.
 */

typedef unsigned long long CSVIndex;

class CSVReader
{
    std::string fileName;
    std::string delimeter;
public:
    CSVReader(std::string filename, std::string delm = ",") :
            fileName(filename), delimeter(delm)
    {
        dataList = new std::vector<std::vector<std::string> >();
        nodesWithFormula = new vector<pair<CSVIndex,CSVIndex>>;

    }
    ~CSVReader()
    {
        for(CSVIndex i=0; i<nodesWithFormula->size(); i++)
        {
            delete []matrixAround[i];
        }
        delete [] matrixAround;
        delete nodesWithFormula;
        delete dataList;
        delete tableRowNames;
        delete tableColumnNames;
    }
    // Function to fetch data from a CSV File
    std::vector<std::vector<std::string> >* getData();


private:

    mutex operationWithMatrix;

    map <string, CSVIndex> *tableColumnNames;
    map <string, CSVIndex> *tableRowNames;
    vector<pair<CSVIndex,CSVIndex>> *nodesWithFormula;
    std::vector<std::vector<std::string> > *dataList;



    regex cellFormat=regex("(^([0-9]+)|(=.+(\\+|\\-|\\*|\\/).+)|(,)).*");
    char **matrixAround;

    void getColumnNamesThread();
    void getRowNamesThread();
    void readFile();
    void parseHeader();

    void getCoordinateFromCellName(string cellName, pair<CSVIndex,CSVIndex> &result);
    pair<pair<CSVIndex,CSVIndex>,pair<CSVIndex,CSVIndex>> getChildNodes(string* value);
    void countStock(CSVIndex i);

    void buildOrGraph();
    void countNodesWithFormula();
    void simpleCount(CSVIndex& i);


};


#endif // CSVREADER_H

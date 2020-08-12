#include "CSVReader.h"



/*
* Parses through csv file line by line and returns the data
* in vector of vector of strings.
*/


std::vector<std::vector<std::string> >* CSVReader::getData()
{

    readFile();

    parseHeader();

    buildOrGraph();

    countNodesWithFormula();


    return dataList;
}

void CSVReader::readFile()
{
    ifstream file(fileName);

    if(!file)
    {
        cout<<"File dont exist\n";
        exit(-1);
    }




    std::string line = "";


    // Iterate through each line and split the content using delimeter
    unsigned long long i=0;

    while (getline(file, line))
    {
        std::vector<std::string> vec;
        unsigned long long j=0;
        while(1)
        {



             if(i!=0 && j!=0 && !regex_match(line, cellFormat))
             {
                 cout<<"Wrong format at (row,col) ("<<i<<", "<<j<<") String: "<<line<<endl;
                 exit(-1);
             }

            if(line.substr(0,1)=="=")
            {
                nodesWithFormula->push_back(make_pair(i,j));
            }
            if(line.find(delimeter)!=string::basic_string::npos)
            {
                vec.push_back(line.substr(0,line.find(delimeter)));

                line.erase(0,line.find(delimeter)+1);
            }
            else
            {
                vec.push_back(line);

                break;
            }
            j++;

        }
        if(i!=0)
        {
            while (vec.size()!=dataList->begin()->size())
            {
                vec.push_back(" ");
            }
        }
        dataList->push_back(vec);
        i++;

    }
    // Close the File
    file.close();
}


void CSVReader::parseHeader()
{

    thread thr1 (&CSVReader::getColumnNamesThread, this);
    thread thr2 (&CSVReader::getRowNamesThread, this);


    thr1.join();
    thr2.join();

    if(tableRowNames->size()!=dataList[0].size()-1)
    {
        cout<<"Error, you have row with same names"<<endl;
        exit(-2);
    }
    if(tableColumnNames->size()!=dataList[0][0].size()-1)
    {
        cout<<"Error, you have column with same names"<<endl;
        exit(-2);
    }
}





 void CSVReader::buildOrGraph()
 {
     matrixAround=(char**)calloc(nodesWithFormula->size(),sizeof(char*));
     for(CSVIndex i=0;i<nodesWithFormula->size(); i++)
     {
         matrixAround[i]=(char*)calloc(nodesWithFormula->size(),sizeof(char));
     }
     for(CSVIndex i=0;i<nodesWithFormula->size(); i++)
     {
         auto childNodes(getChildNodes(&dataList->at(nodesWithFormula->at(i).first).at(nodesWithFormula->at(i).second)));
         for(CSVIndex j=0;j<nodesWithFormula->size(); j++)
         {
             if (nodesWithFormula->at(j)==childNodes.first || nodesWithFormula->at(j)==childNodes.second)
             {

                 if(i==j || matrixAround[i][j]!=0 || matrixAround[j][i]!=0)
                 {
                     matrixAround[i][j]=-2;
                     matrixAround[j][i]=-2;
                     matrixAround[i][i]=-2;
                     //we find cicle. it means, that we cant count values, so we left it as it is.
                 }
                 else
                 {
                     matrixAround[i][j]++;//if in [i][j] we see 1, it means that [i][j] node use [j][i] for count.
                     matrixAround[j][i]--;//if in [i][j] we see -1, it means that [j][i] node use [i][j] for count.
                     matrixAround[i][i]++;//count, how many nodes we need to count this.
                 }

             }
         }

     }
 }

 void CSVReader::countNodesWithFormula()
 {
     vector<thread> launchedThreads;
     operationWithMatrix.lock();
     for(CSVIndex i=0;i<nodesWithFormula->size();i++)
     {
         if(matrixAround[i][i]==0)
         {
             launchedThreads.push_back(thread(&CSVReader::countStock,this,i));

         }
     }
     operationWithMatrix.unlock();
     for(auto it=launchedThreads.begin(); it!=launchedThreads.end(); it++)
     {
         it->join();
     }
 }



 void CSVReader::countStock(CSVIndex i)
 {

     if(matrixAround[i][i]==-2) return;

     operationWithMatrix.lock();

     if(matrixAround[i][i]==2)
     {
          matrixAround[i][i]--;operationWithMatrix.unlock();return;
     }
     matrixAround[i][i]=0;

     operationWithMatrix.unlock();

     simpleCount(i);
     for(CSVIndex j=0; j<nodesWithFormula->size();j++)
     {
         if (matrixAround[i][j]==-1 && i!=j) countStock(j);
     }

     return;
 }

 void CSVReader::simpleCount(CSVIndex& i)
 {
     string& dataListStr = dataList->at(nodesWithFormula->at(i).first).at(nodesWithFormula->at(i).second);
     auto childNodes(getChildNodes(&dataListStr));
     string  &argumentOne = dataList->at(childNodes.first.first).at(childNodes.first.second),
             &argumentTwo = dataList->at(childNodes.second.first).at(childNodes.second.second);

     regex regexp("\\+|\\-|\\*|\\/");
     smatch oper;

     regex_search(dataListStr, oper, regexp);
     if (argumentOne==" "||argumentTwo==" " ||
              argumentOne[0]=='='||argumentTwo[0]=='=') return;


     if (oper.str()=="+")
     {
         dataListStr=to_string( stoi(argumentOne) + stoi(argumentTwo)  );
         return;
     }


     if (oper.str()=="-")
     {
         if(stoi(argumentTwo)>stoi(argumentOne))
         {
             operationWithMatrix.lock();
             cout<<"Value of operation below zero at node ("<<nodesWithFormula->at(i).first<<", "<<nodesWithFormula->at(i).second<<") "<<dataList->at(0).at(i)+dataList->at(nodesWithFormula->at(i).first).at(0)<<endl;
                   //<<dataList->at(nodesWithFormula->at(i).first).at(0)<<endl;

             for(std::vector<std::string> vec : *dataList)
             {
                 auto i=vec.size();
                 for(std::string data : vec)
                 {
                     i--;
                     std::cout<<data;
                    if(i>0) cout<<",";
                 }

                 std::cout<<std::endl;
             }
             operationWithMatrix.unlock();
             exit(-1);
         }
         dataListStr=to_string( stoi(argumentOne) - stoi(argumentTwo)  );
         return;
     }

     if (oper.str()=="/")
     {

         if(stoi(argumentTwo) ==0)
         {
             operationWithMatrix.lock();
             cout<<"Division by zero at node ("<<nodesWithFormula->at(i).first<<", "<<nodesWithFormula->at(i).second<<") "<<dataList->at(0).at(i)+dataList->at(nodesWithFormula->at(i).first).at(0)<<endl;

             for(std::vector<std::string> vec : *dataList)
             {
                 auto i=vec.size();
                 for(std::string data : vec)
                 {
                     i--;
                     std::cout<<data;
                    if(i>0) cout<<",";
                 }

                 std::cout<<std::endl;
             }
             operationWithMatrix.unlock();
             exit(-1);
         }
         dataListStr=to_string( stoi(argumentOne) / stoi(argumentTwo)  );
         return;

     }

     if (oper.str()=="*")
     {
         dataListStr=to_string( stoi(argumentOne) * stoi(argumentTwo)  );
         return;
     }


 }

 void CSVReader::getColumnNamesThread()
 {
     tableColumnNames = new map<string, CSVIndex >;
     CSVIndex index=1;
     for (auto it=dataList->begin()->begin()+1; it!=dataList->begin()->end(); it++,index++)
     {

             tableColumnNames->insert(make_pair(*it,index));
     }

     return;
 }

 void CSVReader::getRowNamesThread()
 {
     tableRowNames = new map<string, CSVIndex>;
     CSVIndex index=1;
     for (auto it=dataList->begin()+1; it!=dataList->end(); it++,index++)
     {

             tableRowNames->insert(make_pair(*it->begin(),index));
     }
     return;
 }



 pair<pair<CSVIndex,CSVIndex>,pair<CSVIndex,CSVIndex>> CSVReader::getChildNodes(string* value)
 {

     regex regexp("\\+|\\-|\\*|\\/");
     smatch oper;
     regex_search(*value, oper, regexp);

     string cellOne=value->substr(1,value->find(oper.str())-1), cellTwo=value->substr(1+value->find(oper.str()));
     pair<CSVIndex,CSVIndex> nodeOne, nodeTwo;



     thread thr1 (&CSVReader::getCoordinateFromCellName,this,cellOne, ref(nodeOne));
     thread thr2 (&CSVReader::getCoordinateFromCellName,this,cellTwo, ref(nodeTwo));


     thr1.join();
     thr2.join();




     return make_pair(nodeOne,nodeTwo);

 }



  void CSVReader::getCoordinateFromCellName(string cellName, pair<CSVIndex,CSVIndex> &result)
 {

     for(auto itJ=tableColumnNames->begin(); itJ!=tableColumnNames->end(); itJ++)
     {
         if (regex_search(cellName,regex(itJ->first)))
         {
             string columnName = cellName.substr(itJ->first.length());
             for(auto itI=tableRowNames->begin(); itI!=tableRowNames->end(); itI++)
             {
                 if(regex_match(columnName, regex(itI->first)))
                 {
                     result=make_pair(itI->second, itJ->second);
                     return;
                 }
             }

         }
     }

     cout<<"Error, this node do not exist: "<<cellName<<endl;
     exit(-1);

 }


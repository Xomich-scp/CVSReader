#include <iostream>

#include "CSVReader.h"



int main(int argc, char* argv[])
{
    // Creating an object of CSVWriter
    if(argc<2)
    {
        cout<<"Need filename\n";
        return 0;
    }
    CSVReader reader(argv[1]);

    // Get the data from CSV File

    std::vector<std::vector<std::string> > *dataList = reader.getData();

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


    return 0;
}

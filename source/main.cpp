/** @file */

#include <string>
#include <iostream>
#include <vector>

#include "./experiments/exp-001.h"
#include "./experiments/exp-002.h"
#include "./experiments/exp-003.h"
#include "./experiments/exp-004.h"
#include "./experiments/exp-005.h"
#include "./experiments/exp-lab.h"

#include "./readers/TT.h"
#include "./readers/reader-complete.h"

ksi::dataset create_dataset(int num_data, int num_attributes) {
    ksi::dataset ds;
    for (int i = 0; i < num_data; ++i) {
        std::vector<double> attributes(num_attributes, i);
        ds.addDatum(ksi::datum(attributes));
    }
    return ds;
}

void test_TT() {
    std::cout << "Testing TT class methods" << std::endl;
    
    ksi::dataset ds = create_dataset(10, 3);
    
    ksi::reader_complete DataReader;
    ksi::TT tt(DataReader);
    
    std::cout << "Testing split method" << std::endl;
    tt.split(ds, 5);
    
    std::cout << "Testing save method" << std::endl;
    tt.save("./test_datasets/test");
    
    std::cout << "Testing read_file method" << std::endl;
    ksi::dataset read_ds = tt.read_file("./test_datasets/ds.data");
    
    std::cout << "Testing read_directory method" << std::endl;
    tt.read_directory("./test_datasets/test");
}

 
int main (int argc, char ** params)
{
    if (argc == 1)
    {
        std::cout << "No experiment chosen." << std::endl;
        std::cout << "Provide number of an experiment as an only parameter, eg." << std::endl;
        std::cout << params[0] << " 2" << std::endl;
        std::cout << "to run the 2nd experiment." << std::endl;
    }   
    else 
    {
        int number = atoi(params[1]);
        try
        {
            switch(number)
            {
                case 1: { 
                    ksi::exp_001 experiment;
                    experiment.execute();
                    break;
                }
                case 2: { 
                    ksi::exp_002 experiment;
                    experiment.execute();
                    break;
                }
                case 3: { 
                    ksi::exp_003 experiment;
                    experiment.execute();
                    break;
                }
                case 4: { 
                    ksi::exp_004 experiment;
                    experiment.execute();
                    break;
                }
                case 5: { 
                    ksi::exp_005 experiment;
                    experiment.execute();
                    break;
                }
                case 0: { 
                    ksi::exp_lab experiment;
                    experiment.execute();
                    break;
                }
				case 8:{
                    test_TT();
	                }
                default: {
                    std::cout << "unknown experiment" << std::endl;
                }
            }
        }
        catch (std::exception & w)
        {
            std::cout << w.what() << std::endl;
        }
        catch (std::string & w)
        {
            std::cout << w << std::endl;
        }
        catch (...)
        {
            std::cout << "unknown exception" << std::endl;
        }
    } 
   return 0;
} 



/** @file */

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

#include "./experiments/exp-001.h"
#include "./experiments/exp-002.h"
#include "./experiments/exp-003.h"
#include "./experiments/exp-004.h"
#include "./experiments/exp-005.h"
#include "./experiments/exp-lab.h"

#include "./readers/TT.h"
#include "./readers/TVT.h"
#include "./readers/reader-complete.h"
#include "./readers/reader-incomplete.h"

#include "./common/data-modifier-normaliser.h"

ksi::dataset create_dataset(int num_data, int num_attributes) 
{
    ksi::dataset ds;
    for (int i = 0; i < num_data; ++i) {
        std::vector<double> values(num_attributes, i);
        ds.addDatum(ksi::datum(values));
    }
    return ds;
}

void test_TT() 
{
    std::cout << "Testing TT class methods" << std::endl;
    
    ksi::dataset ds = create_dataset(10, 3);
    
    ksi::reader_complete DataReader;
    ksi::TT tt(DataReader);
    
    std::cout << "Testing split method" << std::endl;
    tt.split(ds, 5);
    
    std::cout << "Testing save method" << std::endl;
    tt.save("./test");
    
    std::cout << "Testing read_file method" << std::endl;
    //ksi::dataset read_ds = tt.read_file("./ds.data");
    std::string file_name {"../data/exp-lab/train.txt"};
    ksi::dataset read_ds = tt.read_file(file_name);
    tt.split(read_ds, 120);
    tt.save("./test");
    
    std::cout << "Testing read_directory method" << std::endl;
    tt.read_directory("./test");
    
    {
        std::cout << "==========================" << std::endl;
        
        std::string file_name {"../data/exp-lab/train.txt"};
        tt.read_file(file_name);
        
        /** @todo Czy to na pewno dobrze działa?
         */ 
        int licznik = 1;
        for (const auto & [train, test] : tt)
        {
            std::cout << "--------------------------" << std::endl;
            
            debug(licznik);
            debug(train.size());
            debug(train);
            debug(test.size());
            debug(test);
            ++licznik;
        }
        
    }
    // return;
    {
        std::cout << "==========================" << std::endl;
        
        std::string file_name {"../data/exp-lab/train.txt"};
        
        /// @todo Taka składnia nie działa, a fajnie by było, gdyby także działała.
        // tt.read_file(file_name);
        // tt.split(10); 
        
        auto d = tt.read_file(file_name);
        tt.split(d, 10); 
        
        int licznik = 1;
        for (const auto & [train, test] : tt)
        {
            std::cout << "--------------------------" << std::endl;
            
            debug(licznik);
            debug(train.size());
            debug(train);
            debug(test.size());
            debug(test);
            ++licznik;
        }
    }
    // return;
    {
        std::cout << "==========================" << std::endl;
        
        std::string file_name {"../data/exp-lab/train.txt"};
        
        ksi::data_modifier_normaliser normer;
        
        auto d = tt.read_file(file_name);
        tt.split(d, 10); 
        
        int licznik = 1;
        for (auto [train, test] : tt)
        {
            std::cout << "--------------------------" << std::endl;
            
            normer.modify(train);
            debug(licznik);
            debug(train.size());
            debug(train);
            debug(test.size());
            debug(test);
            ++licznik;
        }
    }
    
    // return ;
    {
        std::cout << "==========================" << std::endl;
        
        std::string file_name {"../data/exp-lab/train.txt"};
        
        ksi::reader_incomplete czytnik; 
        ksi::TT tt (czytnik);
        auto d = tt.read_file(file_name);
        tt.split(d, 15); 
        
        int licznik = 1;
        for (auto [train, test] : tt)
        {
            std::cout << "--------------------------" << std::endl;
            
            debug(licznik);
            debug(train.size());
            debug(train);
            debug(test.size());
            debug(test);
            ++licznik;
        }
    }
    
    // return ;
    {
        std::cout << "==========================" << std::endl;
        
        std::string file_name {"../data/exp-lab/train.txt"};
        
        ksi::reader_incomplete czytnik; 
        ksi::TT tt (czytnik);
        auto d = tt.read_file(file_name);
        tt.split(d, 15); 
        
        
        const std::filesystem::path folder {"./do_usuniecia"};
        std::filesystem::remove_all (folder);
        
        tt.save (folder);
        
        tt.read_directory(folder);
        
        int licznik = 1;
        for (auto [train, test] : tt)
        {
            std::cout << "--------------------------" << std::endl;
            
            debug(licznik);
            debug(train.size());
            debug(train);
            debug(test.size());
            debug(test);
            ++licznik;
        }
    }
    
    
    
    return;
    {
        std::cout << "==========================" << std::endl;
        
        std::string file_name {"../data/exp-lab/train.txt"};
        
        auto d = tt.read_file(file_name);
        tt.split(d, 0); 
       
        /** @todo Pojawia się komunikat:
         *        make: *** [makefile:50: 8] Błąd w obliczeniach zmiennoprzecinkowych (zrzut pamięci)
         */
        
        int licznik = 1;
        for (auto [train, test] : tt)
        {
            std::cout << "--------------------------" << std::endl;
            
            debug(licznik);
            debug(train.size());
            debug(train);
            debug(test.size());
            debug(test);
            ++licznik;
        }
    }
    
    
    return;
    {
        std::cout << "==========================" << std::endl;
        
        /** @todo Tu jest jakiś problem. Pojawia się komunikat:
         *        make: *** [makefile:50: 8] Błąd w obliczeniach zmiennoprzecinkowych (zrzut pamięci)
         */
        std::string file_name {"../data/exp-lab/train.txt"};
        
        ksi::reader_complete czytnik;
        ksi::TVT tvt (czytnik);
        auto d = tvt.read_file(file_name);
        tt.split(d, 10); 
        
        int licznik = 1;
        for (const auto & [train, valid, test] : tvt)
        {
            std::cout << "--------------------------" << std::endl;
            
            debug(licznik);
            debug(train.size());
            debug(train);
            debug(valid.size());
            debug(valid);
            debug(test.size());
            debug(test);
            ++licznik;
        }
    }
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
               case 8: {
                          test_TT();
                          break;
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



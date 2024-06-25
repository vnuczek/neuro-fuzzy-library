/** @file */

#include <string>
#include <iostream>
#include <filesystem>

#include "./readers/train_test_model.h"
#include "./readers/train_validation_test_model.h"
#include "./readers/reader-complete.h"

#include "./common/data-modifier-normaliser.h"
#include "common/dataset.h"

void test_cross_validation_models() 
{
   std::cout << "Testing cross validation models methods" << std::endl;
   std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

   /*{
     std::cout << std::endl << "1. Test of modification of a returned dataset" << std::endl;
     std::cout << "====================================================" << std::endl;

     ksi::reader_complete reader;
     ksi::train_test_model tt(reader);
     const std::filesystem::path file{ "../data/exp-lab/train.txt" };
     tt.read_and_split_file(file, 5);

     ksi::data_modifier_normaliser normer;

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
   }*/

   {
      try 
      {
         std::cout << std::endl << "2. Testing read file and save methods" << std::endl;
         std::cout << "====================================================" << std::endl;

         const bool overwrite { true };
         ksi::reader_complete reader;
         ksi::train_test_model tt(reader);
         const std::string file{ "../data/exp-lab/train.txt" };
         tt.read_and_split_file(file, 5);

         const std::filesystem::path directory {"./split_data"};
         // std::filesystem::remove_all (directory);

         tt.save (directory, "dataset", ".data", overwrite); 
      }
      CATCH;
   }
   {
      std::cout << std::endl << "3. Testing read directory method" << std::endl;
      std::cout << "====================================================" << std::endl;

      ksi::reader_complete reader;
      ksi::train_test_model tt(reader);
      const std::filesystem::path directory{ "./read_dir" };
      tt.read_directory(directory);
      // tt.read_directory(directory, "[01].data");
      
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
      debug(licznik); 
   }
   return;
   {
      std::cout << std::endl << "4. Copying of readers" << std::endl;
      std::cout << "====================================================" << std::endl;

      ksi::reader_complete reader;
      ksi::train_test_model tt(reader);
      const std::string file_name{ "./read_dir/train.txt" };
      tt.read_and_split_file(file_name);

      int licznik = 1;
      for (auto [train, test] : tt)
      {
         std::cout << "--------------------------" << std::endl;

         debug(licznik);
         debug(train.size());
         debug(train);
         debug(test.size());
         debug(test);
         break; 
         ++licznik;
      }
      // kopiowanie:
      auto kopia = tt; 

      // tt wczytuje nowy plik
      tt.read_and_split_file("./read_dir/dataset_0.data", 2);
      for (auto [train, test] : tt)
      {
         std::cout << "--------------------------" << std::endl;

         debug(licznik);
         debug(train.size());
         debug(train);
         debug(test.size());
         debug(test);
         break; 
         ++licznik;
      }
   }

   /*{
       std::cout << std::endl << "4. Testing copying for Training and Test model" << std::endl;
       std::cout << "====================================================" << std::endl;

       ksi::reader_complete reader;
       ksi::train_test_model tt(reader);
       const std::string file_name{ "./read_dir/train.txt" };
       tt.read_and_split_file(file_name);

       int licznik = 1;
       for (auto [train, test] : tt)
       {
           std::cout << "--------------------------" << std::endl;

           debug(licznik);
           debug(train.size());
           debug(train);
           debug(test.size());
           debug(test);
           break;
           ++licznik;
       }
       
       auto tt_copy = tt;

       tt.read_and_split_file("./read_dir/dataset_0.data", 2);
       for (auto [train, test] : tt)
       {
           std::cout << "--------------------------" << std::endl;

           debug(licznik);
           debug(train.size());
           debug(train);
           debug(test.size());
           debug(test);
           break;
           ++licznik;
       }
       
       for (auto [train, test] : tt_copy)
       {
           std::cout << "--------------------------" << std::endl;

           debug(licznik);
           debug(train.size());
           debug(train);
           debug(test.size());
           debug(test);
           break;
           ++licznik;
       }

   }

   {
       std::cout << std::endl << "4. Testing copying for Training, Validation and Test model" << std::endl;
       std::cout << "====================================================" << std::endl;

       ksi::reader_complete reader;
       ksi::train_validation_test_model tvt(reader);
       const std::string file_name{ "./read_dir/train.txt" };
       tvt.read_and_split_file(file_name);

       int licznik = 1;
       for (auto [train, valid, test] : tvt)
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

       auto tvt_copy = tvt;

       tvt.read_and_split_file("./read_dir/dataset_0.data", 2);
       for (auto [train, valid, test] : tvt)
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

       for (auto [train, valid, test] : tvt_copy)
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

   {
      std::cout << std::endl << "6. Testing iterator for Training and Test model" << std::endl;
      std::cout              << "====================================================" << std::endl;

      ksi::reader_complete reader;
      ksi::train_test_model tt(reader);
      const std::filesystem::path file{ "../data/exp-lab/train.txt" };
      tt.read_and_split_file(file, 5);

      int licznik = 1;
      for (ksi::train_test_model::iterator it = tt.begin(); it != tt.end(); ++it)
         //for (ksi::train_test_model::const_iterator it = tt.cbegin(); it != tt.cend(); ++it)
      {
         std::cout << "--------------------------" << std::endl;

         debug(licznik);
         debug(std::get<0>(*it).size()); // train
         debug(std::get<0>(*it));
         debug(std::get<1>(*it).size()); // test
         debug(std::get<1>(*it));
         ++licznik;
      }
   }

   {
      std::cout << std::endl << "7. Testing iterator for Training, Validation and Test model" << std::endl;
      std::cout << "====================================================" << std::endl;

      ksi::reader_complete reader;
      ksi::train_validation_test_model tvt(reader);
      const std::filesystem::path file{ "../data/exp-lab/train.txt" };
      tvt.read_and_split_file(file, 5);

   //   int licznik = 1;
   //   for (ksi::train_validation_test_model::iterator it = tvt.begin(); it != tvt.end(); ++it)
   //      //for (ksi::train_validation_test_model::const_iterator it = tvt.cbegin(); it != tvt.cend(); ++it)
   //   {
   //      std::cout << "--------------------------" << std::endl;

   //      // std::tuple<ksi::dataset, ksi::dataset> t_ds;
   //   	 // it->swap(t_ds);

   //      debug(licznik);
   //      debug(std::get<0>(*it).size()); // train
   //      debug(std::get<0>(*it));
   //      debug(std::get<1>(*it).size()); // validation
   //      debug(std::get<1>(*it));
   //      debug(std::get<2>(*it).size()); // test
   //      debug(std::get<2>(*it));
   //      ++licznik;
   //   }
   //}

      ksi::reader_complete reader;
      ksi::train_validation_test_model tvt(reader);
      const std::filesystem::path file{ "../data/exp-lab/train.txt" };
      tvt.read_and_split_file(file, 5);

      int licznik = 1;
      for (ksi::train_validation_test_model::iterator it = tvt.begin(); it != tvt.end(); ++it)
         //for (ksi::train_validation_test_model::const_iterator it = tvt.cbegin(); it != tvt.cend(); ++it)
      {
         std::cout << "--------------------------" << std::endl;

         debug(licznik);
         debug(std::get<0>(*it).size()); // train
         debug(std::get<0>(*it));
         debug(std::get<1>(*it).size()); // validation
         debug(std::get<1>(*it));
         debug(std::get<2>(*it).size()); // test
         debug(std::get<2>(*it));
         ++licznik;
      }
   }

   {
      std::cout << std::endl << "9. Testing for each loop for Training and Test model" << std::endl;
      std::cout << "====================================================" << std::endl;

      ksi::reader_complete reader;
      ksi::train_test_model tt(reader);
      const std::filesystem::path file{ "../data/exp-lab/train.txt" };
      tt.read_and_split_file(file, 5);

      int licznik = 1;
      for (auto [train, test] : tt)
         //for (const auto& [train, valid, test] : tt)
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

   {
      std::cout << std::endl << "10. Testing for each loop for Training, Validation and Test model" << std::endl;
      std::cout << "====================================================" << std::endl;

      ksi::reader_complete reader;
      ksi::train_validation_test_model tvt(reader);
      const std::filesystem::path file{ "../data/exp-lab/train.txt" };
      tvt.read_and_split_file(file, 5);

      int licznik = 1;
      for (auto [train, valid, test] : tvt)
         // for (const auto& [train, valid, test] : tvt)
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

   return;
}

 
int main (int argc, char ** params)
{
    test_cross_validation_models();
    
   return 0;
} 



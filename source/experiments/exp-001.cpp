/** @file */

#include <iostream>
#include <vector>
#include <string>
#include <fstream> 

 
#include "../readers/reader-complete.h"
#include "../readers/reader-incomplete.h"
#include "../readers/train_test_model.h"
#include "../readers/train_validation_test_model.h"
#include "../common/data-modifier-marginaliser.h"
#include "../common/data-modifier-standardiser.h"
#include "../common/data-modifier-normaliser.h"
#include "../common/data-modifier-filter.h"
#include "../common/data-modifier-imputer.h"
#include "../common/data-modifier-imputer-average.h"
#include "../common/data-modifier-imputer-median.h"
#include "../common/data-modifier-imputer-knn.h"
#include "../common/data-modifier-imputer-knn-average.h"
#include "../common/data-modifier-imputer-knn-median.h"
#include "../common/data-modifier-imputer-values-from-knn.h"
#include "../common/data-modifier-outlier-remove-sigma.h"
#include "../common/data_modifier_incompleter_random.h"
#include "../common/data_modifier_incompleter_random_without_last.h"

#include "../tnorms/t-norm-einstein.h"
#include "../common/data-modifier-imputer-granular.h"
#include "../partitions/partitioner.h" 
#include "../partitions/fcm.h"
#include "../partitions/fcm-T-metrics.h"
#include "../metrics/metric-euclidean.h"

#include "../experiments/exp-001.h"

#include "../auxiliary/directory.h"
#include "../tnorms/t-norm-min.h"


void ksi::exp_001::execute()
{
   try
   {
      std::string dataDir ("../data/exp-001");

      // complete data
      {
         std::cout << std::endl;
         std::cout << "=====================" << std::endl;
         std::cout << "reading complete data" << std::endl; 
         std::cout << "=====================" << std::endl;

         std::string CompleteDataset ("/complete.data");
         ksi::reader_complete DataReader;
         auto dane = DataReader.read(dataDir + CompleteDataset);
         std::cout << "original data" << std::endl;
         std::cout << dane << std::endl;
         std::cout << std::endl;

         {
            auto data = dane;
            ksi::data_modifier_normaliser normaliser;
            normaliser.modify(data);
            std::cout << "normalised data" << std::endl;
            std::cout << data << std::endl;
            std::cout << std::endl;
         }

         {
            auto data = dane;
            ksi::data_modifier_standardiser standardiser;
            standardiser.modify(data);
            std::cout << "standarised data" << std::endl;
            std::cout << data << std::endl;
            std::cout << std::endl;
         }

         {
            auto data = dane;
            auto filtering_function = [] (double value) -> bool 
            {
               return value > 5;
            };
            std::size_t attribute {4};  
            ksi::data_modifier_filter filter (attribute, filtering_function);
            filter.modify(data);
            std::cout << "filtered data" << std::endl;
            std::cout << data << std::endl;
            std::cout << std::endl;
         }
         
         {
            auto data = dane;
            const double probability {0.2};
            ksi::data_modifier_incompleter_random incompleter(probability);
            incompleter.modify(data);
            std::cout << "all attributes missing at random with probability p = " << probability << std::endl;
            std::cout << data << std::endl;
            std::cout << std::endl;
         }
         
         {
            auto data = dane;
            const double probability {0.2};
            ksi::data_modifier_incompleter_random_without_last incompleter(probability);
            incompleter.modify(data);
            std::cout << "all attributes but last missing at random with probability p = " << probability << std::endl;
            std::cout << data << std::endl;
            std::cout << std::endl;
         }
      }

      // outliers
      {
         std::cout << std::endl;
         std::cout << "========" << std::endl;
         std::cout << "outliers" << std::endl; 
         std::cout << "========" << std::endl;

         std::string OutlierDataset ("/outliers.data");
         ksi::reader_complete DataReader;
         auto dane = DataReader.read(dataDir + OutlierDataset);
         std::cout << "original data" << std::endl;
         std::cout << dane << std::endl;
         std::cout << std::endl;

         {
            const double n { 1 }; 
            auto data = dane;
            ksi::data_modifier_outlier_remove_sigma remover (n);
            remover.modify(data);
            std::cout << "outliers removed (" << n << " * sigma)" << std::endl;
            std::cout << data << std::endl;
            std::cout << std::endl;
         }

         {
            const double n { 3 }; 
            auto data = dane;
            ksi::data_modifier_outlier_remove_sigma remover (n);
            remover.modify(data);
            std::cout << "outliers removed (" << n << " * sigma)" << std::endl;
            std::cout << data << std::endl;
            std::cout << std::endl;
         }

         {
            const double n { 6 }; 
            auto data = dane;
            ksi::data_modifier_outlier_remove_sigma remover (n);
            remover.modify(data);
            std::cout << "outliers removed (" << n << " * sigma)" << std::endl;
            std::cout << data << std::endl;
            std::cout << std::endl;
         }
      }

      // incomplete data
      {
         std::cout << std::endl;
         std::cout << "===============" << std::endl;
         std::cout << "incomplete data" << std::endl; 
         std::cout << "===============" << std::endl;

         std::string IncompleteDataset ("/incomplete.data");

         ksi::reader_incomplete DataReader;
         auto dane = DataReader.read(dataDir + IncompleteDataset);
         std::cout << "original incomplete data" << std::endl;
         std::cout << dane << std::endl;
         std::cout << std::endl;

         {
            auto marg = dane;
            ksi::data_modifier_marginaliser marginaliser;
            marginaliser.modify(marg);
            std::cout << "marginalization" << std::endl;
            std::cout << marg << std::endl;
            std::cout << std::endl;
            std::cout << marginaliser.getDescription() << std::endl;
         }

         {
            auto marg = dane;
            const double C = 3.14;
            ksi::data_modifier_imputer imputer(C);
            imputer.modify(marg);
            std::cout << "missing values imputed with constant " << C  << std::endl;
            std::cout << marg << std::endl;
            std::cout << std::endl;
         }

         {
            auto marg = dane;
            ksi::data_modifier_imputer_average imputer;
            imputer.modify(marg);
            std::cout << "missing values imputed with averages of the attributes" << std::endl;
            std::cout << marg << std::endl;
            std::cout << std::endl;
         }

         {
            auto marg = dane;
            ksi::data_modifier_imputer_median imputer;
            imputer.modify(marg);
            std::cout << "missing values imputed with medians of the attributes" << std::endl;
            std::cout << marg << std::endl;
            std::cout << std::endl;
         }

         {
            auto marg = dane;
            int k = 3;
            ksi::data_modifier_imputer_knn_average imputer(k);
            imputer.modify(marg);
            std::cout << "missing values imputed with averages of " << k << " nearest neighbours" << std::endl;
            std::cout << marg << std::endl;
            std::cout << std::endl;
         }

         {
            auto marg = dane;
            int k = 3;
            ksi::data_modifier_imputer_knn_median imputer (k);
            imputer.modify(marg);
            std::cout << "missing values imputed with medians of " << k << " nearest neighbours" << std::endl;
            std::cout << marg << std::endl;
            std::cout << std::endl;
         }

         {
            auto marg = dane;
            int k = 3;
            ksi::data_modifier_imputer_values_from_knn imputer (k);
            imputer.modify(marg);
            std::cout << "missing values imputed with values from  " << k << " nearest neighbours" << std::endl;
            std::cout << marg << std::endl;
            std::cout << std::endl;
         }


         // chains of modifiers:
         {
            std::cout << std::endl;
            std::cout << "===================" << std::endl;
            std::cout << "chains of modifiers" << std::endl; 
            std::cout << "===================" << std::endl;

            auto marg = dane;
            int k = 3;
            ksi::data_modifier_imputer_values_from_knn imputer (k);
            ksi::data_modifier_standardiser standardiser;
            imputer.addModifier(standardiser);
            imputer.modify(marg);
            std::cout << "missing values imputed with values from " << k << " nearest neighbours and then the imputed data set standardized" << std::endl;
            std::cout << marg << std::endl;
            std::cout << std::endl;
            std::cout << imputer.print() << std::endl;
         }

         {
            std::cout << std::endl;
            std::cout << "============================" << std::endl;
            std::cout << "imputation of missing values" << std::endl; 
            std::cout << "============================" << std::endl;

            auto marg = dane;

            ksi::data_modifier_imputer dm1;
            ksi::data_modifier_normaliser dm2;
            ksi::data_modifier_standardiser dm3;
            ksi::data_modifier_imputer_average dm4;

            std::cout << dm1.print() << std::endl;
            std::cout << dm2.print() << std::endl;
            std::cout << dm3.print() << std::endl;
            std::cout << dm4.print() << std::endl;

            dm1.addModifier(dm2);
            std::cout << dm1.print() << std::endl;
            dm4.addModifier(dm3);
            std::cout << dm4.print() << std::endl;
            dm1.addModifier(dm4);

            dm1.modify(marg);

            std::cout << marg << std::endl;
            std::cout << std::endl;
            std::cout << dm1.print() << std::endl;
         }

         {
            std::cout << std::endl;
            std::cout << "==================================" << std::endl;
            std::cout << "split into train and test datasets" << std::endl; 
            std::cout << "==================================" << std::endl;

            {
               std::cout << std::endl;
               std::cout << "train and test cross validation" << std::endl;  
               std::cout << std::endl;

               std::string CompleteDataset ("/complete.data");
               ksi::reader_complete cr;
               ksi::train_test_model tt(cr); 

               const int chunks { 5 };
               tt.read_and_split_file(dataDir + CompleteDataset, chunks);

               int counter { 1 };
               for (const auto & [train, test] : tt) // std::tuple<ksi::dataset, ksi::dataset>
               {
                  std::cout << "chunk: " <<  counter++ << ", train dataset size: " <<  train.size() << ", test data set size: " << test.size() << std::endl; 
               }
            }

            {
               std::cout << std::endl;
               std::cout << "train, validate, and test cross validation" << std::endl;  
               std::cout << std::endl;

               std::string CompleteDataset ("/complete.data");
               ksi::reader_complete cr;
               ksi::train_validation_test_model tvt(cr); 

               const int chunks { 5 };
               tvt.read_and_split_file(dataDir + CompleteDataset, chunks);

               int counter { 1 };
               for (const auto & [train, validate, test] : tvt) // std::tuple<ksi::dataset, ksi::dataset, ksi::dataset>
               {
                  std::cout << "chunk: " <<  counter++ << ", train dataset size: " <<  train.size() << ", validation data set size: " << validate.size() << ", test data set size: " << test.size() << std::endl; 
               }
            }
         }
      } 

      {
          std::cout << std::endl;
          std::cout << "=====================================" << std::endl;
          std::cout << "data granulation using the FCM method" << std::endl;
          std::cout << "=====================================" << std::endl;

          ksi::reader_incomplete DataReader;

          const double EPSILON = 1e-8;
          const int NUMBER_OF_ITERATIONS = 1000;

		  std::filesystem::path incompleteDir = std::filesystem::path(dataDir) / "incomplete_data";
          std::filesystem::path resultDir = "../results/exp-001";

          ksi::t_norm_min tnorm;

          for (const auto& incomplete_path : std::filesystem::directory_iterator(incompleteDir)) {
              if (std::filesystem::is_directory(incomplete_path.path())) {
                  std::filesystem::path sub_folder = incomplete_path.path().filename();

                  std::filesystem::path result_folder_path = resultDir / "fcm_modify_results" / sub_folder;
                  std::filesystem::create_directories(result_folder_path);

                  for (const auto& entry : std::filesystem::directory_iterator(incomplete_path)) {
                      if (entry.is_regular_file()) {
                          std::filesystem::path file_path = entry.path();

                          std::cout << std::endl;
                          std::cout << "test of the " << file_path << " incomplete data set" << std::endl;
                          std::cout << std::endl;

                          auto data = DataReader.read(file_path.string());

                          auto NUMBER_OF_CLUSTERS = data.getNumberOfData() / 10;
                          ksi::fcm test_partitioner(NUMBER_OF_CLUSTERS, NUMBER_OF_ITERATIONS);
                          data_modifier_imputer_granular dm(test_partitioner, tnorm);

                          dm.modify(data);

                          const std::filesystem::path result_file_path = result_folder_path / (file_path.stem().string() + "_result" + file_path.extension().string());
                          std::cout << "Saving result of modifying into: " << result_file_path << std::endl;

                          std::ofstream result_file(result_file_path);
                          if (!result_file.is_open()) {
                              std::cerr << "Unable to open file: " << result_file_path << std::endl;
                          }
                          else
                          {
                              result_file << data;
                              result_file.close();
                          }
                      }
                  }
              }
          }
      }
   }
   catch (...)
   {
      throw;
   }
   
   return;
}

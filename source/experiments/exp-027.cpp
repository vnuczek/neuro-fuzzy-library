/** @file */

#include <iostream>
#include <string>
#include <fstream>
 
#include "../readers/reader-complete.h"
#include "../readers/reader-incomplete.h"
#include "../readers/train_test_model.h"
#include "../readers/train_validation_test_model.h"
#include "../common/data-modifier-marginaliser.h"
#include "../common/data-modifier-normaliser.h"
#include "../common/data-modifier-imputer.h"
#include "../common/data-modifier-imputer-average.h"
#include "../common/data-modifier-imputer-median.h"
#include "../common/data-modifier-imputer-knn-average.h"
#include "../common/data-modifier-imputer-knn-median.h"
#include "../common/data-modifier-imputer-values-from-knn.h"
#include "../common/data_modifier_incompleter_random_without_last.h"
#include "../common/data-modifier-imputer-granular.h"
#include "../partitions/fcm.h"

#include "../experiments/exp-027.h"

#include "../tnorms/t-norm-product.h"
#include "../tnorms/t-norm-min.h"
#include "../tnorms/t-norm-lukasiewicz.h"
#include "../tnorms/t-norm-einstein.h"

#include "../implications/imp-reichenbach.h"

#include "../neuro-fuzzy/neuro-fuzzy-system.h"
#include "../neuro-fuzzy/annbfis.h"
#include "../neuro-fuzzy/tsk.h"


void ksi::exp_027::execute()
{
   try
   {
//       ksi::reader_complete DataReader;
//       ksi::data_modifier_normaliser normaliser;
//       ksi::data_modifier_imputer_average imputer;
//       int k = 3;
//       ksi::data_modifier_imputer_knn_median imputer (k);
//       ksi::data_modifier_imputer_knn_average imputer(k);
//       ksi::data_modifier_imputer_values_from_knn imputer (k);
//       
//      
//       ksi::data_modifier_incompleter_random_without_last obiekt;
// 
//       ksi::data_modifier_marginaliser marginaliser;
      
      
      const int NUMBER_OF_RULES = 5;
      const int NUMBER_OF_CLUSTERING_ITERATIONS = 100;
      const int NUMBER_OF_TUNING_ITERATIONS = 100;  
      
      const bool NORMALISATION = false;
      
      const double ETA = 0.001;
      ksi::t_norm_product tnorm;
      ksi::imp_reichenbach implication;

        
        ksi::tsk     t (NUMBER_OF_RULES, NUMBER_OF_CLUSTERING_ITERATIONS, NUMBER_OF_TUNING_ITERATIONS, ETA, NORMALISATION, tnorm);
        ksi::annbfis a (NUMBER_OF_RULES, NUMBER_OF_CLUSTERING_ITERATIONS, NUMBER_OF_TUNING_ITERATIONS, ETA, NORMALISATION, tnorm, implication);

        ksi::dataset train, test;
        std::string output_file;
        
        auto result_t = t.experiment_regression(train, test, output_file);
        auto result_a = a.experiment_regression(train, test, output_file);
        
        
      
   }
   catch (...)
   {
      throw;
   }
   
   return;
}

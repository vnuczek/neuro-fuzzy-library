/** @file */

#include <iostream>
#include <string>
#include <fstream>

#include "../experiments/exp-027.h"

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
      // dla zbioru danych: CH4, Concrete, BoxJ290
      {
         // 1. wczytanie pliku do datasetu ksi::reader_complete DataReader;
         // 2. normalizacja ksi::data_modifier_normaliser
         // 3. ten Pana fikusny aparacik dzieli nam na train i test

         // 4. train wybrakowywujemy ksi::data_modifier_incompleter_random_without_last
         // dla roznych missing_ratio: 0.01, 0.02, 0.05, 0.10, 0.20, 0.50, 0.75
         {
            // 5. uzupelniamy braki roznymi metodami:
            // dla roznych uzupelnien:
            {
               //    ksi::data_modifier_imputer_average
               //    int k = 3;
               //    ksi::data_modifier_imputer_knn_median imputer (k);
               //    ksi::data_modifier_imputer_knn_average imputer(k);
               //    ksi::data_modifier_imputer_values_from_knn imputer (k);
               //    ksi::data_modifier_marginaliser marginaliser;

               // dla liczba granul: 10, 20, 50, 100 (BoxJ290)
               // dla liczba granul: 10, 20, 50, 100 (pozostałe)
               {
                  //    ksi::data_modifier_imputer_granular
                  // 
                  // 6. puszczamy eksperymenty

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
            }
         }
      }
      CATCH;

      return;
   }
}

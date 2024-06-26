/** @file */

#ifndef NEURO_FUZZY_SYSTEM_H
#define NEURO_FUZZY_SYSTEM_H

#include <vector>
#include <iostream>
#include <memory>
#include <tuple>
#include <deque>

#include "../common/dataset.h"
#include "../neuro-fuzzy/rulebase.h"
#include "../implications/implication.h"
#include "../tnorms/t-norm.h"
#include "../auxiliary/roc.h"
#include "../gan/discriminative_model.h"
#include "../gan/generative_model.h"
#include "../partitions/partitioner.h"
#include "../common/result.h"
#include "../common/data-modifier.h"

namespace ksi
{
   class neuro_fuzzy_system  : public generative_model, public discriminative_model
   {
   protected:
      /** number of rules **/
      int _nRules = -1;
      
      /** number of clustering iterations */
      int _nClusteringIterations = -1;
      
      /** epsilon for Frobenius norm in comparision of partition matrices */
      double _dbFrobeniusEpsilon = -1;
      
      /** minimal typicality for the fcom systems */
      double _minimal_typicality = -1;
      
      /** number of tuning iterations */
      int _nTuningIterations = -1;
      
      /** learning coefficient */
      double _dbLearningCoefficient = -1.0;
      
      /** normalisation of data */
      bool _bNormalisation;
      
      /** train dataset */
      dataset _TrainDataset;
      
      /** validation dataset */
      dataset _ValidationDataset;
      
      /** test dataset */
      dataset _TestDataset;
      
      /** rule base of the neuro-fuzzy system. **/
      rulebase * _pRulebase = nullptr;
      
      /** t-norm */
      t_norm * _pTnorm = nullptr;
      
      /** partitioner for identification of a fuzzy model */
      partitioner * _pPartitioner = nullptr;
      
      /** very short (brief) name of a neuro-fuzzy system */
      std::string _brief_name_of_neuro_fuzzy_system;
      /** short name of a neuro-fuzzy system */
      std::string _name_of_neuro_fuzzy_system;
      /** short description of a neuro-fuzzy system showing its main features */
      std::string _description_of_neuro_fuzzy_system;
      
      /** name of train data file */
      std::string _train_data_file; 
      /*** name of test data file */
      std::string _test_data_file; 
      /*** name of validation data file */
      std::string _validation_data_file ; 
      /*** name of output file */
      std::string _output_file; 
      
      /*** label for positive class in classification */
      double _positive_class;  
      /*** label for negative class in classification */
      double _negative_class;  
      /*** threshold type for classification */
      ksi::roc_threshold _threshold_type; 
      /*** value of the threshold for classification */
      double _threshold_value = std::numeric_limits<double>::quiet_NaN(); // <-- illegal value; will be set later. 
            
      std::size_t _original_size_of_training_dataset = 0;
      std::size_t _reduced_size_of_training_dataset = 0;
      
      std::shared_ptr<ksi::data_modifier> _pModyfikator { nullptr };
      
      /** answers for the train set: expected elaborated_numeric elaborated_class */
      std::vector<std::tuple<double, double, double>> _answers_for_train; 
      /** answers for the test set: expected elaborated_numeric elaborated_class */
      std::vector<std::tuple<double, double, double>> _answers_for_test; 
             
   public: 
     /** @return number of item in the train dataset 
         @date 2024-03-08 */
     std::size_t get_train_dataset_size() const;
     
   public:
     /** @return sum of weights of all items in the train dataset 
         @date 2024-03-08 */
     double get_train_dataset_cardinality() const;
   public: 
       /** @return The method return the value of the positive class. 
        @date 2021-12-27 */
       double get_positive_class() const;
       /** @return The method return the value of the negitive class. 
        @date 2021-12-27 */
       double get_negative_class() const;
       
       /** The method sets the positive class.
        @param p positive value to set 
        @date 2021-12-27 */
       void set_positive_class (const double p);
       /** The method sets the negative class.
        @param n negative value to set 
        @date 2021-12-27 */
       void set_negative_class (const double n);
             
   public:
     /** @return number of rules in the fuzzy system
      * @date 2024-03-24 */
      virtual double get_number_of_rules () const;
       
   public:
       /** @return threshold value elaborated for classification
           @date   2021-09-16
         */
      double get_threshold_value () const;
      
       /** @return threshold type
           @date   2021-09-22
         */
      ksi::roc_threshold get_threshold_type () const;
      
      /** The method sets the threshold type for a classifier.
       @param th threshold type to set 
       @date 2021-12-27 */
      void set_threshold_type (const ksi::roc_threshold & th);     

      /** The method sets the threshold value for a classifier. 
       *  The method also sets the threshold type to ksi::roc_threshold::manual.
       @param value threshold type to set 
       @date 2024-05-10 */
      void set_threshold_value (const double value);     
      
      /** @return expected class, elaborated_numeric answer, elaborated_class for the train dataset
          @date   2021-09-16
         */
      virtual std::vector<std::tuple<double, double, double>> get_answers_for_train_classification (); 
      
      /** @return expected class, elaborated_numeric answer, elaborated_class for the test dataset
          @date   2021-09-16
       */
      virtual std::vector<std::tuple<double, double, double>> get_answers_for_test_classification () ;  
       
      
   protected:
       /** @return The method returns some extra data provided by derived classes.*/
       virtual std::string extra_report () const;
       
       /** @return The method returns an intro for a classification experiment.
           @date 2021-09-23
         */
       virtual std::string classification_intro() const;
       
       /** @return The method returns classification threshold value.
           @date   2021-09-28
        */
       virtual std::string get_classification_threshold_value () const;
       
       /** The method is responsible for modifications of learing coefficient in the tuning procedure.
        * If in 4 sequential iterations the mean square error had diminished for the 
        * whole learning set then the learning parameter is increased.
        * If in 4 sequential iterations the learning parameter has been increased and   
        * decreased commutatively then the learning parameter is decreased.  
        * @return a new value of learing_paramter
        * @date 2024-02-07
        */
       double modify_learning_coefficient(const double learning_coefficient, const std::deque<double> & errors);
       
   public:
        
      virtual ~neuro_fuzzy_system();
      
      neuro_fuzzy_system ();
      
      /** 
       @param trainDataFile 
       @param testDataFile,
       @param resultsFile
       @param p partitioner for clustering algorithm
       @date 2020-04-27
       */
      neuro_fuzzy_system (const std::string & trainDataFile, 
                          const std::string & testDataFile, 
                          const std::string & resultsFile,
                          const ksi::partitioner & p,
                          const int nNumberOfRules,
                          const int nNumberOfClusteringIterations,
                          const int nNumberofTuningIterations,
                          const double dbLearningCoefficient,
                          const bool bNormalisation
                         );


      /** 
       @param trainDataFile 
       @param testDataFile,
       @param resultsFile
       @param p partitioner for clustering algorithm
       @date 2020-04-27
       */
      neuro_fuzzy_system (const std::string & trainDataFile, 
                          const std::string & testDataFile, 
                          const std::string & resultsFile,
                          const ksi::partitioner & p
                         );
      
      /** 
       @param trainDataFile 
       @param testDataFile
       @param resultsFile
       @date 2021-09-14
       */
      neuro_fuzzy_system (const std::string & trainDataFile, 
                          const std::string & testDataFile, 
                          const std::string & resultsFile
                         );

      /** 
       @param trainDataFile
       @param validationDataFile
       @param testDataFile
       @param resultsFile
       @date 2024-03-11
       */
      neuro_fuzzy_system (const std::string & trainDataFile,
                          const std::string & validationDataFile, 
                          const std::string & testDataFile, 
                          const std::string & resultsFile
                         );
 
      
      /** 
       @param trainData     train dataset 
       @param testData      test  dataset
       @param resultsFile   name of the result file
       @date 2022-02-02
       */
      neuro_fuzzy_system (const ksi::dataset & trainData, 
                          const ksi::dataset & testData, 
                          const std::string  & resultsFile
                         );
      
      /** 
       @param trainData     train dataset 
       @param validationData validation dataset
       @param testData      test  dataset
       @param resultsFile   name of the result file
       @date 2024-03-11
       */
      neuro_fuzzy_system (const ksi::dataset & trainData, 
                          const ksi::dataset & validationData,
                          const ksi::dataset & testData, 
                          const std::string  & resultsFile
                         );
      

      /** 
       @param trainDataFile 
       @param testDataFile,
       @param resultsFile
       @param p partitioner for clustering algorithm
       @param modifier data modifier
       @date 2021-01-08
       */
      neuro_fuzzy_system (const std::string & trainDataFile, 
                          const std::string & testDataFile, 
                          const std::string & resultsFile,
                          const ksi::partitioner & p,
                          const ksi::data_modifier & modifier
                         );

       /** 
       @param modifier data modifier
       @date 2021-01-08
       */
      neuro_fuzzy_system (const ksi::data_modifier & modifier);

      
       
      /** constructor
       @param nRules number of rules in a fuzzy rule base
       @param nClusteringIterations number of iterations in clustering
       @param nTuningIterations number of tuning iterations
       @param TrainDataset train dataset 
       
       @date 2018-03-28
       */
      neuro_fuzzy_system (int nRules, 
                          int nClusteringIterations,
                          int nTuningIterations,
                          const dataset & TrainDataset,
                          const t_norm * tnorma
                         );
      
      neuro_fuzzy_system (int nRules, 
                          int nClusteringIterations,
                          int nTuningIterations 
                         );
      
      neuro_fuzzy_system (const int nRules, 
                          const int nClusteringIterations,
                          const int nTuningIterations,
                          const double dbLearingCoefficient,
                          const bool bNormalisation,
                          const t_norm & tnorma
                         );
      
      neuro_fuzzy_system (int nRules, 
                          double _dbFrobeniusEpsilon,
                          int nTuningIterations, 
                          const double dbMinimalTypicality
                         );
      
      neuro_fuzzy_system (const ksi::partitioner & p);
      
      neuro_fuzzy_system (const ksi::partitioner & p, 
                          const ksi::data_modifier & d);
      
    
      
      
      neuro_fuzzy_system (const neuro_fuzzy_system & wzor);
      neuro_fuzzy_system (neuro_fuzzy_system && wzor);
      
      neuro_fuzzy_system & operator = (const neuro_fuzzy_system & wzor);
      neuro_fuzzy_system & operator = (neuro_fuzzy_system && wzor);
      
      /** @return a pointer to a copy of the NFS */
      virtual neuro_fuzzy_system * clone () const = 0;
      
      /** The method prints rule base.
       * @param ss ostream to print to */
      virtual void printRulebase(std::ostream & ss);


      /** The method prints rule base linguistic description.
       * @param ss ostream to print to
       */
      virtual void printLinguisticDescriptionRulebase(std::ostream& ss);
      
      /** The method creates a fuzzy rulebase from the dataset.
       * @param nClusteringIterations number of clustering iterations
       * @param nTuningIterations number of tuning iterations
       * @param dbLearningCoefficient learning coefficient for gradient method
       * @param train train dataset  
       * @param validation validation dataset
       * @date  2018-03-29
       * @author Krzysztof Siminski 
       */
      virtual void createFuzzyRulebase (int nClusteringIterations, int nTuningIterations, double dbLearningCoefficient, const ksi::dataset & train, const dataset & validation) = 0; 
      
      
      /** The method creates a fuzzy rulebase from the dataset.
       * @param train train dataset 
       * @param test  test  dataset 
       * @date  2021-09-14
       * @author Krzysztof Siminski 
       */
      virtual void createFuzzyRulebase (const dataset & train, const dataset & test, const dataset & validate);
      
      /** The method executes an experiment for regression.
        * @param trainDataFile name of file with train data
        * @param testDataFile  name of file with test data
        * @param outputfile    name of file to print results to
        * @param nNumberOfRules                number of rules 
        * @param nNumberOfClusteringIterations number of clustering iterations
        * @param nNumberofTuningIterations     number of tuning iterations
        * @param dbLearingCoefficient  learning coefficient for gradient method 
        * @param bNormalisation true, if normalisation of data, false -- otherwise
        * @date  2018-02-14
        * @author Krzysztof Siminski
        */
      virtual result experiment_regression (const std::string & trainDataFile,
                                  const std::string & testDataFile,
                                  const std::string & outputFile,
                                  const int nNumberOfRules,
                                  const int nNumberOfClusteringIterations,
                                  const int nNumberofTuningIterations,
                                  const double dbLearningCoefficient, 
                                  const bool bNormalisation
                                 );
      
      
      virtual result experiment_regression(const ksi::dataset & trainData, 
                                         const ksi::dataset & testData, 
                                         const std::string & trainDataFile,
                                         const std::string & testDataFile,
                                         const std::string & outputFile, 
                                         const int nNumberOfRules, 
                                         const int nNumberOfClusteringIterations, 
                                         const int nNumberofTuningIterations, 
                                         const double dbLearningCoefficient,  
                                         const bool bNormalisation
                                        );

      /** @date 2024-04-22 */
virtual result experiment_regression_core(
    const ksi::dataset & trainDataset,
    const ksi::dataset & validationDataset,
    const ksi::dataset & testDataset, 
    const std::string & trainDataFile,
    const std::string & validationDataFile,
    const std::string & testDataFile,
    const std::string & outputFile, 
    const int nNumberOfRules, 
    const int nNumberOfClusteringIterations, 
    const int nNumberofTuningIterations, 
    const double dbLearningCoefficient,  
    const bool bNormalisation
);

      /** The method executes answers for the regression task.
        * @param trainDataFile name of file with train data
        * @param outputfile    name of file to print results to
        * @param bNormalisation true, if normalisation of data, false -- otherwise
        */
      virtual void elaborate_answers_for_regression (
          const std::string & trainDataFile,
          const std::string & outputFile,
          const bool bNormalisation
      );
                                 
                                 
      /** The method executes an experiment for classification.
        * @param trainDataFile name of file with train data
        * @param testDataFile  name of file with test data
        * @param outputfile    name of file to print results to
        * @param nNumberOfRules                number of rules 
        * @param nNumberOfClusteringIterations number of clustering iterations
        * @param nNumberofTuningIterations     number of tuning iterations
        * @param dbLearingCoefficient  learning coefficient for gradient method 
        * @param bNormalisation true, if normalisation of data, false -- otherwise
        * @param dbPositiveClass label of a positive class
        * @param dbNegativeClass label of a negative class
        * @param threshold_type classification threshold type 
        * @date  2018-02-04
        * @author Krzysztof Siminski
        */
      virtual result experiment_classification (const std::string & trainDataFile,
                                  const std::string & testDataFile,
                                  const std::string & outputFile,
                                  const int nNumberOfRules,
                                  const int nNumberOfClusteringIterations,
                                  const int nNumberofTuningIterations,
                                  const double dbLearningCoefficient, 
                                  const bool bNormalisation,
                                  const double dbPositiveClass,
                                  const double dbNegativeClass,
                                  ksi::roc_threshold threshold_type
                                 );
      
      
      /** The method executes an experiment for classification.
       * @param trainDataFile name of file with train data
       * @param validationDataFile name of file with validation data
       * @param testDataFile  name of file with test data
       * @param outputfile    name of file to print results to
       * @param nNumberOfRules                number of rules 
       * @param nNumberOfClusteringIterations number of clustering iterations
       * @param nNumberofTuningIterations     number of tuning iterations
       * @param dbLearingCoefficient  learning coefficient for gradient method 
       * @param bNormalisation true, if normalisation of data, false -- otherwise
       * @param dbPositiveClass label of a positive class
       * @param dbNegativeClass label of a negative class
       * @param threshold_type classification threshold type 
       * @date  2018-02-04
       * @author Krzysztof Siminski
       */
      virtual result experiment_classification (const std::string & trainDataFile,
                                                const std::string & validationDataFile,
                                                const std::string & testDataFile,
                                                const std::string & outputFile,
                                                const int nNumberOfRules,
                                                const int nNumberOfClusteringIterations,
                                                const int nNumberofTuningIterations,
                                                const double dbLearningCoefficient, 
                                                const bool bNormalisation,
                                                const double dbPositiveClass,
                                                const double dbNegativeClass,
                                                ksi::roc_threshold threshold_type
      );

      /** The method executes an experiment for classification.
        * @param trainData train data set
        * @param testData  test data set 
        * @param outputfile    name of file to print results to
        * @param nNumberOfRules                number of rules 
        * @param nNumberOfClusteringIterations number of clustering iterations
        * @param nNumberofTuningIterations     number of tuning iterations
        * @param dbLearingCoefficient  learning coefficient for gradient method 
        * @param bNormalisation true, if normalisation of data, false -- otherwise
        * @param dbPositiveClass label of a positive class
        * @param dbNegativeClass label of a negative class
        * @param threshold_type classification threshold type 
        * @date  2022-01-30
        * @author Krzysztof Siminski
        */
      virtual result experiment_classification (const ksi::dataset & trainData,
                                  const ksi::dataset & testData,
                                  const std::string & outputFile,
                                  const int nNumberOfRules,
                                  const int nNumberOfClusteringIterations,
                                  const int nNumberofTuningIterations,
                                  const double dbLearningCoefficient, 
                                  const bool bNormalisation,
                                  const double dbPositiveClass,
                                  const double dbNegativeClass,
                                  ksi::roc_threshold threshold_type
                                 );

      
   public:
       virtual result experiment_classification_core (
                                  const ksi::dataset & trainDataset,
                                  const ksi::dataset & testDataset, 
                                  const std::string & trainDataFile,
                                  const std::string & testDataFile,
                                  const std::string & outputFile,
                                  const int nNumberOfRules,
                                  const int nNumberOfClusteringIterations,
                                  const int nNumberofTuningIterations,
                                  const double dbLearningCoefficient, 
                                  const bool bNormalisation,
                                  const double dbPositiveClass,
                                  const double dbNegativeClass,
                                  ksi::roc_threshold threshold_type
                                 );
       
       
       virtual result experiment_classification_core(
         const ksi::dataset& trainDataset, 
         const ksi::dataset& validationDataset,
         const ksi::dataset& testDataset, 
         const std::string & trainDataFile,
         const std::string & validationDataFile,
         const std::string & testDataFile,
         const std::string& outputFile, 
         const int nNumberOfRules, 
         const int nNumberOfClusteringIterations, 
         const int nNumberofTuningIterations, 
         const double dbLearningCoefficient, 
         const bool bNormalisation, 
         const double dbPositiveClass, 
         const double dbNegativeClass, 
         ksi::roc_threshold threshold_type
       ); 
       
      virtual result experiment_classification_core ();
   public:
                       
      /** Just run an experiment for classification. All parameters should be already set. */
      virtual result experiment_classification ();
 
      /** Just run an experiment for classification. All parameters should be already set. */
      virtual result experiment_classification (const std::string & trainDataFile,
                                      const std::string & testDataFile,
                                      const std::string & outputFile);
      
      /** Just run an experiment for classification. All parameters should be already set. */
      virtual result experiment_classification (const std::string & trainDataFile,
                                                const std::string & valiadtionDataFile,
                                                const std::string & testDataFile,
                                                const std::string & outputFile);

      /** Just run an experiment for classification. All parameters should be already set.
       @date 2022-02-02*/
      virtual result experiment_classification (
          const ksi::dataset & trainDataSet,
          const ksi::dataset & testDataSet,
          const std::string & outputFile);

      /** Just run an experiment for regression. All parameters should be already set.
       @date 2023-08-08*/
      virtual result experiment_regression (
          const ksi::dataset & trainDataSet,
          const ksi::dataset & testDataSet,
          const std::string & outputFile);
 
      /** Just run an experiment for regression. All parameters should be already set. */
      virtual result experiment_regression ();
      /** Just run an experiment for regression. All parameters should be already set. */
      virtual result experiment_regression (const std::string & trainDataFile,
                                      const std::string & testDataFile,
                                      const std::string & outputFile);
      

 
      
      /** A method elaborates an answer of a datum (data item).
       * @param d datum to elaborate answer for 
       * @return an incomplete (non-existing) number
       */   
      virtual number elaborate_answer (const datum & d) const = 0;   
            
      virtual double elaborate_rmse_for_dataset (const dataset & ds) override;
      
      /** @return a very short (symbolic) name of the neuro-fuzzy system 
       * @date 2024-04-21 * */
      virtual std::string get_brief_nfs_name () const;
      /** @return a short name of the neuro-fuzzy system */
      virtual std::string get_nfs_name () const;
      /** @return a description of the neuro-fuzzy system */
      virtual std::string get_nfs_description () const;
      
      /** The method sets a rule base for a fuzzy system.
       @param rb rulebase to set */
      void set_rulebase (const rulebase & rb);
      
      /** The method sets train data file.
       @param file file name for train data
       @date 2021-09-21*/
      void set_train_data_file(const std::string & file);
      
      /** The method sets the validation dat file.
       @param file file name for the validation data
       @date 2024-03-16 */
      void set_validation_data_file(const std::string & file);
      
      /** The method sets test data file.
       @param file file name for test data
       @date 2021-09-21*/
      void set_test_data_file(const std::string & file);
      
      
      /** The method sets result file.
       @param file file name for result
       @date 2021-09-21*/
      void set_output_file(const std::string & file);
      
   private:
      /** The method copies non pointer fields. */
      void copy_fields (const neuro_fuzzy_system & wzor);
      
   protected:
       /** The method elaborates a classification threshold.
        @param Expected vector of expected values
        @param Elaborated vector of elaborated values
        @param positiveClassvalue positive class value
        @param negativeClassvalue negative class value
        @param type threshold type
        @return value of the classification threshold
        
        */
       virtual double elaborate_threshold_value (std::vector<double> & Expected, 
                                         std::vector<double> & Elaborated, 
                                         double positiveClassvalue,
                                         double negativeClassvalue,
                                         const ksi::roc_threshold & type);
   public:
       /** 
       @return rulebase's answer of a data item
       @param item data item to elaborate answer for
       */
      virtual double answer (const datum & item) const = 0;
      
      /** The method elaborates answer for classification.
       @return a pair: elaborated numeric, class
       @date   2021-09-27
       */
      virtual std::pair<double, double> answer_classification (const datum & item) const;
      
   public:
       // implemented from generative_model:
       
       /** The method returns a random data item. */
      virtual datum get_random_datum (std::default_random_engine & engine);
      
      /** The method trains the generative model. */
      virtual void train_generative_model (const dataset & ds);
      
      virtual std::string to_string ();
      
   public:
      /** The method sets a train dataset.
       @param ds a dataset to set 
       @date  2021-09-14*/
      void set_train_dataset (const ksi::dataset & ds);
            
      /** The method sets a test dataset.
       @param ds a dataset to set 
       @date 2024-03-16 */
      void set_validation_dataset  (const ksi::dataset & ds);
            
      /** The method sets a test dataset.
       @param ds a dataset to set 
       @date  2021-09-14*/
      void set_test_dataset  (const ksi::dataset & ds);
      
      
   public:
     /** A report entry on the average number of rules for the train dataset.
      *  It is used in three way decision NFS and fuzzy three way decision NFS.
      *  @return empty string 
      *  @date 2024-03-24 */
     virtual std::string report_average_number_of_rules_for_train () const;
   
   public:
     /** A report entry on the average number of rules for the test dataset.
      *  It is used in three way decision NFS and fuzzy three way decision NFS.
         @return empty string 
         @date 2024-03-24 */
     virtual std::string report_average_number_of_rules_for_test () const;
      
   protected:
     /** The body is empty. If some extra activities is needed after
      *  the model has been created, the class implements 
      *  a non-empty body of this method.
      *  @date 2024-05-02 */  
       virtual void run_extra_activities_for_the_model();
   };
}

#endif 

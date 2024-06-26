/** @file */

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>

#include "../auxiliary/confusion-matrix.h"
#include "../auxiliary/mathematics.h"
#include "../service/debug.h"
#include "../common/result.h"

double ksi::confusion_matrix::TrainF1score (const ksi::result & r)
{
   double recall    = 1.0 * (r.TrainPositive2Positive) / (r.TrainPositive2Positive + r.TrainPositive2Negative);
   double precision = 1.0 * (r.TrainPositive2Positive) / (r.TrainPositive2Positive + r.TrainNegative2Positive);
   return (2.0 * recall * precision) / (recall + precision);
}

double ksi::confusion_matrix::TestF1score (const ksi::result & r)
{
   double recall    = 1.0 * (r.TestPositive2Positive) / (r.TestPositive2Positive + r.TestPositive2Negative);
   double precision = 1.0 * (r.TestPositive2Positive) / (r.TestPositive2Positive + r.TestNegative2Positive);
   return (2.0 * recall * precision) / (recall + precision);
}

std::string ksi::confusion_matrix::ca(int n)
{
   std::stringstream ss;
   const int WIDTH = 14;
   ss << std::setw(WIDTH) << n;
   return ss.str();
}

std::string ksi::confusion_matrix::ul(double d)
{
   std::stringstream ss;
   const int WIDTH = 14;
   const int PRECISION = 9;
   ss << std::setw(WIDTH) << std::fixed << std::setprecision(PRECISION) << d;
   return ss.str();
}

bool ksi::confusion_matrix::equal(const double left, const double right)
{
    const double EPSILON = 0.0001;
    return fabs(left - right) < EPSILON;
}

void ksi::confusion_matrix::calculate_statistics(
    const std::vector<double>& expected_classes, 
    const std::vector<double>& elaborated_classes, 
    double PositiveClassLabel,
    double NegativeClassLabel,
    int& nTruePositives, 
    int& nTrueNegatives, 
    int& nFalsePositives, 
    int& nFalseNegatives)
{
    try 
    {
        if (expected_classes.size() != elaborated_classes.size())
        {
            std::stringstream ss;
            ss << "Size of vectors do not match: expected_classes.size() == " << expected_classes.size() << ", elaborated_classes.size() == " << elaborated_classes.size() << std::endl;
            
            throw ss.str();
        }
        
        int nClassNegative = 0;
        int nClassPositive = 0;
        int nElaboratedPositive = 0;
        int nElaboratedNegative = 0;
        
        nTrueNegatives = nTruePositives = nFalsePositives = nFalseNegatives = 0;
        
        auto size = expected_classes.size();
        for (std::size_t i = 0; i < size; i++)
        {
            int ElaboratedClass = elaborated_classes[i];
            int OriginalClass   = expected_classes[i];

            if (equal(OriginalClass, PositiveClassLabel))
                nClassPositive++;
            else
                nClassNegative++;

            if (equal (ElaboratedClass, PositiveClassLabel))
                nElaboratedPositive++;
            else
                nElaboratedNegative++;
            
            if (equal (OriginalClass, PositiveClassLabel)) // original
            {
                if (equal (ElaboratedClass, PositiveClassLabel))
                    nTruePositives++;
                else
                    nFalseNegatives++;
            }
            else
            {
                if (equal (ElaboratedClass, PositiveClassLabel))
                    nFalsePositives++;
                else
                    nTrueNegatives++;
            }
        }
    }
    CATCH;
}


void ksi::confusion_matrix::calculate_statistics (
   const std::vector<double> & original, 
   const std::vector<double> & answers,
   double PositiveClassLabel,
   double NegativeClassLabel,
   int & nTruePositives,
   int & nTrueNegatives,
   int & nFalsePositives,
   int & nFalseNegatives,
   double dbThreshold)
{
    int nClassNegative = 0;
    int nClassPositive = 0;
    int nElaboratedPositive = 0;
    int nElaboratedNegative = 0;
    
    nTrueNegatives = nTruePositives = nFalsePositives = nFalseNegatives = 0;

    double dbProg;
    if (dbThreshold < -99)
        dbProg = (PositiveClassLabel - NegativeClassLabel) / 2.0;
    else
        dbProg = dbThreshold;

    nTrueNegatives = nTruePositives = 0;
    nFalsePositives = nFalseNegatives = 0;

    int size = original.size();
    int i;
    for (i = 0; i < size; i++)
    {
        int ElaboratedClass = answers[i] < dbProg ? NegativeClassLabel : PositiveClassLabel;
        int OriginalClass   = original[i];

        if (equal(OriginalClass, PositiveClassLabel))
            nClassPositive++;
        else
            nClassNegative++;

        if (equal (ElaboratedClass, PositiveClassLabel))
            nElaboratedPositive++;
        else
            nElaboratedNegative++;
        
        if (equal (OriginalClass, PositiveClassLabel)) // original
        {
            if (equal (ElaboratedClass, PositiveClassLabel))
                nTruePositives++;
            else
                nFalseNegatives++;
        }
        else
        {
            if (equal (ElaboratedClass, PositiveClassLabel))
                nFalsePositives++;
            else
                nTrueNegatives++;
        }
    }
}

std::string ksi::confusion_matrix::print(const ksi::result results, const bool print_for_test)
{
    if (print_for_test)
        return ksi::confusion_matrix::print(results.TestPositive2Positive, results.TestNegative2Negative, results.TestNegative2Positive, results.TestPositive2Negative);
    else 
        return ksi::confusion_matrix::print(results.TrainPositive2Positive, results.TrainNegative2Negative, results.TrainNegative2Positive, results.TrainPositive2Negative);
}

double ksi::confusion_matrix::safe_division(const double number, const double divisor)
{
   if (divisor == 0)
     return std::numeric_limits<double>::quiet_NaN();  
   else 
      return number / divisor;
}

double ksi::confusion_matrix::safe(const double d)
{
   return d != 0 ? d : std::numeric_limits<double>::quiet_NaN(); 
}

std::string ksi::confusion_matrix::print(
   int nTruePositives, 
   int nTrueNegatives, 
   int nFalsePositives, 
   int nFalseNegatives)
{
   std::stringstream ss;
   
   int CP; // conditional (original) positives 
   int CN; // conditional (original) negatives 
   int OP; // TestOutcomePositives
   int ON; // TestOutcomeNegatives;
   int To; // TotalPopulation

   CP = nTruePositives + nFalseNegatives;
   CN = nFalsePositives + nTrueNegatives;

   OP = nTruePositives + nFalsePositives;
   ON = nFalseNegatives + nTrueNegatives;

   To = CP + CN;
   
   int TP = nTruePositives;
   int TN = nTrueNegatives;
   int FP = nFalsePositives;
   int FN = nFalseNegatives;
  
   double PRE, PPV, FDR, FOR, NPV, LRP, TPR, FPR, ACC, FNR, TNR, DOR, LRN, F1S, BA, TS, NA, FM, dP, BM, MCC, PT, P4;

   PRE = (double) CP / ksi::confusion_matrix::safe(To);
   PPV = (double) nTruePositives / ksi::confusion_matrix::safe(OP);
   FDR = (double) nFalsePositives / ksi::confusion_matrix::safe(OP);
   FOR = (double) nFalseNegatives / ksi::confusion_matrix::safe(ON);
   NPV = (double) nTrueNegatives /  ksi::confusion_matrix::safe(ON);
   TPR = (double) nTruePositives / ksi::confusion_matrix::safe(CP);
   FNR = (double) nFalsePositives / ksi::confusion_matrix::safe(CP);
   FPR = (double) nFalsePositives / ksi::confusion_matrix::safe(CN);
   TNR = (double) nTrueNegatives / ksi::confusion_matrix::safe(CN);
   ACC = (double) (nTruePositives + nTrueNegatives) / ksi::confusion_matrix::safe(To);
   
   F1S = 2 * TPR * PPV / ksi::confusion_matrix::safe((TPR + PPV));
   LRP = TPR / ksi::confusion_matrix::safe(FPR);
   LRN = FNR / ksi::confusion_matrix::safe(TNR);
   DOR = LRP / ksi::confusion_matrix::safe(LRN);
   
   TS  = TP / ksi::confusion_matrix::safe((TP + FN + FP));
   BA  = (TPR + TNR) / 2.0;
   FM  = sqrt(PPV * TPR);
   dP  = PPV + NPV - 1;
   BM  = TPR + TNR - 1;
   MCC = (TP * TN - FP * FN) / ksi::confusion_matrix::safe((sqrt((TP + FP) * (TP + FN) * (TN + FP) * (TN + FN))));
   PT  = (sqrt(TPR * FPR) - FPR) / ksi::confusion_matrix::safe((TPR - FPR));
   if ((4 * TP * TN  + (TP + TN)*(FP + FN)) == 0)
       P4 = std::numeric_limits<double>::quiet_NaN();
   else
       P4 = (4 * TP * TN) / (4 * TP * TN  + (TP + TN)*(FP + FN));

   ss <<
   "+------------------+------------------+--------------------+-----------------------+---------------------+" << std::endl <<
   "| total            |      original    |     original       | prevalence =          | accuracy, ACC =     |" << std::endl <<
   "| population       |      positive    |     negative       |   original positive   |        TP + TN      |" << std::endl <<
   "|                  |                  |                    | = -----------------   | = ----------------  |" << std::endl <<
   "|                  |                  |                    |   total population    |   total population  |" << std::endl <<
   "| = "<< ca(To)<< " | = "<< ca(CP)<< " | =   "<< ca(CN)<< " | = "<< ul(PRE)<<"      | = "<< ul(ACC)<<"    |" << std::endl <<
   "+------------------+------------------+--------------------+-----------------------+---------------------+" << std::endl <<
   "| predicted        |  true positive   |  false positive    | positive predictive   | false discovery     |" << std::endl <<
   "| positive         |  TP, power       |  FP, type I error  | value, PPV, precision | rate, FDR =         |" << std::endl <<
   "|                  |                  |                    |      TP               |      FP             |" << std::endl <<
   "|                  |                  |                    | = ---------           | = ---------         |" << std::endl <<
   "|                  |                  |                    |    TP + FP            |    TP + FP          |" << std::endl <<
   "| = "<< ca(OP)<< " | = "<< ca(TP)<< " | =   "<< ca(FP)<< " | = "<< ul(PPV)<<"      | = "<< ul(FDR)<<"    |" << std::endl <<
   "+------------------+------------------+--------------------+-----------------------+---------------------+" << std::endl <<
   "| predicted        | false negative   | true negative      | false omission rate   | negative predictive |" << std::endl <<
   "| negative         | FN               | TN                 | FOR                   | value, NPV          |" << std::endl <<
   "|                  | type II error    |                    |      FN               |      TN             |" << std::endl <<
   "|                  |                  |                    | = ---------           | = ---------         |" << std::endl <<
   "|                  |                  |                    |    FN + TN            |    FN + TN          |" << std::endl <<
   "| = "<< ca(ON)<< " | = "<< ca(FN)<< " | =   "<< ca(TN)<< " | = "<< ul(FOR)<<"      | = "<< ul(NPV)<<"    |" << std::endl <<
   "+------------------+------------------+--------------------+-----------------------+---------------------+" << std::endl <<
   "                   | true positive    | false positive     | positive likelihood   | diagnostic odds     |" << std::endl <<
   "                   | rate, TPR,       | rate, FPR, fall-out| ratio, LR+            | ratio, DOR          |" << std::endl <<
   "                   | recall,          |                    |                       |                     |" << std::endl <<
   "                   | sensitivity      |                    |                       |                     |" << std::endl <<
   "                   |      TP          |      FP            |    TPR                |    LR+              |" << std::endl << 
   "                   | = ---------      | = ---------        | = -----               | = -----             |" << std::endl <<
   "                   |    TP + FN       |    TN + FP         |    FPR                |    LR-              |" << std::endl <<
   "                   | = "<< ul(TPR)<<" | = "<< ul(FPR)<<"   | = "<< ul(LRP)<<"      | = "<< ul(DOR)<<"    |" << std::endl <<
   "                   +------------------+--------------------+-----------------------+---------------------+" << std::endl <<
   "                   | false negative   | true negative      | negative likelihood   | F1 score            |" << std::endl <<
   "                   | rate, FNR,       | rate, TNR,         | ratio, LR-            |                     |" << std::endl <<
   "                   | miss rate        | specificity, SPC   |                       |                     |" << std::endl <<
   "                   |      FN          |      TN            |    FNR                |    2 * TPR * PPV    |" << std::endl <<
   "                   | = ---------      | = ---------        | = -----               | = ---------------   |" << std::endl <<
   "                   |    TP + FN       |    TN + FP         |    TNR                |      TPR + PPV      |" << std::endl <<
   "                   | = "<< ul(FNR)<<" | = "<< ul(TNR)<<"   | = "<< ul(LRN)<<"      | = "<< ul(F1S)<<"    |" << std::endl <<
   "+------------------+------------------+--------------------+-----------------------+---------------------+" << std::endl <<
   "| threat score TS, | balanced         | Fowlkes-Mallows    | markedness, deltaP    | informedness,       |" << std::endl <<
   "| critical success | accuracy, BA,    | index, FM          |                       | bookmaker           |" << std::endl <<
   "| index CSI,       |                  |                    |                       | informedness (BM)   |" << std::endl <<
   "| Jaccard index    |                  |                    |                       |                     |" << std::endl <<
   "|        TP        |   TPR + TNR      |                    |                       |                     |" << std::endl << 
   "| = ------------   | = ---------      | = sqrt(PPV * TPR)  | = PPV + NPV - 1       | = TPR + TNR - 1     |" << std::endl <<
   "|   TP + FN + FP   |       2          |                    |                       |                     |" << std::endl <<
   "| = "<< ul(TS)<< " | = "<< ul(BA)<< " | = "<< ul(FM)<< "   | = "<< ul(dP)<< "      | = "<< ul(BM)<< "    |" << std::endl <<
   "+------------------+------------------+--------------------+-----------------------+---------------------+" << std::endl <<
   "                   | Matthews correlation coefficient, MCC | prevalence threshold, PT                    |" << std::endl <<
   "                   |                                       |                                             |" << std::endl <<
   "                   |             TP * TN - FP * FN         |   sqrt(TPR * FPR) - FPR                     |" << std::endl << 
   "                   | = ----------------------------------  | = ---------------------                     |" << std::endl <<
   "                   |   sqrt((TP+FP)(TP+FN)(TN+FP)(TN+FN))  |        TPR - FPR                            |" << std::endl <<
   "                   | = "<< ul(MCC)<<"                      | = "<< ul(PT)<< "                            |" << std::endl <<
   "                   +---------------------------------------+---------------------------------------------+" << std::endl <<
   "                   | P4-metric                             |                                             |" << std::endl <<
   "                   |                                       |                                             |" << std::endl <<
   "                   |              4 * TP * TN              |                                             |" << std::endl << 
   "                   | = ----------------------------------  |                                             |" << std::endl << 
   "                   |   4 * TP * TN  + (TP + TN)*(FP + FN)  |                                             |" << std::endl <<    
   "                   | = "<< ul(P4)<< "                      |                                             |" << std::endl << 
   "                   +---------------------------------------+---------------------------------------------+" << std::endl;

   ss << std::endl << std::endl;
   
   ss << "Total Population:                                               " << ca(To) << std::endl;
   ss << "True Positive (power):                                          " << ca(nTruePositives)  << std::endl;
   ss << "True Negative:                                                  " << ca(nTrueNegatives)  << std::endl;
   ss << "False Positive (type I error):                                  " << ca(nFalsePositives) << std::endl;
   ss << "False Negative (type II error):                                 " << ca(nFalseNegatives) << std::endl;
   ss << std::endl;
   ss << "Condition (original) Positive = True Positive + False Negative: " << ca(CP) << std::endl;
   ss << "Condition (original) Negative = False Positive + True Negative: " << ca(CN) << std::endl;
   ss << "Test Outcome Positive = True Positive + False Positive:         " << ca(OP) << std::endl;
   ss << "Test Outcome Negative = True Negative + False Negative:         " << ca(ON) << std::endl;
   ss << std::endl;
   
   
   ss << "Prevalence = Condition (original) positive / Total population:                      " << ul(PRE) << std::endl;
   ss << "Positive predictive value (PPV, Precision) = True positive / Test outcome positive: " << ul(PPV) << std::endl;
   ss << "False discovery rate (FDR) = False positive / Test outcome positive:                " << ul(FDR) << std::endl;
   ss << "False omission rate (FOR) = False negative / Test outcome negative:                 " << ul(FOR) << std::endl;
   ss << "Negative predictive value (NPV) = True negative / Test outcome negative:            " << ul(NPV) << std::endl;
   ss << "True positive rate (TPR, Sensitivity, Recall) = True positive / Condition positive: " << ul(TPR) << std::endl;
   ss << "False positive rate (FPR, Fall-out) = False positive / Condition negative:          " << ul(FPR) << std::endl;
   ss << "False negative rate (FNR) = False negative / Condition positive:                    " << ul(FNR) << std::endl;
   ss << "True negative rate (TNR, Specificity, SPC) = True negative / Condition negative:    " << ul(TNR) << std::endl;
   ss << "Accuracy (ACC) = (True positive + True negative) / Total population:                " << ul(ACC) << std::endl;
   ss << "Positive likelihood ratio (LR+) = TPR / FPR:                                        " << ul(LRP) << std::endl;
   ss << "Negative likelihood ratio (LR−) = FNR / TNR:                                        " << ul(LRN) << std::endl;
   ss << "Diagnostic odds ratio (DOR) = LR+ / LR−:                                            " << ul(DOR) << std::endl;
   ss << "F1 score = 2 * recall * precision / (recall + precision):                           " << ul(F1S) << std::endl;
   ss << "Threat score (TS, critical success index CSI, Jaccard index) = TP / (TP + FN + FP): " << ul(TS)  << std::endl;
   ss << "Balanced accuracy (BA) = (TPR + TNR) / 2 :                                          " << ul(BA)  << std::endl;
   ss << "Fowlkes–Mallows index (FM) = sqrt(PPV * TPR):                                       " << ul(FM)  << std::endl;
   ss << "markedness (MK, deltaP) = PPV + NPV - 1:                                            " << ul(dP)  << std::endl;
   ss << "informedness, bookmaker informedness (BM)  = TPR + TNR - 1:                         " << ul(BM)  << std::endl;
   ss << "prevalence threshold (PT)  = (sqrt(TPR * FPR) - FPR) / (TPR - FPR):                 " << ul(PT)  << std::endl;
   ss << "                                                          (TP * TN - FP * FN)                   " << std::endl;
   ss << "Matthews correlation coefficient (MCC) = -----------------------------------------------------: " << ul(MCC) << std::endl;
   ss << "                                         (sqrt((TP + FP) * (TP + FN) * (TN + FP) * (TN + FN)))  " << std::endl;
   ss << std::endl;
   ss << "                           4 * TP * TN                                                          " << std::endl;
   ss << "P4-metric (P4) = ----------------------------------:                                            " << ul(P4) << std::endl;
   ss << "                 4 * TP * TN  + (TP + TN)*(FP + FN)                                             " << std::endl;
   
   

   return ss.str();
  
}

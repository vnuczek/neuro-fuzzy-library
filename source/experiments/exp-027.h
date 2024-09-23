/** @file */

#ifndef EXP_027_H
#define EXP_027_H


#include "../experiments/experiment.h"

namespace ksi
{
   /** EXPERIMENT 027  <br/>
    
   granular imputation of missing values
   
   @date 2024-09-23
    
    */
   class exp_027 : virtual public experiment
   {
   public:
      /** The method executes an experiment. */
      virtual void execute ();
   };
}

#endif 

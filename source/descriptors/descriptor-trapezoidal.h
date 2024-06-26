/** @file */ 

#ifndef DESCRIPTOR_TRAPEZOIDAL_H
#define DESCRIPTOR_TRAPEZOIDAL_H

#include "descriptor.h"


namespace ksi
{
   class descriptor_trapezoidal : virtual public descriptor
   {
   protected:
      double _support_min;  ///< minimal value of support 
      double _support_max;  ///< maximal value of support 
      double _core_min;     ///< minimal value of core
      double _core_max;     ///< maximal value of core
      
      double _previous_support_min;
      double _previous_support_max;
      double _previous_core_min;
      double _previous_core_max;

      /** linguistic labels */
      const static std::array<std::string, 7> trapezoidalLocationDescription;

   public:
      /** @param support_min support begin
       *  @param core_min    core begin
       *  @param core_max    core end 
       *  @param support_max support end */
      descriptor_trapezoidal (double support_min, double core_min, double core_max, double support_max);
      
      descriptor_trapezoidal(const descriptor_trapezoidal & wzor);
      
      virtual double getMembership (double x) override;
      
      /** The method elaborates the differentials of the membership function
       *  for an attribute value x 
       *  @param x a parameter to calculate differentials for
       *  @return four element vector with values <BR>
       *          vector[0] : d membership / d _support_min <BR>
       *          vector[1] : d membership / d _core_min    <BR>
       *          vector[2] : d membership / d _core_max    <BR>
       *          vector[3] : d membership / d _support_max
       */
      virtual std::vector<double> getDifferentials (double x) override;
      
      virtual descriptor * clone () const override;
      
      virtual ~descriptor_trapezoidal();
      
      /** The method prints an object into output stream.
      * @param ss an output stream to print to
      */
      virtual std::ostream & print (std::ostream & ss) const override;

      /** The method prints an object linguistic description into output stream
         * @param ss the output stream to print into
         * @param descStat the descriptor statistics to print
         * @date 2023-11-26
         * @author Konrad Wnuk
        */
      virtual std::ostream& printLinguisticDescription(std::ostream& ss, const DescriptorStatistics& descStat) const override;
      
      /** The method returns parameters for an MA triangular consequenses.
       * @return a vector of three values: minimal_support, core, and maximal_support
       * evaluated as: _support_min, (_core_min + _core_max) / 2.0 , _support_max 
       * @date 2018-02-16
       */
      virtual std::vector<double> getMAconsequenceParameters () const override;
     
      /**
      @return the mean value of descriptor's core (the mean of minimal and maximal core values)
      */
      virtual double getCoreMean() const override;
      
      void reset_parameters() override;
      
      virtual double getRandomValue(std::default_random_engine & engine) override;
      
      /** @return The method returs the name of the descriptor.
       *       @ date 2024-02-21 */                
      virtual std::string getName() const override;
      
      
   };
}

#endif

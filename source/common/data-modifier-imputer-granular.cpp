
#include <cmath>

#include "../auxiliary/vector-operators.h"
#include "../common/data-modifier-imputer-granular.h"
#include "../common/data-modifier-imputer.h"
#include "../common/dataset.h"
#include "../common/datum.h"
#include "../partitions/partitioner.h"
#include "../service/exception.h"

std::pair<ksi::dataset, ksi::dataset> ksi::data_modifier_imputer_granular::split_complete_incomplete(const dataset& ds)
{
   try
   {
      incomplete_indices.clear();
      dataset complete_data, incomplete_data;

      for (std::size_t i = 0; i < ds.size(); ++i)
      {
         auto data_item = ds.getDatum(i);
         if (data_item->is_complete())
            complete_data.addDatum(*data_item);
         else {
            incomplete_data.addDatum(*data_item);
            incomplete_indices.push_back(i);
         }
      }

      return std::make_pair(complete_data, incomplete_data);
   }
   CATCH;
}

ksi::dataset ksi::data_modifier_imputer_granular::granular_imputation(const dataset& ds)
{
   try 
   {
      auto result(ds);
      auto [complete_dataset, incomplete_dataset] = split_complete_incomplete(ds);
      auto partitioned_data = _pPartitioner->doPartition(complete_dataset);

      auto V = partitioned_data.getClusterCentres();
      auto S = partitioned_data.getClusterFuzzifications();

      if (S.empty())
      {
         throw ksi::exception("The partitioner has not set the fuzzification of granules (the S variable) that I need here. "
               "It is empty. Please use another partitioner.");
      }

      for (auto t = 0; t < incomplete_dataset.size(); ++t)  // for each incomplete tuple
      {
         std::vector<std::vector<double>> imputed_tuples; 
         imputed_tuples.reserve(partitioned_data.getNumberOfClusters());

         std::vector<double> granule_membership;
         granule_membership.reserve(partitioned_data.getNumberOfClusters());

         auto incmplete_datum = incomplete_dataset.getDatum(t);

         for (auto g = 0; g < partitioned_data.getNumberOfClusters(); ++g)  // for each granule 
         {
            auto attributes = incmplete_datum->getVector();

            for (auto attr = 0; attr < incmplete_datum->getNumberOfAttributes(); ++attr)  // for each attribute in the incomplete tuple
            {	
               if (not incmplete_datum->is_attribute_complete(attr)) 
               {
                  attributes[attr] = (V[g][attr]);
               }
            }

            imputed_tuples.push_back(attributes);

            granule_membership.push_back(calculate_granule_membership(attributes, V[g], S[g]));
         }
         auto imputed_tuple = weighted_average(imputed_tuples, granule_membership);

         result.getDatumNonConst(incomplete_indices[t])->setAttributes(imputed_tuple); // @todo
      }

      return dataset();
   }
   CATCH;
}

ksi::data_modifier_imputer_granular::data_modifier_imputer_granular(const partitioner& Partitioner, const t_norm& Tnorm)
{
	_pPartitioner = std::make_shared < ksi::partitioner >(Partitioner.clone()); ///@todo To sie nie bedzie kompilowac.
	_pTnorm = std::make_shared < ksi::t_norm >(Tnorm.clone());///@todo To sie nie bedzie kompilowac.
}

ksi::data_modifier_imputer_granular::data_modifier_imputer_granular(const data_modifier_imputer_granular& other)
	: data_modifier_imputer(other), incomplete_indices(other.incomplete_indices)
{
	_pPartitioner = std::make_shared < ksi::partitioner > (other._pPartitioner->clone());///@todo To sie nie bedzie kompilowac.
	_pTnorm = std::make_shared < ksi::t_norm >(other._pTnorm->clone());///@todo To sie nie bedzie kompilowac.
}

ksi::data_modifier_imputer_granular::data_modifier_imputer_granular(data_modifier_imputer_granular&& other) noexcept
	: data_modifier_imputer(std::move(other)), _pPartitioner(std::move(other._pPartitioner))
{
	std::swap(incomplete_indices, other.incomplete_indices);
}

ksi::data_modifier_imputer_granular& ksi::data_modifier_imputer_granular::operator=(const data_modifier_imputer_granular& other)
{
	if (this != &other)
	{
		data_modifier_imputer::operator=(other); 
		_pPartitioner = std::make_shared < ksi::partitioner >(other._pPartitioner->clone());
		_pTnorm = std::make_shared < ksi::t_norm >(other._pTnorm->clone());
		incomplete_indices = other.incomplete_indices;
	}
	return *this;
}

ksi::data_modifier_imputer_granular& ksi::data_modifier_imputer_granular::operator=(data_modifier_imputer_granular&& other) noexcept
{
	if (this != &other)
	{
		data_modifier_imputer::operator=(std::move(other));
		_pPartitioner = std::move(other._pPartitioner);
		_pTnorm = std::move(other._pTnorm);
		std::swap(incomplete_indices, other.incomplete_indices);
	}
	return *this;
}

ksi::data_modifier_imputer_granular::~data_modifier_imputer_granular()
{
}

ksi::data_modifier* ksi::data_modifier_imputer_granular::clone() const
{
	return new data_modifier_imputer_granular(*this);
}

void ksi::data_modifier_imputer_granular::modify(dataset& ds)
{
	ds = this->granular_imputation(ds);
}

std::vector<double> ksi::data_modifier_imputer_granular::weighted_average(const std::vector < std::vector<double>>& estimated_values, const std::vector<double>& weights)
{
   try
   {
      std::vector<double> numerator(estimated_values[0].size(), 0);
      double denominator = 0;

      for (auto i = 0; i < weights.size(); ++i) {
         numerator += estimated_values[i] * weights[i];
         denominator += weights[i];
      }

      return numerator / denominator;
   }
   CATCH;
}

double ksi::data_modifier_imputer_granular::calculate_granule_membership(const std::vector<double>& X, const std::vector<double>& V, const std::vector<double>& S)
{
   try
   {
      double total_membership = 1.0;

      for (std::size_t attr = 0; attr < X.size(); ++attr) // for each attribute of X tuple
      {
         auto attribute_membership = std::exp( - (std::pow((X[attr] - V[attr]), 2) / (2 * std::pow(S[attr], 2))));
         total_membership = _pTnorm->tnorm(total_membership, attribute_membership);
      }

      return total_membership;
   }
   CATCH;
}

// end of file :-) 

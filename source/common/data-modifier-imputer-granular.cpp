
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

        validate_fuzzifications(partitioned_data);

        for (auto t = 0; t < incomplete_dataset.size(); ++t) // for each incomplete tuple
        {
            auto imputed_tuple = handle_incomplete_tuple(incomplete_dataset.getDatum(t), partitioned_data);
            result.getDatumNonConst(t)->changeAttributesValues(imputed_tuple);
        }

        return result;
    }
    CATCH;
}

std::vector<double> ksi::data_modifier_imputer_granular::handle_incomplete_tuple(const datum * incomplete_tuple, const partition & partitioned_data)
{
   /// @todo KW: Przemyslec cala hierachie wywolan - zwr�cic uwage na powtarzajace sie zmienne.

   /// @todo Jak Pan tak ladnie porozpisywal na metody, to teraz sie az narzuca pewna optymalizacja.
   ///       Prosze spojrzec: Niezaleznie ile atrybutow krotce brakuje, to my i tak liczymy srednia 
   ///       wazona dla calej krotki. Nawet jak ma ona 1000 atrybutow, a brakuje jej tylko jednego,
   ///       to i tak liczymy te wszystkie srednie dla 1000. A wystarczy sprawdzic, ktorych atrybutow
   ///       brakuje i zajac sie tylko nimi. 
   
    auto [imputed_tuples, granule_membership] = calculate_granule_imputations_and_memberships(incomplete_tuple, partitioned_data);

    auto imputed_tuple = weighted_average(imputed_tuples, granule_membership);

    return imputed_tuple;
}

std::pair<std::vector<std::vector<double>>, std::vector<double>> ksi::data_modifier_imputer_granular::calculate_granule_imputations_and_memberships(const datum* incomplete_datum, const partition& partitioned_data)
{
    std::vector<std::vector<double>> imputed_tuples;
    imputed_tuples.reserve(partitioned_data.getNumberOfClusters());

    std::vector<double> granule_membership;
    /// @todo W tej metodzie wywolujemy metode getNumberOfClusters() trzy razy. 
    ///       Czyli dla kazdej krotki wywolujemy trzy razy. Czyli tych wywolan jest 
    ///       sporo. A wystarczy tylko jedno dla wszystkich krotek. 
    ///       Zapisalbym te wartosc jako pole klasy. - KW:: do zrobienia
    granule_membership.reserve(partitioned_data.getNumberOfClusters());

    auto granules_centers = partitioned_data.getClusterCentres(); ///@todo Wywolujemy metode i kopiujemy granule dla kazdej krotki, a to sie niezmienia. Wystarczy jedna kopia dla wszystkich. - KW:: do przeniesienia wyzej w hierarchii
    auto granules_fuzzifications = partitioned_data.getClusterFuzzifications(); ///@todo Tu tak samo. - KW:: do przeniesienia wyzej w hierarchii

    for (auto gran = 0; gran < partitioned_data.getNumberOfClusters(); ++gran) // for each granule 
    {
        auto attributes = impute_tuple(incomplete_datum, granules_centers[gran]);

        imputed_tuples.push_back(attributes);
        granule_membership.push_back(calculate_granule_membership(attributes, granules_centers[gran], granules_fuzzifications[gran]));
    }

    return std::make_pair(imputed_tuples, granule_membership);
}

std::vector<double> ksi::data_modifier_imputer_granular::impute_tuple(const datum* incomplete_datum, const std::vector<double>& granule_centre)
{
    auto attributes = incomplete_datum->getVector(); ///@todo Ta metoda jest kosztowna. Datum przechowuje dane jako wektor wskaznikow na klase number. Lepiej byloby te metode wywolac raz, a potem kopiowac wektor, ktory ona zwracila. Po prostu kopiowanie wektora bedzie szybsze niz wywolanie kosztownej funkcji i zapisanie do wektora. - KW: przerzuci� wy�ej w hierachi wywo�a�   

    ///@todo getNumberOfAttributes pobierzmy tylko raz. To jest wartosc taka sama dla wszystkich krotek w zbiorze danych. Nie wywolujmy metody setki razy. Zapisalbym te wartosc jako pole klasy. - KW:: do zrobienia
    for (auto attr = 0; attr < incomplete_datum->getNumberOfAttributes(); ++attr) // for each attribute in the incomplete tuple
    {
        if (not incomplete_datum->is_attribute_complete(attr))
        {
            attributes[attr] = granule_centre[attr];
        }
    }

    return attributes;
}

void ksi::data_modifier_imputer_granular::validate_fuzzifications(const partition& partitioned_data)
{
    if (partitioned_data.getClusterFuzzifications().empty())
    {
        throw ksi::exception("The partitioner has not set the fuzzification of granules (the S variable) that I need here. "
            "It is empty. Please use another partitioner.");
    }
}

ksi::data_modifier_imputer_granular::data_modifier_imputer_granular(partitioner& Partitioner, t_norm& Tnorm)
    : data_modifier_imputer(), _pPartitioner(Partitioner.clone()), _pTnorm(Tnorm.clone())
{}

ksi::data_modifier_imputer_granular::data_modifier_imputer_granular(const data_modifier_imputer_granular& other)
	: data_modifier_imputer(other), incomplete_indices(other.incomplete_indices)
{ 
    _pPartitioner = std::shared_ptr<ksi::partitioner>(other._pPartitioner->clone());
    _pTnorm = std::shared_ptr<ksi::t_norm>(other._pTnorm->clone());
}

ksi::data_modifier_imputer_granular::data_modifier_imputer_granular(data_modifier_imputer_granular&& other) noexcept
	: data_modifier_imputer(std::move(other)), _pPartitioner(std::move(other._pPartitioner)), _pTnorm(std::move(other._pTnorm))
{
	std::swap(incomplete_indices, other.incomplete_indices);
}

ksi::data_modifier_imputer_granular& ksi::data_modifier_imputer_granular::operator=(const data_modifier_imputer_granular& other)
{
	if (this != &other)
	{
		data_modifier_imputer::operator=(other); 
		_pPartitioner = std::shared_ptr<ksi::partitioner>(other._pPartitioner->clone());
      _pTnorm = std::shared_ptr<ksi::t_norm>(other._pTnorm->clone());
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
    
    // and call pNext modifier
    if (pNext)
       pNext->modify(ds);                                    
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

double ksi::data_modifier_imputer_granular::calculate_granule_membership(const std::vector<double>& estimated_values, const std::vector<double>& granule_centers, const std::vector<double>& granule_fuzzifications)
{
   try
   {
      double total_membership = 1.0;

      for (std::size_t attr = 0; attr < estimated_values.size(); ++attr) // for each attribute of X tuple
      {
         auto attribute_membership = std::exp( - (std::pow((estimated_values[attr] - granule_centers[attr]), 2) / (2 * std::pow(granule_fuzzifications[attr], 2))));
         total_membership = _pTnorm->tnorm(total_membership, attribute_membership);
      }

      return total_membership;
   }
   CATCH;
}

std::string ksi::data_modifier_imputer_granular::getDescription() const
{
   return "granular imputation";
}

// end of file :-)
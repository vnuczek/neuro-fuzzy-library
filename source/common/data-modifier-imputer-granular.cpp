
#include "../common/data-modifier-imputer-granular.h"
#include "../common/data-modifier-imputer.h"
#include "../partitions/partitioner.h"
#include "../common/dataset.h"
#include "../common/datum.h"

std::pair<ksi::dataset, ksi::dataset> ksi::data_modifier_imputer_granular::split_complete_incomplete(const dataset& ds)
{
	dataset complete_data, incomplete_data;

	for (std::size_t i = 0; i < ds.size(); ++i)
	{
		auto data_item = ds.getDatum(i);
		if (data_item->is_complete())
			complete_data.addDatum(*data_item);
		else
			incomplete_data.addDatum(*data_item);
	}

	return std::make_pair(complete_data, incomplete_data);
}

ksi::dataset ksi::data_modifier_imputer_granular::granular_imputation(const dataset& ds)
{
	auto [complete_dataset, incomplete_dataset] = split_complete_incomplete(ds);
	
	auto partitioned_data = _pPartitioner->doPartition(complete_dataset);
	
	auto U = partitioned_data.getPartitionMatrix();
	auto V = partitioned_data.getClusterCentres();
   // KS: Trzeba jeszcze wyznaczyc rozmycia klastrow. To jest ta metoda z przypisu w pedeefie. 
   // KS: [zakładka] Tutaj trzeba chwilę pomyśleć, czy tak na pewno będzie. 
   // auto S = ...
   
	for (auto i = 0; i < incomplete_dataset.size(); ++i) {
		auto incmplete_datum = incomplete_dataset.getDatum(i);
		auto attributes = incmplete_datum->getVector();
		for (auto j = 0; j < partitioned_data.getNumberOfClusters(); ++j) {
			for (auto k = 0; k < incmplete_datum->getNumberOfAttributes(); ++k) {
				if (incmplete_datum->is_complete_attribute(k)) {
               /// @todo 
				}
			}
		}
	}

	return dataset();
}

ksi::data_modifier_imputer_granular::data_modifier_imputer_granular(const partitioner& Partitioner) 
	: _pPartitioner(std::make_shared<partitioner>(Partitioner))
{
   // KS: Trzeba skopiowac obiekt wskazywany przez _pPartitioner.
}

ksi::data_modifier_imputer_granular::data_modifier_imputer_granular(const data_modifier_imputer_granular& other)
	: data_modifier_imputer(other), _pPartitioner(other._pPartitioner)
{
   // KS: Przy konstuktorze kopiujacym trzeba skopiowac obiekt wskazywany przez _pPartitioner. 
}

ksi::data_modifier_imputer_granular::data_modifier_imputer_granular(data_modifier_imputer_granular&& other) noexcept
	: data_modifier_imputer(std::move(other)), _pPartitioner(std::move(other._pPartitioner))
{
}

ksi::data_modifier_imputer_granular& ksi::data_modifier_imputer_granular::operator=(const data_modifier_imputer_granular& other)
{
	if (this != &other)
	{
		data_modifier_imputer::operator=(other);
		_pPartitioner = other._pPartitioner;
	}
	return *this;
}

ksi::data_modifier_imputer_granular& ksi::data_modifier_imputer_granular::operator=(data_modifier_imputer_granular&& other) noexcept
{
	if (this != &other)
	{
		data_modifier_imputer::operator=(std::move(other));
		_pPartitioner = std::move(other._pPartitioner);
	}
	return *this;
}

ksi::data_modifier_imputer_granular::~data_modifier_imputer_granular()
{
}

ksi::data_modifier* ksi::data_modifier_imputer_granular::clone() const
{
	return new data_modifier_imputer_granular(*_pPartitioner->clone()); // KS: To sie nie bedzie kompilowac.
}

void ksi::data_modifier_imputer_granular::modify(dataset& ds)
{
   // To do :-) 
}

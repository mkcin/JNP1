#ifndef INSERTION_ORDERED_MAP_H
#define INSERTION_ORDERED_MAP_H

#include <unordered_map>
#include <list>
#include <memory>


class lookup_error: public std::exception {
public:
	const char* what() const throw () {
		return "lookup_error exception";
	}
};


template <class K, class V, class Hash>
struct _iom_impl {

	struct MapKey {
		const K* key;
		MapKey(K const &k): key(&k) {}
		bool operator==(MapKey const &other) const {
			return *key == *other.key;
		}
	};

	struct MapHash {
		size_t operator()(MapKey const &mk) const {
			return Hash{}(*mk.key);
		}
	};


	using iterator = typename std::list<std::pair<K, V>>::iterator;
	using const_iterator = typename std::list<std::pair<K, V>>::const_iterator;

	std::list<std::pair<K, V>> list;
	std::unordered_map<MapKey, iterator, MapHash> map;


	_iom_impl() = default;

	_iom_impl(_iom_impl const &other) : list(other.list) {
		for (auto it = list.begin(); it != list.end(); ++it) {
			map.emplace(MapKey(it->first), it);
		}
	}

	V &safe_insert(std::pair<K, V> p) {
		auto list_itr = list.insert(list.end(), p);
		try {
			map.emplace(MapKey(list_itr->first), list_itr);
		} catch (...) {
			list.erase(list_itr);
			throw;
		}

		return list_itr->second;
	}

	bool insert(K const &k, V const &v) {
		auto map_itr = map.find(MapKey(k));
		if (map_itr == map.end()) {
			safe_insert(std::make_pair(k, v));
			return true;
		}
		else {
			list.splice(list.end(), list, map_itr->second);
			return false;
		}
	}

	void erase(K const &k) {
		auto map_itr = map.find(MapKey(k));
		if (map_itr == map.end())
			throw lookup_error();

		auto del_list_itr = map_itr->second;
		map.erase(map_itr);
		list.erase(del_list_itr);
	}

	void merge(_iom_impl const &other) {
		for (auto it : other.list)
			insert(it.first, it.second);
	}

	V &at(K const &k) const {
		auto map_itr = map.find(MapKey(k));
		if (map_itr == map.end())
			throw lookup_error();

		return map_itr->second->second;
	}

	V &operator[](K const &k) {
		auto map_itr = map.find(MapKey(k));
		if (map_itr == map.end())
			return safe_insert(std::make_pair(k, V()));
		else
			return map_itr->second->second;
	}

	size_t size() const noexcept {
		return map.size();
	}

	bool empty() const noexcept {
		return map.empty();
	}

	void clear() noexcept {
		list.clear();
		map.clear();
	}

	bool contains(K const k) const noexcept {
		return map.find(MapKey(k)) != map.end();
	}

	const_iterator begin() const noexcept {
		return list.begin();
	}

	const_iterator end() const noexcept {
		return list.end();
	}
};


template <class K, class V, class Hash = std::hash<K>>
class insertion_ordered_map {

private:
	using impl = _iom_impl<K, V, Hash>;

	std::shared_ptr<impl> pImpl;
	bool has_ref;

	struct guard {
		insertion_ordered_map<K, V, Hash> *map;
		std::shared_ptr<_iom_impl<K, V, Hash>> pImpl;
		bool succeeded;

		guard(insertion_ordered_map<K, V, Hash> *m) {
			map = m;
			if (m->pImpl.use_count() > 1) {
				pImpl = m->pImpl;
				succeeded = false;
				m->pImpl = std::make_shared<impl>(*m->pImpl);
			}
			else {
				succeeded = true;
			}
		}

		void success() {
			succeeded = true;
		}

		~guard() {
			if (!succeeded)
				map->pImpl = pImpl;
		}
	};

public:
	// Konstruktor bezparametrowy tworzący pusty słownik.
	// Złożoność czasowa O(1).
	insertion_ordered_map() {
		pImpl = std::make_shared<impl>();
		has_ref = false;
	}

	// Konstruktor kopiujący. Powinien mieć semantykę copy-on-write, a więc nie
	// kopiuje wewnętrznych struktur danych, jeśli nie jest to potrzebne. Słowniki
	// współdzielą struktury do czasu modyfikacji jednej z nich.
	// Złożoność czasowa O(1) lub oczekiwana O(n), jeśli konieczne jest wykonanie
	// kopii.
	insertion_ordered_map(insertion_ordered_map const &other) {
		if (other.has_ref)
			pImpl = std::make_shared<impl>(*other.pImpl);
		else
			pImpl = other.pImpl;

		has_ref = false;
	}

	// Konstruktor przenoszący.
	// Złożoność czasowa O(1).
	insertion_ordered_map(insertion_ordered_map &&other) {
		pImpl = other.pImpl;
		has_ref = other.has_ref;
	}

	// Operator przypisania. Powinien przyjmować argument przez wartość.
	// Złożoność czasowa O(1).
	insertion_ordered_map &operator=(insertion_ordered_map other) {
		if (other.has_ref)
			pImpl = std::make_shared<impl>(*other.pImpl);
		else
			pImpl = other.pImpl;

		has_ref = false;

		return *this;
	}

	// Wstawianie do słownika. Jeśli klucz k nie jest przechowywany w słowniku, to
	// wstawia wartość v pod kluczem k i zwraca true. Jeśli klucz k już jest
	// w słowniku, to wartość pod nim przypisana nie zmienia się, ale klucz zostaje
	// przesunięty na koniec porządku iteracji, a metoda zwraca false. Jeśli jest to
	// możliwe, należy unikać kopiowania elementów przechowywanych już w słowniku.
	// Złożoność czasowa oczekiwana O(1) + ewentualny czas kopiowania słownika.
	bool insert(K const &k, V const &v) {
		guard grd(this);
		bool res = pImpl->insert(k, v);
		grd.success();

		has_ref = false;

		return res;
	}

	// Usuwanie ze słownika. Usuwa wartość znajdującą się pod podanym kluczem k.
	// Jeśli taki klucz nie istnieje, to podnosi wyjątek lookup_error.
	// Złożoność czasowa oczekiwana O(1) + ewentualny czas kopiowania.
	void erase(K const &k) {
		guard grd(this);
		pImpl->erase(k);
		grd.success();

		has_ref = false;
	}

	// Scalanie słowników. Dodaje kopie wszystkich elementów podanego słownika other
	// do bieżącego słownika (this). Wartości pod kluczami już obecnymi w bieżącym
	// słowniku nie są nadpisywane. Klucze ze słownika other pojawiają się w porządku
	// iteracji na końcu, zachowując kolejność względem siebie.
	// Złożoność czasowa oczekiwana O(n + m), gdzie m to rozmiar słownika other.
	void merge(insertion_ordered_map const &other) {
		auto pImpl_cpy = std::make_shared<impl>(*pImpl);
		pImpl_cpy->merge(*other.pImpl);
		pImpl = pImpl_cpy;

		has_ref = false;
	}

	// Referencja wartości. Zwraca referencję na wartość przechowywaną w słowniku pod
	// podanym kluczem k. Jeśli taki klucz nie istnieje w słowniku, to podnosi
	// wyjątek lookup_error. Metoda ta powinna być dostępna w wersji z atrybutem
	// const oraz bez niego.
	// Złożoność czasowa oczekiwana O(1) + ewentualny czas kopiowania.
	V &at(K const &k) {
		guard grd(this);
		V &res = pImpl->at(k);
		grd.success();

		has_ref = true;

		return res;
	}

	V const &at(K const &k) const {
		return pImpl->at(k);
	}

	// Operator indeksowania. Zwraca referencję na wartość znajdującą się w słowniku
	// pod podanym kluczem k. Podobnie jak w przypadku kontenerów STL, wywołanie tego
	// operatora z kluczem nieobecnym w słowniku powoduje dodanie pod tym kluczem
	// domyślnej wartości typu V. Oczywiście ten operator ma działać tylko wtedy,
	// jeśli V ma konstruktor bezparametrowy.
	// Złożoność czasowa oczekiwana O(1) + ewentualny czas kopiowania.
	V &operator[](K const &k) {
		guard grd(this);
		V &res = pImpl->operator[](k);
		grd.success();

		has_ref = true;

		return res;
	}

	// Rozmiar słownika. Zwraca liczbę par klucz-wartość w słowniku.
	// Złożoność czasowa O(1).
	size_t size() const noexcept {
		return pImpl->size();
	}

	// Sprawdzenie niepustości słownika. Zwraca true, gdy słownik jest pusty, a false
	// w przeciwnym przypadku.
	// Złożoność czasowa O(1).
	bool empty() const noexcept {
		return pImpl->empty();
	}

	// Czyszczenie słownika. Usuwa wszystkie elementy ze słownika.
	// Złożoność czasowa O(n).
	void clear() {
		if (pImpl.use_count() > 1)
			pImpl = std::make_shared<impl>();
		else
			pImpl->clear();

		has_ref = false;
	}

	// Sprawdzenie klucza. Zwraca wartość boolowską mówiącą, czy podany klucz k jest
	// w słowniku.
	// Złożoność czasowa oczekiwana O(1).
	bool contains(K const &k) const noexcept {
		return pImpl->contains(k);
	}

	// Klasa iteratora o nazwie iterator oraz metody begin i end, pozwalające
	// przeglądać zbiór kluczy w kolejności ich wstawienia. Iteratory mogą być
	// unieważnione przez dowolną operację modyfikacji zakończoną powodzeniem.
	// Iterator powinien udostępniać przynajmniej następujące operacje:
	// - konstruktor bezparametrowy i kopiujący
	// - operator++ prefiksowy
	// - operator== i operator!=
	// - operator* (dereferencji)
	// Wszystkie operacje w czasie O(1). Przejrzenie całego słownika w czasie O(n).
	using iterator = typename impl::const_iterator;

	iterator begin() const noexcept {
		return pImpl->begin();
	}

	iterator end() const noexcept {
		return pImpl->end();
	}
};

#endif

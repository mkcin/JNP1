#include "fibo.h"

namespace {
    static std::vector<uint64_t> fib_sequence = {1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946, 17711, 28657, 46368, 75025, 121393, 196418, 317811, 514229, 832040, 1346269, 2178309, 3524578, 5702887, 9227465, 14930352, 24157817, 39088169, 63245986, 102334155, 165580141, 267914296, 433494437, 701408733, 1134903170, 1836311903, 2971215073, 4807526976, 7778742049, 12586269025, 20365011074, 32951280099, 53316291173, 86267571272, 139583862445, 225851433717, 365435296162, 591286729879, 956722026041, 1548008755920, 2504730781961, 4052739537881, 6557470319842, 10610209857723, 17167680177565, 27777890035288, 44945570212853, 72723460248141, 117669030460994, 190392490709135, 308061521170129, 498454011879264, 806515533049393, 1304969544928657, 2111485077978050, 3416454622906707, 5527939700884757, 8944394323791464, 14472334024676221, 23416728348467685, 37889062373143906, 61305790721611591, 99194853094755497, 160500643816367088, 259695496911122585, 420196140727489673, 679891637638612258, 1100087778366101931, 1779979416004714189, 2880067194370816120, 4660046610375530309, 7540113804746346429, 12200160415121876738u};

    void reverse(boost::dynamic_bitset<>& number) {
        for (size_t i = 0; i < number.size() / 2; i++) {
            bool tmp = number[i];
            number[i] = number[number.size() - 1 - i];
            number[number.size() - 1 - i] = tmp;
        }
    }

    void normalize(boost::dynamic_bitset<>& number) {
        reverse(number);

        uint32_t last_significant_bit = number.size() - 1;

        while(last_significant_bit > 0 && !number[last_significant_bit]){
            --last_significant_bit;
        }

        number.resize(last_significant_bit + 1);

        for (int32_t i = number.size() - 2; i >= 0; i--) {
            if (number[i] == true) {
                uint32_t j = i;

                while (j + 1 < number.size() && number[j + 1]) {
                    number[j] = number[j + 1] = false;
                    j += 2;
                }

                if (j == number.size()) {
                    number.push_back(true);
                } else {
                    number[j] = true;
                }
            }
        }

        reverse(number);
    }

    void add(boost::dynamic_bitset<>& number, const boost::dynamic_bitset<>& fibo) {
        normalize(number);
        reverse(number);

        while (number.size() < fibo.size()) {
            number.push_back(false);
        }

        uint32_t tmp[] = {0, 0, 0};

        for (int32_t i = number.size() - 1; i >= 0; i--) {
            if ((int32_t)fibo.size() > i) {
                tmp[2] += fibo[fibo.size() - 1 - i];
            }

            if (i == 0) {
                tmp[2] += tmp[1];
                tmp[1] = 0;
            }

            while (tmp[2] > 0) {
                if (number[i] == false) {
                    number[i] = true;
                    --tmp[2];
                } else {
                    uint32_t j = i;

                    while (j < number.size() && number[j] == true) {
                        if (j - i <= 2) {
                            ++tmp[j - i];
                        } else {
                            number[j - 2] = true;
                        }

                        number[j] = false;
                        ++j;
                    }

                    if (j == number.size()) {
                        number.push_back(true);
                    } else {
                        number[j] = true;
                    }

                    --tmp[2];
                }

                if (i == 0) {
                    tmp[2] += tmp[1];
                    tmp[1] = 0;
                }
            }

            tmp[2] = tmp[1];
            tmp[1] = tmp[0];
            tmp[0] = 0;
        }

        reverse(number);
        normalize(number);
    }

    std::string assignString(const char *str){
        assert(str != nullptr);

        return std::string(str);
    }
}

uint32_t Fibo::length() const {
    return number.size();
}

Fibo::Fibo(const Fibo &fibo) = default;

Fibo::Fibo(Fibo &&fibo) noexcept : number(std::move(fibo.number)){}

Fibo::Fibo () : Fibo(0) {};

Fibo::Fibo (int32_t n) : Fibo((uint64_t) n) {
    assert(n >= 0);
};

Fibo::Fibo (uint32_t n) : Fibo((uint64_t) n) {};

Fibo::Fibo (int64_t n) : Fibo((uint64_t) n) {
    assert(n >= 0);
};

Fibo::Fibo (uint64_t n) {
    if (n == 0) {
        number.push_back(false);
        return;
    }

    int32_t counter = fib_sequence.size() - 1;
    bool started = false;

    while (counter != -1) {
        if (n >= fib_sequence[counter]) {
            number.push_back(true);
            n -= fib_sequence[counter];
            started = true;
        } else if (started) {
            number.push_back(false);
        }

        --counter;
    }

    normalize(this->number);
}

Fibo::Fibo(const char *str) : Fibo(assignString(str)) {};

Fibo::Fibo(const std::string &str) {
    if (str.size() > 0)
        assert(str[0] != '0');

    for (char it:str){
        assert(it == '0' || it == '1');
    }

    for (char it:str) {
        number.push_back((it == '1' ? true : false));
    }

    normalize(this->number);
}

Fibo &Fibo::operator=(const Fibo &fibo) {
    if(this != &fibo) {
        number.clear();

        for (size_t i = 0; i < fibo.number.size(); ++i) {
            number.push_back(fibo.number[i]);
        }
    }

    return *this;
}

Fibo &Fibo::operator=(Fibo &&fibo) noexcept {
    number = std::move(fibo.number);
    return *this;
}

Fibo &Fibo::operator+=(const Fibo &fibo) {
    if (this == &fibo){
        const boost::dynamic_bitset <> copy = fibo.number;
        add(this->number, copy);
    }else{
        add(this->number, fibo.number);
    }

    return *this;
}

Fibo &Fibo::operator&=(const Fibo &fibo) {
    uint32_t this_size = this->number.size();
    uint32_t f_size = fibo.number.size();

    for (size_t i = 1; i <= this_size; ++i) {
        if (i > f_size) {
            this->number[this_size - i] = false;
            continue;
        }

        this->number[this_size - i] = this->number[this_size - i] && fibo.number[f_size - i];
    }

    normalize(this->number);
    return *this;
}

Fibo &Fibo::operator|=(const Fibo &fibo) {
    uint32_t this_size = this->number.size();
    uint32_t f_size = fibo.number.size();

    for (size_t i = 1; i <= this_size; i++) {
        if (i > f_size) {
            break;
        }

        this->number[this_size - i] = this->number[this_size - i] || fibo.number[f_size - i];
    }

    if (this_size < f_size) {
        reverse(this->number);

        for (size_t i = this_size + 1; i <= f_size; ++i) {
            this->number.push_back(fibo.number[f_size - i]);
        }

        reverse(this->number);
    }

    normalize(this->number);
    return *this;
}

Fibo &Fibo::operator^=(const Fibo &fibo) {
    uint32_t this_size = this->number.size();
    uint32_t f_size = fibo.number.size();

    for (size_t i = 1; i <= this_size; ++i) {
        if (i > f_size) {
            break;
        }

        this->number[this_size - i] = !(this->number[this_size - i] == fibo.number[f_size - i]);
    }

    if (this_size < f_size) {
        reverse(this->number);

        for (size_t i = this_size + 1; i <= f_size; ++i) {
            this->number.push_back(fibo.number[f_size - i]);
        }

        reverse(this->number);
    }

    normalize(this->number);
    return *this;
}

Fibo &Fibo::operator<<=(const uint64_t n) {
    this->number.resize(this->number.size() + n);

    return *this;
}

bool Fibo::operator==(const Fibo &fibo) const {
    if (fibo.number.size() != this->number.size()) {
        return false;
    }

    for (size_t i = 0; i < this->number.size(); ++i) {
        if (this->number[i] != fibo.number[i]) {
            return false;
        }
    }

    return true;
}

bool Fibo::operator!=(const Fibo &fibo) const {
    return !(*this == fibo);
}

bool Fibo::operator<(const Fibo &fibo) const {
    uint32_t this_size = this->number.size();
    uint32_t f_size = fibo.number.size();

    if (f_size < this_size) {
        return false;
    }

    if (f_size > this_size) {
        return true;
    }

    for (size_t i = 0; i < this->number.size(); ++i) {
        if (this->number[i] < fibo.number[i]) {
            return true;
        }

        if (this->number[i] > fibo.number[i]) {
            return false;
        }
    }

    return false;
}

bool Fibo::operator<=(const Fibo &fibo) const {
    return *this == fibo || *this < fibo;
}

bool Fibo::operator>(const Fibo &fibo) const {
    return !(*this <= fibo);
}

bool Fibo::operator>=(const Fibo &fibo) const {
    return *this > fibo || *this == fibo;
}

const Fibo operator+(Fibo f1, const Fibo &f2) {
    return f1 += f2;
}

const Fibo operator&(Fibo f1, const Fibo &f2) {
    return f1 &= f2;
}

const Fibo operator|(Fibo f1, const Fibo &f2) {
    return f1 |= f2;
}

const Fibo operator^(Fibo f1, const Fibo &f2) {
    return f1 ^= f2;
}

const Fibo operator<<(Fibo f1, const uint64_t n) {
    return Fibo(f1) <<= n;
}

bool operator==(uint64_t f1, const Fibo &f2) {
    return Fibo(f1) == f2;
}

bool operator!=(uint64_t f1, const Fibo &f2) {
    return Fibo(f1) != f2;
}

bool operator<(uint64_t f1, const Fibo &f2) {
    return Fibo(f1) < f2;
}

bool operator<=(uint64_t f1, const Fibo &f2) {
    return Fibo(f1) <= f2;
}

bool operator>(uint64_t f1, const Fibo &f2) {
    return Fibo(f1) > f2;
}

bool operator>=(uint64_t f1, const Fibo &f2) {
    return Fibo(f1) >= f2;
}

const Fibo &Zero() {
    static Fibo z;
    return z;
}

const Fibo &One() {
    static Fibo o(1);
    return o;
}

std::ostream& operator<<(std::ostream &os, const Fibo &f) {
    for (size_t i = 0; i < f.number.size(); ++i) {
        os << f.number[i];
    }

    return os;
}

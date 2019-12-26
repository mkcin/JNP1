#ifndef FIBO_H
#define FIBO_H

#include <boost/dynamic_bitset.hpp>

class Fibo {
    private:
        boost::dynamic_bitset <> number;

    public:
        uint32_t length() const;

        Fibo(const Fibo &fibo);

        Fibo(Fibo&& fibo) noexcept;

        Fibo();

        Fibo(char) = delete;

        Fibo(bool) = delete;

        Fibo(double) = delete;

        Fibo(float) = delete;

        Fibo (int32_t n);

        Fibo (uint32_t n);

        Fibo (int64_t n);

        Fibo (uint64_t n);

        explicit Fibo(const char *str);

        explicit Fibo(const std::string &str);

        Fibo &operator=(Fibo &&fibo) noexcept;

        Fibo &operator=(const Fibo &fibo);

        Fibo &operator+=(const Fibo &fibo);

        Fibo &operator&=(const Fibo &fibo);

        Fibo &operator|=(const Fibo &fibo);

        Fibo &operator^=(const Fibo &fibo);

        Fibo &operator<<=(const uint64_t n);

        bool operator==(const Fibo &fibo) const;

        bool operator!=(const Fibo &fibo) const;

        bool operator<(const Fibo &fibo) const;

        bool operator<=(const Fibo &fibo) const;

        bool operator>(const Fibo &fibo) const;

        bool operator>=(const Fibo &fibo) const;

    friend std::ostream& operator<<(std::ostream&, const Fibo &f);
};

const Fibo operator+(Fibo f1, const Fibo &f2);

const Fibo operator&(Fibo f1, const Fibo &f2);

const Fibo operator|(Fibo f1, const Fibo &f2);

const Fibo operator^(Fibo f1, const Fibo &f2);

const Fibo operator<<(Fibo f1, const uint64_t n);

bool operator==(uint64_t f1, const Fibo &f2);

bool operator!=(uint64_t f1, const Fibo &f2);

bool operator<(uint64_t f1, const Fibo &f2);

bool operator<=(uint64_t f1, const Fibo &f2);

bool operator>(uint64_t f1, const Fibo &f2);

bool operator>=(uint64_t f1, const Fibo &f2);

const Fibo &Zero();

const Fibo &One();

std::ostream& operator<<(std::ostream&, const Fibo &f);

#endif /* FIBO_H */

#include <iostream>
#include <algorithm>
#include <cassert>
#include <sstream>
#include <utility>
#include <cstdlib>


class NumberString {
public:
    NumberString(long long number) {
        std::stringstream ss; ss << std::abs(number); ss >> strRepresent;
        isNegative = number < 0;
    };

    NumberString(const char* strNum = "0") : strRepresent(strNum), isNegative(false) {
        assert(strNum != "");
        if (strRepresent.front() == '-') {
            strRepresent = strRepresent.substr(1);
            isNegative = true;
        }
    }

    NumberString(NumberString const&) = default;
    ~NumberString() = default;


    [[nodiscard]] std::string Get() const noexcept {
        return (this->isNegative ? "-" : "") + strRepresent;
    }

    NumberString& operator= (NumberString const&) = default;

    friend NumberString operator+ (NumberString firstNum, NumberString secondNum);
    friend NumberString operator- (NumberString firstNum, NumberString secondNum);
    friend NumberString operator* (NumberString firstNum, NumberString secondNum);
    friend NumberString operator/ (NumberString firstNum, NumberString secondNum);
    friend NumberString operator% (const NumberString& firstNum, const NumberString& secondNum);
    friend NumberString operator^ (const NumberString& number, const NumberString& power);
    friend NumberString operator! (const NumberString& number);


    NumberString& operator+= (const NumberString& other) {
        return *this = *this + other;
    }
    NumberString& operator-= (const NumberString& other) {
        return *this = *this - other;
    }
    NumberString& operator*= (const NumberString& other) {
        return *this = *this * other;
    }
    NumberString& operator/= (const NumberString& other) {
        return *this = *this / other;
    }
    NumberString& operator%= (const NumberString& other) {
        return *this = *this % other;
    }
    NumberString& operator^= (const NumberString& other) {
        return *this = *this ^ other;
    }

    bool operator== (NumberString const& other) const noexcept {
        return this->strRepresent.length() == other.strRepresent.length()
               && this->strRepresent == other.strRepresent;
    }

    bool operator!= (NumberString const& other) const noexcept {
        return !(*this == other);
    }


    bool operator> (NumberString const& other) const noexcept {
        return (this->strRepresent.length() > other.strRepresent.length())
            || this->strRepresent.length() >= other.strRepresent.length()
            && this->strRepresent > other.strRepresent;
    }

    bool operator< (NumberString const& other) const noexcept {
        return !(*this > other || *this == other);
    }

    bool operator>= (NumberString const& other) const noexcept {
        return (*this > other || *this == other);
    }

    bool operator<= (NumberString const& other) const noexcept {
        return (*this < other || *this == other);
    }

private:
    void AddLeadingZeros(size_t neededLength) noexcept {
        if (strRepresent.size() < neededLength) {
            strRepresent =
                    std::string(neededLength - strRepresent.length(), '0') + strRepresent;
        }
    }

    void RemoveTrailingZeros() {
        size_t trailingZerosEntryIndex = strRepresent.find_first_not_of('0');
        if (trailingZerosEntryIndex != std::string::npos) {
            strRepresent = strRepresent.substr(trailingZerosEntryIndex);
        }
        if (IsZero()) strRepresent.erase(1);
    }

    [[nodiscard]] bool IsZero() const noexcept {
        return (strRepresent == std::string(strRepresent.length(), '0'));
    }

private:
    std::string strRepresent;
    bool isNegative;
};


NumberString operator+ (NumberString firstNum, NumberString secondNum) {
    unsigned char carriableTen = 0;
    size_t maxLength = std::max(firstNum.strRepresent.size(), secondNum.strRepresent.size());
    auto result = NumberString{ std::string(maxLength + 1, '0').c_str() };

    bool isNegative = false;
    if (!firstNum.isNegative && secondNum.isNegative) {
        secondNum.isNegative = false;
        return firstNum - secondNum;
    }
    else if (firstNum.isNegative && !secondNum.isNegative) {
        firstNum.isNegative = false;
        return secondNum - firstNum;
    }
    else if (firstNum.isNegative && secondNum.isNegative) {
        isNegative = true;
        firstNum.isNegative = secondNum.isNegative = false;
    }
    result.isNegative = isNegative;

    firstNum.AddLeadingZeros(maxLength); secondNum.AddLeadingZeros(maxLength);

    std::transform(firstNum.strRepresent.rbegin(), firstNum.strRepresent.rend(),
                   secondNum.strRepresent.rbegin(), result.strRepresent.rbegin(),

                   [&carriableTen](char x, char y) noexcept -> char {
                       char cDigit = (x - '0') + (y - '0') + carriableTen;
                       if (cDigit > 9) { carriableTen = 1; cDigit -= 10; }
                       else { carriableTen = 0; }
                       return cDigit + '0';
                   });
    result.strRepresent.front() = carriableTen + '0';

    result.RemoveTrailingZeros();
    return result;
}

NumberString operator- (NumberString firstNum, NumberString secondNum) {
    unsigned char carriableTen = 0;
    size_t maxLength = std::max(firstNum.strRepresent.size(), secondNum.strRepresent.size());
    auto result = NumberString{ std::string(maxLength + 1, '0').c_str() };

    if (secondNum.isNegative) {
        secondNum.isNegative = false;
        return firstNum + secondNum;
    }
    else if (firstNum.isNegative) {
        secondNum.isNegative = true;
        return firstNum + secondNum;
    }

    bool IsNegative = false;
    if (firstNum < secondNum) {
        IsNegative = true;
        std::swap(firstNum, secondNum);
        firstNum.isNegative = false;
    }

    firstNum.AddLeadingZeros(maxLength); secondNum.AddLeadingZeros(maxLength);

    std::transform(firstNum.strRepresent.rbegin(), firstNum.strRepresent.rend(),
                   secondNum.strRepresent.rbegin(), result.strRepresent.rbegin(),

                   [&carriableTen](char x, char y) noexcept -> char {
                       char cDigit = (x - '0') - (y - '0') - carriableTen;
                       if (cDigit < 0) { carriableTen = 1; cDigit += 10; }
                       else { carriableTen = 0; }
                       return cDigit + '0';
                   });

    result.RemoveTrailingZeros();
    result.isNegative = IsNegative;
    return result;
}

NumberString operator* (NumberString firstNum, NumberString secondNum) {
    unsigned char carry = 0;
    size_t thisSize = firstNum.strRepresent.size();
    size_t otherSize = secondNum.strRepresent.size();
    std::string resultString(thisSize + otherSize, 0);

    for (int i = firstNum.strRepresent.size() - 1; i >= 0; --i) {
        for (int j = secondNum.strRepresent.size() - 1; j >= 0; --j) {
            char n = (firstNum.strRepresent[i] - '0') * (secondNum.strRepresent[j] - '0') +
                     resultString[i + j + 1];
            resultString.at(i + j) += n / 10;
            resultString.at(i + j + 1) = n % 10;
        }
    }
    std::transform(resultString.begin(), resultString.end(), resultString.begin(),
                   [](char sym) { return sym + '0'; });

    auto result = NumberString{ resultString.c_str() };
    result.isNegative = firstNum.isNegative != secondNum.isNegative;
    result.RemoveTrailingZeros();
    return result;
}

NumberString operator/ (NumberString firstNum, NumberString secondNum) {
    if (secondNum.IsZero()) {
        throw std::overflow_error("Division by zero exception");
    }
    if (firstNum == secondNum) {
        return NumberString{ 1 };
    }
    if (secondNum > firstNum) {
        return NumberString{};
    }

    NumberString quotient;
    quotient.isNegative = firstNum.isNegative != secondNum.isNegative;
    firstNum.isNegative = secondNum.isNegative = false;
    size_t dividendLength = firstNum.strRepresent.length(), divisorLength = secondNum.strRepresent.length();
    size_t quotientLength = dividendLength - divisorLength + 1;

    quotient.strRepresent.resize(quotientLength);
    auto remain = NumberString{ firstNum.strRepresent.substr(0, divisorLength).c_str() };

    for (int i = 0, entriesCount = 0; i < quotientLength; entriesCount = 0) {
        while (remain >= secondNum) {
            remain -= secondNum;
            ++entriesCount;
        }
        quotient.strRepresent[i] = static_cast<unsigned char>(entriesCount) + '0';

        if (remain.strRepresent == NumberString{}.strRepresent) {
            remain.strRepresent.clear();
            while (i < quotientLength && firstNum.strRepresent[divisorLength + i] == '0') {
                quotient.strRepresent[++i] = '0';
            }
        }
        while (i < quotientLength
               && (remain.strRepresent.length() < secondNum.strRepresent.length()
                   || remain.strRepresent < secondNum.strRepresent
                      && (remain.strRepresent.length() == secondNum.strRepresent.length()))) {
            remain.strRepresent.resize(remain.strRepresent.size() + 1);
            remain.strRepresent.back() = firstNum.strRepresent[divisorLength + i];
            quotient.strRepresent[++i] = '0';
        }
    }

    quotient.RemoveTrailingZeros();
    return quotient;
}

NumberString operator% (const NumberString& firstNum, const NumberString& secondNum) {
    if (secondNum.IsZero()) {
        throw std::overflow_error("Division by zero exception");
    }
    return NumberString{ firstNum - (firstNum / secondNum) * secondNum };
}

NumberString operator^ (const NumberString& number, const NumberString& power) {
    if (power == "0") return 1;
    return number * (number ^ (power - 1));
}

NumberString operator! (const NumberString& number) {
    if (number == "1") return 1;
    return number * !(number - 1);
}


int main(int argc, char** argv) {

    NumberString str{"-45723645350934253495734598345340453753498567340"
                     "5368459345890346859346953495346953465934695"
                     "4695346958349653496534968534695346953469534698534698534695" };
    str *= -2 - NumberString{ "2353" } / "43" + 464;

    std::cout << (str ^ 11).Get() << std::endl;

    std::cin.get();
    return 0;
}

#include <iostream>
#include <iomanip>
#include <numeric>
#include <cassert>
#include <cstdint>
#include <cmath>

class Fraction {
public:
	typedef long long ll;

public:
	Fraction(int32_t _numerator = 0, int32_t _denominator = 1);
	Fraction(Fraction const& other);
	Fraction(double_t number);
	~Fraction() = default;

	void Show() noexcept;
	[[nodiscard]] constexpr double_t ToDecimal() const noexcept(false);

	void Reduce() noexcept(false);
	[[nodiscard]] Fraction Reduced() const noexcept(false);

	Fraction& operator+= (Fraction const& other);
	Fraction& operator-= (Fraction const& other);
	Fraction& operator*= (Fraction const& other);
	Fraction& operator/= (Fraction const& other);
	Fraction& operator= (Fraction const&) = default;

	bool operator== (Fraction const& other) const;
	bool operator!= (Fraction const& other) const;
	bool operator< (Fraction const& other) const;
	bool operator> (Fraction const& other) const;

	Fraction& operator-- () noexcept;
	Fraction& operator++ () noexcept;
	Fraction operator-- (int) noexcept;
	Fraction operator++ (int) noexcept;

	friend Fraction operator+ (Fraction const& thisFract, Fraction const& otherFract);
	friend Fraction operator- (Fraction const& thisFract, Fraction const& otherFract);
	friend Fraction operator* (Fraction const& thisFract, Fraction const& otherFract);
	friend Fraction operator/ (Fraction const& thisFract, Fraction const& otherFract);

	friend std::ostream& operator<< (std::ostream& out, Fraction const& fract) noexcept;
	friend std::istream& operator>> (std::istream& in, Fraction& fract);

private:
	static size_t CountWholeDigits(ll number);
	static size_t CountFractionalDigits(double_t number);
	void AdjustSigns();

private:
	ll numerator;
	ll denominator;
};


Fraction::Fraction(int32_t _numerator, int32_t _denominator)
: numerator(_numerator), denominator(_denominator) {
if (denominator == 0) {
	throw std::logic_error("Fraction with denominator = 0 cannot be created");
}
 }

Fraction::Fraction(Fraction const& other)
: numerator(other.numerator), denominator(other.denominator)
{ assert(denominator != 0); }


Fraction::Fraction(double_t number) {
	assert(number != 0);
	size_t fractPart = CountFractionalDigits(number);
	numerator = static_cast<ll>(number * (pow(10, fractPart)));
	denominator = (!fractPart) ? 1 : static_cast<ll>(pow(10, fractPart));
}

size_t Fraction::CountWholeDigits(ll number) {
	size_t count = 0; for (count; number; ++count) number /= 10; return count;
}

size_t Fraction::CountFractionalDigits(double_t number) {
	const int maxDoublePrecision = 16;
	size_t unusedPrec = 0, wholePart = CountWholeDigits(static_cast<ll>(number));
	ll fractBits = static_cast<ll>(number * (pow(10, (maxDoublePrecision - wholePart))));

	if ((number - static_cast<ll>(number)) == 0) {
		return 0;
	}
	for (int i = 0, IsZero = 1; IsZero && i < maxDoublePrecision; ++i, fractBits /= 10) {
		if (IsZero = (fractBits % 10 == 0)) {
			unusedPrec++;
		}
	}
	return maxDoublePrecision - unusedPrec - wholePart;
}

void Fraction::Show() noexcept {
	AdjustSigns();
std::cout << numerator << std::endl;
	if (numerator != 0) {
		std::cout << numerator << '/' << denominator << std::endl;
}
}

constexpr double_t Fraction::ToDecimal() const {
	if (!denominator) {
		throw std::overflow_error("Division by zero exception");
	}
	return static_cast<double_t>(numerator) / denominator;
}

Fraction Fraction::Reduced() const {
	ll gcd;
	Fraction reducedFract{ *this };
    gcd = std::gcd(::abs(reducedFract.numerator), ::abs(reducedFract.denominator));
    if (gcd) {
		reducedFract.numerator /= gcd; reducedFract.denominator /= gcd;
	}
	return reducedFract;
}

void Fraction::Reduce() {
	*this = Reduced();
}

void Fraction::AdjustSigns() {
	if (denominator < 0) {
		numerator = -numerator; denominator = -denominator;
	}
}


Fraction& Fraction::operator+= (Fraction const& other) {
	return *this = *this + other;
}

Fraction& Fraction::operator-= (Fraction const& other) {
	return *this = *this - other;
}

Fraction& Fraction::operator*= (Fraction const& other) {
	return *this = *this * other;
}

Fraction& Fraction::operator/= (Fraction const& other) {
	return *this = *this / other;
}


bool Fraction::operator== (Fraction const& other) const {
	const double epsilon = 1e-6;
	return (std::abs(this->ToDecimal() - other.ToDecimal()) <= epsilon);
}

bool Fraction::operator!= (Fraction const& other) const {
	return !(*this == other);
}

bool Fraction::operator> (Fraction const& other) const {
	const double epsilon = 1e-6;
	return (this->ToDecimal() - other.ToDecimal() >= epsilon);
}

bool Fraction::operator< (Fraction const& other) const {
	return !(*this == other) && !(*this > other);
}


Fraction& Fraction::operator-- () noexcept {
	numerator -= denominator;
	return *this;
}

Fraction& Fraction::operator++ () noexcept {
	numerator += denominator;
	return *this;
}

Fraction Fraction::operator-- (int) noexcept {
	Fraction tempFract(*this); --(*this);
	return tempFract;
}

Fraction Fraction::operator++ (int) noexcept {
	Fraction tempFract(*this); ++(*this);
	return tempFract;
}


Fraction operator+ (Fraction const& thisFract, Fraction const& otherFract) {
	assert(thisFract.denominator && otherFract.denominator != 0);
	long long newNumerator = thisFract.numerator * otherFract.denominator +
		otherFract.numerator * thisFract.denominator;
	long long newDenominator = thisFract.denominator * otherFract.denominator;
	return Fraction(newNumerator, newDenominator).Reduced();
}

Fraction operator- (Fraction const& thisFract, Fraction const& otherFract) {
	assert(thisFract.denominator && otherFract.denominator != 0);
	long long newNumerator = thisFract.numerator * otherFract.denominator -
		otherFract.numerator * thisFract.denominator;
	long long newDenominator = thisFract.denominator * otherFract.denominator;
	return Fraction(newNumerator, newDenominator).Reduced();
}

Fraction operator* (Fraction const& thisFract, Fraction const& otherFract) {
	assert(thisFract.denominator && otherFract.denominator != 0);
	return Fraction(thisFract.numerator * otherFract.numerator,
		thisFract.denominator * otherFract.denominator).Reduced();
}

Fraction operator/ (Fraction const& thisFract, Fraction const& otherFract) {
	assert(thisFract.denominator && otherFract.denominator != 0);
	return Fraction(thisFract.numerator * otherFract.denominator,
		thisFract.denominator * otherFract.numerator).Reduced();
}

std::ostream& operator<< (std::ostream& out, Fraction const& fract) noexcept {
	if ((fract.numerator > 0 && fract.denominator < 0)
	|| (fract.numerator < 0 && fract.denominator > 0)) {
		out << '-';
	}
	return (out << std::abs(fract.numerator) << '/' << std::abs(fract.denominator)
<< std::endl);
}

std::istream& operator>> (std::istream& in, Fraction& fract) {
	std::cout << "Enter the fraction ( by the format X/X ) : ";
	in >> fract.numerator; in.ignore(); in >> fract.denominator;
	assert(fract.denominator != 0);
	return in;
}


int main(int argc, char** argv) {

    try {
        Fraction fraction{ 5, 0
    };
    } catch(std::logic_error const& lErr) {
        std::cout << lErr.what() << std::endl;
    }

	Fraction f{ -135, -642 };
	std::cout << std::fixed << std::setprecision(5)
		<< (23 - f-- + Fraction(34, 56) - 44e-3 * Fraction(29, 112) -
		++Fraction(3456e-3) / (Fraction(f.Reduced() + 45.65)
        * 35343.78)).ToDecimal() << std::endl;

	Fraction fu{ -455477, 18944042 };
	std::cout << std::fixed << std::setprecision(5)
		<< (--((fu / Fraction(78.1e-8)).Reduced())).ToDecimal() << std::endl;

	Fraction fract{ 3456e-6 };
	fract *= Fraction{ 34, -24552 };

	std::cout << fract << std::fixed << std::setprecision(10);
	std::cout << (++fract).ToDecimal() << std::endl;
	std::cout << (fract++).ToDecimal() << std::endl;
	std::cout << fract.ToDecimal() << std::endl;

	std::cin.get();
	return 0;
}

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

namespace BigNumerics {

class BigInteger {

public:
    BigInteger(std::string n) {
        if (n.size() > 0 && n[0] == '-') {
            this->negative = true;
            n.erase(0, 1);
        }
        else {
            this->negative = false;
        }

        this->integral = std::vector<int>(n.size());

        int sizeInt = this->integral.size();
        for (int i = 0; i < sizeInt; i++) {
            this->integral[i] = n[sizeInt - i - 1] - '0';
        }

        removeIntegralLeadingZeroes(this->integral);
    }

    BigInteger() : integral{}, negative{false} {}

    BigInteger(std::vector<int> n) : integral{n}, negative{false} {
        removeIntegralLeadingZeroes(this->integral);
    }

    ~BigInteger() = default;

    BigInteger& operator+=(const BigInteger& rhs) {

        if ((this->negative && rhs.negative) ||
            (this->negative && !rhs.negative) ||
            (!this->negative && rhs.negative)) {
            BigInteger rightHandSide = rhs;

            if (this->negative && !rhs.negative) {
                std::swap(*this, rightHandSide);
            }

            rightHandSide.negative = false;
            *this -= rightHandSide;
            return *this;
        }

        BigInteger result;
        result.negative = false;

        bool carry = false;

        int integralSize = std::max(this->integral.size(), rhs.integral.size());

        result.integral = std::vector<int>(integralSize);

        int minIntegralSize = std::min(this->integral.size(), rhs.integral.size());

        for (int i = 0; i < minIntegralSize; i++) {
            int r = this->integral[i] + rhs.integral[i] + carry;
            if (r >= 10) {
                carry = true;
            }
            else {
                carry = false;
            }
            result.integral[i] = (r % 10);
        }

        if (this->integral.size() != rhs.integral.size()) {
            for (int i = minIntegralSize; i < integralSize; i++) {
                int r = 0;
                if (this->integral.size() == integralSize) {
                    r = this->integral[i] + carry;
                }
                else {
                    r = rhs.integral[i] + carry;
                }

                if (r >= 10) {
                    carry = true;
                }
                else {
                    carry = false;
                }
                result.integral[i] = (r % 10);
            }
        }

        if (carry) {
            result.integral.push_back(1);
        }

        *this = result;

        removeIntegralLeadingZeroes(this->integral);

        return *this;
    }

    friend BigInteger operator+(BigInteger lhs, const BigInteger& rhs) {
        lhs += rhs;
        return lhs;
    }

    BigInteger& operator-=(const BigInteger& rhs) {
        if (*this == rhs) {
            *this = BigInteger("0");
            return *this;
        }

        BigInteger result;
        BigInteger rightHandSide = rhs;

        if (this->negative && !rightHandSide.negative) {
            this->negative = false;
            *this += rightHandSide;
            this->negative = true;
            return *this;
        }
        else if (!this->negative && rightHandSide.negative) {
            rightHandSide.negative = false;
            *this += rightHandSide;
            this->negative = false;
            return *this;
        }

        if (!this->negative && !rightHandSide.negative) {
            if (*this < rightHandSide) {
                std::swap(*this, rightHandSide);
                result.negative = true;
            }
            else {
                result.negative = false;
            }
        }

        if (this->negative && rightHandSide.negative) {
            if (*this > rightHandSide) {
                std::swap(*this, rightHandSide);
                result.negative = false;
            }
            else {
                result.negative = true;
            }
        }

        if (this->integral.size() != rightHandSide.integral.size()) {
            int delta = 0;
            if (this->integral.size() > rightHandSide.integral.size()) {
                delta = this->integral.size() -
                    rightHandSide.integral.size();
                appendZeroes(rightHandSide.integral, delta);
            }
            else {
                delta = rightHandSide.integral.size() -
                    this->integral.size();
                appendZeroes(this->integral, delta);
            }
        }

        result.integral = std::vector<int>(this->integral.size());

        int j = 0;
        int k = 0;

        while (j < this->integral.size()) {
            int r = this->integral[j] - rightHandSide.integral[j];
            result.integral[j] = positiveModulo(r + k, 10);
            k = std::floor((r + k) / 10.0);
            j++;
        }

        *this = result;
        removeIntegralLeadingZeroes(this->integral);

        return *this;
    }

    friend BigInteger operator-(BigInteger lhs, const BigInteger& rhs) {
        lhs -= rhs;
        return lhs;
    }

    BigInteger& operator*=(const BigInteger& rhs) {
        BigInteger result;

        if ((this->negative && !rhs.negative) ||
            (!this->negative && rhs.negative)) {
            result.negative = true;
        }
        else {
            result.negative = false;
        }

        int m = this->integral.size();
        int n = rhs.integral.size();

        result.integral = std::vector<int>(m + n);

        int j = 0;

        while (j < n) {
            if (rhs.integral[j] == 0) {
                result.integral[j + m] = 0;
            }

            int i = 0;
            int k = 0;

            while (i < m) {
                int t = this->integral[i] * rhs.integral[j]
                    + result.integral[i + j] + k;
                result.integral[i + j] = t % 10;
                k = std::floor(t / 10.0);
                i++;
            }

            result.integral[j + m] = k;

            j++;
        }

        removeIntegralLeadingZeroes(result.integral);

        *this = result;
        return *this;
    }

    friend BigInteger operator*(BigInteger lhs, BigInteger rhs) {
        lhs *= rhs;
        return lhs;
    }

    BigInteger& operator/=(const BigInteger& rhs) {
        if (rhs == BigInteger("0")) {
            *this = BigInteger("nan");
            return *this;
        }

        if (rhs > *this) {
            *this = BigInteger("0");
            return *this;
        }

        if (this->negative || rhs.negative) {
            this->negative = true;
        }
        else {
            this->negative = false;
        }

        if (rhs == *this) {
            *this = BigInteger("1");
            return *this;
        }

        BigInteger u = *this;
        BigInteger v = rhs;

        unsigned long long int m = u.integral.size() - v.integral.size();
        unsigned long long int n = v.integral.size();

        BigInteger q;
        q.integral = std::vector<int>(m+1);

        if ((!u.negative && v.negative) || (u.negative && !v.negative)) {
            q.negative = true;
        }

        if (n == 1) {
            long long int j = u.integral.size() - 1;
            int r = 0;

            while (j >= 0) {
                int w_j = std::floor(( r * 10 + u.integral[j]) / v.integral[0]);
                r = ( r * 10 + u.integral[j]) % v.integral[0];
                q.integral[j] = w_j;
                j--;
            }

            removeIntegralLeadingZeroes(q.integral);

            *this = q;
            return *this;
        }

        // D1
        int d = std::floor(10.0 / (rhs.integral[rhs.integral.size() - 1] + 1));
        BigInteger D(std::to_string(d));

        u *= D;
        u.integral.push_back(0);
        v *= D;

        // D2
        long long int j = m;

        while (j >= 0) {
            // D3
            int q_hat = std::floor((u.integral[j + n] * 10.0 +
                        u.integral[j + n - 1]) / (double)v.integral[n - 1]);

            int r_hat = (u.integral[j + n] * 10 + u.integral[j + n - 1]) %
                v.integral[n - 1];

            if ((q_hat >= 10) || (q_hat * v.integral[n - 2] >
                10 * r_hat + u.integral[j + n - 2])) {
                q_hat--;
                r_hat += v.integral[n-1];

                while (r_hat < 10) {
                    if ((q_hat >= 10) || (q_hat * v.integral[n - 2] >
                        10 * r_hat + u.integral[j + n - 2])) {
                        q_hat--;
                        r_hat += v.integral[n-1];
                    }
                    else {
                        break;
                    }
                }
            }

            // D4
            std::vector<int> a(u.integral.begin() + j, u.integral.begin() + j + n + 1);
            BigInteger tempU(a);
            tempU -= BigInteger(std::to_string(q_hat)) * v;

            bool resultOfStepD4Negative = tempU.negative;
            if (resultOfStepD4Negative) {
                BigInteger complement("10");
                for (long long int i = 0; i < n+1; i++) {
                    complement *= BigInteger("10");
                }
                tempU += complement;
            }

            while (tempU.integral.size() < n+1) {
                tempU.integral.push_back(0);
            }

            for (long long int i = j; i <= j + n; i++) {
                u.integral[i] = tempU.integral[i-j];
            }

            // D5
            unsigned long long int q_j = q_hat;

            if (resultOfStepD4Negative) {
                // D6
                q_j--;

                std::vector<int> uJtoJN(n+1);
                std::copy(u.integral.begin() + j,
                          u.integral.begin() + j + n + 1,
                          uJtoJN.begin());
                tempU = BigInteger(uJtoJN);
                tempU += v;

                std::copy(tempU.integral.begin(), tempU.integral.begin() + n,
                          u.integral.begin() + j);
            }

            q.integral[j] = q_j;

            // D7
            j--;
        }

        removeIntegralLeadingZeroes(q.integral);
        
        // D8
        *this = q;
        return *this;
    }

    friend BigInteger operator/(BigInteger lhs, BigInteger rhs) {
        lhs /= rhs;
        return lhs;
    }

    friend inline bool operator==(const BigInteger& l, const BigInteger& r) {
        if (l.integral.size() != r.integral.size()) {
            return false;
        }

        for (size_t i = 0; i < l.integral.size(); i++) {
            if (l.integral[i] != r.integral[i]) {
                return false;
            }
        }

        return true;
    }

    friend inline bool operator!=(const BigInteger& l, const BigInteger& r) {
        return !operator==(l, r);
    }

    friend inline bool operator<(const BigInteger& l, const BigInteger& r) {
        if (l.negative && !r.negative) {
            return true;
        }
        if (!l.negative && r.negative) {
            return false;
        }
        if (l.integral.size() != r.integral.size()) {
            if (l.negative && r.negative) {
                return l.integral.size() > r.integral.size();
            }
            if (!l.negative && !r.negative) {
                return l.integral.size() < r.integral.size();
            }
        }

        for (int i = (int)l.integral.size() - 1; i >= 0; i--) {
            if (l.integral[i] < r.integral[i]) {
                if (!l.negative && !r.negative) {
                    return true;
                }
                else {
                    return false;
                }
            }
            else if (l.integral[i] > r.integral[i]){
                if (!l.negative && !r.negative) {
                    return false;
                }
                else {
                    return true;
                }
            }
        }

        return false;
    }

    friend inline bool operator>(const BigInteger& l, const BigInteger& r) {
        return operator<(r, l);
    }

    friend inline bool operator<=(const BigInteger& l, const BigInteger& r) {
        return !operator>(l, r);
    }

    friend inline bool operator>=(const BigInteger& l, const BigInteger& r) {
        return !operator<(l, r);
    }

    friend std::ostream& operator<<(std::ostream& os, const BigInteger& bI) {
        if (bI.negative) {
            os << '-';
        }

        for (int i = bI.integral.size() - 1; i >= 0; i--) {
            os << bI.integral[i];
        }

        if (bI.integral.size() == 0) {
            os << '0';
        }
        return os;
    }

private:
    std::vector<int> integral;
    bool negative;

    static void removeIntegralLeadingZeroes(std::vector<int>& v) {
        if (v.empty()) {
            return;
        }

        auto nonZeroIndex = 0;
        for (int i = v.size() - 1; i >= 0; i--) {
            if (v[i] != 0) {
                nonZeroIndex = i;
                break;
            }
        }

        v.erase(v.begin() + nonZeroIndex + 1, v.end());
    }

    static void appendZeroes(std::vector<int>& v, long long int d) {
        for (long long int i = 0; i < d; i++) {
            v.push_back(0);
        }
    }

    static unsigned long long int countLeadingZeroes(const std::vector<int>& v) {
        unsigned long long int numberOfZeroes = 0;
        for (unsigned long long int i = 0; i < v.size(); i++) {
            if (v[i] != 0) {
                break;
            }
            numberOfZeroes++;
        }
        return numberOfZeroes;
    }

    static int positiveModulo(int i, int n) {
        return (i % n + n) % n;
    }
};

} /* namespace BigInteger */

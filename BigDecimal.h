#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

namespace BigNumerics {

class BigDecimal {

public:
    BigDecimal(std::string n) {
        if (n.size() > 0 && n[0] == '-') {
            this->negative = true;
            n.erase(0, 1);
        }
        else {
            this->negative = false;
        }
        std::vector<std::string> nSplitted = split(n, '.');

        if (nSplitted.size() > 1) {
            this->integral = std::vector<int>(nSplitted[0].size());
            this->floatingPoint = std::vector<int>(nSplitted[1].size());

            int sizeInt = this->integral.size();
            for (int i = 0; i < sizeInt; i++) {
                this->integral[i] = nSplitted[0][sizeInt - i - 1] - '0';
            }

            int sizeFP = this->floatingPoint.size();
            for (int i = 0; i < sizeFP; i++) {
                this->floatingPoint[i] = nSplitted[1][i] - '0';
            }

            removeIntegralLeadingZeroes(this->integral);
            removeFloatingPointTrailingZeroes(this->floatingPoint);
        }
        else {
            int sizeInt = n.size();
            this->integral = std::vector<int>(sizeInt);
            for (int i = 0; i < sizeInt; i++) {
                this->integral[i] = nSplitted[0][sizeInt - i - 1] - '0';
            }

            removeIntegralLeadingZeroes(this->integral);
        }
    }

    BigDecimal() : integral{}, floatingPoint{}, negative{false} {}

    BigDecimal(std::vector<int> n) : integral{n}, negative{false} {
        removeIntegralLeadingZeroes(this->integral);
    }

    BigDecimal(std::vector<int> integral, std::vector<int> floatingPoint) :
        integral{integral},
        floatingPoint{floatingPoint}, negative{false} {
        removeIntegralLeadingZeroes(this->integral);
        removeFloatingPointTrailingZeroes(this->floatingPoint);
    }

    ~BigDecimal() = default;

    BigDecimal& operator+=(const BigDecimal& rhs) {

        if ((this->negative && rhs.negative) ||
            (this->negative && !rhs.negative) ||
            (!this->negative && rhs.negative)) {
            BigDecimal rightHandSide = rhs;

            if (this->negative && !rhs.negative) {
                std::swap(*this, rightHandSide);
            }

            rightHandSide.negative = false;
            *this -= rightHandSide;
            return *this;
        }

        BigDecimal result;
        result.negative = false;

        int floatingPointSize = std::max(this->floatingPoint.size(),
                                    rhs.floatingPoint.size());

        result.floatingPoint = std::vector<int>(floatingPointSize);

        int minFloatingPointSize = std::min(this->floatingPoint.size(),
                                       rhs.floatingPoint.size());
        if (this->floatingPoint.size() != rhs.floatingPoint.size()) {
            if (this->floatingPoint.size() == floatingPointSize) {
                for (int i = floatingPointSize - 1; i >= minFloatingPointSize;
                     i--) {
                    result.floatingPoint[i] = this->floatingPoint[i];
                }
            }
            else {
                for (int i = floatingPointSize - 1; i >= minFloatingPointSize;
                     i--) {
                    result.floatingPoint[i] = rhs.floatingPoint[i];
                }
            }
        }

        bool carry = false;

        for (int i = minFloatingPointSize - 1; i >= 0; i--) {
            int r = this->floatingPoint[i] + rhs.floatingPoint[i] + carry;
            if (r >= 10) {
                carry = true;
            }
            else {
                carry = false;
            }
            result.floatingPoint[i] = (r % 10);
        }

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
        removeFloatingPointTrailingZeroes(this->floatingPoint);

        return *this;
    }

    friend BigDecimal operator+(BigDecimal lhs, const BigDecimal& rhs) {
        lhs += rhs;
        return lhs;
    }

    BigDecimal& operator-=(const BigDecimal& rhs) {
        if (*this == rhs) {
            *this = BigDecimal("0");
            return *this;
        }

        BigDecimal result;
        BigDecimal rightHandSide = rhs;

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

        if (this->floatingPoint.size() != rightHandSide.floatingPoint.size()) {
            int delta = 0;
            if (this->floatingPoint.size() > rightHandSide.floatingPoint.size()) {
                delta = this->floatingPoint.size() -
                    rightHandSide.floatingPoint.size();
                appendZeroes(rightHandSide.floatingPoint, delta);
            }
            else {
                delta = rightHandSide.floatingPoint.size() -
                    this->floatingPoint.size();
                appendZeroes(this->floatingPoint, delta);
            }
        }

        result.floatingPoint = std::vector<int>(this->floatingPoint.size());
        
        if (!this->floatingPoint.empty() &&
            !rightHandSide.floatingPoint.empty()) {

            j = this->floatingPoint.size() - 1;
            k = 0;

            // Always check that the floating point substraction is bigger - smaller
            BigDecimal fpA({}, this->floatingPoint);
            BigDecimal fpB({}, rightHandSide.floatingPoint);

            if (fpA < fpB) {
                std::swap(fpA, fpB);
            }

            while (j >= 0) {
                int r = fpA.floatingPoint[j] - fpB.floatingPoint[j];
                result.floatingPoint[j] = positiveModulo(r + k, 10);
                k = std::floor((r + k) / 10.0);
                j--;
            }
        }

        *this = result;
        removeIntegralLeadingZeroes(this->integral);
        removeFloatingPointTrailingZeroes(this->floatingPoint);

        return *this;
    }

    friend BigDecimal operator-(BigDecimal lhs, const BigDecimal& rhs) {
        lhs -= rhs;
        return lhs;
    }

    BigDecimal& operator*=(const BigDecimal& rhs) {
        BigDecimal result;

        if (this->negative || rhs.negative) {
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

        m = this->floatingPoint.size();
        n = rhs.floatingPoint.size();

        result.floatingPoint = std::vector<int>(m + n);

        j = 0;

        while (j < n) {
            if (rhs.floatingPoint[j] == 0) {
                result.floatingPoint[j + m] = 0;
            }

            int i = 0;
            int k = 0;

            while (i < m) {
                int t = this->floatingPoint[i] * rhs.floatingPoint[j]
                    + result.floatingPoint[i + j] + k;
                result.floatingPoint[i + j] = t % 10;
                k = std::floor(t / 10.0);
                i++;
            }

            result.floatingPoint[j + m] = k;

            j++;
        }
        std::reverse(result.floatingPoint.begin(), result.floatingPoint.end());

        removeFloatingPointLeadingZeroes(result.floatingPoint);
        removeFloatingPointTrailingZeroes(result.floatingPoint);

        if (m + n - result.floatingPoint.size() != 0) {
            std::vector<int> newFloatingPoint(m + n);
            std::copy(result.floatingPoint.begin(), result.floatingPoint.end(),
                      newFloatingPoint.begin() + (m + n -
                      result.floatingPoint.size()));
            result.floatingPoint = newFloatingPoint;
        }

        *this = result;
        return *this;
    }

    friend BigDecimal operator*(BigDecimal lhs, BigDecimal rhs) {
        lhs *= rhs;
        return lhs;
    }

    BigDecimal& operator/=(const BigDecimal& rhs) {
        if (rhs == BigDecimal("0")) {
            *this = BigDecimal("nan");
            return *this;
        }

        if (rhs > *this) {
            *this = BigDecimal("0");
            return *this;
        }

        if (this->negative || rhs.negative) {
            this->negative = true;
        }
        else {
            this->negative = false;
        }

        if (rhs == *this) {
            *this = BigDecimal("1");
            return *this;
        }

        BigDecimal u = *this;
        BigDecimal v = rhs;

        unsigned long long int m = u.integral.size() - v.integral.size();
        unsigned long long int n = v.integral.size();

        BigDecimal q;
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
        BigDecimal D(std::to_string(d));

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
            BigDecimal tempU(a);
            tempU -= BigDecimal(std::to_string(q_hat)) * v;

            bool resultOfStepD4Negative = tempU.negative;
            if (resultOfStepD4Negative) {
                BigDecimal complement("10");
                for (long long int i = 0; i < n+1; i++) {
                    complement *= BigDecimal("10");
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
                tempU = BigDecimal(uJtoJN);
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

    friend BigDecimal operator/(BigDecimal lhs, BigDecimal rhs) {
        lhs /= rhs;
        return lhs;
    }

    friend inline bool operator==(const BigDecimal& l, const BigDecimal& r) {
        if (l.integral.size() != r.integral.size() ||
            l.floatingPoint.size() != r.floatingPoint.size()) {
            return false;
        }

        for (size_t i = 0; i < l.integral.size(); i++) {
            if (l.integral[i] != r.integral[i]) {
                return false;
            }
        }

        for (size_t i = 0; i < l.floatingPoint.size(); i++) {
            if (l.floatingPoint[i] != r.floatingPoint[i]) {
                return false;
            }
        }

        return true;
    }

    friend inline bool operator!=(const BigDecimal& l, const BigDecimal& r) {
        return !operator==(l, r);
    }

    friend inline bool operator<(const BigDecimal& l, const BigDecimal& r) {
        if ((l.negative && !r.negative) ||
            (l.integral.size() < r.integral.size())) {
            return true;
        }
        if ((!l.negative && r.negative) ||
            (l.integral.size() > r.integral.size())) {
            return false;
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

        for (int i = 0; i < (int)std::min(l.floatingPoint.size(),
                                          r.floatingPoint.size()); i++) {
            if (l.floatingPoint[i] < r.floatingPoint[i]) {
                if (!l.negative && !r.negative) {
                    return true;
                }
                else {
                    return false;
                }
            }
            else if (l.floatingPoint[i] > r.floatingPoint[i]){
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

    friend inline bool operator>(const BigDecimal& l, const BigDecimal& r) {
        return operator<(r, l);
    }

    friend inline bool operator<=(const BigDecimal& l, const BigDecimal& r) {
        return !operator>(l, r);
    }

    friend inline bool operator>=(const BigDecimal& l, const BigDecimal& r) {
        return !operator<(l, r);
    }

    friend std::ostream& operator<<(std::ostream& os, const BigDecimal& bI) {
        if (bI.negative) {
            os << '-';
        }

        for (int i = bI.integral.size() - 1; i >= 0; i--) {
            os << bI.integral[i];
        }

        if (bI.integral.size() == 0) {
            os << '0';
        }
        if (bI.floatingPoint.size() > 0) {
            os << '.';
            for (int i = 0; i < bI.floatingPoint.size(); i++) {
                os << bI.floatingPoint[i];
            }
        }
        return os;
    }

    static BigDecimal& floor(BigDecimal& a) {
        a.floatingPoint = std::vector<int>();
        return a;
    }

    static BigDecimal& ceil(BigDecimal& a) {
        a.floatingPoint = std::vector<int>();
        return a;
    }

private:
    std::vector<int> integral;
    std::vector<int> floatingPoint;
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

    static void removeFloatingPointTrailingZeroes(std::vector<int>& v) {
        if (v.empty()) {
            return;
        }

        auto nonZeroIndex = 0;
        for (auto i = v.size() - 1; i >= 0; i--) {
            if (v[i] != 0) {
                nonZeroIndex = i;
                break;
            }
        }

        v.erase(v.begin() + nonZeroIndex + 1, v.end());
    }

    static void removeFloatingPointLeadingZeroes(std::vector<int>& v) {
        if (v.empty()) {
            return;
        }

        size_t i = 0;
        for (; i < v.size(); i++) {
            if (v[i] != 0) {
                break;
            }
        }

        v.erase(v.begin(), v.begin() + i);
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

    static std::vector<std::string> split(const std::string s, const char c) {
        std::vector<std::string> splitted;
        std::stringstream ss(s);
        std::string element;
        while (getline(ss, element, c)) {
            splitted.push_back(element);
        }
        return splitted;
    }

    static int positiveModulo(const int i, const int n) {
        return (i % n + n) % n;
    }
};

} /* namespace BigNumerics */


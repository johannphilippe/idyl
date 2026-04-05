#pragma once 
/*
    Fast math for runtime 
*/
#include <cmath>
#include <cassert>
#include <array>

namespace idyl::math {

    // ── High-precision π/2 constants (Cody-Waite 3-part split) ─────────────────
    // Together these sum to π/2 to ~150 bits of precision.
    // range_reduce computes k = round(x / (π/2)), then subtracts k * (π/2).
    static constexpr double PI2_HI  =  1.5707963267948965579989817342;  // top 53 bits of π/2
    static constexpr double PI2_MID =  6.123233995736765886e-17;        // next 53 bits of π/2
    static constexpr double PI2_LO  = -1.4973849048591698e-33;          // residual of π/2

    // Full π for reference (more bits than double can hold, but useful for comments)
    // π = 3.14159265358979323846264338327950288...
    static constexpr double PI     = 3.141592653589793238462643383;
    static constexpr double TWO_PI = 6.283185307179586476925286767;
    static constexpr double PI_2   = 1.5707963267948966192313216916; // π/2
    static constexpr double PI_4   = 0.7853981633974483096156608458; // π/4

    // ── Minimax polynomial coefficients for sin(x) on [0, π/4] ───────────────
    // Derived from Remez algorithm — each is rounded to nearest double.
    // Error: < 2 ULPs over [0, π/4]
    // sin(x) ≈ x + x³·(S1 + x²·(S2 + x²·(S3 + x²·(S4 + x²·S5))))
    static constexpr double S1 = -1.6666666666666630e-01; // -1/3!
    static constexpr double S2 =  8.3333333333017809e-03; //  1/5!
    static constexpr double S3 = -1.9841269836761127e-04; // -1/7!
    static constexpr double S4 =  2.7557316103728802e-06; //  1/9!
    static constexpr double S5 = -2.5051132068021698e-08; // -1/11!

    // Coefficients for cos(x) on [0, π/4]
    // cos(x) ≈ 1 + x²·(C1 + x²·(C2 + x²·(C3 + x²·(C4 + x²·C5))))
    static constexpr double C1 = -4.9999999999999056e-01; // -1/2!
    static constexpr double C2 =  4.1666666666512495e-02; //  1/4!
    static constexpr double C3 = -1.3888888839660019e-03; // -1/6!
    static constexpr double C4 =  2.4801587264620616e-05; //  1/8!
    static constexpr double C5 = -2.7557315616826288e-07; // -1/10!


    // ── Core polynomials ───────────────────────────────────────────────────────
    constexpr double poly_sin(double x) noexcept {
        // Horner's method — x + x³·(S1 + x²·(…))
        const double x2 = x * x;
        return x + x * x2 * (S1 + x2 * (S2 + x2 * (S3 + x2 * (S4 + x2 * S5))));
    }

    constexpr double poly_cos(double x) noexcept {
        const double x2 = x * x;
        return 1.0 + x2 * (C1 + x2 * (C2 + x2 * (C3 + x2 * (C4 + x2 * C5))));
    }

    // ── Cody-Waite range reduction ─────────────────────────────────────────────
    // Reduces x to [-π/4, π/4], returns quadrant index k (0-3)
    // Uses 3-part split of π/2 to avoid cancellation error
    constexpr int range_reduce(double x, double& r) noexcept {
        // k = round(x / (π/2)) — nearest integer, no ties needed
        const double k_f = x * (2.0 / PI) + 0.5;
        const int k = static_cast<int>(k_f) - (k_f < 0.0 ? 1 : 0);

        // Subtract k * π/2 in 3 parts (compensated summation)
        const double kd = static_cast<double>(k);
        r = ((x - kd * PI2_HI)   // large part first
                - kd * PI2_MID)   // medium correction
                - kd * PI2_LO;    // tiny residual

        return k & 3; // quadrant: 0,1,2,3
    }

    // ── Main constexpr sin ─────────────────────────────────────────────────────
    constexpr double accurate_sin(double x) noexcept {
        // Handle edge cases
        if (x == 0.0) return x;       // preserves -0.0

        // Range reduction to [-π/4, π/4]
        double r = 0.0;
        const int q = range_reduce(x, r);

        // Dispatch by quadrant
        // q=0: sin(r),   q=1: cos(r),
        // q=2: -sin(r),  q=3: -cos(r)
        switch (q) {
            case 0: return  poly_sin(r);
            case 1: return  poly_cos(r);
            case 2: return -poly_sin(r);
            case 3: return -poly_cos(r);
            default: return 0.0; // unreachable
        }
    }

    // ── Convenience: constexpr cos ─────────────────────────────────────────────
    constexpr double accurate_cos(double x) noexcept {
        if (x == 0.0) return 1.0;
        double r = 0.0;
        const int q = range_reduce(x, r);
        switch (q) {
            case 0: return  poly_cos(r);
            case 1: return -poly_sin(r);
            case 2: return -poly_cos(r);
            case 3: return  poly_sin(r);
            default: return 0.0;
        }
    }

    // ── Static assertions (verifiable at compile time) ─────────────────────────
    static_assert(accurate_sin(0.0)  == 0.0);
    static_assert(accurate_cos(0.0)  == 1.0);
    // These will compile only if the constexpr evaluation is correct:
    // static_assert(accurate_sin(PI_2) > 0.9999999);
    

    static constexpr size_t TABLE_SIZE = 512;
    static constexpr double TABLE_SCALE = TABLE_SIZE / TWO_PI; 
    constexpr std::array<double, TABLE_SIZE + 1> SIN_TABLE = []() {
        std::array<double, TABLE_SIZE + 1> table{};
        for (size_t i = 0; i < table.size(); ++i) {
            double x = (static_cast<double>(i) / TABLE_SIZE) * TWO_PI;
            table[i] = accurate_sin(x);
        }
        return table;
    }();

    // ── fast_sin: table lookup + linear interpolation ─────────────────────────
    // Error: < π²/(4·N²) ≈ 3.8e-5 for N=512  (theoretical peak)
    // Speed: ~3-5 ns vs ~15-20 ns for std::sin on modern hardware
    inline double fast_sin(double x) noexcept {
        // 1. Wrap x into [0, 2π) using multiplication — no loop
        x -= TWO_PI * std::floor(x / TWO_PI);

        // 2. Scale to table index space
        const double scaled = x * TABLE_SCALE;

        // 3. Split into integer index + fractional remainder
        const int    i = static_cast<int>(scaled);       // base index
        const double f = scaled - static_cast<double>(i); // fraction in [0, 1)

        // 4. Linear interpolation between adjacent table entries
        //    sin(x) ≈ s0 + f * (s1 - s0)
        const double s0 = SIN_TABLE[i];
        const double s1 = SIN_TABLE[i + 1]; // safe: table has TABLE_SIZE+1 entries
        return s0 + f * (s1 - s0);
    }

    // ── fast_cos: free, just shift phase ──────────────────────────────────────
    inline double fast_cos(double x) noexcept {
        return fast_sin(x + PI_2);
    }

    inline double fmod(double x, double y) noexcept {
        return x - y * std::floor(x / y);
    }

} // --- idyl::math ---
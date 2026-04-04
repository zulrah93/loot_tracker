#ifndef TRUE_RNG_T_HPP
#define TRUE_RNG_T_HPP'

#include <vector>
#include <cstring>
#include <cstdint>
#include <limits>
#include "/usr/lib/gcc/x86_64-linux-gnu/15/include/cpuid.h"

// Purpose exploit the fact that x86 has an instruction to generate random from environment might have some performance implications
// So I wrapped that x86 instruction into a class within a header only library
class true_rng_t {
public:
    true_rng_t() : m_reseed_rate{0} {
        while (!get_new_seed(m_next_random_value)) {}
    }

    //reseed_rate a higher value trades true randomness for faster pseudo if you want to have every new random value be from rdseed use a 0 or 1 as rate
    true_rng_t(size_t reseed_rate) : m_reseed_rate{reseed_rate} {
        while (!get_new_seed(m_next_random_value)) {}
    }

    // Helper used to check if your x86-64 machine supports it
    static constexpr bool rdseed64_instruction_supported() {
        int32_t eax{7};
        int32_t ebx{0};
        int32_t ecx{0};
        int32_t edx{0};
        __cpuid(0, eax, ebx, ecx, edx);
        return (ebx & (1 << 18)) != 0; // If bit 18th is set then RDSEED is supported
    }

    size_t next_random_value() {
        [[unlinkely]]
        if (m_counter_to_reseed > 0 &&
                (m_reseed_rate == 0 || ((m_counter_to_reseed % m_reseed_rate) == 0))) {
             while (!get_new_seed(m_next_random_value)) {}
        }
        m_counter_to_reseed++;
        //Source: https://wiki.osdev.org/Random_Number_Generator#x86_RDSEED_Instruction
        m_next_random_value = (m_next_random_value * 1103515245) + 12345;
        return ((m_next_random_value / 65536) % std::numeric_limits<size_t>::max());
    }

    // Same as calling next_random_value() directly
    size_t operator()() {
        return next_random_value();
    }

    // *rng is the same as rng()
    size_t operator*() {
        return next_random_value();
    }

    // doing rng++ effectively reseeds
    void operator++(int) {
        while (!get_new_seed(m_next_random_value)) {}
    }

    // Min inclusive and max exclusive
    size_t operator()(const size_t& min_value, const size_t& max_value) {
        return (next_random_value() % max_value) + min_value;
    }

    constexpr const size_t get_reseeed_rate() {
        return m_reseed_rate;
    }

    void fill_buffer(std::vector<size_t>& random_values, size_t how_many) {
        while (how_many > 0) {
            random_values.emplace_back(next_random_value());
            how_many--;
        }
    }

    void fill_buffer(size_t* random_buffer, size_t how_many) {
        while (how_many > 0) {
            *random_buffer = next_random_value();
            random_buffer++;
            how_many--;
        }
    }

private:
    static constexpr bool get_new_seed(size_t& seed) {
        size_t temp_random_value{0};
        int got_carried{0};

        asm ("rdseed %0; "
            :"=r"(temp_random_value),
            "=@ccc"(got_carried));

        if ((1 == (got_carried & 1))) {
            seed = temp_random_value;
            return true;
        }

        return false;
    }
    size_t m_reseed_rate{0};
    size_t m_counter_to_reseed{0};
    size_t m_next_random_value{0};
};

#endif
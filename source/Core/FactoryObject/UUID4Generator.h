#ifndef UUID4GENERATOR_H
#define UUID4GENERATOR_H

#include "absl/numeric/int128.h"
#include "Utils.h"
#include <bit>

#ifndef ABSL_NUMERIC_INT128_H_

namespace UUID4Generator
{
    // UUID4
    class UUID4 final
    {
    public:

        // nested types
        struct TransparentComparator
        {
            using is_transparent = UUID4;

        public:

            bool operator()(UUID4 _A, UUID4 _B) const
            {
                return  _A < _B;
            }
        };

        // constructors
        UUID4(const uint_fast32_t& time_low,
              const uint_fast16_t& time_mid,
              const uint_fast16_t& time_hi_and_version,
              const uint_fast8_t&  clock_seq_hi_and_reserved,
              const uint_fast8_t&  clock_seq_low,
              const uint_fast64_t& node)
        {
            BITSET_UTILS::__read_bits_from_uint__(m_Key, time_low, 0, 32);
            BITSET_UTILS::__read_bits_from_uint__(m_Key, time_mid, 32, 48);
            BITSET_UTILS::__read_bits_from_uint__(m_Key, time_hi_and_version, 48, 64);
            BITSET_UTILS::__read_bits_from_uint__(m_Key, clock_seq_hi_and_reserved, 64, 72);
            BITSET_UTILS::__read_bits_from_uint__(m_Key, clock_seq_low, 72, 80);
            BITSET_UTILS::__read_bits_from_uint__(m_Key, node, 80, 128);
        }

        UUID4(const std::string& _GUID)
        {
            this->from_string(_GUID);
        }

        // destructor
        ~UUID4(){}

        // public methods
        std::string to_string(bool _Delimeted = false) const
        {
            // read GUID attributes
            std::bitset<32> time_low                  = BITSET_UTILS::__extract_bits__<128, 32>(m_Key, 0, 32);
            std::bitset<16> time_mid                  = BITSET_UTILS::__extract_bits__<128, 16>(m_Key, 32, 48);
            std::bitset<16> time_hi_and_version       = BITSET_UTILS::__extract_bits__<128, 16>(m_Key, 48, 64);
            std::bitset<8>  clock_seq_hi_and_reserved = BITSET_UTILS::__extract_bits__<128, 8>(m_Key, 64, 72);
            std::bitset<8>  clock_seq_low             = BITSET_UTILS::__extract_bits__<128, 8>(m_Key, 72, 80);
            std::bitset<48> node                      = BITSET_UTILS::__extract_bits__<128, 48>(m_Key, 80, 128);

            // convert to string
            if(_Delimeted)
            {
                return BITSET_UTILS::__write_to_hex_string__<32>(time_low) + "-" +
                       BITSET_UTILS::__write_to_hex_string__<16>(time_mid) + "-" +
                       BITSET_UTILS::__write_to_hex_string__<16>(time_hi_and_version) + "-" +
                       BITSET_UTILS::__write_to_hex_string__<8>(clock_seq_hi_and_reserved) +
                       BITSET_UTILS::__write_to_hex_string__<8>(clock_seq_low) + "-" +
                       BITSET_UTILS::__write_to_hex_string__<48>(node);
            }
            else
            {
                return BITSET_UTILS::__write_to_hex_string__<32>(time_low) +
                       BITSET_UTILS::__write_to_hex_string__<16>(time_mid) +
                       BITSET_UTILS::__write_to_hex_string__<16>(time_hi_and_version) +
                       BITSET_UTILS::__write_to_hex_string__<8>(clock_seq_hi_and_reserved) +
                       BITSET_UTILS::__write_to_hex_string__<8>(clock_seq_low) +
                       BITSET_UTILS::__write_to_hex_string__<48>(node);
            }
        }

        // operators overload
        operator std::bitset<128>() const
        {
            return m_Key;
        }

        bool operator == (const UUID4& _UUID) const
        {
            return BITSET_UTILS::__big_unsigned_numbers_equal__(m_Key, _UUID.m_Key);
        }

        bool operator != (const UUID4& _UUID) const
        {
            return !BITSET_UTILS::__big_unsigned_numbers_equal__(m_Key, _UUID.m_Key);
        }

        bool operator < (const UUID4& _UUID) const
        {
            return BITSET_UTILS::__big_unsigned_number_is_less__(m_Key, _UUID.m_Key);
        }

        bool operator > (const UUID4& _UUID) const
        {
            return BITSET_UTILS::__big_unsigned_number_is_greater__(m_Key, _UUID.m_Key);
        }

    private:

        // info
        std::bitset<128> m_Key;

        // define hex format table
        static uint_fast64_t convert_from_hex(const char& _Symbol)
        {
            if( _Symbol == '0' )
                return 0;

            if( _Symbol == '1' )
                return 1;

            if( _Symbol == '2' )
                return 2;

            if( _Symbol == '3' )
                return 3;

            if( _Symbol == '4' )
                return 4;

            if( _Symbol == '5' )
                return 5;

            if( _Symbol == '6' )
                return 6;

            if( _Symbol == '7' )
                return 7;

            if( _Symbol == '8' )
                return 8;

            if( _Symbol == '9' )
                return 9;

            if( _Symbol == 'a' )
                return 10;

            if( _Symbol == 'b' )
                return 11;

            if( _Symbol == 'c' )
                return 12;

            if( _Symbol == 'd' )
                return 13;

            if( _Symbol == 'e' )
                return 14;

            if( _Symbol == 'f' )
                return 15;

            return 16;
        }

        // service methods
        void read_with_delimeter(const std::string& _GUID)
        {
            // split input into sections
            std::vector< std::string > input = STRING_EXTENSION::__split__(_GUID, "-");

            // check input size
            if(input.empty() || input.size() < 5)
            {
                // show error here !!!
                return;
            }

            // check input sections size
            if(
                input[0].size() < 8 ||
                input[1].size() < 4 ||
                input[2].size() < 4 ||
                input[3].size() < 4 ||
                input[4].size() < 12 )
            {
                // show error here
                return;
            }

            // main code
            uint_fast8_t l = 0;
            for(uint_fast8_t i = 0 ; i < input.size() ; i++)
            {
                for(uint_fast8_t j = 0 ; j < input[i].size() ; j++)
                {
                    uint_fast64_t num = convert_from_hex(input[i][j]);

                    // check
                    if(num >= 16)
                    {
                        m_Key.reset();
                        return;
                    }

                    for(uint_fast8_t k = 0 ; k < 4 ; k++)
                    {
                        m_Key[l++] = BIT_UTILS::__get_bit__(num , k);
                    }
                }
            }
        }

        void read_without_delimeter(const std::string& _GUID)
        {
            // check input size
            if( _GUID.size() < 32 )
                return;

            // main code
            uint_fast8_t k = 0;
            for(uint_fast8_t i = 0 ; i < _GUID.size() ; i++)
            {
                uint_fast64_t num = convert_from_hex(_GUID[i]);

                // check parsed hex number
                if( num >= 16 )
                {
                    m_Key.reset();
                    return;
                }

                for(uint_fast8_t j = 0 ; j < 4 ; j++)
                {
                    m_Key[k++] = BIT_UTILS::__get_bit__(num , j);
                }
            }
        }

        void from_string(std::string _GUID)
        {
            if(STRING_EXTENSION::__string_contains__(_GUID, '-' ))
            {
                read_with_delimeter(_GUID);
            }
            else
            {
                read_without_delimeter(_GUID);
            }
        }
    };

    // UUID4Generator
    class UUID4Generator final
    {
    public:

        // constructors
        UUID4Generator()
        {
            // setup the seed
            m_PseudoRandomNumberGenerator.seed( (uint_fast64_t)this );

            // warmup pseudo random number generator
            for(uint_fast64_t i = 0; i < 1e2; i++)
                (void)m_PseudoRandomNumberGenerator();
        }

        // virtual destructor
        virtual ~UUID4Generator(){}

        // public methods
        UUID4 guid()
        {
            // generate 128 random bits
            uint_fast64_t m = m_PseudoRandomNumberGenerator();
            uint_fast64_t n = m_PseudoRandomNumberGenerator();

            // generate UUID attributes
            uint_fast32_t time_low                  = BIT_UTILS::__retrieve_bits__<uint_fast64_t, uint_fast32_t>(m, 0, 32);
            uint_fast16_t time_mid                  = BIT_UTILS::__retrieve_bits__<uint_fast64_t, uint_fast16_t>(m, 32, 48);
            uint_fast16_t time_hi_and_version       = BIT_UTILS::__retrieve_bits__<uint_fast64_t, uint_fast16_t>(m, 48, 64);
            uint_fast8_t  clock_seq_hi_and_reserved = BIT_UTILS::__retrieve_bits__<uint_fast64_t, uint_fast8_t>(n, 0, 8);
            uint_fast8_t  clock_seq_low             = BIT_UTILS::__retrieve_bits__<uint_fast64_t, uint_fast8_t>(n, 8, 16);
            uint_fast64_t node                      = BIT_UTILS::__retrieve_bits__<uint_fast64_t, uint_fast64_t>(n, 16, 64);

            // 1 Set the two most significant bits (bits 6 and 7) of the
            // clock_seq_hi_and_reserved to zero and one, respectively.
            BIT_UTILS::__reset_bit__<uint_fast8_t>(clock_seq_hi_and_reserved, 0);
            BIT_UTILS::__set_bit__<uint_fast8_t>(clock_seq_hi_and_reserved, 1);

            // 2 Set the four most significant bits (bits 12 through 15) of the
            // time_hi_and_version field to the 4-bit version number from RFC4122
            BIT_UTILS::__reset_bit__<uint_fast16_t>(time_hi_and_version, 0);
            BIT_UTILS::__reset_bit__<uint_fast16_t>(time_hi_and_version, 1);
            BIT_UTILS::__set_bit__<uint_fast16_t>(time_hi_and_version, 2);
            BIT_UTILS::__reset_bit__<uint_fast16_t>(time_hi_and_version, 3);

            return UUID4(
                time_low,
                time_mid,
                time_hi_and_version,
                clock_seq_hi_and_reserved,
                clock_seq_low, node
                );
        }

    protected:

        // info
        std::mt19937_64 m_PseudoRandomNumberGenerator;
    };
};

#else

namespace UUID4Generator
{
    // UUID4
    class UUID4 final
    {
    public:

        // nested types
        struct TransparentComparator
        {
            using is_transparent = UUID4;

        public:

            bool operator()(UUID4 _A, UUID4 _B) const
            {
                return  _A < _B;
            }
        };

        struct Hasher
        {
            std::size_t operator()(const UUID4Generator::UUID4& _Key) const
            {
                return absl::Uint128Low64(_Key.m_Key) ^ absl::Uint128High64(_Key.m_Key);
            }
        };

        // constructors
        UUID4(const absl::uint128& _UUID) :
            m_Key(_UUID){}

        UUID4(const std::string& _GUID)
        {
            this->from_string(_GUID);
        }

        // destructor
        ~UUID4(){}

        // public methods
        std::string to_string(bool _Delimited = false) const
        {
            if(_Delimited)
            {
                // retrieve bits
                std::bitset<128> bitset;

                for(int i = 0 ; i < 128; i++)
                    bitset[i] = BIT_UTILS::__get_bit__<absl::uint128>(m_Key, i);

                // read GUID attributes
                std::bitset<32> time_low                  = BITSET_UTILS::__extract_bits__<128, 32>(bitset, 0, 32);
                std::bitset<16> time_mid                  = BITSET_UTILS::__extract_bits__<128, 16>(bitset, 32, 48);
                std::bitset<16> time_hi_and_version       = BITSET_UTILS::__extract_bits__<128, 16>(bitset, 48, 64);
                std::bitset<8>  clock_seq_hi_and_reserved = BITSET_UTILS::__extract_bits__<128, 8>(bitset, 64, 72);
                std::bitset<8>  clock_seq_low             = BITSET_UTILS::__extract_bits__<128, 8>(bitset, 72, 80);
                std::bitset<48> node                      = BITSET_UTILS::__extract_bits__<128, 48>(bitset, 80, 128);

                // return a string where GUID attributes are delimited by '-'
                return BITSET_UTILS::__write_to_hex_string__<32>(time_low) + "-" +
                       BITSET_UTILS::__write_to_hex_string__<16>(time_mid) + "-" +
                       BITSET_UTILS::__write_to_hex_string__<16>(time_hi_and_version) + "-" +
                       BITSET_UTILS::__write_to_hex_string__<8>(clock_seq_hi_and_reserved) +
                       BITSET_UTILS::__write_to_hex_string__<8>(clock_seq_low) + "-" +
                       BITSET_UTILS::__write_to_hex_string__<48>(node);
            }

            // return undelimited string
            return BITSET_UTILS::__write_to_hex_string__<64>(absl::Uint128Low64(m_Key)) +
                   BITSET_UTILS::__write_to_hex_string__<64>(absl::Uint128High64(m_Key));
        }

        operator absl::uint128()
        {
            return m_Key;
        }

        bool operator == (const UUID4& _UUID) const
        {
            return m_Key == _UUID.m_Key;
        }

        bool operator != (const UUID4& _UUID) const
        {
            return m_Key != _UUID.m_Key;
        }

        bool operator < (const UUID4& _UUID) const
        {
            return m_Key < _UUID.m_Key;
        }

        bool operator > (const UUID4& _UUID) const
        {
            return m_Key > _UUID.m_Key;
        }

    private:

        // info
        absl::uint128 m_Key;

        // servise methods
        template<typename Uint>
        void bit_set_to(Uint& number, int n, bool x)
        {
            number = (number & ~((Uint)1 << n)) | ((Uint)x << n);
        }

        static uint_fast64_t convert_from_hex(const char& _Symbol)
        {
            if( _Symbol == '0' )
                return 0;

            if( _Symbol == '1' )
                return 1;

            if( _Symbol == '2' )
                return 2;

            if( _Symbol == '3' )
                return 3;

            if( _Symbol == '4' )
                return 4;

            if( _Symbol == '5' )
                return 5;

            if( _Symbol == '6' )
                return 6;

            if( _Symbol == '7' )
                return 7;

            if( _Symbol == '8' )
                return 8;

            if( _Symbol == '9' )
                return 9;

            if( _Symbol == 'a' )
                return 10;

            if( _Symbol == 'b' )
                return 11;

            if( _Symbol == 'c' )
                return 12;

            if( _Symbol == 'd' )
                return 13;

            if( _Symbol == 'e' )
                return 14;

            if( _Symbol == 'f' )
                return 15;

            return 16;
        }

        void read_with_delimeter(const std::string& _GUID)
        {
            // split input into sections
            std::vector<std::string> input = STRING_EXTENSION::__split__(_GUID, "-");

            // check input size
            if( input.empty() || input.size() < 5 )
            {
                // show error here !!!
                return;
            }

            // check input sections size
            if(
                input[0].size() < 8 ||
                input[1].size() < 4 ||
                input[2].size() < 4 ||
                input[3].size() < 4 ||
                input[4].size() < 12 )
            {
                // show error here
                return;
            }

            // main code
            uint_fast8_t l = 0;
            for( uint_fast8_t i = 0 ; i < input.size() ; i++ )
            {
                for( uint_fast8_t j = 0 ; j < input[i].size() ; j++ )
                {
                    uint_fast64_t num = UUID4::convert_from_hex(input[i][j]);

                    // check
                    if(num >= 16)
                    {
                        m_Key = m_Key & 0;
                        return;
                    }

                    for( uint_fast8_t k = 0 ; k < 4 ; k++ )
                    {
                        bit_set_to<absl::uint128>(
                            m_Key,
                            l++,
                            BIT_UTILS::__get_bit__(num , k));
                    }
                }
            }
        }

        void read_without_delimeter(const std::string& _GUID)
        {
            // check input size
            if(_GUID.size() < 32 ||
                _GUID.size() > 32)
                return;

            m_Key = m_Key & 0;

            // main code
            uint_fast8_t k = 0;
            for(uint_fast8_t i = 0 ; i < _GUID.size() ; i++)
            {
                uint_fast64_t num = UUID4::convert_from_hex(_GUID[i]);

                // check parsed hex number
                if(num >= 16)
                {
                    m_Key = m_Key & 0;
                    return;
                }

                for(uint_fast8_t j = 0 ; j < 4 ; j++)
                {
                    bit_set_to<absl::uint128>(
                        m_Key,
                        k++,
                        BIT_UTILS::__get_bit__(num , j));
                }
            }
        }

        void from_string(const std::string& _GUID)
        {
            if(STRING_EXTENSION::__string_contains__(_GUID, '-'))
            {
                read_with_delimeter(_GUID);
            }
            else
            {
                read_without_delimeter(_GUID);
            }
        }
    };

    // UUID4Generator
    class UUID4Generator final
    {
    public:

        // constructors
        UUID4Generator()
        {
            // setup the seed
            m_PseudoRandomNumberGenerator.seed((uint_fast64_t)this);

            // warmup pseudo random number generator
            for(uint_fast64_t i = 0; i < 1e2; i++)
                (void)m_PseudoRandomNumberGenerator();
        }

        // virtual destructor
        virtual ~UUID4Generator(){}

        // public methods
        UUID4 guid()
        {
            // generate 128 random bits
            uint_fast64_t high = m_PseudoRandomNumberGenerator();
            uint_fast64_t low  = m_PseudoRandomNumberGenerator();

            // The two masks set the uuid version (4) and variant (1)
            absl::uint128 and_mask = absl::MakeUint128(0xFFFFFFFFFFFFFF3Full, 0xFF0FFFFFFFFFFFFFull);
            absl::uint128 or_mask  = absl::MakeUint128(0x0000000000000080ull, 0x0040000000000000ull);

            return UUID4((absl::MakeUint128(high, low) & and_mask) | or_mask);
        }

    protected:

        // info
        std::mt19937_64 m_PseudoRandomNumberGenerator;
    };

}

#endif

#endif // UUID4GENERATOR_H

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

#include "catch2/catch_all.hpp"

#include "extensions/serialization.hpp"

using namespace Extensions::Serialization;

TEMPLATE_TEST_CASE("Integer (De)Serialization Tests 1", "", 
    int, char, short, long, long long, 
    uint32_t, uint64_t, uint16_t, uint8_t
)
{
    std::array<char, sizeof(TestType)> outBuf { };
    std::array<char, sizeof(TestType)> desiredBuf { 0x0A };

    for (size_t i = 1; i < desiredBuf.size(); i++)
        desiredBuf[i] = 0x00;

    // Write
    {
        std::stringstream ss;
        ss.rdbuf()->pubsetbuf(outBuf.data(), outBuf.size());

        TestType integer { static_cast<TestType>(0x0A) };
        SerializeInteger(integer, ss);

        CHECK(outBuf == desiredBuf);
    }

    // Read
    {
        std::stringstream ss;
        ss.rdbuf()->pubsetbuf(outBuf.data(), outBuf.size());

        TestType integer {};
        DeserializeInteger(integer, ss);

        CHECK(integer == 0x0A);
    }
}

TEMPLATE_TEST_CASE("Integer (De)Serialization Tests 2", "",
    int, char, short, long, long long,
    uint32_t, uint64_t, uint16_t, uint8_t
)
{
    TestType g { static_cast<TestType>(0x0F0A) };
    std::ofstream tmpOut { "tmp.bin", std::ios::binary }; 
    SerializeInteger(g, tmpOut);
    tmpOut.close();

    TestType h { 0x00 };
    std::ifstream tmpIn { "tmp.bin", std::ios::binary };
    DeserializeInteger(h, tmpIn);
    tmpIn.close();

    std::filesystem::remove("tmp.bin");

    CHECK(g == h);
}

TEST_CASE("Float (De)Serialization Test 1")
{
    std::array<unsigned char, sizeof(float)> outBuf { };
    std::array<unsigned char, sizeof(float)> desiredBuf { 
        0x29,
        0x5C,
        0xAF,
        0x40
    };

    // Write
    {
        std::stringstream ss;
        ss.rdbuf()->pubsetbuf((char*)outBuf.data(), outBuf.size());

        float fl { 5.480000019073486328125 };
        SerializeFloat(fl, ss);

        CHECK(outBuf == desiredBuf);
    }

    // Read
    {
        std::stringstream ss;
        ss.rdbuf()->pubsetbuf((char*)outBuf.data(),outBuf.size());

        float fl {};
        DeserializeFloat(fl, ss);

        CHECK(fl == 5.480000019073486328125);
    }

}

TEST_CASE("Float (De)Serialization Test 2")
{
    float g { static_cast<float>(0x0F0A) };
    std::ofstream tmpOut { "tmp.bin", std::ios::binary }; 
    SerializeFloat(g, tmpOut);
    tmpOut.close();

    float h { 0x00 };
    std::ifstream tmpIn { "tmp.bin", std::ios::binary };
    DeserializeFloat(h, tmpIn);
    tmpIn.close();

    std::filesystem::remove("tmp.bin");

    CHECK(g == h);
}

TEST_CASE("Container (De)Serialization Test [Basic]")
{
    // I don't really want to make tests for complex
    // and custom data types. If you write them accordingly
    // they'll be okay. I believe you.

    // 1 byte for size
    // sizeof(uint32_t) * 5 byte for data (4*5 = 20)
    const std::array<uint32_t, 5> container { 1, 3, 8, 15, 76 };

    std::array<char, 21> outBuf { }; 

    // Write
    {
        const std::array<char, 21> desiredBuf { 
            0x05,
            0x01, 0x00, 0x00, 0x00,
            0x03, 0x00, 0x00, 0x00,
            0x08, 0x00, 0x00, 0x00,
            0x0F, 0x00, 0x00, 0x00,
            0x4C, 0x00, 0x00, 0x00
        };

        std::stringstream ss;
        ss.rdbuf()->pubsetbuf(outBuf.data(), outBuf.size());

        SerializeContainer<std::array<uint32_t, 5>, char, uint32_t>(container, ss, &SerializeInteger<uint32_t>);

        CHECK(desiredBuf == outBuf);
    }

    // Read
    {
        std::stringstream ss;
        ss.rdbuf()->pubsetbuf(outBuf.data(), outBuf.size());
        
        std::vector<uint32_t> resCont { };

        DeserializeContainer<std::vector<uint32_t>, char, uint32_t>(resCont, ss, &DeserializeInteger<uint32_t>);

        CHECK(container.size() == resCont.size());

        for (size_t i = 0; i < resCont.size(); i++)
            CHECK(resCont.at(i) == container.at(i));
    }
}

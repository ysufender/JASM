#include "extensions/serialization.hpp"
#include <streambuf>

namespace Extensions::Serialization
{
    void DeserializeFloat(float& data, std::istream& stream)
    {
        float temp;
        stream.read(reinterpret_cast<char*>(&temp), sizeof(float));

        if (std::endian::native == std::endian::big)
            data = temp;
        else
        {
            char* floatToConvert = (char*)&temp;
            char* returnFloat = (char*)&data;

            returnFloat[0] = floatToConvert[3];
            returnFloat[1] = floatToConvert[2];
            returnFloat[2] = floatToConvert[1];
            returnFloat[3] = floatToConvert[0];
        }
    }

    void SerializeFloat(const float& data, std::ostream& stream)
    {
        if (std::endian::native == std::endian::big)
            stream.write(reinterpret_cast<const char*>(&data), sizeof(float));
        else
        {
            float temp;
            char* floatToConvert = (char*)&data;
            char* returnFloat = (char*)&temp;

            returnFloat[0] = floatToConvert[3];
            returnFloat[1] = floatToConvert[2];
            returnFloat[2] = floatToConvert[1];
            returnFloat[3] = floatToConvert[0];

            stream.write(reinterpret_cast<char*>(&temp), sizeof(data));
        }
    }
}

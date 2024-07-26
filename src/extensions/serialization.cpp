#include "extensions/serialization.hpp"

namespace Extensions::Serialization
{
    void DeserializeFloat(float& data, std::ifstream& stream)
    {
        float temp;
        stream.read(reinterpret_cast<char*>(&temp), sizeof(float));

        if (std::endian::native == std::endian::little)
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
}
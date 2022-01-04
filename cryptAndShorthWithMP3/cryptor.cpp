#include "cryptor.h"

std::string cryptor::Code(const std::string &mes,
                          const std::string &key,
                          const uint32_t    round) const
{
    std::string encodMes = mes;

    char temp = '0', l = '0', r = '0';

    for (uint32_t desc = 0; desc < mes.size(); desc += 2)
    {
        l = mes[desc];
        r = mes[desc + 1];
        for (uint32_t i = 0; i < round; ++i)
        {
            temp = r ^ fo(l, key, i);
            r = l;
            l = temp;
        }
        encodMes[desc] = l;
        encodMes[desc + 1] = r;
    }

    return encodMes;
}

std::string cryptor::DeCode(const std::string &mes,
                            const std::string &key,
                            const int32_t    round) const
{
    std::string decodMes = mes;

    char temp = '0', l = '0', r = '0';

    for (uint32_t desc = 0; desc < mes.size(); desc += 2)
    {
        l = mes[desc];
        r = mes[desc + 1];
        for (int32_t i = round-1; i >= 0; --i)
        {
            temp = l ^ fo(r, key, i);
            l = r;
            r = temp;
        }

        decodMes[desc] = l;
        decodMes[desc + 1] = r;
    }

    return decodMes;
}

char cryptor::fo(const char        subblock,
                 const std::string &key,
                 const int32_t     round) const
{
    return (subblock * key[round]) % static_cast<char>(pow(2, round)+1);
}

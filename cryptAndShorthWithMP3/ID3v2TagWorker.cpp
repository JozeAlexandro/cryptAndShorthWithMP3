#include "ID3v2TagWorker.h"

ID3v2TagWorker::ID3v2TagWorker(std::string filename) : filename(filename) {}

std::string ID3v2TagWorker::readMessage() const
{
    std::ifstream reader;

    reader.open(filename, std::ios_base::in);
    if(!reader.is_open())
    {
        return "";
    }

    const size_t size = getMessageSize();

    if(!size)
    {
        return "";
    }

    const uint32_t SIZE_ENCR_BYTE = 1;

    reader.seekg(ID3v2_HEADER_SIZE + FRAME_HEADER_SIZE + SIZE_ENCR_BYTE,
                 std::ios_base::beg);

    char *message = new char[size];

    reader.read(message, size);

    std::string forReturn(message);

    delete []message;
    return forReturn;
}

void ID3v2TagWorker::writeMessage(const std::string message)
{
    std::ifstream reader;

    reader.open(filename, std::ios_base::in);
    if(!reader.is_open())
    {
        return;
    }

    /// Копируем старый заголовок
    ID3v2Header tagHeader{};
    reader.read(reinterpret_cast<char*>(&tagHeader), ID3v2_HEADER_SIZE);

    uint32_t *pA = &tagHeader.size;
    const uint32_t tagSize = id3v2_get_size(reinterpret_cast<uint8_t*>(pA)) +
                             ID3v2_HEADER_SIZE;

    /// Запоминаем данные о звуке
    uint32_t fileSize = std::filesystem::file_size(
                std::filesystem::current_path() / filename);
    const uint32_t bufferSize = fileSize - tagSize;
    std::string audioBuffer;
    audioBuffer.reserve(bufferSize);

    reader.seekg(tagSize, std::ios_base::beg);

    while(!reader.eof())
    {
        audioBuffer.push_back(reader.get());
    }

    frameHeader headerTALB {};
    headerTALB.id[0] = 'T';
    headerTALB.id[1] = 'A';
    headerTALB.id[2] = 'L';
    headerTALB.id[3] = 'B';

    /// Обновление рамера сообщения в заголовке тэга
    const uint32_t messageSize = message.size();

    headerTALB.size = messageSize + 1; /// 1 - encoding byte

    uint32_t tagSizeExcludeTagHeader = headerTALB.size + FRAME_HEADER_SIZE;

    tagHeader.size = convert32BitSizeTo28BitSize(
                reinterpret_cast<uint8_t *>(&tagSizeExcludeTagHeader));

    const uint8_t encodingByte = 0x01;

    /// Перезапись файла
    std::ofstream writer;

    writer.open(filename, std::ios_base::out);
    if(!writer.is_open())
    {
        return;
    }

    tagHeader.size = __builtin_bswap32(tagHeader.size);

    writer.write(reinterpret_cast<char*>(&tagHeader), ID3v2_HEADER_SIZE);
    writer.write(reinterpret_cast<char*>(&headerTALB), FRAME_HEADER_SIZE);
    writer << encodingByte;
    writer << message;
    writer << audioBuffer;

    /// Запись инфо в ID3v1
    reader.seekg(-ID3v1_HEADER_SIZE, std::ios_base::end);
    ID3v1Header ID3v1;
    reader.read(reinterpret_cast<char*>(&ID3v1), ID3v1_HEADER_SIZE);

    ID3v1.tag[0] = ID3v1.tag[1] = ID3v1.tag[2] = STOP_SIGN;
    int32_t *size = reinterpret_cast<int32_t*>(&ID3v1.name);
    *size = messageSize;

    writer.seekp(-ID3v1_HEADER_SIZE, std::ios_base::end);

    writer.write(reinterpret_cast<char*>(ID3v1.tag), ID3v1_HEADER_SIZE);

    writer.flush();

    reader.close();
    writer.close();
}

size_t ID3v2TagWorker::getMessageSize() const
{
    if(!isFileHasMessage())
    {
        return 0;
    }
    else
    {
        std::ifstream reader;

        reader.open(filename, std::ios_base::in);
        if(!reader.is_open())
        {
            return false;
        }

        reader.seekg(-ID3v1_HEADER_SIZE, std::ios_base::end);

        ID3v1Header ID3v1{};

        reader.read(reinterpret_cast<char*>(&ID3v1), ID3v1_HEADER_SIZE);

        int32_t *size = reinterpret_cast<int32_t*>(&ID3v1.name);

        return *size;
    }
}

bool ID3v2TagWorker::isFileHasMessage() const
{
    std::ifstream reader;

    reader.open(filename, std::ios_base::in);
    if(!reader.is_open())
    {
        return false;
    }

    ID3v1Header header;

    reader.seekg(-ID3v1_HEADER_SIZE, std::ios_base::end);

    reader.read(reinterpret_cast<char*>(&header), ID3v1_HEADER_SIZE);

    if(header.tag[0] != STOP_SIGN ||
       header.tag[1] != STOP_SIGN ||
       header.tag[2] != STOP_SIGN)
    {
        return false;
    }

    return true;
}

int32_t id3v2_get_size(const uint8_t *p)
{
    return ((uint32_t)p[0] << 21) +
            ((uint32_t)p[1] << 14) +
            ((uint32_t)p[2] << 7 ) +
            p[3];
}

uint32_t convert32BitSizeTo28BitSize(const uint8_t *size32)
{
    const uint32_t max28BitValue = 0xFFFFFFF;

    uint32_t size28 = 0;
    if(false)
    {
        size28 = max28BitValue;
    }
    else
    {
        const uint8_t firstByte = (size32[3] << 3) |
                ((size32[2] >> 5) & 0x07);
        const uint8_t secondByte = ((size32[2] & 0x1F) << 2) |
                ((size32[1] >> 6) & 0x03);
        const uint8_t thirdByte = ((size32[1] & 0x3F) << 1) |
                ((size32[0] >> 7) & 0X01);
        const uint8_t fourthByte = size32[0] & 0x7F;


        size28  = static_cast<uint32_t>(firstByte << 24);
        size28 |= static_cast<uint32_t>(secondByte << 16);
        size28 |= static_cast<uint32_t>(thirdByte << 8);
        size28 |= static_cast<uint32_t>(fourthByte);
    }

    return size28;
}

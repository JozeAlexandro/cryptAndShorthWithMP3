/** ****************************************************************************
 * Класс для шифрования сообщений
 *
 * \file cryptor.h
 * ****************************************************************************/

#ifndef CRYPTOR_H
#define CRYPTOR_H

#include <cmath>
#include <string>

class cryptor
{
public:
    /** ************************************************************************
     * \brief Зашифровать сообщение
     *
     * \param [in] mes   Исходное сообщение
     * \param [in] key   Ключ
     * \param [in] round Количество раундов
     *
     * \return Зашифрованное сообщение
     * ************************************************************************/
    std::string Code(const std::string &mes,
                     const std::string &key,
                     const uint32_t    round) const;

    /** ************************************************************************
     * \brief Расшифровать сообщение
     *
     * \param [in] mes   Зашифрованное сообщение
     * \param [in] key   Ключ
     * \param [in] round Количество раундов
     *
     * \return Исходное сообщение
     * ************************************************************************/
    std::string DeCode(const std::string &mes,
                       const std::string &key,
                       const int32_t     round) const;

private:
    /** ************************************************************************
     * \brief Образующая функция
     *
     * \param [in] mes   Блок данных
     * \param [in] key   Ключ
     * \param [in] round Раунд
     * ************************************************************************/
    char fo(const char        subblock,
            const std::string &key,
            const int32_t     round) const;
};
#endif // CRYPTOR_H

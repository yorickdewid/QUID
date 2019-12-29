/*
* Copyright (c) 2012-2020, Yorick de Wid <yorick17 at outlook dot com>
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*   * Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*   * Neither the name of Redis nor the names of its contributors may be used
*     to endorse or promote products derived from this software without
*     specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __QUIDPP_H__
#define __QUIDPP_H__

#ifdef _WIN32
# pragma once
#endif

#include <string>
#include <memory.h>
#include <cassert>

#include <quid.h>

namespace quidpp
{

class InvalidQuidException {/*TODO set what()*/ };

class Quid
{
    quid_t quid;

    inline void GenerateNewLocalGuid()
    {
        quid_create(&this->quid);
    }

    void CopyToLocalGuid(const Quid& other)
    {
        assert(memcpy(this->quid, quid, sizeof(quid_t)));
    }

    void StringToQuid(const char *strquid)
    {
        char rs = strquid_format(strquid);
        if (!rs) {
            throw InvalidQuidException();
        }

        strtoquid(strquid, &this->quid);
    }

public:
    /**
    * Default constrcutor generates a new Quid object.
    */
    Quid()
    {
        GenerateNewLocalGuid();
    }

    /**
    * Copy constructor will copy the provided object to the
    * internal Quid structure.
    *
    * @param   other  Quid object
    */
    Quid(const Quid& other)
    {
        CopyToLocalGuid(other);
    }

    /**
    * Move constructor will consume the provided object and copy it
    * to the internal Quid structure. This operation is destructive
    * and the provided object cannot be used afterwards.
    *
    * @param   other  Quid object
    */
    Quid(Quid&& other)
    {
        CopyToLocalGuid(other);
    }

    /**
    * Constructor accepting a string as input. The string is supposted to
    * be a new Quid object. The constructor will try to convert the provided
    * string as Quid object. If the conversion fails, an exception is thrown.
    *
    * @param   cquid  Quid as std::string
    * @throws         InvalidQuidException
    */
    Quid(const std::string& cquid)
        : Quid(cquid.c_str())
    {
    }

    explicit Quid(const char *strquid)
    {
        StringToQuid(strquid);
    }

    size_t UnpackedSize = UNPACKED_LENGTH;

    inline void Uniform(char node[4]) const noexcept
    {
        assert(memcpy(node, this->quid.node, sizeof(node)));
    }

    std::string Uniform() const
    {
        char u[4];
        this->Uniform(u);
        return std::string(u, 4);
    }

    std::string ToString(bool useCompactFormat = false) const noexcept
    {
        char s[40];
        quidtostr(s, &this->quid);

        if (useCompactFormat) { return this->Crop(s); }

        return std::string(s);
    }

    bool operator==(const Quid& other) const noexcept
    {
        return quidcmp(&this->quid, &other.quid) == 1;
    }

    bool operator!=(const Quid& other) const noexcept
    {
        return quidcmp(&this->quid, &other.quid) == 0;
    }

    /**
    * Write Quid object to output stream.
    *
    * @param   os  Output file stream
    * @param   qt  Quid to write to output stream
    * @return      Output stream
    */
    friend std::ostream& operator<<(std::ostream& os, const Quid& qt)
    {
        os << qt.ToString();
        return os;
    }

public:
    static std::string Crop(const std::string& cquid)
    {
        if (cquid.size() == (UNPACKED_LENGTH + 2) && cquid[0] == '{' && cquid[UNPACKED_LENGTH + 1] == '}') {
            return cquid.substr(1, UNPACKED_LENGTH);
        }

        return "";
    }

    static Quid NewGuid()
    {
        return std::move(Quid{});
    }
};

}

#endif // __QUIDPP_H__

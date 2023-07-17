// Copyright (c) 2009 Satoshi Nakamoto
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.

class CTransaction;

enum
{
    SIGHASH_ALL = 1,
    SIGHASH_NONE = 2,
    SIGHASH_SINGLE = 3,
    SIGHASH_ANYONECANPAY = 0x80,
};

// Enum for opcode types
enum opcodetype
{
    // push value
    OP_0=0, // Pushes the value 0 onto the stack
    OP_FALSE=OP_0, // An alias for OP_0
    OP_PUSHDATA1=76, // Pushes the next byte of data onto the stack
    OP_PUSHDATA2, // Pushes the next 2 bytes of data onto the stack
    OP_PUSHDATA4, // Pushes the next 4 bytes of data onto the stack
    OP_1NEGATE, // Pushes -1 onto the stack
    OP_RESERVED, // Reserved for future use
    OP_1, // Pushes 1 onto the stack
    OP_TRUE=OP_1, // An alias for OP_1
    // ... continue for all opcodes
};

// Function to get the name of an opcode
inline const char* GetOpName(opcodetype opcode)
{
    // Switch on the opcode type
    switch (opcode)
    {
        // push value
        case OP_0                      : return "0"; // Returns the string "0" for OP_0
        case OP_PUSHDATA1              : return "OP_PUSHDATA1"; // Returns the string "OP_PUSHDATA1" for OP_PUSHDATA1
        case OP_PUSHDATA2              : return "OP_PUSHDATA2"; // Returns the string "OP_PUSHDATA2" for OP_PUSHDATA2
        case OP_PUSHDATA4              : return "OP_PUSHDATA4"; // Returns the string "OP_PUSHDATA4" for OP_PUSHDATA4
        case OP_1NEGATE                : return "-1"; // Returns the string "-1" for OP_1NEGATE
        case OP_RESERVED               : return "OP_RESERVED"; // Returns the string "OP_RESERVED" for OP_RESERVED
        case OP_1                      : return "1"; // Returns the string "1" for OP_1
        // ... continue for all opcodes
    }
};

inline string ValueString(const vector<unsigned char>& vch)
{
    if (vch.size() <= 4)
        return strprintf("%d", CBigNum(vch).getint());
    else
        return HexNumStr(vch.begin(), vch.end());
        //return string("(") + HexStr(vch.begin(), vch.end()) + string(")");
}

inline string StackString(const vector<vector<unsigned char> >& vStack)
{
    string str;
    foreach(const vector<unsigned char>& vch, vStack)
    {
        if (!str.empty())
            str += " ";
        str += ValueString(vch);
    }
    return str;
}

class CScript : public vector<unsigned char>
{
protected:
    CScript& push_int64(int64 n)
    {
        if (n == -1 || (n >= 1 && n <= 16))
        {
            push_back(n + (OP_1 - 1));
        }
        else
        {
            CBigNum bn(n);
            *this << bn.getvch();
        }
        return (*this);
    }

    CScript& push_uint64(uint64 n)
    {
        if (n == -1 || (n >= 1 && n <= 16))
        {
            push_back(n + (OP_1 - 1));
        }
        else
        {
            CBigNum bn(n);
            *this << bn.getvch();
        }
        return (*this);
    }

public:
    CScript() { }
    CScript(const CScript& b) : vector<unsigned char>(b.begin(), b.end()) { }
    CScript(const_iterator pbegin, const_iterator pend) : vector<unsigned char>(pbegin, pend) { }
#ifndef _MSC_VER
    CScript(const unsigned char* pbegin, const unsigned char* pend) : vector<unsigned char>(pbegin, pend) { }
#endif

    CScript& operator+=(const CScript& b)
    {
        insert(end(), b.begin(), b.end());
        return *this;
    }

    friend CScript operator+(const CScript& a, const CScript& b)
    {
        CScript ret = a;
        ret += b;
        return (ret);
    }

    explicit CScript(char b)           { operator<<(b); }
    explicit CScript(short b)          { operator<<(b); }
    explicit CScript(int b)            { operator<<(b); }
    explicit CScript(long b)           { operator<<(b); }
    explicit CScript(int64 b)          { operator<<(b); }
    explicit CScript(unsigned char b)  { operator<<(b); }
    explicit CScript(unsigned int b)   { operator<<(b); }
    explicit CScript(unsigned short b) { operator<<(b); }
    explicit CScript(unsigned long b)  { operator<<(b); }
    explicit CScript(uint64 b)         { operator<<(b); }

    explicit CScript(opcodetype b)     { operator<<(b); }
    explicit CScript(const uint256& b) { operator<<(b); }
    explicit CScript(const CBigNum& b) { operator<<(b); }
    explicit CScript(const vector<unsigned char>& b) { operator<<(b); }

    CScript& operator<<(char b)           { return (push_int64(b)); }
    CScript& operator<<(short b)          { return (push_int64(b)); }
    CScript& operator<<(int b)            { return (push_int64(b)); }
    CScript& operator<<(long b)           { return (push_int64(b)); }
    CScript& operator<<(int64 b)          { return (push_int64(b)); }
    CScript& operator<<(unsigned char b)  { return (push_uint64(b)); }
    CScript& operator<<(unsigned int b)   { return (push_uint64(b)); }
    CScript& operator<<(unsigned short b) { return (push_uint64(b)); }
    CScript& operator<<(unsigned long b)  { return (push_uint64(b)); }
    CScript& operator<<(uint64 b)         { return (push_uint64(b)); }

    CScript& operator<<(opcodetype opcode)
    {
        if (opcode <= OP_SINGLEBYTE_END)
        {
            insert(end(), (unsigned char)opcode);
        }
        else
        {
            assert(opcode >= OP_DOUBLEBYTE_BEGIN);
            insert(end(), (unsigned char)(opcode >> 8));
            insert(end(), (unsigned char)(opcode & 0xFF));
        }
        return (*this);
    }

    CScript& operator<<(const uint160& b)
    {
        insert(end(), sizeof(b));
        insert(end(), (unsigned char*)&b, (unsigned char*)&b + sizeof(b));
        return (*this);
    }

    CScript& operator<<(const uint256& b)
    {
        insert(end(), sizeof(b));
        insert(end(), (unsigned char*)&b, (unsigned char*)&b + sizeof(b));
        return (*this);
    }

    CScript& operator<<(const CBigNum& b)
    {
        *this << b.getvch();
        return (*this);
    }

    CScript& operator<<(const vector<unsigned char>& b)
    {
        if (b.size() < OP_PUSHDATA1)
        {
            insert(end(), (unsigned char)b.size());
        }
        else if (b.size() <= 0xff)
        {
            insert(end(), OP_PUSHDATA1);
            insert(end(), (unsigned char)b.size());
        }
        else
        {
            insert(end(), OP_PUSHDATA2);
            unsigned short nSize = b.size();
            insert(end(), (unsigned char*)&nSize, (unsigned char*)&nSize + sizeof(nSize));
        }
        insert(end(), b.begin(), b.end());
        return (*this);
    }

    CScript& operator<<(const CScript& b)
    {
        // I'm not sure if this should push the script or concatenate scripts.
        // If there's ever a use for pushing a script onto a script, delete this member fn
        assert(("warning: pushing a CScript onto a CScript with << is probably not intended, use + to concatenate", false));
        return (*this);
    }

    bool GetOp(iterator& pc, opcodetype& opcodeRet, vector<unsigned char>& vchRet)
    {
         // This is why people hate C++
         const_iterator pc2 = pc;
         bool fRet = GetOp(pc2, opcodeRet, vchRet);
         pc = begin() + (pc2 - begin());
         return fRet;
    }

    bool GetOp(const_iterator& pc, opcodetype& opcodeRet, vector<unsigned char>& vchRet) const
    {
        opcodeRet = OP_INVALIDOPCODE;
        vchRet.clear();
        if (pc >= end())
            return false;

        // Read instruction
        unsigned int opcode = *pc++;
        if (opcode >= OP_SINGLEBYTE_END)
        {
            if (pc + 1 > end())
                return false;
            opcode <<= 8;
            opcode |= *pc++;
        }

        // Immediate operand
        if (opcode <= OP_PUSHDATA4)
        {
            unsigned int nSize = opcode;
            if (opcode == OP_PUSHDATA1)
            {
                if (pc + 1 > end())
                    return false;
                nSize = *pc++;
            }
            else if (opcode == OP_PUSHDATA2)
            {
                if (pc + 2 > end())
                    return false;
                nSize = 0;
                memcpy(&nSize, &pc[0], 2);
                pc += 2;
            }
            else if (opcode == OP_PUSHDATA4)
            {
                if (pc + 4 > end())
                    return false;
                memcpy(&nSize, &pc[0], 4);
                pc += 4;
            }
            if (pc + nSize > end())
                return false;
            vchRet.assign(pc, pc + nSize);
            pc += nSize;
        }

        opcodeRet = (opcodetype)opcode;
        return true;
    }

    void FindAndDelete(const CScript& b)
    {
        iterator pc = begin();
        opcodetype opcode;
        vector<unsigned char> vchPushValue;
        int count = 0;
        do
        {
            while (end() - pc >= b.size() && memcmp(&pc[0], &b[0], b.size()) == 0)
            {
                erase(pc, pc + b.size());
                count++;
            }
        }
        while (GetOp(pc, opcode, vchPushValue));
        //printf("FindAndDeleted deleted %d items\n", count); /// debug
    }

    void PrintHex() const
    {
        printf("CScript(%s)\n", HexStr(begin(), end()).c_str());
    }

    string ToString() const
    {
        string str;
        opcodetype opcode;
        vector<unsigned char> vch;
        const_iterator it = begin();
        while (GetOp(it, opcode, vch))
        {
            if (!str.empty())
                str += " ";
            if (opcode <= OP_PUSHDATA4)
                str += ValueString(vch);
            else
                str += GetOpName(opcode);
        }
        return str;
    }

    void print() const
    {
        printf("%s\n", ToString().c_str());
    }
};

bool EvalScript(const CScript& script, const CTransaction& txTo, unsigned int nIn, int nHashType=0,
                vector<vector<unsigned char> >* pvStackRet=NULL);
uint256 SignatureHash(CScript scriptCode, const CTransaction& txTo, unsigned int nIn, int nHashType);
bool IsMine(const CScript& scriptPubKey);
bool ExtractPubKey(const CScript& scriptPubKey, bool fMineOnly, vector<unsigned char>& vchPubKeyRet);
bool ExtractHash160(const CScript& scriptPubKey, uint160& hash160Ret);
bool SignSignature(const CTransaction& txFrom, CTransaction& txTo, unsigned int nIn, int nHashType=SIGHASH_ALL, CScript scriptPrereq=CScript());
bool VerifySignature(const CTransaction& txFrom, const CTransaction& txTo, unsigned int nIn, int nHashType=0);

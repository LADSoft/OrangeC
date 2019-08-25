#include "DotNetPELib.h"
#include "bigdigits.h"
namespace DotNetPELib {

class ByteArray
{
public:
    ByteArray(size_t len) : len_(len), mem_(new Byte[len]) { memset(mem_,0, len_); }
    ~ByteArray()
    {
        memset(mem_, 0, len_);
        delete[] mem_;
    }
    
    Byte *operator()() { return mem_; }
    const Byte *operator()() const { return mem_; }
    Byte &operator[](size_t pos) { return *(mem_ + pos); }
    size_t size() { return len_; }

    Byte *begin() { return mem_; }
    Byte *end() { return mem_ + len_; }
private:
    Byte *mem_;
    size_t len_;
};
// format a message hash into PKCS1 format
// assumes SHA-1 hashing
class PKCS1Formatter
{
    public:
        PKCS1Formatter(const Byte *msg) : msg_(msg) { }
        void Calculate(ByteArray &result);
    private:
        const Byte *msg_;
        static Byte DerHeader[];
};

// Manage SNK file & perform RSA signature
class RSAEncoder
{
    public:
	RSAEncoder() : modulusBits(0), privateExponent(0), publicExponent(0), keyPair(0), modulus(0) { }
	virtual ~RSAEncoder() { 
        if (privateExponent)
            memset(privateExponent, 0, 2048);
        if (keyPair)
            memset(keyPair, 0, 2048);
        if (modulus)
            memset(modulus, 0, 2048);
		delete [] privateExponent;
		delete [] keyPair;
		delete [] modulus;
	}
	size_t LoadStrongNameKeys(const std::string & file);
	void GetPublicKeyData(Byte *key, size_t *keySize);
	void GetStrongNameSignature(Byte *sig, size_t *sigSize, const Byte *hash, size_t hashSize );

//private:
	size_t modulusBits;
    int publicExponent;
	Byte *privateExponent;
	Byte *keyPair;
	Byte *modulus;
};
}
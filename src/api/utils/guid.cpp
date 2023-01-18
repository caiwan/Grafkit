

#include "guid.h"

#ifdef GUID_LIBUUID
#include <uuid/uuid.h>
#endif

#ifdef GUID_CFUUID
#include <CoreFoundation/CFUUID.h>
#endif

#ifdef GUID_WINDOWS
#include <objbase.h>
#endif

using namespace std;


Grafkit::Archive& operator<<(Grafkit::Archive &archive, const Guid &guid){
/// @todo std::vector support	archive << guid._bytes;
	return archive;
}

Grafkit::Archive& operator>>(Grafkit::Archive &archive, Guid &guid){
///@todo std::vector support	archive >> guid._bytes;
	return archive;
}

// overload << so that it's easy to convert to a string
ostream &operator<<(ostream &s, const Guid &guid)
{
	return s << hex << setfill('0')
		<< setw(2) << (int)guid._bytes[0]
		<< setw(2) << (int)guid._bytes[1]
		<< setw(2) << (int)guid._bytes[2]
		<< setw(2) << (int)guid._bytes[3]
		<< "-"
		<< setw(2) << (int)guid._bytes[4]
		<< setw(2) << (int)guid._bytes[5]
		<< "-"
		<< setw(2) << (int)guid._bytes[6]
		<< setw(2) << (int)guid._bytes[7]
		<< "-"
		<< setw(2) << (int)guid._bytes[8]
		<< setw(2) << (int)guid._bytes[9]
		<< "-"
		<< setw(2) << (int)guid._bytes[10]
		<< setw(2) << (int)guid._bytes[11]
		<< setw(2) << (int)guid._bytes[12]
		<< setw(2) << (int)guid._bytes[13]
		<< setw(2) << (int)guid._bytes[14]
		<< setw(2) << (int)guid._bytes[15];
}

// create a guid from vector of bytes
Guid::Guid(const vector<unsigned char> &bytes)
{
	_bytes = bytes;
}

// create a guid from array of bytes
Guid::Guid(const unsigned char *bytes)
{
	_bytes.assign(bytes, bytes + 16);
}

// converts a single hex char to a number (0 - 15)
unsigned char hexDigitToChar(char ch)
{
	if (ch > 47 && ch < 58)
		return ch - 48;

	if (ch > 96 && ch < 103)
		return ch - 87;

	if (ch > 64 && ch < 71)
		return ch - 55;

	return 0;
}

// converts a single hex char to a number (0 - 15)
char charToHexDigit(unsigned char ch)
{
	if (ch >= 0 && ch < 10)
		return ch + 48;

	if (ch >= 10 && ch < 16)
		return ch + 87;

	return 0;
}

void charToHex(unsigned char ch, std::string &in)
{
	in.push_back(charToHexDigit(ch & 0x0f));
	in.push_back(charToHexDigit(ch >> 4  ));
}

// converts the two hexadecimal characters to an unsigned char (a byte)
unsigned char hexPairToChar(char a, char b)
{
	return hexDigitToChar(a) * 16 + hexDigitToChar(b);
}

// create a guid from string
Guid::Guid(const string &fromString)
{
	_bytes.clear();

	char charOne, charTwo;
	bool lookingForFirstChar = true;

#if 0
	for (const char &ch : fromString)
	{
#else 
	for(const char *p = fromString.c_str(); p[0]; p++)
	{
		const char &ch = p[0];
#endif

		if (ch == '-')
			continue;

		if (lookingForFirstChar)
		{
			charOne = ch;
			lookingForFirstChar = false;
		}
		else
		{
			charTwo = ch;
			auto byte = hexPairToChar(charOne, charTwo);
			_bytes.push_back(byte);
			lookingForFirstChar = true;
		}
	}

}


// create empty guid
Guid::Guid()
{
	_bytes = vector<unsigned char>(16, 0);
}

// copy constructor
Guid::Guid(const Guid &other)
{
	_bytes = other._bytes;
}

// overload assignment operator
Guid &Guid::operator=(const Guid &other)
{
	_bytes = other._bytes;
	return *this;
}

// overload equality operator
bool Guid::operator==(const Guid &other) const
{
	return _bytes == other._bytes;
}

bool Guid::operator<(const Guid &other) const
{
	return _bytes < other._bytes;
}

bool Guid::operator>(const Guid &other) const
{
	return _bytes > other._bytes;
}

// overload inequality operator
bool Guid::operator!=(const Guid &other) const
{
	return !((*this) == other);
}

void Guid::toBinary(unsigned char* data) {
	for (size_t i = 0; i < this->_bytes.size(); i++)
	{
		data[i] = _bytes[i];
	}
}

std::string Guid::toString() const{
	std::string out;

	charToHex(this->_bytes[0], out);

	charToHex(this->_bytes[0], out);
	charToHex(this->_bytes[1], out);
	charToHex(this->_bytes[2], out);
	charToHex(this->_bytes[3], out);
	out.push_back('-');
	charToHex(this->_bytes[4], out);
	charToHex(this->_bytes[5], out);
	out.push_back('-');
	charToHex(this->_bytes[6], out);
	charToHex(this->_bytes[7], out);
	out.push_back('-');
	charToHex(this->_bytes[8], out);
	charToHex(this->_bytes[9], out);
	out.push_back('-');
	charToHex(this->_bytes[10], out);
	charToHex(this->_bytes[11], out);
	charToHex(this->_bytes[12], out);
	charToHex(this->_bytes[13], out);
	charToHex(this->_bytes[14], out);
	charToHex(this->_bytes[15], out);

	return out;
}

// This is the linux friendly implementation, but it could work on other
// systems that have libuuid available
#ifdef GUID_LIBUUID
Guid GuidGenerator::newGuid()
{
	uuid_t id;
	uuid_generate(id);
	return id;
}
#endif

// this is the mac and ios version 
#ifdef GUID_CFUUID
Guid GuidGenerator::newGuid()
{
	auto newId = CFUUIDCreate(NULL);
	auto bytes = CFUUIDGetUUIDBytes(newId);
	CFRelease(newId);

	const unsigned char byteArray[16] =
	{
		bytes.byte0,
		bytes.byte1,
		bytes.byte2,
		bytes.byte3,
		bytes.byte4,
		bytes.byte5,
		bytes.byte6,
		bytes.byte7,
		bytes.byte8,
		bytes.byte9,
		bytes.byte10,
		bytes.byte11,
		bytes.byte12,
		bytes.byte13,
		bytes.byte14,
		bytes.byte15
	};
	return byteArray;
}
#endif

// obviously this is the windows version
#ifdef GUID_WINDOWS
Guid GuidGenerator::newGuid()
{
	GUID newId;
	CoCreateGuid(&newId);

	const unsigned char bytes[16] = 
	{
		(newId.Data1 >> 24) & 0xFF,
		(newId.Data1 >> 16) & 0xFF,
		(newId.Data1 >> 8) & 0xFF,
		(newId.Data1) & 0xff,

		(newId.Data2 >> 8) & 0xFF,
		(newId.Data2) & 0xff,

		(newId.Data3 >> 8) & 0xFF,
		(newId.Data3) & 0xFF,

		newId.Data4[0],
		newId.Data4[1],
		newId.Data4[2],
		newId.Data4[3],
		newId.Data4[4],
		newId.Data4[5],
		newId.Data4[6],
		newId.Data4[7]
	};

	return bytes;
}
#endif
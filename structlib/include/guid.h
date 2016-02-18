#pragma once

// ****
#define GUID_WINDOWS
// ****

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <iomanip>

#include "archive.h"

/**
// Class to represent a GUID/UUID. Each instance acts as a wrapper around a
// 16 byte value that can be passed around by value. It also supports
// conversion to string (via the stream operator <<) and conversion from a
// string via constructor.
*/


	///@todo set, get, to string, serialize
class Guid
{
	public:
		/// create a guid from vector of bytes
		Guid(const std::vector<unsigned char> &bytes);

		/// create a guid from array of bytes
		Guid(const unsigned char *bytes);

		/// create a guid from string
		Guid(const std::string &fromString);

		/// create empty guid
		Guid();

		/// copy constructor
		Guid(const Guid &other);

		/// overload assignment operator
		Guid &operator= (const Guid &other);

		/// overload equality and inequality operator
		bool operator==(const Guid &other) const;
		bool operator!=(const Guid &other) const;

		bool operator<(const Guid &other) const;
		bool operator>(const Guid &other) const;

		void toBinary(unsigned char* data);

		std::string toString() const;

		/*
		operator std::string() const { return this->toString(); }
		operator const char*() const { return this->toString().c_str(); }
		*/

	private:

		/// actual data
		std::vector<unsigned char> _bytes;

		/// make the << operator a friend so it can access _bytes
		friend std::ostream &operator<<(std::ostream &s, const Guid &guid);

		friend FWutils::Archive& operator<<(FWutils::Archive &archive, const Guid &guid);
		friend FWutils::Archive& operator>>(FWutils::Archive &archive, Guid &guid);
};

/**
// Class that can create new guids. The only reason this exists instead of
// just a global "newGuid" function is because some platforms will require
// that there is some attached context. In the case of android, we need to
// know what JNIEnv is being used to call back to Java, but the newGuid()
// function would no longer be cross-platform if we parameterized the android
// version. Instead, construction of the GuidGenerator may be different on
// each platform, but the use of newGuid is uniform.
*/
class GuidGenerator
{
	public:
		GuidGenerator() { }
		Guid newGuid();
};

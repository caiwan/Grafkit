/**
Type-length-value format descriptor

@author caiwan

*/

/// @todo tudjon mapet is

enum TLVFieldType {
	TLVF_INVALID = 0,
	TLVF_char,
	TLVF_short,
	TLVF_int,
	TLVF_long,
	TLVF_longlong,
	TLVF_float,
	TLVF_double,
	TLVF_string,
	TLVF_object,
	TLVF_map,
};

class TLVRecord;
/** Describes one TLV record
*/
class TLVRecord {
private:
	unsigned int type;	///< primitive type
	unsigned char nComponents; ///< number of vector and matrix field components (1,2,3,4,9,16)
	//unsigned char isKeyValue;	///< is data represented as 
	char * szField; ///< field name
	char * szClazz; ///< class name
	
	unsigned int nElemLen;
	unsigned int nElemCount;

	void* pData;

	unsigned int nSubTLVCount;
	TLVRecord *pSubTLVs;

	// get+set
};

/** Parses and buiold a TLV tree
*/
class TLVParser {

};

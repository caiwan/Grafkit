/**
Type-length-value format descriptor

out-of-scope

@author caiwan

*/

/// @todo tudjon mapet is

enum TLVFieldType {
	TLVF_INVALID = 0,
	TVLF_primitive,
	TVLF_array,
	TLVF_string,
	TLVF_object,
	TLVF_map,
	TLVF_COUNT
};

struct TLVRecord;
/** Describes one TLV record */
struct TLVRecord {
	enum TLVFieldType type;	///< primitive type

	unsigned int nElemLen;
	unsigned int nElemCount;

	char *szClazz; ///< class name
	char *szField; ///< field name

	void* pData;

	TLVRecord() {
		this->type = TLVF_INVALID;
		this->szClazz = nullptr;
		this->szField = nullptr;
		this->nElemLen = 0;
		this->nElemCount = 0;
		this->pData = nullptr;
	}
};

class TLVParser{
public:
	static void fromByte(void* in_data, TLVRecord& out_tlv);
	static void toByte(TLVRecord& in_tlv, void* out_data, unsigned int& out_length);
};

/* C:\Users\psari\prog\structlib\Debug\out.bin (2015.12.26. 13:46:02)
StartOffset: 00000000, EndOffset: 00000037, Length: 00000038 */

#define size_test_data_0 56
static const unsigned char test_data_0[56] = {
	// -- header
	0x03, 0x00, 0x00, 0x00,			// signature length
	0x43, 0x41, 0x49, 0x00,			// signature
	0x00, 0x00,						// maj version
	0x01, 0x00,						// min version
	
	// -- data
	0x1A, 0x00, 0x00, 0x00,			// class name length + name: "ArchivePersistentTestClass"
	0x41, 0x72, 0x63, 0x68, 0x69, 0x76, 0x65, 0x50, 0x65, 0x72, 0x73, 0x69, 0x73, 0x74, 0x65, 0x6E, 0x74, 0x54, 0x65, 0x73, 0x74, 0x43, 0x6C, 0x61, 0x73, 0x73, 0x00, 
	0x01, 0x00, 0x00, 0x00,			// class version
	0xBA, 0xBA, 0xCA, 0xFA,			// first member
	0x71, 0x3D, 0x0A, 0x40			// second member
};

// this fits to meet the data structure and alignment of test_data_0
struct td0_data_header_t
{
	unsigned int slen;
	char s[4];  // s0, s1, s2, s3;
	unsigned short major, minor;
};

struct td0_class_header_t {
	unsigned int strlen;
	char classname[0x1b];
	unsigned int classversion;
};

struct td0_t {
	td0_data_header_t header;
	td0_class_header_t clazz;
	ArchiveBasicData_t data;
};
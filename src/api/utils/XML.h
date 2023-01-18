#ifndef __XML_H_
#define __XML_H_

#include "../../GKSDK_config.h"

#ifdef UTILS_XML_PARSER

#include <string>

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef WORD
#define WORD short int
#endif

#ifndef DWORD
#define DWORD unsigned int
#endif

#ifndef NULL
#define NULL (void*)(0)
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef BOOL
#define BOOL int
#endif

using namespace std;

class Element;
class RootElement;
class Attribute;

/////////////////////////////////
// Az XML lista kezelgeto osztaly
//
class XMLParser {

public:
	XMLParser();	// konstruktor
	~XMLParser();	// destruktor

	// parser - beolvasas fajlb�l
	int ReadXML(string *FileName);																	// belvas egy XML fajlt es letrehozza belole a fat
	int ReadXMLFromMemory(const char *data);

	// parser - beolvasas fajlb�l
	void ExtractElement(Element *Parent, string *strText, size_t *start_pos, size_t *end_pos);		// kibanyassza a talalt elemet
	void ExtractAllAttribute(Element *ToThis, string *strText, size_t *start_pos, size_t *end_pos);	// kiszedi az osszes attrib�tumot es hozza is csapja a megadotthoz
	void ExtractAttribute(string *strText, size_t *start_pos, size_t *end_pos,						// kiolvas egy attrib�tumot strText-bol start_pos-t�l
							string *strName, string *strValue);										// es belerakja a megadott valtoz�kba

	// kiiratasert felelos rutinok
	int WriteXMLToFile(string *FileName, string *XMLText);	// ez rakja ki a kesz XML-t egy fajlba
	void WriteXMLToStdOut(string *XMLText);				// ez rakja ki a kesz XML-t a kimenetre

	// Get, Set es egyeb allatfajtak
	inline RootElement *GetRoot() { return m_Root;}
	inline void SetXMLDecl(string *strVer, string *strEnc, BOOL bStandalone) {
		m_XMLVersion = strVer; m_XMLEncoding = strEnc; m_bStandalone = bStandalone;}
	
private:
	// dolgok itten ni nekie
	string *m_XMLVersion;
	string *m_XMLEncoding;
	BOOL m_bStandalone;
	RootElement *m_Root;	// a gyoker elem - nincs szuloje es testvere
};
//
/////////////////////////////////


//////////////////////
// Megjegyzes nevekhez
//
// Az XML nevek tartalmazhatnak: A..Z, a..z, 0..9, nem angol betuk/szamok/kepirasjelek es _-.
// Nem tartalmazhatnak: ", ', $, %, ;, ^ es whitespace karaktereket.
// :-ot csak namespace-ek jelolesere lehet hasznalni.
// Semmilyen formaban nem kezdodhetnek XML-lel, mert ezt a W3C standardositasra hasznalja.
// Csak betukkel/kepirasjelekkel es _ jellel kezdodhetnek.
// Nem kezdodhetnek szammal, --lel es .-tal.
// Nincs hossz beli korlat.
//
// Alap entitas referenciak:
//  < &lt;
//  > &gt;
//  " &quot;
//  ' &apos;
//  & &amp;
//
//////////////////////

/////////////////////////////////
// XML elemek tarolasahoz osztaly
//
class Element {
	friend XMLParser;

public:
	Element(Element *Parent, string *strName, BOOL IsEmpty, Attribute *FirstAttribute = NULL, Element *FirstChild = NULL);	// konstruktor
	virtual ~Element();	// destruktor

	// Attrib�tum es Child kezelgeto rutinok
	void AddChild(Element *NewChild);				// hozzacsapja a Child lista vegehez
	void RemoveChild(Element *OldChild);			// kiveszi a Child listab�l
	void AddAttribute(Attribute *NewAttribute);	// hozzacsapja az attrib�tum lista vegehez
	void RemoveAttribute(Attribute *OldAttribute);	// kiveszi az attrib�tum listab�l
	
	// kiirat� rutinok
	void AppendAsText(string *strText);	// a megadott string vegehez fuzi az elemet formazva szovegkent
	
	// Get, Set, Is meg ami belefer
	inline string *GetName() { return m_strName;}
	inline BOOL IsEmpty() { return m_bEmpty;}
	inline Element *GetFirstChild() { return m_FirstChild;}
	inline Element *GetLastChild() { return m_LastChild;}
	inline Attribute *GetFirstAttribute() { return m_FirstAttribute;}
	inline Attribute *GetLastAttribute() { return m_LastAttribute;}
	inline string *GetText() {return m_strText;}
	inline Element *GetParent() { return m_Parent;}
	inline Element *GetNext() { return m_Next;}
	inline Element *GetPrev() { return m_Prev;}
	
private:
	// setterek privatok lesznek
	inline void SetEmpty(BOOL IsEmpty) { m_bEmpty = IsEmpty;}
	inline void SetName(string *strName) { m_strName = strName;}
	inline void SetFirstChild(Element *Child) { m_FirstChild = Child;}
	inline void SetLastChild(Element *Child) { m_LastChild = Child;}
	inline void SetFirstAttribute(Attribute *NewFirst) { m_FirstAttribute = NewFirst;}
	inline void SetLastAttribute(Attribute *NewLast) { m_LastAttribute = NewLast;}
	inline void SetText(string *strText) { m_strText = strText;}
	inline void SetParent(Element *Parent) { m_Parent = Parent;}	
	inline void SetNext(Element *Next) { m_Next = Next;}
	inline void SetPrev(Element *Prev) { m_Prev = Prev;}

	// tulajdonsagok
	string *m_strName;				// a tag neve
	BOOL m_bEmpty;					// TRUE ha ures tag, azaz nincs child-ja es end-tag-je
	Attribute *m_FirstAttribute;	// az attrib�tum lista elso eleme
	Attribute *m_LastAttribute;		// az attrib�tum lista utols� eleme
	Element *m_FirstChild;			// elso eleme azoknak az elemeknek, amelyeknek szuloje ez
	Element *m_LastChild;			// utols� eleme azoknak az elemeknek, amelyeknek szuloje ez
	string *m_strText;				// raw data a tag-ek kozott

	// listahoz
	Element *m_Parent;	// ez a szulo cime, ami itt tuti NULL
	Element *m_Next;	// kovetkezo elem (testver)
	Element *m_Prev;	// elozo elem (testver)	
};
//
/////////////////////////////////

/////////////////////////////////
// a spec XML root elem tarolasahoz osztaly
//
class RootElement : public Element  {
	friend XMLParser;

public:
	RootElement(string *strName, BOOL IsEmpty, Attribute *FirstAttribute = NULL, Element *FirstChild = NULL);	// konstruktor
	virtual ~RootElement();	// destruktor

};
//
/////////////////////////////////

////////////////////////////////////////
// elemek attrib�tumainak tarol� osztaly
//
class Attribute {
	friend XMLParser;
	friend Element;

public:
	Attribute();									// alap konstructor
	Attribute(string *strName, string *strValue);	// konstructor
	~Attribute();									// destructor

	// kiirat� rutin
	void AppendAsText(string *strText);	// a megadott string vegehez fuzi az attrib�tumot formazva szovegkent

	// Get, Set meg ilyenek
	inline string GetName() { return *m_strName;}
	inline string GetValue() { return *m_strValue;}
	inline Attribute *GetNext() { return m_Next;}
	inline Attribute *GetPrev() { return m_Prev;}

private:

	inline void SetName(string *strName) { m_strName = strName;}
	inline void SetValue(string *strValue) { m_strValue = strValue;}
	inline void SetNext(Attribute *Next) { m_Next = Next;}
	inline void SetPrev(Attribute *Prev) { m_Prev = Prev;}

	// tulajdonsagok
	string *m_strName;	// neve
	string *m_strValue;	// erteke
	
	// listahoz
	Attribute *m_Next;	// kovetkezo attrib�tum
	Attribute *m_Prev;	// elozo attrib�tum
};
//
////////////////////////////////////////

#endif // UTILS_XML_PARSER

#endif //__XML_H_
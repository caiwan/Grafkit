#include "XML.h"

#ifdef UTILS_XML_PARSER

#include <fstream>
#include <stack>

#include <iostream>
#include <sstream>

#include "../../debug/memory.h"

using namespace std;

// konstruktor
XMLParser::XMLParser() {
	this->m_XMLVersion = NULL;
	this->m_XMLEncoding = NULL;

	this->m_Root = NULL;
}

// destruktor
XMLParser::~XMLParser() {
	// a destruktor mindig jobbra indul el
	if (this->m_XMLEncoding) delete this->m_XMLEncoding;
	if (this->m_XMLVersion) delete this->m_XMLVersion;

	if (this->m_Root) delete this->m_Root;
}

//belvas egy XML fajlt es letrehozza belole a fat
int XMLParser::ReadXML(string *FileName) {

	// megnyitunk egy fajlt olvasasra
	ifstream XMLfile;
	XMLfile.open(FileName->c_str(), ios::in);
	
	// volt hiba?
	if(XMLfile.fail())
		return 1;	// aha
		
	// kiolvasunk mindent egyszerre egy stringbe
	string strText, *strTemp; strTemp = new string;
	while(!XMLfile.eof()) {
	
		getline(XMLfile, *strTemp);
		strText += *strTemp + "\n";
	}

	delete strTemp;
	
	// az epiteshez ami kell
	size_t spos = 0, epos;			// ez jeloli majd mindig az aktualis poziciot es a vegpoziciot

	// rekurzivan felepitjuk a fat
	ExtractElement(NULL, &strText, &spos, &epos);	
	
	// bezarjuk a fajlt
	XMLfile.close();
	return 0;	// cucc beolvasva
}


int XMLParser::ReadXMLFromMemory(const char *data) {
	stringstream ss(stringstream::in | stringstream::out);

	ss << data;
		
	// kiolvasunk mindent egyszerre egy stringbe
	string strText, *strTemp; strTemp = new string;
	while(!ss.eof()) {
	
		getline(ss, *strTemp);
		strText += *strTemp + "\n";
	}

	delete strTemp;

	//strText += "\n\n\n";
	
	// az epiteshez ami kell
	size_t spos = 0, epos = 0;			// ez jeloli majd mindig az aktualis poziciot es a vegpoziciot

	// rekurzivan felepitjuk a fat
	ExtractElement(NULL, &strText, &spos, &epos);	
	
	// bezarjuk a fajlt
	//ss.close();
	return 0;	// cucc beolvasva

}

// kibanyassza a talalt elemet
void XMLParser::ExtractElement(Element *Parent, string *strText, size_t *start_pos, size_t *end_pos) {

	//BUG: ha sortores van afile vegen akkor vegtelen ciklusba kerul. Ezeket le kell vagni valahogy.
	size_t pos = strText->length()-1;
	BOOL kesz = FALSE;
	
	while (!kesz && pos){
		char a = strText->at(pos);
		if (a == '>'){
				kesz = TRUE;
		} else
			kesz = FALSE;
		pos --;
	}

	// atvesszuk helyi valtozonak
	size_t spos = *start_pos; size_t epos = *end_pos;

	// addig keresunk, amig a fajl vegere erunk, vagy ki nem lepunk egy zaro tag-nel
	BOOL bEndTag = FALSE;
	
	//size_t FileEnd = strText->length() - 1;
	size_t FileEnd = pos - 1;
	
	while(spos < FileEnd && bEndTag == FALSE) {

		// tag-ek es egyeb allatfajtak keresese es feldolgozasa
		//string *strTemp; strTemp = new string;
		string strTemp;

		spos = strText->find_first_of('<', spos) + 1;	// tag kezdetenek megkeresese - ra allunk a megkulonbozteto jelre
		switch(strText->at(spos)) {						// el kell dontenunk, hogy milyen tag-et talaltunk
		
			case '?': {	// instrukcio (innen csak az xml declaracio erdekel most meg)
			
				// beolvassuk a nevet
				spos = strText->find_first_not_of(" \t\n?", spos);	// megkeressuk az elejet
				epos = strText->find_first_of(" \t\n?", spos);		// megkeressuk a veget
				strTemp = strText->substr(spos, epos-spos);		// kiolvassuk
				// kitalaljuk mi legyen vele
				if(strTemp == "xml")  {	// xml deklaraciot talaltunk
				
					// megkeressuk az attributumait
					string strName, *strValue;								// tarolok a nevnek es az erteknek
					spos = strText->find_first_not_of(" \t\n", epos + 1);	// a whitespace-ek utan
					while(strText->at(spos) != '?') {						// megnezzuk van-e attributum

						strValue = new string;											// string init
						ExtractAttribute(strText, &spos, &epos, &strName, strValue);	// kiszedjuk az attributumot

						// lassuk mit talaltunk
						if(strName == "version")		// verzioszam
							m_XMLVersion = strValue;		// elmentjuk a stringet
						if(strName == "encoding")		// encoding
							m_XMLEncoding = strValue;		// elmentjuk a stringet
						if(strName == "standalone") {	// encoding
							if(*strValue == "no") {		// nem standalone
							
								m_bStandalone = FALSE;			// egyenlore meg nincs DTD
								
								
								//////////////////////////
								// DTD implementacio!!! //
								//////////////////////////
								
							}
							else							// standalone
								m_bStandalone = TRUE;			// nem kell DTD
						}
						
						// az utana levo nem whitespace karakterre alunk
						spos = strText->find_first_not_of(" \t\n", epos + 1);	// a whitespace-ek utan
					}
				}

				// megkeressuk a veget az instrukcionak es utana allunk
				spos = strText->find_first_of('>', epos + 1) + 1;
			} break;
			
			case '!': {	// ez lehet CDATA vagy comment (egyenlore nem foglalkozunk vele)
			
				// mivel ezzel nem foglalkozunk, at kell lepnunk
				spos = strText->find_first_of('>', spos) + 1;		
			} break;

			case '-': {	// ez a megjegyzes lesz (egyenlore nem erdekel)
			
				// mivel ezzel nem foglalkozunk, at kell lepnunk
				spos = strText->find_first_of('>', spos) + 1;		
			} break;
		
			case '/': {	// ez a zaras

				// megkeressuk a veget a tag-nek es utana allunk
				spos = strText->find_first_of('>', spos) + 1;
				
				// jelezzuk, hogy innen mar ennek a szulojehez tartozik a tobbi
				bEndTag = TRUE;
			} break;

			default: {		// minden mas, valoszinuleg egy elem
			
				// kiolvassuk a tag nevet
				spos = strText->find_first_not_of(" \t\n", spos);	// megkeressuk az elejet
				epos = strText->find_first_of(" \t\n/>", spos);		// megkeressuk a veget
				strTemp = strText->substr(spos, epos-spos);		// kimasoljuk a nevet

				// eldontjuk, hogy ures-e
				BOOL bEmpty;
				if(strText->at(strText->find_first_of('>',spos) - 1) == '/')	// empty
					bEmpty = TRUE;
				else							// not empty
					bEmpty = FALSE;
				
				// elem letrehozas meg helyre teves
				if(Parent != NULL) {	// megadtunk szulot
				
					// letrehozzuk az elemet es hozzaadjuk
					Element *E_Temp;		// hogy kesobb is meglegyen
					E_Temp = new Element(Parent, new string(strTemp), bEmpty);
					Parent->AddChild(E_Temp);
					
					// megkeressuk az attributumait
					ExtractAllAttribute(E_Temp, strText, &spos, &epos);
					
					// kiszedjuk a szoveget a tag-ek kozul ha van
					string *strTextTemp = new string;
					epos = strText->find_first_of('<', spos);
					*strTextTemp = strText->substr(spos, epos - spos);
					E_Temp->SetText(strTextTemp);

					// johetnek a child-ek ha vannak
					if(bEmpty == FALSE)
						ExtractElement(E_Temp, strText, &spos, &epos);
				}	
				else {					// ures, tehat ez a Root
				
					// letrehozzuk a Root-ot
					m_Root = new RootElement(new string(strTemp), bEmpty);
					
					// megkeressuk az attributumait
					ExtractAllAttribute(m_Root, strText, &spos, &epos);
					
					// kiszedjuk a szoveget a tag-ek kozul ha van
					string *strTextTemp = new string;
					epos = strText->find_first_of('<', spos);
					*strTextTemp = strText->substr(spos, epos - spos);
					m_Root->SetText(strTextTemp);

					// johetnek a child-ek ha vannak
					if(bEmpty == FALSE)
						ExtractElement(m_Root, strText, &spos, &epos);
				}
			} break;
		}
	}

	// frissitjuk az ertekeket, mert ugye itt valtozott picit
	*start_pos = spos; *end_pos = epos;
}

// kiszedi az osszes attributumot es hozza is csapja a megadotthoz
// az end_pos legyen a nev utani whitespace-eken
void XMLParser::ExtractAllAttribute(Element *ToThis, string *strText, size_t *start_pos, size_t *end_pos) {

	// atvesszuk helyi valtozonak
	size_t spos; size_t epos = *end_pos;
	
	// tarolo attributumoknak
	string *strName, *strValue;	// tarolok a nevnek es az erteknek
	Attribute *A_Temp; 			// ez meg az attributumnak
				
	// megkeressuk az attributumait
	spos = strText->find_first_not_of(" \t\n", epos);					// a whitespace-ek utan
	while(strText->at(spos) != '/' && strText->at(spos) != '>') {	// megnezzuk van-e attributum
	
		// kiolvasas
		strName = new string; strValue = new string;					// string init
		ExtractAttribute(strText, &spos, &epos, strName, strValue);	// kiszedjuk az attributumot
		
		// lassuk mit talaltunk
		A_Temp = new Attribute(strName, strValue); 					// letrehozzuk az attributumot
		ToThis->AddAttribute(A_Temp);									// hozzaadjuk
		
		// az utana levo nem whitespace karakterre alunk
		spos = strText->find_first_not_of(" \t\n", epos + 1);			// a whitespace-ek utan
	}

	// a tag vege utan allunk
	spos = strText->find_first_of('>', spos) + 1;

	// frissitjuk az ertekeket, mert ugye itt valtozott picit
	*start_pos = spos; *end_pos = epos;
}

// kiolvas egy attributumot strText-bol start_pos-tol es belerakja a megadott valtozokba
// a start_pos mar legyen az elso karakteren, a nev es ertek string-jei legyen inicializalva!!!
void XMLParser::ExtractAttribute(string *strText, size_t *start_pos, size_t *end_pos, string *strName, string *strValue) {

	// atvesszuk helyi valtozonak
	size_t spos = *start_pos; size_t epos;
	
	// kiolvassuk az attrubutum nevet
	epos = strText->find_first_of(" \t\n=", spos);	// az attributum nevenek a vege
	*strName = strText->substr(spos, epos - spos);	// ez az attributum neve

	// kiolvassuk az attributum erteket
	spos = strText->find_first_of('"', epos) + 1;		// string kezdete
	epos = strText->find_first_of('"', spos);			// string vege
	*strValue = strText->substr(spos, epos - spos);	// elmentjuk a stringet

	// frissitjuk az ertekeket, mert ugye itt valtozott picit
	*start_pos = spos; *end_pos = epos;
}

// ez rakja ki a kesz XML-t egy fajlba
int XMLParser::WriteXMLToFile(string *FileName, string *XMLText) {

	// megnyitunk egy fajlt irasra
	fstream XMLfile;
	XMLfile.open(FileName->c_str(), ios::out | ios::trunc);
	
	// volt hiba?
	if(XMLfile.fail())
		return 1;	// aha


	// beleirjuk a cuccost
	XMLfile << "<?xml version=\"" << *m_XMLVersion << "\" encoding=\"" << *m_XMLEncoding << "\" standalone=\"";
	if(m_bStandalone == TRUE)
		XMLfile << "yes\"?>\n";
	else {
	
		XMLfile << "no\"?>\n";
		
		///////////////////////////////////////////
		// itt kell meg implementalni a DTD-t!!! //
		///////////////////////////////////////////
	}
	XMLfile << *XMLText;	// az XML torzse


	// volt hiba?
	if(XMLfile.fail()) {
		XMLfile.close();
		return 2;	// aha
	}
	
	// bezarjuk a fajlt
	XMLfile.close();
	return 0;	// cucc kiirva
}

// ez rakja ki a kesz XML-t a kimenetre
void XMLParser::WriteXMLToStdOut(string *XMLText) {

	// beleirjuk a cuccost
	cout << "<?xml version=\"" << m_XMLVersion << "\" encoding=\"" << m_XMLEncoding << "\" standalone=\"";
	if(m_bStandalone == TRUE)
		cout << "yes\"?>\n";
	else {
	
		cout << "no\"?>\n";
		
		///////////////////////////////////////////
		// itt kell meg implementalni a DTD-t!!! //
		///////////////////////////////////////////
	}
	cout << *XMLText;	// az XML torzse
}


// konstruktor
Element::Element(Element *Parent, string *strName, BOOL IsEmpty, Attribute *FirstAttribute, Element *FirstChild) {

	// atvesszuk az ertekeket
	m_Parent = Parent;

	m_strName = strName;
	m_bEmpty = IsEmpty;
	m_FirstAttribute = FirstAttribute;
	m_FirstChild = FirstChild;
	
	m_strText = NULL;
	
	// ha meg lett adva attributum vagy child, akkor megkeressuk a veget es beallitjuk a last mutatokat
	// ha nem, akkor NULL
	if(FirstAttribute != NULL) {
	
		while(FirstAttribute->GetNext() != NULL) {
		
			FirstAttribute = FirstAttribute->GetNext();
		}
	}
	m_LastAttribute = FirstAttribute;
	if(FirstChild != NULL) {
	
		while(FirstChild->GetNext() != NULL) {
		
			FirstChild = FirstChild->GetNext();
		}
	}
	m_LastChild = FirstChild;
	
	// nincs se kovetkezo se elozo elem
	m_Next = m_Prev = NULL;
}

// destructor
Element::~Element() {
	// eloszor a tulajdonsagok
	if (this->m_strText) delete this->m_strText;
	if (this->m_strName) delete this->m_strName;

	// attributumok
	if (this->m_FirstAttribute) delete this->m_FirstAttribute;
	this->m_FirstAttribute = NULL;

	// majd rekurzive tovabbmegy az azonos szinten
	if (this->m_Next) delete this->m_Next;
	this->m_Next = NULL;

	// majd lefele
	if (this->m_FirstChild) delete this->m_FirstChild;
	this->m_FirstChild = NULL;
	
}

// hozzacsapja a Child lista vegehez
void Element::AddChild(Element *NewChild) {

	// ellenorizzuk, hogy van-e utolso elem egyaltalan
	if(m_LastChild != NULL) {	// van
	
		m_LastChild->SetNext(NewChild);	// az uj utolso utani elem
		NewChild->SetPrev(m_LastChild);	// az eddigi utolso lesz az uj elott
		m_LastChild = NewChild;			// ez lesz az uj utolso elem
	}
	else {						// nincs
	
		// ez tehat az elso child elemunk
		m_FirstChild = NewChild;
		m_LastChild = NewChild;
	}	
}

// kiveszi a Child listabol
void Element::RemoveChild(Element *OldChild) {

	// van elotte valami?
	if(OldChild->GetPrev() != NULL)	// van
		OldChild->GetPrev()->SetNext(OldChild->GetNext());	// az elotte levo utan az ez utani
	else								// nincs
		m_FirstChild = OldChild->GetNext();	// az ezutani az uj elso elem
		
	// van utana valami?
	if(OldChild->GetNext() != NULL)	// van	
		OldChild->GetNext()->SetPrev(OldChild->GetPrev());	// az ez utani ele az ez elotti
	else								// nincs
		m_LastChild = OldChild->GetPrev();	// az ez elotti az uj utolso elem
}

// hozzacsapja az attributum lista vegehez
void Element::AddAttribute(Attribute *NewAttribute) {

	// ellenorizzuk, hogy van-e utolso attributum egyaltalan
	if(m_LastAttribute != NULL) {	// van
	
		m_LastAttribute->SetNext(NewAttribute);	// az uj utolso utani attributum
		NewAttribute->SetPrev(m_LastAttribute);	// az eddigi utolso lesz az uj elott
		m_LastAttribute = NewAttribute;			// az uj utolso attributum
	}
	else {							// nincs
	
		// ez tehat az elso attributumunk
		m_FirstAttribute = NewAttribute;
		m_LastAttribute = NewAttribute;
	}	
}

// kiveszi az attributum listabol
void Element::RemoveAttribute(Attribute *OldAttribute) {

	// van elotte valami?
	if(OldAttribute->GetPrev() != NULL)	// van
		OldAttribute->GetPrev()->SetNext(OldAttribute->GetNext());	// az elotte levo utan az ez utani
	else							// nincs
		m_FirstAttribute = OldAttribute->GetNext();	// az ezutani az uj elso elem
		
	// van utana valami?
	if(OldAttribute->GetNext() != NULL)	// van	
		OldAttribute->GetNext()->SetPrev(OldAttribute->GetPrev());	// az ez utani ele az ez elotti
	else							// nincs
		m_LastAttribute = OldAttribute->GetPrev();	// az ez elotti az uj utolso elem
}

// a megadott string vegehez fuzi az elemet formazva szovegkent
void Element::AppendAsText(string *strText) {

	// eloszor a nyito tag-et gyartjuk le
	*strText += "<" + *m_strName;	// megnyitjuk a tag-et
	if(m_FirstAttribute != NULL) {	// ha vannak attributumok
	
		Attribute *Temp = m_FirstAttribute;	// innen kezdjuk
		do {
		
			*strText += " ";				// elvalaszto hozzaadasa
			Temp->AppendAsText(strText);	// attributum beillesztese
			Temp = Temp->GetNext();		// kovetkezo elem
		} while(Temp != NULL);	// addig megyunk, amig az osszes attributumot hozza nem adtuk
	}
	if(m_bEmpty == TRUE) {	// ha ures, akkor nincs tovabb
	
		*strText += "/>";	// vegeztunk, lezarhatunk
		return;
	}
	*strText += ">";	// nyito tag-et lezarjuk
	
	// na most johetnek a child-ek ha vannak
	if(m_FirstChild != NULL) {	// ha vannak child elemek
	
		size_t pos = strText->length();	// innen kell minden tabot egyel novelni
		Element *Temp = m_FirstChild;		// innen kezdjuk
		do {
		
			*strText += "\n";				// elvalaszto hozzaadasa
			Temp->AppendAsText(strText);	// child beillesztese
			Temp = Temp->GetNext();		// kovetkezo elem
		} while(Temp != NULL);	// addig megyunk, amig az osszes elemet hozza nem adtuk
		
		// minden tag ele teszunk egy TAB-ot (ha volt mar, akkor is kap meg egyet)
		do {
		
			pos = strText->find_first_of('<', pos);		// keresunk TAB-ot
			if(pos != string::npos) {						// ha talaltunk
				strText->insert(pos, "\t");					// adunk hozza meg
				pos += 2;// = strText->find_first_not_of('<', ++pos);	// atlepjuk
			}
		} while(pos != string::npos);	// keressuk a kovetkezot, ha nem ertuk el a veget
	}
	
	// ennyi most mar johet a tag lezaras
	*strText += "\n</" + *m_strName + ">";

}

// konstruktor
RootElement::RootElement(string *strName, BOOL IsEmpty, Attribute *FirstAttribute, Element *FirstChild)
	: Element(NULL, strName, IsEmpty, FirstAttribute, FirstChild) {
}

// destructor
RootElement::~RootElement() {

}

// konstruktorok
Attribute::Attribute() {

	// nincs semmink
	m_strName = m_strValue = NULL;
	m_Next = m_Prev = NULL;
}

Attribute::Attribute(string *strName, string *strValue) {

	// atvesszuk az ertekeket
	m_strName = strName;
	m_strValue = strValue;
	
	// nincs se kovetkezo se elozo attributum
	m_Next = m_Prev = NULL;
}

// destructor
Attribute::~Attribute() {

	if(m_strName != NULL)
		delete m_strName;
	if(m_strValue != NULL)
		delete m_strValue;

	if (this->m_Next) delete this->m_Next;
	this->m_Next = NULL;
}

// a megadott string vegehez fuzi az attributumot formazva szovegkent
void Attribute::AppendAsText(string *strText) {

	*strText += *m_strName + "=\"" + *m_strValue + "\"";
}

#endif // UTILS_XML_PARSER

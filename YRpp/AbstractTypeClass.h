/*
	AbstractTypes are abstract template objects initialized by INI files
*/
#pragma once

#include <AbstractClass.h>
#include <Memory.h>

//forward declarations
class CCINIClass;

//Macro for the static Array of every AbstractTypeClass!
#define ABSTRACTTYPE_ARRAY(class_name, address)	public:\
	static constexpr constant_ptr<DynamicVectorClass<class_name*>, address> const Array{};\
	static __declspec(noinline) class_name* __fastcall Find(const char* pID)\
	{\
		for(auto pItem : *Array)\
			if(!_strcmpi(pItem->ID, pID))\
				return pItem;\
		return nullptr;\
	}\
	static __declspec(noinline) class_name* __fastcall FindOrAllocate(const char* pID)\
	{\
		if(!_strcmpi(pID, "<none>") || !_strcmpi(pID, "none")) {\
			return nullptr;\
		}\
		if(auto pRet = Find(pID)) {\
			return pRet;\
		}\
		return GameCreate<class_name>(pID);\
	}\
	static __declspec(noinline) int __fastcall FindIndex(const char* pID)\
	{\
		for(int i = 0; i < Array->Count; ++i)\
			if(!_strcmpi(Array->Items[i]->ID, pID))\
				return i;\
		return -1;\
	}
//---

class NOVTABLE AbstractTypeClass : public AbstractClass
{
public:
	static const AbstractType AbsID = AbstractType::Abstract;

	//Static
	static constexpr constant_ptr<DynamicVectorClass<AbstractTypeClass*>, 0xA8E968u> const Array{};

	//Destructor
	virtual ~AbstractTypeClass() JMP_THIS(0x410C30);

	//AbstractClass
	virtual void CalculateChecksum(Checksummer& checksum) const JMP_THIS(0x410BE0);

	//AbstractTypeClass
	virtual void LoadTheaterSpecificArt(TheaterType th_type) { }
	virtual bool LoadFromINI(CCINIClass* pINI) JMP_THIS(0x410A60);
	virtual bool SaveToINI(CCINIClass* pINI) JMP_THIS(0x410B90);

	const char* get_ID() const 
	{
		return this->ID;
	}

	bool IsThis(const char* pID)
	{
		return _strcmpi(pID, this->ID) == 0;
	}

	//Constructor
	AbstractTypeClass(const char* pID) noexcept
		: AbstractTypeClass(noinit_t())
	{ JMP_THIS(0x410800); }

protected:
	explicit __forceinline AbstractTypeClass(noinit_t) noexcept
		: AbstractClass(noinit_t())
	{ }

	//===========================================================================
	//===== Properties ==========================================================
	//===========================================================================

public:

	char ID [0x18];
	PROTECTED_PROPERTY(BYTE, zero_3C);
	char UINameLabel [0x20];
	const wchar_t* UIName;
	char Name [0x31];
};

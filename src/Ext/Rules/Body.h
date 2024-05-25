#pragma once

#include <CCINIClass.h>
#include <RulesClass.h>

#include <Utilities/Anchor.h>
#include <Utilities/Container.h>
#include <Utilities/Constructs.h>
#include <Utilities/Template.h>
#include <Utilities/Enum.h>
#include <Utilities/Debug.h>

class RulesExt
{
public:
	using base_type = RulesClass;

	static constexpr DWORD Canary = 0x12341234;

	class ExtData final : public Extension<RulesClass>
	{
	public:
		// Ares 0.A
		Valueable<double> DeactivateDimEMP;
		Valueable<double> DeactivateDimPowered;

		ExtData(RulesClass* OwnerObject) : Extension<RulesClass>(OwnerObject)
			, DeactivateDimEMP { 0.8 }
			, DeactivateDimPowered { 0.5 }
		{ }

		virtual ~ExtData() = default;

		virtual void LoadFromINIFile(CCINIClass* pINI) override;
		virtual void LoadBeforeTypeData(RulesClass* pThis, CCINIClass* pINI);
		virtual void LoadAfterTypeData(RulesClass* pThis, CCINIClass* pINI);
		virtual void InitializeConstants() override;
		void InitializeAfterTypeData(RulesClass* pThis);

		virtual void InvalidatePointer(void* ptr, bool bRemoved) override { }

		virtual void LoadFromStream(PhobosStreamReader& Stm) override;
		virtual void SaveToStream(PhobosStreamWriter& Stm) override;

	private:
		template <typename T>
		void Serialize(T& Stm);
	};

private:
	static std::unique_ptr<ExtData> Data;

public:
	static IStream* g_pStm;

	static void Allocate(RulesClass* pThis);
	static void Remove(RulesClass* pThis);

	static void LoadFromINIFile(RulesClass* pThis, CCINIClass* pINI);
	static void LoadBeforeTypeData(RulesClass* pThis, CCINIClass* pINI);
	static void LoadAfterTypeData(RulesClass* pThis, CCINIClass* pINI);

	static ExtData* Global()
	{
		return Data.get();
	}

	static void Clear()
	{
		Allocate(RulesClass::Instance);
	}

	static void PointerGotInvalid(void* ptr, bool removed)
	{
		Global()->InvalidatePointer(ptr, removed);
	}

	static bool LoadGlobals(PhobosStreamReader& Stm);
	static bool SaveGlobals(PhobosStreamWriter& Stm);

	static bool DetailsCurrentlyEnabled();
	static bool DetailsCurrentlyEnabled(int minDetailLevel);
};

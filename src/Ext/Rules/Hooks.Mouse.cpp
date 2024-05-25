#include "Body.h"

#include <Helpers/Macro.h>
#include <WWMouseClass.h>
#include <DisplayClass.h>
#include <InputManagerClass.h>
#include <InfantryClass.h>
#include <HouseClass.h>

bool EngineerAllowAttack(InfantryClass* pThis, TechnoClass* pTechno)
{
	bool canCapture = false;
	bool canC4 = false;

	if (auto const pBuilding = abstract_cast<BuildingClass*>(pTechno))
	{
		canCapture = pBuilding->Type->Capturable && !pBuilding->Type->BridgeRepairHut;
		canC4 = pBuilding->Type->CanC4 && pThis->Type->C4;
	}

	if (!InputManagerClass::Instance->IsForceFireKeyPressed())
	{
		if (pThis->Owner == pTechno->Owner || pThis->Owner->Allies.Contains(pTechno->Owner))
		{
			if (!pThis->Type->AttackCursorOnFriendlies)
			{
				return false;
			}
		}
		else
		{
			if (canCapture)
			{
				return false;
			}
		}
	}
	else
	{
		if (InputManagerClass::Instance->IsForceMoveKeyPressed())
		{
			return false;
		}
	}

	if (canC4 && pThis->Type->C4)
	{
		return true;
	}
	else if (pThis->GetWeapon(1) && pThis->GetWeapon(1)->WeaponType)
	{
		return true;
	}

	return false;
}

bool EngineerAllowEnterBuilding(InfantryClass* pThis, BuildingClass* pBuilding)
{
	if (!pBuilding)
		return false;

	if (pBuilding->Type->BridgeRepairHut ||
		(pBuilding->Type->Capturable &&
		pThis->Owner != pBuilding->Owner &&
		!pThis->Owner->Allies.Contains(pBuilding->Owner)))
		return false;

	if (!InputManagerClass::Instance->IsForceMoveKeyPressed())
	{
		if (!pBuilding->Type->InfantryAbsorb ||
			!pBuilding->Type->Repairable)
			return false;

		if (pBuilding->Health < pBuilding->Type->Strength)
			return false;
	}

	return true;
}

bool IsForceKeyPressed()
{
	if (InputManagerClass::Instance->IsForceFireKeyPressed() ||
		InputManagerClass::Instance->IsForceSelectKeyPressed() ||
		InputManagerClass::Instance->IsForceMoveKeyPressed())
		return true;

	return false;
}

DEFINE_HOOK(0x51E49E, InfantryClass_MouseOverObject_EngineerSkip, 0x6)
{
	GET(InfantryClass*, pThis, EDI);
	GET_STACK(ObjectClass*, pObject, STACK_OFFSET(0x38, 0x4));
	enum { SkipGameCode = 0x51E6D8, C4 = 0x51EA4A };

	auto const pTechno = abstract_cast<TechnoClass*>(pObject);

	if (!pThis ||
		!pThis->Type->Engineer ||
		!pTechno)
		return 0;

	return IsForceKeyPressed() ||
		EngineerAllowAttack(pThis, pTechno) ||
		EngineerAllowEnterBuilding(pThis, abstract_cast<BuildingClass*>(pTechno)) ?
		SkipGameCode : 0;
}

DEFINE_HOOK(0x51E7C7, InfantryClass_MouseOverObject_VehicleThief, 0x6)
{
	return IsForceKeyPressed() ? 0x51E85A : 0;
}

DEFINE_HOOK(0x51EA00, InfantryClass_MouseOverObject_C4, 0x6)
{
	return IsForceKeyPressed() ? 0x51EA59 : 0;
}

DEFINE_HOOK(0x51EE4E, InfantryClass_MouseOverObject_Spy, 0x6)
{
	return IsForceKeyPressed() ? 0x51F095 : 0;
}

DEFINE_HOOK(0x6F33CD, TechnoClass_WhatWeaponShouldIUse_ForceFire, 0x6)
{
	GET(TechnoClass*, pThis, ESI);
	GET_STACK(AbstractClass*, pTarget, STACK_OFFSET(0x18, 0x4));
	enum { Secondary = 0x6F3745 };

	if (const auto pTechno = abstract_cast<TechnoClass*>(pTarget))
	{
		if (!pTechno->AttachedBomb && pThis->IsEngineer() && pThis->GetWeapon(1) && pThis->GetWeapon(1)->WeaponType)
			return Secondary;
	}

	return 0;
}

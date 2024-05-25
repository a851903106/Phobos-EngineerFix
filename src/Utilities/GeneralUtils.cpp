#include "GeneralUtils.h"
#include "Debug.h"
#include <ScenarioClass.h>
#include <Drawing.h>

bool GeneralUtils::IsValidString(const char* str)
{
	return str != nullptr && strlen(str) != 0 && !INIClass::IsBlank(str);
}

bool GeneralUtils::IsValidString(const wchar_t* str)
{
	return str != nullptr && wcslen(str) != 0 && !wcsstr(str, L"MISSING:");
}

void GeneralUtils::IntValidCheck(int* source, const char* section, const char* tag, int defaultValue, int min, int max)
{
	if (*source < min || *source>max)
	{
		Debug::Log("[Developer warning][%s]%s=%d is invalid! Reset to %d.\n", section, tag, *source, defaultValue);
		*source = defaultValue;
	}
}

void GeneralUtils::DoubleValidCheck(double* source, const char* section, const char* tag, double defaultValue, double min, double max)
{
	if (*source < min || *source>max)
	{
		Debug::Log("[Developer warning][%s]%s=%f is invalid! Reset to %f.\n", section, tag, *source, defaultValue);
		*source = defaultValue;
	}
}

const wchar_t* GeneralUtils::LoadStringOrDefault(const char* key, const wchar_t* defaultValue)
{
	if (GeneralUtils::IsValidString(key))
		return StringTable::LoadString(key);
	else
		return defaultValue;
}

const wchar_t* GeneralUtils::LoadStringUnlessMissing(const char* key, const wchar_t* defaultValue)
{
	return wcsstr(LoadStringOrDefault(key, defaultValue), L"MISSING:") ? defaultValue : LoadStringOrDefault(key, defaultValue);
}

std::vector<CellStruct> GeneralUtils::AdjacentCellsInRange(unsigned int range)
{
	std::vector<CellStruct> result;

	for (CellSpreadEnumerator it(range); it; ++it)
		result.push_back(*it);

	return result;
}

const int GeneralUtils::GetRangedRandomOrSingleValue(Point2D range)
{
	return range.X >= range.Y ?
		range.X : ScenarioClass::Instance->Random.RandomRanged(range.X, range.Y);
}

const double GeneralUtils::GetWarheadVersusArmor(WarheadTypeClass* pWH, Armor ArmorType)
{
	return double(MapClass::GetTotalDamage(100, pWH, ArmorType, 0)) / 100.0;
}

// Weighted random element choice (weight) - roll for one.
// Takes a vector of integer type weights, which are then summed to calculate the chances.
// Returns chosen index or -1 if nothing is chosen.
int GeneralUtils::ChooseOneWeighted(const double dice, const std::vector<int>* weights)
{
	float sum = 0.0;
	float sum2 = 0.0;

	for (size_t i = 0; i < weights->size(); i++)
		sum += (*weights)[i];

	for (size_t i = 0; i < weights->size(); i++)
	{
		sum2 += (*weights)[i];
		if (dice < (sum2 / sum))
			return i;
	}

	return -1;
}

// Direct multiplication pow
double GeneralUtils::FastPow(double x, double n)
{
	double r = 1.0;

	while (n > 0)
	{
		r *= x;
		--n;
	}

	return r;
}

// Checks if health ratio has changed threshold (Healthy/ConditionYellow/Red).
bool GeneralUtils::HasHealthRatioThresholdChanged(double oldRatio, double newRatio)
{
	if (oldRatio == newRatio)
		return false;

	if (oldRatio > RulesClass::Instance->ConditionYellow && newRatio <= RulesClass::Instance->ConditionYellow)
	{
		return true;
	}
	else if (oldRatio <= RulesClass::Instance->ConditionYellow && oldRatio > RulesClass::Instance->ConditionRed &&
		(newRatio <= RulesClass::Instance->ConditionRed || newRatio > RulesClass::Instance->ConditionYellow))
	{
		return true;
	}
	else if (oldRatio <= RulesClass::Instance->ConditionRed && newRatio > RulesClass::Instance->ConditionRed)
	{
		return true;
	}

	return false;
}

std::string GeneralUtils::IntToDigits(int num)
{
	std::string sDigits;

	if (num == 0)
	{
		sDigits.push_back('0');
		return sDigits;
	}

	while (num)
	{
		sDigits.push_back(static_cast<char>(num % 10) + '0');
		num /= 10;
	}

	std::reverse(sDigits.begin(), sDigits.end());

	return sDigits;
}

// Gets integer representation of color from ColorAdd corresponding to given index, or 0 if there's no color found.
// Code is pulled straight from game's draw functions that deal with the tint colors.
int GeneralUtils::GetColorFromColorAdd(int colorIndex)
{
	auto const& colorAdd = RulesClass::Instance->ColorAdd;
	int colorValue = 0;

	if (colorIndex < 0 || colorIndex >= (sizeof(colorAdd) / sizeof(ColorStruct)))
		return colorValue;

	auto const& color = colorAdd[colorIndex];
	int red = color.R;
	int green = color.G;
	int blue = color.B;

	if (Drawing::ColorMode() == RGBMode::RGB565)
		colorValue |= blue | (32 * (green | (red << 6)));

	if (Drawing::ColorMode() != RGBMode::RGB655)
		colorValue |= blue | (((32 * red) | (green >> 1)) << 6);

	colorValue |= blue | (32 * ((32 * red) | (green >> 1)));

	return colorValue;
}

int GeneralUtils::DrawHealthBar_Pip(int health, int strength, Vector3D<int> v3d, const bool isBuilding)
{
	int frame = -1;

	if (health <= int(RulesClass::Instance->ConditionRed * strength))
	{
		frame = (v3d.Z == -1) ? v3d.X : v3d.Z;
	}
	else if (health <= int(RulesClass::Instance->ConditionYellow * strength))
	{
		frame = (v3d.Y == -1) ? v3d.X : v3d.Y;
	}
	else
	{
		frame = v3d.X;
	}

	if (frame >= 0)
		return frame;
	else
		return isBuilding ? 1 : 16;
}

int GeneralUtils::DrawHealthBar_PipAmount(int health, int strength, int Minimum, int iLength)
{
	const auto ratio = static_cast<double>(health) / strength;

	return health > 0
		? Math::clamp((int)round(ratio * iLength), Minimum, iLength)
		: 0;
}

// Calculates a new coordinates based on current & target coordinates within specified distance (can be negative to switch the direction) in leptons.
CoordStruct GeneralUtils::CalculateCoordsFromDistance(CoordStruct currentCoords, CoordStruct targetCoords, int distance)
{
	int deltaX = currentCoords.X - targetCoords.X;
	int deltaY = targetCoords.Y - currentCoords.Y;

	double atan = Math::atan2(deltaY, deltaX);
	double radians = (((atan - Math::HalfPi) * (1.0 / Math::GameDegreesToRadiansCoefficient)) - Math::GameDegrees90) * Math::GameDegreesToRadiansCoefficient;
	int x = static_cast<int>(targetCoords.X + Math::cos(radians) * distance);
	int y = static_cast<int>(targetCoords.Y - Math::sin(radians) * distance);

	return CoordStruct { x, y, targetCoords.Z };
}

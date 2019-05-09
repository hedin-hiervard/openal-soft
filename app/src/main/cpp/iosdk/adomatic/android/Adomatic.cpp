#include "iosdk/adomatic/Adomatic.h"
#include "iosdk/adomatic/android/AdomaticHelper.h"

using namespace iosdk;
using namespace std;

Adomatic* Adomatic::sm_instance = nullptr;
const string marketingUrl = "http://adomatic.palmkingdoms.com";


void Adomatic::init(string productId, string secretKey)
{
	AdomaticHelper::sharedInstance()->init(productId, secretKey);
}

void Adomatic::requestMoreGames(std::string forOS)
{
	AdomaticHelper::sharedInstance()->requestMoreGames(forOS);
}

void Adomatic::requestFullscreenAd(std::string testCountry)
{
	AdomaticHelper::sharedInstance()->requestFullscreenAd(testCountry);
}


#include <string>

namespace iosdk
{
    class AdomaticHelper
    {
    public:
        static AdomaticHelper* sharedInstance()
        {
            if(!sm_instance) sm_instance = new AdomaticHelper();
            return sm_instance;
        }        
    public:
        void init(std::string productId, std::string secretKey);
        void requestMoreGames(std::string forOS = "");
        void requestFullscreenAd(std::string testCountry = "");
    private:
        AdomaticHelper() {}
        static AdomaticHelper* sm_instance;
    };
}

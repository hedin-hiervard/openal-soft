//
//  Adomatic.h

#include <string>

namespace iosdk
{
    class Adomatic
    {
    public:
        static Adomatic* sharedInstance()
        {
            if(!sm_instance) sm_instance = new Adomatic();
            return sm_instance;
        }        
    public:
        void init(std::string productId, std::string secretKey);
        void requestMoreGames(std::string forOS = "");
        void requestFullscreenAd(std::string testCountry = "");
    private:
        Adomatic() {}
        static Adomatic* sm_instance;
    };
}

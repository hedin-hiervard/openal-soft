#ifndef __pheroes__Analytics__
#define __pheroes__Analytics__

namespace iosdk
{
    class Analytics
    {
    public:
        void Init(const char* trackerID);
        void SetCustomDimension(int idx, const char* value);
        void SetCustomMetricsInt(int idx, int value);
        void TrackEvent(const char* category, const char* action);
        void TrackEvent(const char* category, const char* action, const char* label);
        void TrackEvent(const char* category, const char* action, const char* label, int value);
        
        static Analytics* sharedInstance()
        {
            if(!sm_sharedInstance) sm_sharedInstance = new Analytics();
            return sm_sharedInstance;
        }
    private:
        Analytics() {}
        static Analytics *sm_sharedInstance;
    };
}

#endif /* defined(__pheroes__Analytics__) */

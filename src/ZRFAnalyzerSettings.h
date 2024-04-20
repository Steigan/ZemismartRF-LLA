#ifndef ZRF_ANALYZER_SETTINGS
#define ZRF_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class ZRFAnalyzerSettings : public AnalyzerSettings
{
  public:
    ZRFAnalyzerSettings();
    virtual ~ZRFAnalyzerSettings();

    virtual bool SetSettingsFromInterfaces();
    void UpdateInterfacesFromSettings();
    virtual void LoadSettings( const char* settings );
    virtual const char* SaveSettings();


    Channel mInputChannel;
    bool mUSBDongle;

  protected:
    std::auto_ptr<AnalyzerSettingInterfaceChannel> mInputChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceBool> mUSBDongleInterface;
};

#endif // ZRF_ANALYZER_SETTINGS

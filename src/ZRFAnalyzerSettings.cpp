#include "ZRFAnalyzerSettings.h"
#include <AnalyzerHelpers.h>


ZRFAnalyzerSettings::ZRFAnalyzerSettings() : mInputChannel( UNDEFINED_CHANNEL ), mLongIsOne( false ), mUSBDongle( false )
{
    mInputChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mInputChannelInterface->SetTitleAndTooltip( "ZRF", "Zemismart RF decoding" );
    mInputChannelInterface->SetChannel( mInputChannel );

    mLongIsOneInterface.reset( new AnalyzerSettingInterfaceBool() );
    mLongIsOneInterface->SetTitleAndTooltip( "", "True - long impulse == bit 1, short impulse == bit 0. False - long impulse == bit 0, short impulse == bit 1." );
    mLongIsOneInterface->SetCheckBoxText( "Long = 1 / Short = 0 (if checked)" );
    mLongIsOneInterface->SetValue( mLongIsOne );

    mUSBDongleInterface.reset( new AnalyzerSettingInterfaceBool() );
    mUSBDongleInterface->SetTitleAndTooltip( "", "True - USB Dongle. False - Remote Control." );
    mUSBDongleInterface->SetCheckBoxText( "USB Dongle (not Remote Control)" );
    mUSBDongleInterface->SetValue( mUSBDongle );

    AddInterface( mInputChannelInterface.get() );
    AddInterface( mLongIsOneInterface.get() );
    AddInterface( mUSBDongleInterface.get() );

    AddExportOption( 0, "Export as text/csv file" );
    AddExportExtension( 0, "text", "txt" );
    AddExportExtension( 0, "csv", "csv" );

    ClearChannels();
    AddChannel( mInputChannel, "ZRF", false );
}

ZRFAnalyzerSettings::~ZRFAnalyzerSettings() = default;

bool ZRFAnalyzerSettings::SetSettingsFromInterfaces()
{
    mInputChannel = mInputChannelInterface->GetChannel();
    mLongIsOne = mLongIsOneInterface->GetValue();
    mUSBDongle = mUSBDongleInterface->GetValue();

    ClearChannels();
    AddChannel( mInputChannel, "ZRF", true );

    return true;
}

void ZRFAnalyzerSettings::UpdateInterfacesFromSettings()
{
    mInputChannelInterface->SetChannel( mInputChannel );
    mLongIsOneInterface->SetValue( mLongIsOne );
    mUSBDongleInterface->SetValue( mUSBDongle );
}

void ZRFAnalyzerSettings::LoadSettings( const char* settings )
{
    SimpleArchive text_archive;
    text_archive.SetString( settings );

    text_archive >> mInputChannel;

    bool longIsOne;
    if( text_archive >> longIsOne )
        mLongIsOne = longIsOne;

    bool dongle;
    if( text_archive >> dongle )
        mUSBDongle = dongle;

    ClearChannels();
    AddChannel( mInputChannel, "ZRF", true );

    UpdateInterfacesFromSettings();
}

const char* ZRFAnalyzerSettings::SaveSettings()
{
    SimpleArchive text_archive;

    text_archive << mInputChannel;
    text_archive << mLongIsOne;
    text_archive << mUSBDongle;

    return SetReturnString( text_archive.GetString() );
}

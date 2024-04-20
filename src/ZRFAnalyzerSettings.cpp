#include "ZRFAnalyzerSettings.h"
#include <AnalyzerHelpers.h>


ZRFAnalyzerSettings::ZRFAnalyzerSettings() : mInputChannel( UNDEFINED_CHANNEL ), mUSBDongle( false )
{
    mInputChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mInputChannelInterface->SetTitleAndTooltip( "ZRF", "Zemismart RF decoding" );
    mInputChannelInterface->SetChannel( mInputChannel );

    mUSBDongleInterface.reset( new AnalyzerSettingInterfaceBool() );
    mUSBDongleInterface->SetTitleAndTooltip( "", "True - USB Dongle. False - Remote Control." );
    mUSBDongleInterface->SetCheckBoxText( "USB Dongle (not Remote Control)" );
    mUSBDongleInterface->SetValue( mUSBDongle );

    AddInterface( mInputChannelInterface.get() );
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
    mUSBDongle = mUSBDongleInterface->GetValue();

    ClearChannels();
    AddChannel( mInputChannel, "ZRF", true );

    return true;
}

void ZRFAnalyzerSettings::UpdateInterfacesFromSettings()
{
    mInputChannelInterface->SetChannel( mInputChannel );
    mUSBDongleInterface->SetValue( mUSBDongle );
}

void ZRFAnalyzerSettings::LoadSettings( const char* settings )
{
    SimpleArchive text_archive;
    text_archive.SetString( settings );

    text_archive >> mInputChannel;

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
    text_archive << mUSBDongle;

    return SetReturnString( text_archive.GetString() );
}

#include "ZRFAnalyzer.h"
#include "ZRFAnalyzerSettings.h"
#include <AnalyzerChannelData.h>
#include <iostream>
#include <string>

ZRFAnalyzer::ZRFAnalyzer()
    : Analyzer2(), mSettings( new ZRFAnalyzerSettings() ), mSampleRateHz( 0 ), mZRF( nullptr ), mSimulationInitilized( false )
{
    SetAnalyzerSettings( mSettings.get() );
    UseFrameV2();
}

ZRFAnalyzer::~ZRFAnalyzer()
{
    KillThread();
}

void ZRFAnalyzer::SetupResults()
{
    mResults.reset( new ZRFAnalyzerResults( this, mSettings.get() ) );
    SetAnalyzerResults( mResults.get() );
    mResults->AddChannelBubblesWillAppearOn( mSettings->mInputChannel );
}

void ZRFAnalyzer::WorkerThread()
{
    U8 bitCount = 0;
    U8 byteBitCount = 0;

    U64 startByte = 0;
    U64 startHigh = 0;
    U64 startLow = 0;
    U64 endPulse = 0;
    U64 markerPossition = 0;

    U8 data = 0;

    mSampleRateHz = GetSampleRate(); // samples per 1 sec
    mZRF = GetAnalyzerChannelData( mSettings->mInputChannel );

    U32 samplesPerPulseMin = mSampleRateHz * 6 / 10000;     // samples per 600 microsec
    U32 samplesPerPulseMax = mSampleRateHz * 2 / 1000;      // samples per 2000 microsec
    U32 samplesPerOneBitPulseMax = samplesPerPulseMax >> 1; // samples per 1000 microsec

    mZRF->AdvanceToNextEdge();

    // skip low level
    if( mZRF->GetBitState() == BIT_LOW )
    {
        mZRF->AdvanceToNextEdge();
    }

    endPulse = mZRF->GetSampleNumber(); // sample with RISING signal

    for( ;; )
    {
        startHigh = endPulse; // sample with RISING signal
        if( byteBitCount == 0 )
        {
            startByte = startHigh;
        }

        mZRF->AdvanceToNextEdge(); // skip high level

        startLow = mZRF->GetSampleNumber(); // sample with FALLING signal

        mZRF->AdvanceToNextEdge(); // skip low level

        endPulse = mZRF->GetSampleNumber(); // sample with RISING signal

        U64 period = ( endPulse - startHigh ); // pulse HIGH+LOW duration (in samples)
        U64 duty = ( startLow - startHigh );   // HIGH duration (in samples)

        U64 period25pc = period >> 2; // 25% of period (in samples)

        if( period > samplesPerPulseMin ) // not noise
        {
            markerPossition = ( startHigh + startLow ) / 2; // HIGH level middle -> marker position

            if( period > samplesPerPulseMax ) // long pulse - start or stop
            {
                if( duty > period25pc ) // start pulse
                {
                    mResults->AddMarker( markerPossition, AnalyzerResults::Start, mSettings->mInputChannel );
                }
                else
                {
                    mResults->AddMarker( markerPossition, AnalyzerResults::Stop, mSettings->mInputChannel );
                }
                bitCount = 0;
                byteBitCount = 0;
            }
            else
            {
                if( bitCount < 72 )
                {
                    data = data << 1;

                    if( period > samplesPerOneBitPulseMax ) // zero bit pulse
                    {
                        mResults->AddMarker( markerPossition, AnalyzerResults::Zero, mSettings->mInputChannel );
                    }
                    else // one bit pulse
                    {
                        data = data | 1;
                        mResults->AddMarker( markerPossition, AnalyzerResults::One, mSettings->mInputChannel );
                    }

                    byteBitCount++;
                    if( byteBitCount == 8 )
                    {
                        byteBitCount = 0;
                        RecordFrameV1( startByte, endPulse, ZRF_BYTE, data, 0 );
                        RecordFrameV2( startByte, endPulse, data );
                        data = 0;
                    }
                    bitCount++;
                }
            }
        }
    }
}

void ZRFAnalyzer::RecordFrameV1( U64 starting_sample, U64 ending_sample, ZRFframeType type, U64 data1, U64 data2 )
{
    Frame frame;

    frame.mStartingSampleInclusive = starting_sample;
    frame.mEndingSampleInclusive = ending_sample;
    frame.mFlags = 0;
    frame.mType = ( U8 )type;
    frame.mData1 = data1;
    frame.mData2 = data2;

    mResults->AddFrame( frame );
    mResults->CommitResults();
    ReportProgress( frame.mEndingSampleInclusive );
}

void ZRFAnalyzer::RecordFrameV2( U64 starting_sample, U64 ending_sample, char data )
{
    FrameV2 frameV2;

    frameV2.AddByte( "data", data );

    mResults->AddFrameV2( frameV2, "data", starting_sample, ending_sample );
    mResults->CommitResults();
}

bool ZRFAnalyzer::NeedsRerun()
{
    return false;
}

U32 ZRFAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate,
                                        SimulationChannelDescriptor** simulation_channels )
{
    if( mSimulationInitilized == false )
    {
        mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), mSettings.get() );
        mSimulationInitilized = true;
    }

    return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 ZRFAnalyzer::GetMinimumSampleRateHz()
{
    return 25000;
}

const char* ZRFAnalyzer::GetAnalyzerName() const
{
    return "Zemismart RF Signal";
}

const char* GetAnalyzerName()
{
    return "Zemismart RF Signal";
}

Analyzer* CreateAnalyzer()
{
    return new ZRFAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
    delete analyzer;
}
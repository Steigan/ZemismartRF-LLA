#include "ZRFSimulationDataGenerator.h"
#include "ZRFAnalyzerSettings.h"

#include <AnalyzerHelpers.h>

ZRFSimulationDataGenerator::ZRFSimulationDataGenerator() : mZRFindex( 0 ), mSimulationSampleRateHz( 0 ), mSettings( new ZRFAnalyzerSettings() )
{
}

ZRFSimulationDataGenerator::~ZRFSimulationDataGenerator()
{
}

void ZRFSimulationDataGenerator::Initialize( U32 simulation_sample_rate, ZRFAnalyzerSettings* settings )
{
    mSimulationSampleRateHz = simulation_sample_rate;
    mSettings = settings;

    mClockGenerator.Init( simulation_sample_rate, simulation_sample_rate );

    mZRFSimulationData.SetChannel( mSettings->mInputChannel );
    mZRFSimulationData.SetSampleRate( simulation_sample_rate );
    mZRFSimulationData.SetInitialBitState( BIT_LOW );
}

U32 ZRFSimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate,
                                                       SimulationChannelDescriptor** simulation_channel )
{
    U64 adjusted_largest_sample_requested =
        AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

    while( mZRFSimulationData.GetCurrentSampleNumber() < adjusted_largest_sample_requested )
    {
        CreateZRFstream();
    }

    *simulation_channel = &mZRFSimulationData;
    return 1;
}

void ZRFSimulationDataGenerator::CreateZRFstream()
{
    // 5 series (uS):
    // Start: 2040, -4100 // RC: 2120, -4250
    // Bit 1: 410, -410   // RC: 422, -323 - last bit; 422, -427 - other bits
    // Bit 0: 620, -1030  // RC: 633, -962 - last bit; 633, -1063 - other bits
    // Stop: 250, -3120   // RC: 524, -6450

    const std::bitset<8> samplecode[ 3 ][ 9 ] = { { 0x2F, 0xCB, 0x9C, 0x3E, 0x2E, 0xBA, 0x5D, 0x3F, 0x9F },
                                                  { 0x1F, 0xF8, 0xAF, 0x0D, 0x1D, 0x89, 0x6E, 0x0C, 0xAA },
                                                  { 0x08, 0xE9, 0xBE, 0x1C, 0x0C, 0x98, 0x7F, 0x1D, 0xB8 } };

    U32 startHigh = 0;
    U32 startLow = 0;
    U32 oneHigh = 0;
    U32 oneLastLow = 0;
    U32 oneLow = 0;
    U32 zeroHigh = 0;
    U32 zeroLastLow = 0;
    U32 zeroLow = 0;
    U32 stopHigh = 0;
    U32 stopLow = 0;

    if( mSettings->mUSBDongle )
    {
        startHigh = mClockGenerator.AdvanceByTimeS( 2040E-6 );
        startLow = mClockGenerator.AdvanceByTimeS( 4010E-6 );
        oneHigh = mClockGenerator.AdvanceByTimeS( 410E-6 );
        oneLow = oneHigh;
        oneLastLow = oneLow;
        zeroHigh = mClockGenerator.AdvanceByTimeS( 620E-6 );
        zeroLow = mClockGenerator.AdvanceByTimeS( 1030E-6 );
        zeroLastLow = zeroLow;
        stopHigh = mClockGenerator.AdvanceByTimeS( 250E-6 );
        stopLow = mClockGenerator.AdvanceByTimeS( 3120E-6 );
    }
    else
    {
        startHigh = mClockGenerator.AdvanceByTimeS( 2120E-6 );
        startLow = mClockGenerator.AdvanceByTimeS( 4250E-6 );
        oneHigh = mClockGenerator.AdvanceByTimeS( 422E-6 );
        oneLastLow = mClockGenerator.AdvanceByTimeS( 323E-6 );
        oneLow = mClockGenerator.AdvanceByTimeS( 427E-6 );
        zeroHigh = mClockGenerator.AdvanceByTimeS( 633E-6 );
        zeroLastLow = mClockGenerator.AdvanceByTimeS( 962E-6 );
        zeroLow = mClockGenerator.AdvanceByTimeS( 1063E-6 );
        stopHigh = mClockGenerator.AdvanceByTimeS( 524E-6 );
        stopLow = mClockGenerator.AdvanceByTimeS( 6450E-6 );
    }


    U32 pause = mClockGenerator.AdvanceByTimeS( 0.5 );

    // Pause
    mZRFSimulationData.Advance( pause );

    if( mZRFindex == 0 )
    {
        mZRFSimulationData.TransitionIfNeeded( BIT_LOW );
    }

    for( U8 s = 0; s < 5; s++ )
    {
        // Start impulse
        mZRFSimulationData.TransitionIfNeeded( BIT_HIGH );
        mZRFSimulationData.Advance( startHigh );
        mZRFSimulationData.TransitionIfNeeded( BIT_LOW );
        mZRFSimulationData.Advance( startLow );

        for( U8 b = 0; b < 9; b++ )
        {
            for( U32 x = 0; x < 8; x++ )
            {
                bool bit = samplecode[ mZRFindex ][ b ].test( 7 - x );

                if( bit == true )
                {
                    // Bit 1
                    mZRFSimulationData.TransitionIfNeeded( BIT_HIGH );
                    mZRFSimulationData.Advance( oneHigh );
                    mZRFSimulationData.TransitionIfNeeded( BIT_LOW );
                    mZRFSimulationData.Advance( ( b == 8 && x == 7 ) ? oneLastLow : oneLow );
                }
                else
                {
                    // Bit 0
                    mZRFSimulationData.TransitionIfNeeded( BIT_HIGH );
                    mZRFSimulationData.Advance( zeroHigh );
                    mZRFSimulationData.TransitionIfNeeded( BIT_LOW );
                    mZRFSimulationData.Advance( ( b == 8 && x == 7 ) ? zeroLastLow : zeroLow );
                }
            }
        }

        // Stop impulse
        mZRFSimulationData.TransitionIfNeeded( BIT_HIGH );
        mZRFSimulationData.Advance( stopHigh );
        mZRFSimulationData.TransitionIfNeeded( BIT_LOW );
        mZRFSimulationData.Advance( stopLow );
    }

    // Pause
    mZRFSimulationData.Advance( pause );

    mZRFindex++;

    if( mZRFindex == 3 )
    {
        mZRFindex = 0;
    }
}

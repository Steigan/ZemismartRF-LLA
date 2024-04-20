#ifndef ZRF_ANALYZER_H
#define ZRF_ANALYZER_H

#include <Analyzer.h>
#include "ZRFAnalyzerResults.h"
#include "ZRFSimulationDataGenerator.h"

enum ZRFframeType
{
    ZRF_BYTE
};

class ZRFAnalyzerSettings;
class ANALYZER_EXPORT ZRFAnalyzer : public Analyzer2
{
  public:
    ZRFAnalyzer();
    virtual ~ZRFAnalyzer();

    virtual void SetupResults();
    virtual void WorkerThread();

    virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
    virtual U32 GetMinimumSampleRateHz();

    virtual const char* GetAnalyzerName() const;
    virtual bool NeedsRerun();

  protected: // vars
    std::auto_ptr<ZRFAnalyzerSettings> mSettings;
    std::auto_ptr<ZRFAnalyzerResults> mResults;
    AnalyzerChannelData* mZRF;

    void RecordFrameV1( U64 starting_sample, U64 ending_sample, ZRFframeType type, U64 data1, U64 data2 );
    void RecordFrameV2( U64 starting_sample, U64 ending_sample, char data );

    ZRFSimulationDataGenerator mSimulationDataGenerator;
    bool mSimulationInitilized;

    U32 mSampleRateHz;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer();
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif // ZRF_ANALYZER_H

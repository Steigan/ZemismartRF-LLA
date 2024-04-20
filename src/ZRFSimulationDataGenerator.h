#ifndef ZRF_SIMULATION_DATA_GENERATOR
#define ZRF_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <string>
#include <bitset>
#include <AnalyzerHelpers.h>

class ZRFAnalyzerSettings;

class ZRFSimulationDataGenerator
{
  public:
    ZRFSimulationDataGenerator();
    ~ZRFSimulationDataGenerator();

    void Initialize( U32 simulation_sample_rate, ZRFAnalyzerSettings* settings );
    U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

  protected:
    ZRFAnalyzerSettings* mSettings;
    U32 mSimulationSampleRateHz;

  protected:
    void CreateZRFstream();
    ClockGenerator mClockGenerator;
    U32 mZRFindex;

    SimulationChannelDescriptor mZRFSimulationData;
};
#endif // ZRF_SIMULATION_DATA_GENERATOR
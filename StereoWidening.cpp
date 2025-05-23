#include "StereoWidening.h"

StereoWidening::StereoWidening() {}

void StereoWidening::process(juce::AudioBuffer<float>& buffer, float widthAmount) {
    auto* dataL = buffer.getWritePointer(0);
    auto* dataR = buffer.getWritePointer(1);

    for (int i = 0; i < buffer.getNumSamples(); ++i) {
        float mid = (dataL[i] + dataR[i]) * 0.5f;
        float side = (dataL[i] - dataR[i]) * 0.5f * widthAmount;

        dataL[i] = mid + side;
        dataR[i] = mid - side;
    }
}

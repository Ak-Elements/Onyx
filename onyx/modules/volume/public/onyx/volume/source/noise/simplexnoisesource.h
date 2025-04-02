#pragma once

#include <onyx/geometry/vector3.h>
#include <onyx/geometry/vector4.h>

#include <onyx/volume/source/volumebase.h>
#include <onyx/volume/source/noise/simplexnoised.h>

namespace Onyx::Volume
{
    class SimplexNoiseSource : public VolumeBase
    {
    public:
        SimplexNoiseSource() = default;
        SimplexNoiseSource(onyxU32 octaves, onyxF32 frequency, onyxF32 amplitude, onyxF32 lacunarity, onyxF32 persistence)
            : m_Octaves(octaves)
            , m_Frequency(frequency)
            , m_Amplitude(amplitude)
            , m_Lacunarity(lacunarity)
            , m_Gain(persistence)
        {
        }

        enum class Dimension
        {
            Dimension_1D,
            Dimension_2D,
            Dimension_3D
        };

        Dimension GetDimension() const { return m_Dimension; }
        void SetDimension(Dimension value) { m_Dimension = value; }

        onyxU32 GetOctaves() const { return m_Octaves; }
		void SetOctaves(onyxU32 value) { m_Octaves = value; }

        onyxF32 GetFrequency() const { return m_Frequency; }
		void SetFrequency(onyxF32 value) { m_Frequency = value; }

        onyxF32 GetAmplitude() const { return m_Amplitude; }
		void SetAmplitude(onyxF32 value) { m_Amplitude = value; }

        onyxF32 GetLacunarity() const { return m_Lacunarity; }
		void SetLacunarity(onyxF32 value) { m_Lacunarity = value; }

        onyxF32 GetGain() const { return m_Gain; }
		void SetGain(onyxF32 value) { m_Gain = value; }

        onyxF32 GetScale() const { return m_Scale; }
		void SetScale(onyxF32 scale) { m_Scale = scale; }

        void SetFrequencies(std::vector<onyxF32>& frequencies)
        {
            for (onyxU32 i = 0; i < frequencies.size(); ++i)
            {
                if (i >= m_PredefinedOctaves.size())
                {
                    m_PredefinedOctaves.emplace_back();
                }

                m_PredefinedOctaves[i].m_Frequency = frequencies[i];
            }
        }

        void SetAmplitudes(std::vector<onyxF32>& amplitudes)
        {
            for (onyxU32 i = 0; i < amplitudes.size(); ++i)
            {
                if (i >= m_PredefinedOctaves.size())
                {
                    m_PredefinedOctaves.emplace_back();
                }

                m_PredefinedOctaves[i].m_Amplitude = amplitudes[i];
            }
        }


		Vector3f GetAnalyticalNormal(const Vector3f& position) const
		{
			Vector3f deriative; //optimize away
			GetInternalValue(position, deriative, true);

            Vector3f analyticalNormal(-deriative[0], 1.0f - deriative[1], -deriative[2]);
            analyticalNormal.Normalize();

            return analyticalNormal;
		}

        virtual Vector4f GetValueAndGradient(const Vector3f& position) const override
        {
            /*
            const onyxF32 offset = 0.001f;// m_Noise.GetGradientOffset();
            onyxF32 val = GetInternalValue(position, deriative);
            Vector3f otherNormal;
            Vector3f centralDiffNormal(-(GetInternalValue(Vector3
            f(position[0] + offset, position[1], position[2]), otherNormal) - GetInternalValue(Vector3f(position[0] - offset, position[1], position[2]), otherNormal)),
                -(GetInternalValue(Vector3f(position[0], position[1] + offset, position[2]), otherNormal) - GetInternalValue(Vector3f(position[0], position[1] - offset, position[2]), otherNormal)),
                -(GetInternalValue(Vector3f(position[0], position[1], position[2] + offset), otherNormal) - GetInternalValue(Vector3f(position[0], position[1], position[2] - offset), otherNormal)));

            centralDiffNormal.Normalize();

            return Vector4f(centralDiffNormal[0], centralDiffNormal[1], centralDiffNormal[2], val);*/

            Vector3f deriative; //optimize away
            onyxF32 val = GetInternalValue(position, deriative, true);

            Vector3f analyticalNormal(-deriative[0], 1.0f - deriative[1], -deriative[2]);
            analyticalNormal.Normalize();

            return Vector4f(analyticalNormal, val);
            
        }

        virtual onyxF32 GetValue(const Vector3f& position) const override
        {
			Vector3f normal;
            return GetInternalValue(position, normal);
        }

    private:
        inline onyxF32 GetInternalValue(const Vector3f& position, Vector3f& normal, bool getGradient = false) const
        {
			onyxF32 distance = 0;
			Vector3f planeNormal(0, 1, 0);

			onyxF32 value = distance - static_cast<onyxF32>(planeNormal.Dot(position));
            onyxF32 noise = GetNoiseVal(position, normal, getGradient);

            return value + noise;
        }

		onyxF32 GetNoiseVal(const Vector3f& position, Vector3f& gradient, bool getGradient = false) const
		{
			onyxF32 frequency = m_Frequency;
			onyxF32 amplitude = 0.5f;

			Vector3f pos = position * m_Scale;
            
            onyxF32 noiseValue = 0.0f;

            //Vector3f deriviateSum;
            onyxU32 octaves = m_Octaves;
            if (m_PredefinedOctaves.empty() == false)
            {
                octaves = static_cast<onyxU32>(m_PredefinedOctaves.size());
            }

			for (size_t i = 0; i < octaves; i++)
			{
                Vector3f noiseD;
                onyxF32 noiseVal = 0.0f;
                Vector3f deriative;

                if (m_PredefinedOctaves.empty() == false)
                {
                    frequency = m_PredefinedOctaves[i].m_Frequency;
                    amplitude = m_PredefinedOctaves[i].m_Amplitude;
                }

                switch (m_Dimension)
                {
                case Dimension::Dimension_1D:
                        SimplexNoiseD::sdnoise1(pos[0] * frequency, noiseD);
                        break;
                    case Dimension::Dimension_2D:
                    {
                        Vector3f tempPos(pos[0], pos[2], 0);
                        SimplexNoiseD::sdnoise2(tempPos * frequency, noiseD, getGradient);
                        noiseVal = noiseD[0];
                        deriative = Vector3f(noiseD[1], 0.0f, noiseD[2]);
                        break;
                    }
                    case Dimension::Dimension_3D:
                    {
                        Vector4f noise3;
                        SimplexNoiseD::sdnoise3(pos, noise3);
                        noiseVal = noise3[0];
                        deriative = Vector3f(noise3[1], noise3[2], noise3[3]);
                        break;
                    }
                    default:
                    {
                        deriative = Vector3f();
                        break;
                    }
                }

                noiseValue += (amplitude * noiseVal);
                if (getGradient)
                    gradient += (deriative * frequency * amplitude);

                if (m_PredefinedOctaves.empty())
                {
                    frequency *= m_Lacunarity;
                    amplitude *= m_Gain;
                }
			}

            const onyxF32 amplitudeScaled = m_Amplitude * m_Scale;
            Vector4f output(noiseValue * m_Amplitude, gradient[0] * amplitudeScaled, gradient[1] * amplitudeScaled, gradient[2] * amplitudeScaled);
            gradient[0] = output[1];
            gradient[1] = output[2];
            gradient[2] = output[3];

			return output[0];	
		}

    private:
        struct Octave
        {
            onyxF32 m_Frequency = 1.0f;
            onyxF32 m_Amplitude = 0.5f;
        };

		onyxF32 m_Scale = 0.0f;
		onyxU32 m_Octaves = 0;
		onyxF32 m_Frequency = 0.0f;
		onyxF32 m_Amplitude = 0.0f;
		onyxF32 m_Lacunarity = 0.0f;
		onyxF32 m_Gain = 0.0f;

        std::vector<Octave> m_PredefinedOctaves;

        Dimension m_Dimension = Dimension::Dimension_2D;
    };
}
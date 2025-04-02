#pragma once

#include <catch2/catch_all.hpp>

#include <tuple>
#include <random>

// This class shows how to implement a simple generator for Catch tests
template <typename T>
class RandomNumberTupleGenerator : public Catch::Generators::IGenerator<std::tuple<T, T>> {
    std::minstd_rand m_rand;
    std::uniform_int_distribution<T> m_dist;
    std::tuple<T, T> current;
public:

    RandomNumberTupleGenerator(T low, T high) :
        m_rand(std::random_device{}()),
        m_dist(low, high)
    {
        static_cast<void>(next());
    }

    std::tuple<T, T> const& get() const override;

    bool next() override {
        current = std::make_tuple<T,T>(m_dist(m_rand), m_dist(m_rand));
        return true;
    }
};

//// Avoids -Wweak-vtables
template <typename T>
std::tuple<T, T> const& RandomNumberTupleGenerator<T>::get() const
{
    return current;
}

// This helper function provides a nicer UX when instantiating the generator
// Notice that it returns an instance of GeneratorWrapper<int>, which
// is a value-wrapper around std::unique_ptr<IGenerator<int>>.
template <typename T>
Catch::Generators::GeneratorWrapper<std::tuple<T, T>> randomNumberTuple(T low, T high)
{
    return Catch::Generators::GeneratorWrapper<std::tuple<T, T>>(Catch::Generators::GeneratorPtr<std::tuple<T, T>>(new RandomNumberTupleGenerator<T>(low, high)));
}

// This class shows how to implement a simple generator for Catch tests
template <typename T>
class RandomNumberTripleGenerator : public Catch::Generators::IGenerator<std::tuple<T, T, T>> {
    std::minstd_rand m_rand;
    std::uniform_int_distribution<T> m_dist;
    std::tuple<T, T, T> current;
public:

    RandomNumberTripleGenerator(T low, T high) :
        m_rand(std::random_device{}()),
        m_dist(low, high)
    {
        static_cast<void>(next());
    }

    std::tuple<T, T, T> const& get() const override;

    bool next() override {
        current = std::make_tuple<T, T, T>(m_dist(m_rand), m_dist(m_rand), m_dist(m_rand));
        return true;
    }
};

//// Avoids -Wweak-vtables
template <typename T>
std::tuple<T, T, T> const& RandomNumberTripleGenerator<T>::get() const
{
    return current;
}

// This helper function provides a nicer UX when instantiating the generator
// Notice that it returns an instance of GeneratorWrapper<int>, which
// is a value-wrapper around std::unique_ptr<IGenerator<int>>.
template <typename T>
Catch::Generators::GeneratorWrapper<std::tuple<T, T, T>> randomNumberTriple(T low, T high)
{
    return Catch::Generators::GeneratorWrapper<std::tuple<T, T, T>>(Catch::Generators::GeneratorPtr<std::tuple<T, T, T>>(new RandomNumberTripleGenerator<T>(low, high)));
}

#include "../../src/policy/RenderStrategyFactory.hpp"

RenderStrategyFactory::RenderStrategyFactory()
{
}

RenderStrategyFactory::RenderStrategyFactory(
    const RenderStrategyFactory &other)
{
    *this = other;
}

RenderStrategyFactory::~RenderStrategyFactory()
{
}

RenderStrategyFactory &RenderStrategyFactory::operator=(
    const RenderStrategyFactory &other)
{
    (void)other;
    return (*this);
}

const RenderDistanceStrategy &RenderStrategyFactory::select(bool headless_mode)
{
    static AdaptiveRenderStrategy adaptive_strategy;
    static FixedRenderStrategy fixed_strategy;

    if (headless_mode == true)
        return (fixed_strategy);
    return (adaptive_strategy);
}

#pragma once
#include "ppCommon.h"
#include <memory>
class TokenizerSettings
{
  private:
    static std::unique_ptr<TokenizerSettings> instance;
    bool disableExtensions;
    Dialect dialect;

  public:
    static TokenizerSettings Instance()
    {
        if (!instance)
            instance = std::unique_ptr<TokenizerSettings>(new TokenizerSettings());
        return *instance;
    }
};